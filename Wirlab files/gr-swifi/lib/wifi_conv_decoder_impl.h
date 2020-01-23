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

#ifndef INCLUDED_SWIFI_WIFI_CONV_DECODER_IMPL_H
#define INCLUDED_SWIFI_WIFI_CONV_DECODER_IMPL_H

#include <swifi/wifi_conv_decoder.h>
#include <swifi/wifi_conv_1_2.h>
#include <gnuradio/trellis/fsm.h>

namespace gr {
  namespace swifi {

    class wifi_conv_decoder_impl : public wifi_conv_decoder
    {
     private:
      int d_state;
      pmt::pmt_t d_phy_tag_key;
      std::vector<char> d_buffer;
      int d_blocksize, d_codedblocksize, d_stopsize, d_nitems_stored;
      long d_start_offset_read, d_start_offset_write;
      wifi_conv_1_2 code;
      std::vector<tag_t> d_stored_tags;
      std::vector<int> d_puncture_map;

      void set_blocksize(int blocksize);
      void set_stopsize(int blocksize);
      void set_puncture_map(int rate);
      
     public:
      wifi_conv_decoder_impl(const std::string &phy_tag_key);
      wifi_conv_decoder_impl(int blocksize, int rate);
      ~wifi_conv_decoder_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      // Where all the action really happens
      int general_work(int noutput_items,
          gr_vector_int& ninput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_WIFI_CONV_DECODER_IMPL_H */

