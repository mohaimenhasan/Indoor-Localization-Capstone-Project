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

#include "dsss_frame_sync_impl.h"
#include <swifi/phy_utils.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/expj.h>
#include <stdio.h>
#include <algorithm>

namespace gr {
  namespace swifi {

    #define PRINT_PHY_RAW_PAYLOAD   0

    #define LONG_SFD   1
    #define SHORT_SFD  0

    static char SFD[2][16] = { {1,1,1,1, 0,0,1,1, 1,0,1,0, 0,0,0,0},
                               {0,0,0,0, 0,1,0,1, 1,1,0,0, 1,1,1,1} };

    #define LONG_THRESHOLD  100
    #define SHORT_THRESHOLD 40

    dsss_frame_sync::sptr
    dsss_frame_sync::make(int samprate_mhz,
              const std::vector<float> &taps)
    {
      return gnuradio::get_initial_sptr(
        new dsss_frame_sync_impl(samprate_mhz, taps));
    }

    dsss_frame_sync_impl::dsss_frame_sync_impl(int samprate_mhz,
              const std::vector<float> &taps)
      : gr::block("dsss_frame_sync",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
      d_history(1),
      d_decimation(samprate_mhz),
      d_interpolation(11),
      d_ctr(0),
      d_firs(11),
      d_state(DSSS_UNSYNCED),
      d_bit_idx(0),
      d_peak_detector(new peak_detector(11, 1.0)),
      d_modem(new diffmodem()),
      d_descrambler(new ieee80211b_descrambler()),
      d_tag_added(false),
      d_out_idx(0),
      d_onezero_count(0)
    {
      set_relative_rate(1.0 * d_interpolation / d_decimation);
      set_output_multiple(1);

      std::vector<float> dummy_taps;
      for(unsigned i = 0; i < d_interpolation; i++) {
        d_firs[i] = new gr::filter::kernel::fir_filter_ccf(1, dummy_taps);
      }

      set_taps(taps);
      install_taps(d_new_taps);

      set_demod_reset_tag_key();  // set default demod_reset_tag_key
    }

    dsss_frame_sync_impl::~dsss_frame_sync_impl()
    {
      for (int i = 0; i < d_firs.size(); i++)
        delete d_firs[i];

      // if (d_corr_pwr)
      //   delete[] d_corr_pwr;

      if (d_peak_detector)
        delete d_peak_detector;

      if (d_modem)
        delete d_modem;

      if (d_descrambler)
        delete d_descrambler;
    }

    void dsss_frame_sync_impl::set_resampler_reset_tag_key(const std::string &resampler_reset_tag_key) {
      assert(resampler_reset_tag_key.size() > 0);
      d_resampler_reset_tag_key = pmt::intern(resampler_reset_tag_key);
    }

    void dsss_frame_sync_impl::set_demod_reset_tag_key(const std::string &demod_reset_tag_key) {
      assert(demod_reset_tag_key.size() > 0);
      d_demod_reset_tag_key = pmt::intern(demod_reset_tag_key);
    }

    void dsss_frame_sync_impl::set_descrambler_reset_tag_key(const std::string &descrambler_reset_tag_key) {
      assert(descrambler_reset_tag_key.size() > 0);
      d_descrambler_reset_tag_key = pmt::intern(descrambler_reset_tag_key);
    }

    void dsss_frame_sync_impl::set_control_message_queue(gr::msg_queue::sptr msgq) {
      assert(msgq != NULL);
      d_msgq = msgq;
    }

    void dsss_frame_sync_impl::set_taps(const std::vector<float> &taps)
    {
      std::vector<float> tmp_taps;
      if (taps.size() == 0) {
        // create default taps
        float beta = 5.0;
        float fractional_bw = 0.4;
        float trans_width = 0.5 - fractional_bw;
        float mid_trans_band = 0.5 - trans_width / 2;
        tmp_taps = gr::filter::firdes::low_pass(d_interpolation, // gain
                    1,
                    mid_trans_band / d_interpolation,
                    trans_width / d_interpolation,
                    gr::filter::firdes::WIN_KAISER,
                    beta);
      }
      else tmp_taps = taps;

      compute_taps_with_barker(tmp_taps, d_new_taps);
      // d_new_taps = tmp_taps;

      // round up length to a multiple of the interpolation factor
      int n = d_new_taps.size() % d_interpolation;
      if(n > 0) {
        n = d_interpolation - n;
        while(n-- > 0) {
          d_new_taps.insert(d_new_taps.end(), 0);
        }
      }

      assert(d_new_taps.size() % d_interpolation == 0);
    }

    void dsss_frame_sync_impl::install_taps(const std::vector<float> &taps)
    {
      int nfilters = d_interpolation;
      int nt = taps.size() / nfilters;

      assert(nt * nfilters == (int) taps.size());

      std::vector< std::vector <float> > xtaps(nfilters);

      for(int n = 0; n < nfilters; n++)
        xtaps[n].resize (nt);

      for(int i = 0; i < (int)taps.size(); i++)
        xtaps[i % nfilters][i / nfilters] = taps[i];

      for(int n = 0; n < nfilters; n++)
        d_firs[n]->set_taps(xtaps[n]);

      set_history(nt);
    }

    void dsss_frame_sync_impl::compute_taps_with_barker(
      const std::vector<float> &taps, std::vector<float> &results) 
    {
      int barker_size = 11;
      int barker[] = {1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1};
      std::reverse(barker, barker + barker_size); // create match filter with Barker sequence

      /*
       * The taps with Barker sequence are computed as follows:
       *
       * Diagram:
       *             -----    ------------    -----
       *     x ----->| h |--->|Decimation|--->| g |---> z
       *             -----    ------------    -----
       *     with h: N-coefficient filter
       *          Decimation factor: M
       *          g: L-coefficient filter
       *
       * Goal: We want to find filter f such that the following is equivalent:
       *
       *             -----    ------------
       *     x ----->| f |----|Decimation|---> z
       *             -----    ------------
       *
       * First, write the output of the filter h followed by the decimator as:
       *     y[m] = \sum_{k=0}^{N-1} h(k)u(mM-k)
       * Then, write the output of filter g as:
       *     z[i] = \sum_{j=0}^{L-1} g(j)y(i-j)
       *          = \sum_{j=0}^{L-1} \sum_{k=0}^{N-1} g(j)h(k)u(iM - (jM+k))
       * Let l = jM+k for all j and k, the above is equivalent to
       *     z[i] = \sum_{l=0}^{(L-1)M+N-1} f(l)u(iM - l)
       * and we found that
       *     f(l) = \sum_{jM+k=l} g(j)h(k)
       *     f(l) has size of (L-1)M+N
       *
       * For Barker sequence, L=11
       */
      results.resize((barker_size-1)*d_decimation + taps.size(), 0);
      for (int j = 0; j < barker_size; j++) {
        for (int k = 0; k < taps.size(); k++) {
          results[j*d_decimation+k] += barker[j]*taps[k];
        }
      }
    }

    void dsss_frame_sync_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      if (d_state != DSSS_UNSYNCED) {
        int nreqd = std::max((unsigned)1, (int)((double) (noutput_items+1) * \
                  d_decimation / d_interpolation) + history() - 1);
        unsigned ninputs = ninput_items_required.size();
        for(unsigned i = 0; i < ninputs; i++)
          ninput_items_required[i] = nreqd;
      }
      else {
        for (unsigned i = 0; i < ninput_items_required.size(); i++)
          ninput_items_required[i] = noutput_items;
      }
    }


