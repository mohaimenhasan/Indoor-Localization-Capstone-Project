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


#include "ofdm_chan_est_impl.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/expj.h>
#include <stdio.h>
#include <stdexcept>

namespace gr {
  namespace swifi {

    ofdm_chan_est::sptr
    ofdm_chan_est::make(int fft_len,
      const std::vector<std::vector<gr_complex> > &sync_ofdm_symbols,
      float alpha,
      const std::string &channel_states_tag_key,
      const std::string &len_tag_key,
      int total_ofdm_symbols,
      bool copy_sync_symbols)
    {
      return gnuradio::get_initial_sptr(
        new ofdm_chan_est_impl(fft_len,
          sync_ofdm_symbols, alpha, channel_states_tag_key,
          len_tag_key, total_ofdm_symbols, copy_sync_symbols));
    }

    ofdm_chan_est_impl::ofdm_chan_est_impl(int fft_len,
      const std::vector<std::vector<gr_complex> > &sync_ofdm_symbols,
      float alpha,
      const std::string &channel_states_tag_key,
      const std::string &len_tag_key,
      int total_ofdm_symbols,
      bool copy_sync_symbols
      )
    : block("ofdm_chan_est",
            gr::io_signature::make(1, 1, fft_len * sizeof(gr_complex)),
            gr::io_signature::make(1, 1, fft_len * sizeof(gr_complex))),
      d_fft_len(fft_len),
      d_sync_ofdm_symbols(sync_ofdm_symbols),
      d_alpha(alpha), d_one_minus_alpha(1 - alpha),
      d_channel_states_tag_key(pmt::intern(channel_states_tag_key)),
      d_len_tag_key(pmt::intern(len_tag_key)),
      d_uses_len_tag_key(!len_tag_key.empty()),
      d_total_ofdm_symbols(total_ofdm_symbols),
      d_copy_sync(copy_sync_symbols),
      d_sym_idx(0)
    {
      set_tag_propagation_policy(TPP_DONT);
      
      // sanity check
      if (!d_fft_len)
        throw std::runtime_error("Size of each OFDM symbol must be non-zero");

      d_num_sync_symbols = d_sync_ofdm_symbols.size();
      if (!d_num_sync_symbols)
        throw std::runtime_error("Sync OFDM symbols must be specified");


      for (int i = 0; i < d_num_sync_symbols; i++) {
        if (d_sync_ofdm_symbols[i].size() != d_fft_len)
          throw std::runtime_error("Size of every OFDM symbols must be equal");
      }

      if (!d_uses_len_tag_key) {
        if (d_total_ofdm_symbols < d_num_sync_symbols + 1)
          throw std::runtime_error("Total number of OFDM symbols must be greater than number of sync OFDM symbols");
      }

      // initialize channel states
      d_channel_states.resize(64, gr_complex(1.0, 0.0));

      // find non-zero sync symbols
      d_nonzero_sync_ofdm_symbols.resize(d_num_sync_symbols);
      for (int i = 0; i < d_num_sync_symbols; i++) {
        d_nonzero_sync_ofdm_symbols[i].resize(d_sync_ofdm_symbols[i].size());
        for (int j = 0; j < d_sync_ofdm_symbols[i].size(); j++) {
          d_nonzero_sync_ofdm_symbols[i][j] =
            d_sync_ofdm_symbols[i][j] == gr_complex(0, 0) ? false : true;
        }
      }

      // find diff sync, used for frequency offset estimation
      d_diff_sync.resize(d_num_sync_symbols);
      for (int i = 0; i < d_num_sync_symbols; i++) {
        // initialize diff sync with zero-phased complex numbers
        d_diff_sync[i].resize(d_sync_ofdm_symbols[i].size(), gr_complex(1, 0));
        for (int j = 0; j < d_sync_ofdm_symbols[i].size() - 1; j++) {
          // only compute diff for non-zero pairs of sync symbols.
          // Ex: For WiFi, with 6 zeros on left, 6 zeros on right, 1 zero at DC,
          // we will use only 50 (50=64-6-6-1-3) diff sync symbols
          // for frequency offset correction
          if (d_nonzero_sync_ofdm_symbols[i][j] &&
              d_nonzero_sync_ofdm_symbols[i][j + 1])
          {
            d_diff_sync[i][j] = d_sync_ofdm_symbols[i][j] *
              conj(d_sync_ofdm_symbols[i][j + 1]);
          }
        }
      }

      // default estimation method
      set_estimation_method(ESTIMATE_LS);

      d_fft = new fft::fft_complex(d_fft_len, true, 1);

      d_pll = blocks::control_loop(2*M_PI/100.0, 0.25, -0.25);
      d_constellation = phy_utils::WIFI_CONSTELLATIONS[phy_utils::BPSK_1_2];
      d_points = d_constellation->points();

      // PLL per carrier
      d_sc_pll = new blocks::control_loop[64];
      for (int i = 0; i < 64; i++)
        d_sc_pll[i] = blocks::control_loop(2*3.14159 / 100, 0.25, -0.25);
    }

