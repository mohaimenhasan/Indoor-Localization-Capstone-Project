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

#ifndef INCLUDED_SWIFI_PAYLOAD_PARSER_IMPL_H
#define INCLUDED_SWIFI_PAYLOAD_PARSER_IMPL_H

#include <swifi/payload_parser.h>
#include <swifi/mac_utils.h>
#include <swifi/mac_parser.h>
#include <swifi/wifi_dump_formatter.h>
 
namespace gr {
  namespace swifi {

    class payload_parser_impl : public payload_parser
    {
     private:
      pmt::pmt_t d_phy_tag_key, d_info_tag_key;
      phy_frame_rx_info d_rx_info;
      int d_n_data_bytes;  // number of bytes in the DATA field including payload and tail + padding bytes
      int d_n_bytes_copied; // number of bytes copied so far; when this equals d_n_data_bytes, parsing starts
      std::vector<char> d_buffer; // store bits before parsing
      mac_utils::sptr d_mac_utils;
      mac_parser d_mac_parser;
      wifi_dump_formatter::sptr d_wifi_dump;

      void process_tag_value(pmt::pmt_t v);
      void print_output(const char *data, int len);

     public:
      payload_parser_impl(const std::string &phy_tag_key,
        const std::string &filename,
        const std::string &wifi_dump_format,
        const std::string &filter_string,
        bool is_print_raw);
      ~payload_parser_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_PAYLOAD_PARSER_IMPL_H */