    void dsss_frame_sync_impl::add_tag_info(unsigned long tag_offset) {
      add_item_tag(0, tag_offset, d_resampler_reset_tag_key,
        pmt::make_tuple(pmt::from_long(d_items_to_send)));
      std::cout << "dsss_frame_sync: add tag " << d_resampler_reset_tag_key
        << " offset " << tag_offset << " value " << d_items_to_send << std::endl;

      // add_item_tag(0, tag_offset, d_demod_reset_tag_key,
      //   pmt::make_tuple(pmt::from_long(d_dbpsk->get_last_demodulated()),
      //     pmt::from_double(d_dbpsk->get_frequency()),
      //     pmt::from_double(d_dbpsk->get_phase())));
      // std::cout << "dsss_frame_sync: add tag " << d_demod_reset_tag_key << 
      //   " offset " << tag_offset << std::endl;

      add_item_tag(0, tag_offset, d_descrambler_reset_tag_key,
        pmt::from_long(d_descrambler->shiftreg()));
      std::cout << "dsss_frame_sync: add tag " << d_descrambler_reset_tag_key
        << " offset " << tag_offset << " value " << (int)d_descrambler->shiftreg() << std::endl;
    }

    void dsss_frame_sync_impl::process_control_msg(gr::message::sptr msg) {
      printf("dsss_frame_sync: received control msg\n");
    }