    ofdm_chan_est_impl::~ofdm_chan_est_impl() {
      delete d_fft;
      delete[] d_sc_pll;
    }

    void ofdm_chan_est_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      int nreqd = noutput_items;
      if (d_num_sync_symbols > d_sym_idx)
        nreqd -= d_num_sync_symbols - d_sym_idx;
      if (nreqd < 1) 
        nreqd = 1;  // min required number of items

      for (int i = 0; i < ninput_items_required.size(); i++)
        ninput_items_required[i] = nreqd;
    }

    void ofdm_chan_est_impl::set_estimation_method(int method) {
      d_method = method;
    }

    void ofdm_chan_est_impl::estimate_ls(const gr_complex *in) {
      gr_complex h, corrected;
      float error;

      for (int n = 0; n < d_fft_len; n++) {
        if (d_nonzero_sync_ofdm_symbols[d_sym_idx][n]) {
          // estimated channel coefficient
          h = in[n] / d_sync_ofdm_symbols[d_sym_idx][n];

          // moving averaging channel state
          // d_channel_states[n] = d_channel_states[n] * d_one_minus_alpha +
          //   h * d_alpha;

          // initial channel states
          if (d_sym_idx == 0)
            d_channel_states[n] = gr_complex(0, 0);

          // averaging over number of sync symbols
          d_channel_states[n] += h / float(d_num_sync_symbols);
        }
      }
    }

    void ofdm_chan_est_impl::fo_correction(const gr_complex *in,
      gr_complex *out) 
    {
      // frequency offset estimation
      if (d_sym_idx == 0)
        d_foc_sum = gr_complex(0, 0);

      for (int n = 6; n < 32; n++) {
        d_foc_sum += in[n+27] * conj(d_sync_ofdm_symbols[d_sym_idx][n+27]) *
          conj(in[n] * conj(d_sync_ofdm_symbols[d_sym_idx][n]));
      }

      d_fo = arg(d_foc_sum) / 27;

      // frequency offset correction
      for (int n = 0; n < d_fft_len; n++) {
        out[n] = in[n] * gr_expj(-d_fo * n);
      }
    }

    void ofdm_chan_est_impl::estimate_ls_foc(const gr_complex *in) {
      gr_complex corrected[d_fft_len];
      fo_correction(in, corrected);
      estimate_ls(corrected);
    }

    void ofdm_chan_est_impl::estimate_lpi(const gr_complex *in) {
      memset(d_channel_states.data(), 0, sizeof(gr_complex) * d_fft_len);
      for (int n = 0; n < d_fft_len; n++) {
        if (d_nonzero_sync_ofdm_symbols[d_sym_idx][n]) {
          // estimated channel coefficient
          d_channel_states[n] = in[n] / d_sync_ofdm_symbols[d_sym_idx][n];
        }
      }

      gr_complex pilot_channel_states[d_fft_len];
      memset(pilot_channel_states, 0, sizeof(gr_complex) * d_fft_len);
      pilot_channel_states[11] = d_channel_states[11];
      pilot_channel_states[25] = d_channel_states[25];
      pilot_channel_states[39] = d_channel_states[39];
      pilot_channel_states[53] = d_channel_states[53];

      gr_complex fpilot_channel_states[d_fft_len];
      gr_complex fchannel_states[d_fft_len];

      // get FFT of pilot channel states
      memcpy(d_fft->get_inbuf(), pilot_channel_states, sizeof(gr_complex) * d_fft_len);
      d_fft->execute();
      memcpy(fpilot_channel_states, d_fft->get_outbuf(), sizeof(gr_complex) * d_fft_len);

      // get FFT of received symbols
      memcpy(d_fft->get_inbuf(), d_channel_states.data(), sizeof(gr_complex) * d_fft_len);
      d_fft->execute();
      memcpy(fchannel_states, d_fft->get_outbuf(), sizeof(gr_complex) * d_fft_len);

      // compute interpolating lowpass filter
      gr_complex G[d_fft_len];
      for (int i = 0; i < d_fft_len; i++) {
        G[i] = fchannel_states[i] / fpilot_channel_states[i];
        d_channel_states[i] = G[i];
      }
    }

