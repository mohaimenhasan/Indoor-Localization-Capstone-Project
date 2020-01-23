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


#ifndef INCLUDED_SWIFI_DSSS_FRAME_SYNC_2_IMPL_H
#define INCLUDED_SWIFI_DSSS_FRAME_SYNC_2_IMPL_H

#include "diffmodem.h"
#include "peak_detector.h"
#include "scrambling.h"
#include <swifi/dsss_frame_sync_2.h>
#include <swifi/mac_utils.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/thread/thread.h>
#include <gnuradio/runtime_types.h>
#include <gnuradio/blocks/control_loop.h>
#include <boost/crc.hpp>
#include <boost/shared_array.hpp>

namespace gr {
  namespace swifi {

    class dsss_frame_sync_2_impl : public dsss_frame_sync_2
    {
     public:
      dsss_frame_sync_2_impl();
      virtual ~dsss_frame_sync_2_impl();

      // Where all the action really happens
      int general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);

     private:
      enum State { UNSYNCED, DSSS_SEARCH_SFD, 
        DSSS_GET_1MBPS, DSSS_GET_2MBPS };
      enum SubState { DSSS_PARSE_HEADER, DSSS_PARSE_PAYLOAD };

      typedef bool (dsss_frame_sync_2_impl::*ReceiveAction)(const gr_complex&);

      peak_detector *d_barker_peak_detector, *d_cck_peak_detector;
      diffmodem *d_modem;
      ieee80211b_descrambler *d_descrambler;

      // frequency offset correction
      blocks::control_loop d_pll;
      bool d_phase_tracking_enabled;

      // frame sync
      int d_state, d_substate;
      char *d_sfd;
      filter::kernel::fir_filter_ccf *d_barker_filter;
      filter::kernel::fir_filter_ccc *d_cck_filter;

      std::list<char> d_received_bits;
      int d_one_count;

      // receiving data
      int d_bit_idx, d_bytes_to_recv;
      boost::shared_array<char> d_data;
      char *d_bptr;

      // MAC parser
      mac_utils d_mac_utils;
      int d_rate;

      // statistics
      unsigned long d_rx_time, d_rx_time_end;  // time in microsecs

      // CRC for DSSS header: similar to boost::crc_ccitt_type (x^16+x^12+x^5+1)
      // but with final XOR value of 0xFFFF
      // and with reflected input and output
      boost::crc_optimal<16, 0x1021, 0xFFFF, 0xFFFF, true, true> d_crc;

      void setup_filter();
      void prepare_buffer(int n);
      int receive_barker(const gr_complex *in,
        gr_complex *out, int ninput_items,
        ReceiveAction action);
      int receive_cck(const gr_complex *in,
        gr_complex *out, int ninput_items,
        ReceiveAction action);

      gr_complex despread_barker(const gr_complex *in, gr_complex *out);
      gr_complex despread_cck(const gr_complex *in, gr_complex *out);
      char decode_dbpsk(const gr_complex &c);
      char decode_dqpsk(const gr_complex &c);
      bool detect_preamble(const gr_complex &sample);
      bool search_sfd(const gr_complex &sample);
      bool get_1mbps(const gr_complex &sample);
      bool get_2mbps(const gr_complex &sample);

      void count_bits(char bit) {
        if (d_received_bits.front()) d_one_count--;
        if (bit) d_one_count++;
        d_received_bits.pop_front();
        d_received_bits.push_back(bit);
      }

      int num_zeros() { return d_received_bits.size() - d_one_count; }
      int num_ones() { return d_one_count; }

      void parse_received_bytes();
      void parse_header();
      void parse_payload();

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_DSSS_FRAME_SYNC_2_IMPL_H */