    void dsss_frame_sync_impl::prepare_recv_bytes(int n) {
      d_bit_idx = 0;
      d_bytes_to_recv = n;
      d_data.reset(new char[n]);
      memset(d_data.get(), 0, n);
      d_bptr = d_data.get();
    }


    void dsss_frame_sync_impl::process_receive_bytes() {
      switch (d_substate) {
        case SYNCED_GET_HEADER:
          {
            // parsing DSSS header
            phy80211_dsss_frame_header *hdr = (phy80211_dsss_frame_header*)d_data.get();
            d_crc.reset();
            d_crc.process_bytes(hdr, 4);

            printf("-----------------------------------------------------------------------------------------------------------\n");
            d_time_rx_frame /= 20;
            d_time_rx_frame_end = d_time_rx_frame + hdr->length +
              (d_sfd == SFD[LONG_SFD] ? 144 + 48 : 72 + 24);

            printf("DSSS PHY Header: Preamble: %s\t",
              d_sfd == SFD[LONG_SFD] ? "long " : "short");

            if (d_crc.checksum() != hdr->crc) {
              d_state = DSSS_UNSYNCED;
              printf("Checksum incorrect! %lu\n", nitems_read(0));
              printf("Header: ");
              for (int i = 0; i < d_bytes_to_recv; i++)
                printf("%02X ", d_data[i] & 0xFF);
              printf("\n");
            }
            else {
              d_substate = SYNCED_GET_PAYLOAD;
              int nbytes = (d_state == DSSS_SYNCED_DBPSK) ?
                int(ceil(hdr->length / 8.0f)) :
                int(ceil(hdr->length * 2 / 8.0f));

              d_rate = hdr->signal;
              printf("Service: %02X\tMPDU Length: %4d (%5dus)\tRate: %s",
                hdr->service, nbytes, hdr->length,
                phy_utils::rate_string(d_rate, phy_utils::MODE_DSSS).c_str());

              switch (d_rate) {
                case 0x0A:  // 1 Mbps
                  prepare_recv_bytes(nbytes);
                  break;
                case 0x14:  // 2 Mbps
                case 0x37:  // 5.5 Mbps
                case 0x6e:  // 11 Mbps
                default:
                  d_state = DSSS_UNSYNCED;
                  printf("not supported");
                  break;
              }

              printf("\n");
            }
          }
          break;

        case SYNCED_GET_PAYLOAD:
#if PRINT_PHY_RAW_PAYLOAD_
          printf("Payload:\n");
          for (int i = 0; i < d_bytes_to_recv; i++) {
            printf("%02X ", d_data[i] & 0xFF);
            if (i % 8 == 7) printf("\n");
          }
          printf("\n");
#endif
          d_mac_utils.parse_recv_bytes(d_data.get(), d_bytes_to_recv,
            phy_frame_rx_info(phy_utils::MODE_DSSS, d_rate, d_time_rx_frame, d_time_rx_frame_end));
          d_state = DSSS_UNSYNCED;
          break;
      }
    }

    char dsss_frame_sync_impl::receive_dqpsk_symbol(const gr_complex &sample) {
      char reg = d_descrambler->shiftreg();
      char symbol = d_modem->demodulate_dqpsk(sample);
      char bit0 = d_descrambler->execute(symbol & 1);
      char bit1 = d_descrambler->execute((symbol >> 1) & 1);
      // printf("(%f,%f) %d %02x %d %d\n", sample.real(), sample.imag(),
      //   symbol & 0xFF, reg & 0xFF, bit0 & 0xFF, bit1 & 0xFF);
      return bit0 | (bit1 << 1);
    }

    int dsss_frame_sync_impl::general_work(int noutput_items,
              gr_vector_int &ninput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[d_out_idx];
      gr_complex corr;
      int nconsumed = 0, nproduced = 0;
      int ninputs = ninput_items[0];
      char symbol;
      unsigned int ctr = d_ctr;

#define count_one_zero(bit, count) { \
  /* increase for 1, decrease for 0 */ \
  count += (bit << 1) - 1;  \
  /* limit number of ones/zeros to the length of long/short preamble */ \
  count = std::max(std::min(count, 128), -56); \
}

#define receive_dbpsk_symbol(sample)   d_descrambler->execute(d_modem->demodulate_dbpsk(sample));


