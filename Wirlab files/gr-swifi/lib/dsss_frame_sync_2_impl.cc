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

#include "dsss_frame_sync_2_impl.h"
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

    #define LONG_THRESHOLD  64
    #define SHORT_THRESHOLD 28
    #define PREAMBLE_THRESHOLD  28

    dsss_frame_sync_2::sptr
    dsss_frame_sync_2::make()
    {
      return gnuradio::get_initial_sptr(
        new dsss_frame_sync_2_impl());
    }

    dsss_frame_sync_2_impl::dsss_frame_sync_2_impl()
      : gr::block("dsss_frame_sync_2",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
      d_state(UNSYNCED),
      d_barker_peak_detector(new peak_detector(11, 0.1)),
      d_cck_peak_detector(new peak_detector(8, 0.1)),
      d_modem(new diffmodem()),
      d_descrambler(new ieee80211b_descrambler()),
      d_pll(M_PI * 2 / 100, 0.25, -0.25),
      d_phase_tracking_enabled(true),
      d_bptr(NULL),

      d_received_bits(PREAMBLE_THRESHOLD*1.2, 0),
      d_one_count(0)
    {
      set_relative_rate(1.0);
      set_output_multiple(1);
      set_history(11);

      setup_filter();
    }

    dsss_frame_sync_2_impl::~dsss_frame_sync_2_impl()
    {
      if (d_barker_filter)
        delete d_barker_filter;

      if (d_cck_filter)
        delete d_cck_filter;

      if (d_barker_peak_detector)
        delete d_barker_peak_detector;

      if (d_cck_peak_detector)
        delete d_cck_peak_detector;

      if (d_modem)
        delete d_modem;

      if (d_descrambler)
        delete d_descrambler;
    }

    void dsss_frame_sync_2_impl::setup_filter() {
      float barker[] = {1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1};
      std::reverse(barker, barker + 11); // create match filter with Barker sequence
      d_barker_filter = new filter::kernel::fir_filter_ccf(
        1, std::vector<float>(barker, barker + 11));

      gr_complex cck[] = {};
      std::reverse(cck, cck + 8); // match filter with CCK sequence
      d_cck_filter = new filter::kernel::fir_filter_ccc(
        1, std::vector<gr_complex>(cck, cck + 8));
    }

    void dsss_frame_sync_2_impl::prepare_buffer(int n) {
      d_bit_idx = 0;
      d_bytes_to_recv = n;
      d_data.reset(new char[n]);
      memset(d_data.get(), 0, n);
      d_bptr = d_data.get();
    }

    gr_complex dsss_frame_sync_2_impl::despread_barker(const gr_complex *in,
      gr_complex *out) 
    {
      gr_complex c, peak;
      float phase_error = 0;  // important to initialize phase error to 0 in the case phase tracking is disabled
      char symbol;

      for (int i = 0; i < 11; i++) {
        // phase tracking with BPSK
        out[i] = in[i] * gr_expj(d_pll.get_phase());
        if (d_phase_tracking_enabled) {
          if (out[i].real() > 0) phase_error = -arg(out[i] * conj(gr_complex(1, 0)));
          else phase_error = - arg(out[i] * conj(gr_complex(-1, 0)));
        }
        d_pll.advance_loop(phase_error);
        d_pll.phase_wrap();
        d_pll.frequency_limit();

        // despreading
        c = d_barker_filter->filter(&out[i-11]);
        d_barker_peak_detector->process(c);
        d_barker_peak_detector->get_peak(peak);
      }

      return peak; 
    }

    gr_complex dsss_frame_sync_2_impl::despread_cck(const gr_complex *in,
      gr_complex *out) 
    {
      gr_complex c, peak;
      float phase_error = 0;
      char symbol;

      for (int i = 0; i < 8; i++) {
        // phase tracking
        out[i] = in[i] * gr_expj(d_pll.get_phase());
        d_pll.advance_loop(phase_error);
        d_pll.phase_wrap();
        d_pll.frequency_limit();

        // despreading
        c = d_cck_filter->filter(&out[i-8]);
        d_cck_peak_detector->process(c);
        d_cck_peak_detector->get_peak(peak);
      }

      return peak; 
    }

    char dsss_frame_sync_2_impl::decode_dbpsk(const gr_complex &c) {
      return d_descrambler->execute(d_modem->demodulate_dbpsk(c));
    }

    char dsss_frame_sync_2_impl::decode_dqpsk(const gr_complex &c) {
      char symbol = d_modem->demodulate_dqpsk(c);
      char bit0 = d_descrambler->execute(symbol & 1);
      char bit1 = d_descrambler->execute((symbol >> 1) & 1);
      return bit0 | (bit1 << 1);
    }

    int dsss_frame_sync_2_impl::receive_barker(const gr_complex *in,
      gr_complex *out, int ninput_items,
      ReceiveAction action)
    {
      // set pointer to the current samples (note: we did call set_history(11) )
      in += 11;
      out += 11;

      // round down to multiple of 11
      ninput_items = 11 * (ninput_items / 11);

      for (int i = 0; i < ninput_items; i += 11) {
        gr_complex sample = despread_barker(&in[i], &out[i]);
        if ((this->*action)(sample))
          return i + 11;  // return number of read items
      }
      return ninput_items;  // return number of read items
    }

    int dsss_frame_sync_2_impl::receive_cck(const gr_complex *in,
      gr_complex *out, int ninput_items,
      ReceiveAction action)
    {
      // set pointer to the current samples
      in += 8;
      out += 8;

      // round down to multiple of 8
      ninput_items = 8 * (ninput_items / 8);

      for (int i = 0; i < ninput_items; i += 8) {
        gr_complex sample = despread_cck(&in[i], &out[i]);
        if ((this->*action)(sample))
          return i + 8;  // return number of read items
      }
      return ninput_items;  // return number of read items
    }

    bool dsss_frame_sync_2_impl::detect_preamble(const gr_complex &sample)
    {
      // get decoded bit
      char bit = decode_dbpsk(sample);

      count_bits(bit);

      if ((num_ones() >= PREAMBLE_THRESHOLD && !bit)
          || (num_zeros() >= PREAMBLE_THRESHOLD && bit)) 
      {
        d_state = DSSS_SEARCH_SFD;
        d_sfd = SFD[!bit];
        d_bit_idx = 1;  // SEARCH_SFD state will start with the 2nd bit of SFD
        return true;
      }

      return false;
    }

    bool dsss_frame_sync_2_impl::search_sfd(const gr_complex &sample)
    {
      // get decoded bit
      char bit = decode_dbpsk(sample);

      count_bits(bit);

      // check every bit against SFD's bit
      if (bit == d_sfd[d_bit_idx]) {
        d_bit_idx++;
        // got the whole SFD field correctly
        if (d_bit_idx == 16) {
          d_state = (d_sfd == SFD[LONG_SFD]) ? DSSS_GET_1MBPS : DSSS_GET_2MBPS;
          d_substate = DSSS_PARSE_HEADER;

          // prepare buffer to receive the next 6 bytes of header including
          // |SIGNAL(1)|SERVICE(1)|LENGTH(2)|CRC(2)|
          prepare_buffer(6);

          // reset ones/zeros count
          // d_onezero_count = 0;

          return true;
        }
      }
      // wrong SFD, go back to unsynced
      else {
        // go back to UNSYNCED state
        d_state = UNSYNCED;
        return true;
      }

      return false;
    }

    bool dsss_frame_sync_2_impl::get_1mbps(const gr_complex &sample)
    {
      // get decoded bit
      char bit = decode_dbpsk(sample);
      (*d_bptr) |= (char)(bit << (d_bit_idx++));
      if (d_bit_idx >= 8) {
        d_bit_idx = 0;
        d_bptr++;
        if (d_bptr - d_data.get() >= d_bytes_to_recv) {
          parse_received_bytes();
          return true;
        }
      }

      return false;
    }

    bool dsss_frame_sync_2_impl::get_2mbps(const gr_complex &sample)
    {
      // get decoded symbol
      char symbol = decode_dqpsk(sample);
      (*d_bptr) |= (char)(symbol << d_bit_idx);
      d_bit_idx += 2;
      if (d_bit_idx >= 8) {
        d_bit_idx = 0;
        d_bptr++;
        if (d_bptr - d_data.get() >= d_bytes_to_recv) {
          parse_received_bytes();
          return true;
        }
      }

      return false;
    }

    void dsss_frame_sync_2_impl::parse_received_bytes() {
      switch (d_substate) {
        case DSSS_PARSE_HEADER:
          parse_header();
          break;
        case DSSS_PARSE_PAYLOAD:
          parse_payload();
          break;
      }
    }

    void dsss_frame_sync_2_impl::parse_header() {
      // parsing DSSS header
      phy80211_dsss_frame_header *hdr = (phy80211_dsss_frame_header*)d_data.get();
      d_crc.reset();
      d_crc.process_bytes(hdr, 4);

      printf("-----------------------------------------------------------------------------------------------------------\n");
      printf("DSSS PHY Header: Time: %luus\tPreamble: %s\t", d_rx_time,
        d_sfd == SFD[LONG_SFD] ? "long " : "short");

      if (d_crc.checksum() != hdr->crc) {
        d_state = UNSYNCED;
        printf("Checksum incorrect!\n");
        printf("Raw Header: ");
        for (int i = 0; i < d_bytes_to_recv; i++)
          printf("%02X ", d_data[i] & 0xFF);
        printf("\n");
      }
      else {
        // compute number of bytes from LENGTH value, which is number of microseconds of the payload
        int nbytes = (d_state == DSSS_GET_1MBPS) ?
          int(ceil(hdr->length / 8.0f)) :
          int(ceil(hdr->length * 2 / 8.0f));

        // time (in us) where frame ends
        d_rx_time_end = d_rx_time + hdr->length 
          + (d_sfd == SFD[LONG_SFD] ? 144 + 48 : 72 + 24);

        d_rate = hdr->signal;
        printf("Service: %02X\tMPDU Length: %4d (%5dus)\tRate: %s",
          hdr->service, nbytes, hdr->length,
          phy_utils::rate_string(d_rate, phy_utils::MODE_DSSS).c_str());

        switch (d_rate) {
          case 0x0A:  // 1 Mbps
            prepare_buffer(nbytes);
            d_state = DSSS_GET_1MBPS;
            d_substate = DSSS_PARSE_PAYLOAD;
            break;
          case 0x14:  // 2 Mbps
            prepare_buffer(nbytes);
            d_state = DSSS_GET_2MBPS;
            d_substate = DSSS_PARSE_PAYLOAD;
            break;
          case 0x37:  // 5.5 Mbps
          case 0x6e:  // 11 Mbps
          default:
            d_state = UNSYNCED;
            printf("not supported");
            break;
        }

        printf("\n");
      }
    }

    void dsss_frame_sync_2_impl::parse_payload() {
#if PRINT_PHY_RAW_PAYLOAD_
      printf("Payload:\n");
      for (int i = 0; i < d_bytes_to_recv; i++) {
        printf("%02X ", d_data[i] & 0xFF);
        if (i % 8 == 7) printf("\n");
      }
      printf("\n");
#endif
      d_mac_utils.parse_recv_bytes(d_data.get(), d_bytes_to_recv,
        phy_frame_rx_info(phy_utils::MODE_DSSS, d_rate, d_rx_time, d_rx_time_end));
      d_state = UNSYNCED;
    }

    int dsss_frame_sync_2_impl::general_work(int noutput_items,
              gr_vector_int &ninput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];
      int n_in = std::min(noutput_items, ninput_items[0]);
      int n = 0;
      char symbol;

      while (n_in > 0) {
        // enable PLL for phase tracking only in UNSYNCED state
        d_phase_tracking_enabled = (d_state == UNSYNCED);
        d_barker_peak_detector->lock(d_state != UNSYNCED);

        switch (d_state) {
          case UNSYNCED:
            n = receive_barker(in, out, n_in, 
              &dsss_frame_sync_2_impl::detect_preamble);
            break;
          case DSSS_SEARCH_SFD:
            n = receive_barker(in, out, n_in,
              &dsss_frame_sync_2_impl::search_sfd);

            // record time
            if (d_state == DSSS_GET_1MBPS || d_state == DSSS_GET_2MBPS) {
              if (d_substate == DSSS_PARSE_HEADER) {
                // get time (in us) at the first sample of preamble
                d_rx_time = (nitems_read(0) + n) / 11 
                  - (d_sfd == SFD[LONG_SFD] ? 144 : 72);
              }
            }

            break;
          case DSSS_GET_1MBPS:
            n = receive_barker(in, out, n_in,
              &dsss_frame_sync_2_impl::get_1mbps);
            break;
          case DSSS_GET_2MBPS:
            n = receive_barker(in, out, n_in,
              &dsss_frame_sync_2_impl::get_2mbps);
            break;
        }

        if (n == 0)
          break;  // break while

        in += n;
        out += n;
        n_in -= n;
        consume(0, n);
        produce(0, n);
      }

      return WORK_CALLED_PRODUCE;
    }



  } /* namespace swifi */
} /* namespace gr */

