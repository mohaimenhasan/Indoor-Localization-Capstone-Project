/* -*- c++ -*- */
/*
 * Copyright 2014 Triet Vo-Huu <vohuudtr@ccs.neu.edu>
 *
 * Wireless Security Lab - College of Computer and Information Science
 * Northeastern University, Boston, MA 02115, USA.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ofdm_frame_sync_impl.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/expj.h>
#include <stdio.h>
#include <algorithm>
#include <stdexcept>

namespace gr {
  namespace swifi {

    ofdm_frame_sync::sptr
    ofdm_frame_sync::make(float short_threshold, int min_short_plateau_len,
      float long_threshold, float avg_alpha, float drop_ratio_threshold,
      const std::string &trigger_tag_key)
    {
      return gnuradio::get_initial_sptr(
        new ofdm_frame_sync_impl(short_threshold, min_short_plateau_len,
          long_threshold, avg_alpha, drop_ratio_threshold,
          trigger_tag_key));
    }

    ofdm_frame_sync_impl::ofdm_frame_sync_impl(
      float short_threshold, int min_short_plateau_len,
      float long_threshold, float avg_alpha, float drop_ratio_threshold,
      const std::string &trigger_tag_key)
      : gr::sync_block("ofdm_frame_sync",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make2(1, 2, sizeof(gr_complex), sizeof(char))),
        d_state(SHORT_SEARCH),
        d_short_threshold(short_threshold), d_long_threshold(long_threshold),
        d_avg_alpha(avg_alpha), d_avg_one_minus_alpha(1 - avg_alpha),
        d_drop_ratio_threshold(drop_ratio_threshold),
        d_min_short_plateau_len(min_short_plateau_len),
        d_trigger_tag_key(pmt::intern(trigger_tag_key)),
        d_uses_trigger_tag_key(!trigger_tag_key.empty()),
        d_plateau_len(0),
        d_coarse_fo(0), d_fine_fo(0), d_current_phase(0),
        d_signal_pwr(0)
    {
      if (trigger_tag_key.empty()) {
        throw std::runtime_error("Trigger tag key must be specified\n");
      }
    }

    ofdm_frame_sync_impl::~ofdm_frame_sync_impl()
    {
    }

    void ofdm_frame_sync_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      switch (d_state) {
        case SHORT_SEARCH:
          ninput_items_required[0] = noutput_items + 32;
          break;
        case LONG_SEARCH:
          ninput_items_required[0] = noutput_items + 128;
          break;
        default:
          ninput_items_required[0] = noutput_items;
          break;
      }
    }

    int ofdm_frame_sync_impl::short_search(const gr_complex *in, int ninput_items) {
      gr_complex c;
      float e;
      int i = 0, k;

      ninput_items -= 32; // require at least 32 samples for repeated short symbol detection
      for (i = 0; i < ninput_items; i++) {
        c = gr_complex(0, 0);
        e = 0;
        for (k = 0; k < 16; k++) {
          c += in[k + 16] * conj(in[k]);  // cross correlation between 2 consecutive short symbols
          e += in[k].real() * in[k].real() + in[k].imag() * in[k].imag(); // sum energy
        }

        // ratio between cross correlation and energy exceeds threshold when repeated short symbols are found
        if ((c.real() * c.real() + c.imag() * c.imag()) / (e * e) > d_short_threshold) {
          // update length of plateau
          d_plateau_len++;

          // check plateau length to meet requirement
          if (d_plateau_len >= d_min_short_plateau_len) {
            // update coarse frequency offset
            d_coarse_fo = arg(c) / 16;

            // switch to search for long symbols for exact synchronization
            d_state = LONG_SEARCH;

            return i + 32; // return number of read items
          }
        }
        else d_plateau_len--; // reset plateau length due to correlation reduction below threshold

        if (d_plateau_len > 160) d_plateau_len = 160;
        else if (d_plateau_len < 0) d_plateau_len = 0;

        in++;
      }

      return i; // return number of read items
    }

    void ofdm_frame_sync_impl::fo_correction(const gr_complex *in,
      int nitems, float fo, gr_complex *out) 
    {
      fo = -fo;
      for (int i = 0; i < nitems; i++) {
        d_current_phase += fo;
        out[i] = in[i] * gr_expj(d_current_phase);
        // printf("in %f out %f\n", arg(in[i]), arg(out[i]));
      }
      while (d_current_phase > M_PI) d_current_phase -= M_PI * 2;
      while (d_current_phase < -M_PI) d_current_phase += M_PI * 2;
    }

    bool ofdm_frame_sync_impl::find_peak(const gr_complex *in, const gr_complex *ref,
      int len, float threshold)
    {
      gr_complex c(0, 0);
      float e = 0;
      for (int k = 0; k < len; k++) {
        c += in[k] * conj(ref[k]);
        e += in[k].real() * in[k].real() + in[k].imag() * in[k].imag();
      }
      return (c.real() * c.real() + c.imag() * c.imag() > e * threshold);
    }

    int ofdm_frame_sync_impl::long_search(const gr_complex *in, int ninput_items,
      unsigned long nread) 
    {
      boost::shared_array<gr_complex> fo_corrected;
      gr_complex c, corrected_in;
      int i = 0, k;
      int n;
      bool peak;

      // we don't need to search for long symbols beyond d_expected_max_long_location
      n = d_expected_max_long_location - nread + 1;

      ninput_items -= 128;  // require at least 128 samples for 2 long symbols searching
      if (n > ninput_items) n = ninput_items; // make sure we don't use more than what are available on input

      // printf("n %d ninput_items %d exp %lu nread %lu\n", n, ninput_items,
      //   d_expected_max_long_location, nread);
      if (n <= 0)
        return 0; // need more items

      // coarse frequency offset compensation
      fo_corrected.reset(new gr_complex[n + 128]);
      fo_correction(in, n + 128, d_coarse_fo, fo_corrected.get());

      for (i = 0; i < n; i++) {
        peak = find_peak(&fo_corrected[i], TD_LONG_SYM, 64, d_long_threshold * TD_LONG_SYM_PWR);
        if (peak) {
          // printf("found first peak\n");
          peak = find_peak(&fo_corrected[i+64], TD_LONG_SYM, 64, d_long_threshold * TD_LONG_SYM_PWR);
          if (peak) {
            // printf("found second peak\n");
            // cross correlation between 2 long symbols
            c = gr_complex(0, 0);
            for (int k = 0; k < 64; k++) {
              c += fo_corrected[i+64+k] * conj(fo_corrected[i+k]);
            }

            // update fine frequency offset
            d_fine_fo = arg(c) / 64;

            // get signal power
            d_preamble_pwr = abs(c) / 64;
            d_signal_pwr = d_preamble_pwr;

            // switch to synced state
            d_state = SYNCED;

            // printf("coarse_fo %f fine_fo %f preamble_pwr %f start %lu\n",
            //   d_coarse_fo, d_fine_fo, d_preamble_pwr, nread + i + 128);

            return i + 128; // return number of read items
          }
        }
      }

      if (nread + n >= d_expected_max_long_location + 1)
        d_state = SHORT_SEARCH;

      return n; // return number of read items
    }

    int ofdm_frame_sync_impl::work(int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];
      char *detect;
      int n = 0;
      bool has_detect_stream, found;
      float avg;

      has_detect_stream = output_items.size() == 2;
      if (has_detect_stream) {
        detect = (char*)output_items[1];
        memset(detect, 0, sizeof(char) * noutput_items);
      }

      switch (d_state) {
        case SHORT_SEARCH:
          n = short_search(in, noutput_items);
          if (d_state == LONG_SEARCH) {
            // printf("go to long search %lu coarse_fo %f\n", nitems_read(0) + n,
            //   d_coarse_fo);
            
            // set max index where the first long symbol can be found
            d_expected_max_long_location = nitems_read(0) + 
              n - d_plateau_len + 1 + 320;

            // limit plateau length to avoid infinite loop when n could be 0 due to long plateau length
            d_plateau_len = std::min(d_plateau_len, 160);
            // d_plateau_len = 0;

          }
          memcpy(out, in, sizeof(gr_complex) * n);
          break;
        case LONG_SEARCH:
          n = long_search(in, noutput_items, nitems_read(0));
          if (d_state == SYNCED) {
            // frequency offset correction
            fo_correction(in, n, d_coarse_fo + d_fine_fo, out);

            // signal to the downstream block
            // note: the following condition may not happen when
            // the signal is not WIFI signal
            if (n >= 160) {
              // add trigger tag at the first sample of the long symbol sequence
              if (d_uses_trigger_tag_key) {
                add_item_tag(0, nitems_written(0) + n - 160,
                  d_trigger_tag_key, 
                  trigger_tag::create()
                    .set_state(true)
                    .set_sample_index(nitems_read(0) + n - 320)
                    .set_freq_offset(d_coarse_fo + d_fine_fo));
              }
              
              // use the second stream for signaling
              if (has_detect_stream) {
                detect[n - 160] = 1;
              }
            }
          }
          else {
            memcpy(out, in, sizeof(gr_complex) * n);
          }
          break;
        case SYNCED:
          n = noutput_items;

          // detect energy drop in signal
          avg = d_signal_pwr;
          for (int i = 0; i < n; i++) {
            avg = avg * d_avg_one_minus_alpha + 
              (in[i].real() * in[i].real() + in[i].imag() * in[i].imag()) * d_avg_alpha;

            // if there is energy drop
            if (avg < d_preamble_pwr * d_drop_ratio_threshold) {
              n = i;

              // add trigger tag to announce the end of packet
              if (d_uses_trigger_tag_key) {
                add_item_tag(0, nitems_written(0) + n,
                  d_trigger_tag_key,
                  trigger_tag::create().set_state(false));
              }

              // printf("phy frame ends at %lu\n", nitems_read(0) + n);
              d_state = SHORT_SEARCH;
              break;
            }
          }
          d_signal_pwr = avg;

          // frequency offset correction
          fo_correction(in, n, d_coarse_fo + d_fine_fo, out);
          break;
        // case SYNCED:
        //   n = short_search(in, noutput_items);
        //   if (d_state == LONG_SEARCH) {
        //     // printf("go to long search %lu coarse_fo %f\n", nitems_read(0) + n,
        //     //   d_coarse_fo);
            
        //     // set max index where the first long symbol can be found
        //     d_expected_max_long_location = nitems_read(0) + 
        //       n - d_plateau_len + 1 + 320;

        //     // limit plateau length to avoid infinite loop when n could be 0 due to long plateau length
        //     d_plateau_len = std::min(d_plateau_len, 160);
        //     // d_plateau_len = 0;
        //     memcpy(out, in, sizeof(gr_complex) * n);
        //     break;
        //   }

        //   n = noutput_items;

        //   // detect energy drop in signal
        //   avg = d_signal_pwr;
        //   for (int i = 0; i < n; i++) {
        //     avg = avg * d_avg_one_minus_alpha + 
        //       (in[i].real() * in[i].real() + in[i].imag() * in[i].imag()) * d_avg_alpha;

        //     // if there is energy drop
        //     if (avg < d_preamble_pwr * d_drop_ratio_threshold) {
        //       n = i;

        //       // add trigger tag to announce the end of packet
        //       if (d_uses_trigger_tag_key) {
        //         add_item_tag(0, nitems_written(0) + n,
        //           d_trigger_tag_key,
        //           trigger_tag::create().set_state(false));
        //       }

        //       // printf("phy frame ends at %lu\n", nitems_read(0) + n);
        //       d_state = SHORT_SEARCH;
        //       break;
        //     }
        //   }
        //   d_signal_pwr = avg;

        //   // frequency offset correction
        //   fo_correction(in, n, d_coarse_fo + d_fine_fo, out);
        //   break;
      }

      return n;
    }

    const gr_complex ofdm_frame_sync_impl::TD_LONG_SYM[64] = {
      // generated
      gr_complex(0.156250, 0.000000),
      gr_complex(-0.005121, -0.120325),
      gr_complex(0.039750, -0.111158),
      gr_complex(0.096832, 0.082798),
      gr_complex(0.021112, 0.027886),
      gr_complex(0.059824, -0.087707),
      gr_complex(-0.115131, -0.055180),
      gr_complex(-0.038316, -0.106171),
      gr_complex(0.097541, -0.025888),
      gr_complex(0.053338, 0.004076),
      gr_complex(0.000989, -0.115005),
      gr_complex(-0.136805, -0.047380),
      gr_complex(0.024476, -0.058532),
      gr_complex(0.058669, -0.014939),
      gr_complex(-0.022483, 0.160657),
      gr_complex(0.119239, -0.004096),
      gr_complex(0.062500, -0.062500),
      gr_complex(0.036918, 0.098344),
      gr_complex(-0.057206, 0.039299),
      gr_complex(-0.131263, 0.065227),
      gr_complex(0.082218, 0.092357),
      gr_complex(0.069557, 0.014122),
      gr_complex(-0.060310, 0.081286),
      gr_complex(-0.056455, -0.021804),
      gr_complex(-0.035041, -0.150888),
      gr_complex(-0.121887, -0.016566),
      gr_complex(-0.127324, -0.020501),
      gr_complex(0.075074, -0.074040),
      gr_complex(-0.002806, 0.053774),
      gr_complex(-0.091888, 0.115129),
      gr_complex(0.091717, 0.105872),
      gr_complex(0.012285, 0.097600),
      gr_complex(-0.156250, 0.000000),
      gr_complex(0.012285, -0.097600),
      gr_complex(0.091717, -0.105872),
      gr_complex(-0.091888, -0.115129),
      gr_complex(-0.002806, -0.053774),
      gr_complex(0.075074, 0.074040),
      gr_complex(-0.127324, 0.020501),
      gr_complex(-0.121887, 0.016566),
      gr_complex(-0.035041, 0.150888),
      gr_complex(-0.056455, 0.021804),
      gr_complex(-0.060310, -0.081286),
      gr_complex(0.069557, -0.014122),
      gr_complex(0.082218, -0.092357),
      gr_complex(-0.131263, -0.065227),
      gr_complex(-0.057206, -0.039299),
      gr_complex(0.036918, -0.098344),
      gr_complex(0.062500, 0.062500),
      gr_complex(0.119239, 0.004096),
      gr_complex(-0.022483, -0.160657),
      gr_complex(0.058669, 0.014939),
      gr_complex(0.024476, 0.058532),
      gr_complex(-0.136805, 0.047380),
      gr_complex(0.000989, 0.115005),
      gr_complex(0.053338, -0.004076),
      gr_complex(0.097541, 0.025888),
      gr_complex(-0.038316, 0.106171),
      gr_complex(-0.115131, 0.055180),
      gr_complex(0.059824, 0.087707),
      gr_complex(0.021112, -0.027886),
      gr_complex(0.096832, -0.082798),
      gr_complex(0.039750, 0.111158),
      gr_complex(-0.005121, 0.120325),
    };

  } /* namespace swifi */
} /* namespace gr */

