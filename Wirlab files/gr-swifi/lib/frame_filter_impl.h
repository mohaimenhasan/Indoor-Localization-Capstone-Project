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


#ifndef INCLUDED_SWIFI_FRAME_FILTER_IMPL_H
#define INCLUDED_SWIFI_FRAME_FILTER_IMPL_H

#include <swifi/frame_filter.h>
#include "xml_node.h"
#include "simple_filter.h"

namespace gr {
  namespace swifi {

    class frame_filter_impl : public frame_filter
    {
     private:
      pmt::pmt_t d_phy_tag_key;
      boost::shared_ptr<simple_filter> d_filter;
      XMLNode d_frames;
      FILE *xml_out_f;
      int d_frame_idx;
      long d_start_sample, d_end_sample;
      int d_psdu_size, d_rate;
      unsigned long d_rx_time;
      std::string d_start_sample_tagattrname, d_end_sample_tagattrname;

     public:
      frame_filter_impl(const std::string &xml_dump_file,
        const std::string &filter_string,
        const std::string &start_sample_tagattrname,
        const std::string &end_sample_tagattrname,
        const std::string &phy_tag_key,
        const std::string &xml_out_file);

      ~frame_filter_impl();

      // Where all the action really happens
      int general_work(int noutput_items,
                        gr_vector_int& ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_FRAME_FILTER_IMPL_H */