    int ofdm_chan_est_impl::general_work (int noutput_items, 
        gr_vector_int& ninput_items,
        gr_vector_const_void_star& input_items,
        gr_vector_void_star& output_items) 
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];
      gr_complex g;
      std::vector<tag_t> tags, all_tags;
      tag_t *tag = NULL;
      int consumed = 0, produced = 0;

      get_tags_in_window(all_tags, 0, 0, ninput_items[0]);
      std::sort(all_tags.begin(), all_tags.end(), tag_t::offset_compare);
      if (all_tags.size())
        tag = &all_tags[0];

      get_tags_in_window(tags, 0, 0, ninput_items[0], d_len_tag_key);
      std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
      tag_range tr(nitems_read(0), ninput_items[0], tags);
      for (int k = 0; k < tr.size(); k++) {
        if (tr.tag(k).key != pmt::PMT_NIL && pmt::is_integer(tr.tag(k).value)) {
          d_total_ofdm_symbols = pmt::to_long(tr.tag(k).value);
          if (d_total_ofdm_symbols < d_num_sync_symbols + 1)
            throw std::runtime_error("Total number of OFDM symbols must be greater than number of sync OFDM symbols");
          d_sym_idx = 0; // reset to beginning
          for (int n = 0; n < d_fft_len; n++)
            d_channel_states[n] = gr_complex(1, 0);
        }

        int nsyms = tr.range_nitems(k); // num. OFDM symbols in this range
        for (int i = 0; i < nsyms; i++) {
          if (d_sym_idx < d_num_sync_symbols) {
            // estimation on each subcarrier
            switch (d_method) {
              case ESTIMATE_LS:
                estimate_ls(in);
                break;
              case ESTIMATE_LS_FOC:
                estimate_ls_foc(in);
                break;
              case ESTIMATE_LPI:
                estimate_lpi(in);
                break;
            }

            // if there are tags on sync symbols, store them and will add later to output
            if (tag && tag->offset <= tr.abs_first_itemidx(k) + i) {
              // for length tag, we need to modify the length value
              if (pmt::eqv(tag->key, d_len_tag_key)) {
                tag->value = pmt::from_long(pmt::to_long(tag->value) - d_num_sync_symbols);
              }
              d_tags_on_sync_symbols.push_back(*tag);
              tag++;
              if (tag >= all_tags.data() + all_tags.size()) tag = NULL; // no more tags
            }
          }
          else {
            // add tags found on sync symbols to the first data symbol
            if (d_sym_idx == d_num_sync_symbols) {
              // add channel states tag
              add_item_tag(0, nitems_written(0) + produced, d_channel_states_tag_key,
                channel_states_tag::create()
                  .set_phase_offset(0)
                  .set_freq_offset(d_fo)
                  .set_channel_states(d_channel_states));

              // add other tags found on the sync symbols
              for (int j = 0; j < d_tags_on_sync_symbols.size(); j++) {
                add_item_tag(0, nitems_written(0) + produced,
                  d_tags_on_sync_symbols[j].key, d_tags_on_sync_symbols[j].value);
              }
              d_tags_on_sync_symbols.clear();
            }

            // propagate tags found on data symbols to downstream blocks
            if (tag && tag->offset == tr.abs_first_itemidx(k) + i) {
              add_item_tag(0, nitems_written(0) + produced,
                tag->key, tag->value);
              tag++;
              if (tag >= all_tags.data() + all_tags.size()) tag = NULL; // no more tags
            }

            // copy to output
            memcpy(out, in, sizeof(gr_complex) * d_fft_len);
            out += d_fft_len;
            produced++;
          }

          d_sym_idx++;
          if (d_sym_idx >= d_total_ofdm_symbols)
            d_sym_idx = 0;

          in += d_fft_len;
          consumed++;
        }
      }

      consume(0, consumed);
      return produced;
    }
  }
}