      while (nconsumed < ninputs && nproduced < noutput_items) {
        // resampling with filtering to get correlation with Barker sequence
        corr = d_firs[ctr]->filter(in + nconsumed);
        ctr += d_decimation;
        while (ctr >= d_interpolation) {
          ctr -= d_interpolation;
          nconsumed++;
        }
        // out[nproduced++] = corr;

        // get peak correlation
        d_peak_detector->process(corr);
        if (d_peak_detector->get_peak(corr)) {
          switch (d_state) {
            // Note: UNSYNCED and SEARCH_SFD state use DBPSK modulation
            // search for repeated ones (long preamble) or zeros (short preamble)
            case DSSS_UNSYNCED:
              symbol = receive_dbpsk_symbol(corr);
              count_one_zero(symbol, d_onezero_count);

              // preamble detected and first bit of SFD matched
              if ((d_onezero_count >= LONG_THRESHOLD && !symbol)
                  || (d_onezero_count <= -SHORT_THRESHOLD && symbol)) 
              {
                d_state = DSSS_SEARCH_SFD;
                d_sfd = SFD[!symbol];
                d_bit_idx = 1;  // SEARCH_SFD state will start with the 2nd bit of SFD
              }
              break;
  
            // search for SFD field
            case DSSS_SEARCH_SFD:
              symbol = receive_dbpsk_symbol(corr);
              // check every bit against SFD's bit
              if (symbol == d_sfd[d_bit_idx]) {
                d_bit_idx++;
                // got the whole SFD field correctly
                if (d_bit_idx == 16) {
                  d_substate = SYNCED_GET_HEADER;
                  if (d_sfd == SFD[LONG_SFD]) {
                    // long preamble followed by 1Mbps header
                    d_state = DSSS_SYNCED_DBPSK;
                    // d_modem->change_modulation(diffmodem::MODULATION_DBPSK);
                  }
                  else {
                    // short preamble followed by 2Mbps header
                    d_state = DSSS_SYNCED_DQPSK;
                    // d_modem->change_modulation(diffmodem::MODULATION_DQPSK);
                  }

                  // prepare buffer to receive the next 6 bytes of header including
                  // |SIGNAL(1)|SERVICE(1)|LENGTH(2)|CRC(2)|
                  prepare_recv_bytes(6);

                  // reset ones/zeros count
                  d_onezero_count = 0;
                }
              }
              // wrong SFD, go back to unsynced
              else {
                count_one_zero(symbol, d_onezero_count);
                d_state = DSSS_UNSYNCED;
              }
              break;

            // start to receive IEEE80211B header/payload as a sequence of determined number of bytes
            case DSSS_SYNCED_DBPSK:
              // get time at the first bit of BPSK
              if (d_bptr == d_data.get() && d_bit_idx == 0) {
                d_time_rx_frame = nitems_read(0) + nconsumed;
              }

              symbol = receive_dbpsk_symbol(corr);
              (*d_bptr) |= (char)(symbol << (d_bit_idx++));
              if (d_bit_idx >= 8) {
                d_bit_idx = 0;
                d_bptr++;
                if (d_bptr - d_data.get() >= d_bytes_to_recv)
                  process_receive_bytes();
              }
              break;

            case DSSS_SYNCED_DQPSK:
            // get time at the first bit of BPSK
              if (d_bptr == d_data.get() && d_bit_idx == 0) {
                d_time_rx_frame = nitems_read(0) + nconsumed;
              }

              symbol = receive_dqpsk_symbol(corr);
              (*d_bptr) |= (char)(symbol << d_bit_idx);
              d_bit_idx += 2;
              if (d_bit_idx >= 8) {
                d_bit_idx = 0;
                d_bptr++;
                if (d_bptr - d_data.get() >= d_bytes_to_recv)
                  process_receive_bytes();
              }
              break;

          } // switch
        }

      } // while

      d_ctr = ctr;


      consume(0, nconsumed);
      produce(0, nproduced);

      return WORK_CALLED_PRODUCE;
    }



  } /* namespace swifi */
} /* namespace gr */

