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

#ifndef INCLUDED_SWIFI_PHY_FRAME_GENERATOR_IMPL_H
#define INCLUDED_SWIFI_PHY_FRAME_GENERATOR_IMPL_H

#include <swifi/phy_frame_generator.h>
#include <swifi/phy_utils.h>

namespace gr {
  namespace swifi {

    class phy_frame_generator_impl : public phy_frame_generator
    {
     private:
      enum ProcessingState { GENERATE_HEADER, GENERATE_SERVICE,
        COPY_DATA, GENERATE_TAIL_PAD, NUM_STATES };
      pmt::pmt_t d_phy_tag_key, d_len_tag_key;
      phy80211_frame_header d_header;
      int d_state;
      int d_nitems_to_copy, d_nitems_copied;
      int d_rate, d_length;
      bool d_added;

     public:
      phy_frame_generator_impl(const std::string &phy_tag_key,
        const std::string &len_tag_key);
      ~phy_frame_generator_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      // Where all the action really happens
      int general_work(int noutput_items,
          gr_vector_int& ninput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_PHY_FRAME_GENERATOR_IMPL_H */

