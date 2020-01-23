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


#ifndef INCLUDED_SWIFI_DSSS_FRAME_SYNC_IMPL_H
#define INCLUDED_SWIFI_DSSS_FRAME_SYNC_IMPL_H

#include "diffmodem.h"
#include "peak_detector.h"
#include "scrambling.h"
#include <swifi/dsss_frame_sync.h>
#include <swifi/mac_utils.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/thread/thread.h>
#include <gnuradio/runtime_types.h>
#include <gnuradio/blocks/control_loop.h>
#include <boost/crc.hpp>
#include <boost/shared_array.hpp>

namespace gr {
  namespace swifi {

    class dsss_frame_sync_impl : public dsss_frame_sync
    {
     public:
      dsss_frame_sync_impl(int samprate_mhz,
                const std::vector<float> &taps);
      virtual ~dsss_frame_sync_impl();

      void set_resampler_reset_tag_key(
        const std::string &resampler_reset_tag_key = "resampler_reset");
      
      void set_demod_reset_tag_key(
        const std::string &demod_reset_tag_key = "demod_reset");
      
      void set_descrambler_reset_tag_key(
        const std::string &descrambler_reset_tag_key = "descrambler_reset");

      void set_control_message_queue(gr::msg_queue::sptr msgq);

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      // Where all the action really happens
      int general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);

     private:
      enum State { DSSS_UNSYNCED, DSSS_SEARCH_SFD, 
        DSSS_SYNCED_DBPSK, DSSS_SYNCED_DQPSK };
      enum SubState { SYNCED_GET_HEADER, SYNCED_GET_PAYLOAD };

      unsigned d_history;
      unsigned d_interpolation;
      unsigned d_decimation;
      unsigned d_ctr;
      std::vector<float> d_new_taps;
      std::vector<gr::filter::kernel::fir_filter_ccf *> d_firs;

      peak_detector *d_peak_detector;
      diffmodem *d_modem;
      ieee80211b_descrambler *d_descrambler;

      // frame sync
      int d_state, d_substate, d_bit_idx, d_onezero_count;
      char *d_sfd;

      // CRC for DSSS header: similar to boost::crc_ccitt_type (x^16+x^12+x^5+1)
      // but with final XOR value of 0xFFFF
      // and with reflected input and output
      boost::crc_optimal<16, 0x1021, 0xFFFF, 0xFFFF, true, true> d_crc;

      char d_recv_byte;
      int d_bytes_to_recv;
      boost::shared_array<char> d_data;
      char *d_bptr;

      mac_utils d_mac_utils;
      int d_rate;

      int d_items_to_send;  //!< number of samples to send downstream
      int d_items_sent;     //!< number of samples already sent to downstream
      int d_out_idx;
      gr::msg_queue::sptr d_msgq;
      int d_tail_size;
      int d_tail_sent;

      unsigned long d_time_rx_frame, d_time_rx_frame_end;

      // tag
      pmt::pmt_t d_resampler_reset_tag_key;
      pmt::pmt_t d_demod_reset_tag_key;
      pmt::pmt_t d_descrambler_reset_tag_key;
      bool d_tag_added;

      unsigned history() const { return d_history; }
      void set_history(unsigned history) { d_history = history; }

      void set_taps(const std::vector<float> &taps);
      void install_taps(const std::vector<float> &taps);
      void compute_taps_with_barker(const std::vector<float> &taps, 
                  std::vector<float> &results);
      void add_tag_info(unsigned long tag_offset);

      void prepare_recv_bytes(int n);

      char receive_dqpsk_symbol(const gr_complex &sample);

      void process_receive_bytes();
      void process_control_msg(gr::message::sptr msg);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_DSSS_FRAME_SYNC_IMPL_H */

