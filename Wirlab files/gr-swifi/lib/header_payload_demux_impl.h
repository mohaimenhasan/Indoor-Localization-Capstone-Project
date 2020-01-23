/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_SWIFI_HEADER_PAYLOAD_DEMUX_IMPL_H
#define INCLUDED_SWIFI_HEADER_PAYLOAD_DEMUX_IMPL_H

#include <swifi/header_payload_demux.h>

namespace gr {
  namespace swifi {

    class header_payload_demux_impl : public header_payload_demux
    {
     private:
      int d_header_len; //!< Number of bytes per header
      int d_output_items_per_symbol; //!< Items per output symbol
      int d_gi; //!< Items per guard interval
      int d_input_items_per_symbol; //!< Items per input symbol = d_gi + d_output_items_per_symbol
      size_t d_itemsize; //!< Bytes per item
      size_t d_input_symbol_size; //!< bytes per input symbol
      size_t d_output_symbol_size; //!< bytes per output symbol
      pmt::pmt_t d_len_tag_key; //!< Key of length tag
      pmt::pmt_t d_trigger_tag_key; //!< Key of trigger tag (if used)
      pmt::pmt_t d_phy_tag_key; //!< Key of PHY tag
      bool d_uses_phy_tag;
      trigger_tag d_trigger_tag;
      bool d_output_symbols; //!< If true, output is symbols, not items
      int d_state; //!< Current read state
      int d_curr_payload_len; //!< number of symbols to be copied to payload port
      int d_header_ncopied; //!< number of symbols copied to header port
      std::vector<pmt::pmt_t> d_payload_tag_keys; //!< Temporary buffer for PMTs that go on the payload (keys)
      std::vector<pmt::pmt_t> d_payload_tag_values; //!< Temporary buffer for PMTs that go on the payload (values)

      // Helper functions to make the state machine more readable

      //! Message handler: Reads the result from the header demod and sets length tag (and other tags)
      void parse_header_data_msg(pmt::pmt_t header_data);

      //! Copies n symbols from in to out, makes sure tags are propagated properly. Does neither consume nor produce.
      void copy_n_symbols(const unsigned char *in, unsigned char *out,
        int port, int n_symbols);


     public:
      header_payload_demux_impl(int header_len,
        int output_items_per_symbol, int guard_interval,
        const std::string &length_tag_key,
        const std::string &trigger_tag_key,
        bool output_symbols, size_t itemsize,
        const std::string &phy_tag_key);
      ~header_payload_demux_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_HEADER_PAYLOAD_DEMUX_IMPL_H */

