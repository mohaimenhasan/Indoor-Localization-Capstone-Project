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

#ifndef INCLUDED_SWIFI_SCRAMBLER_IMPL_H
#define INCLUDED_SWIFI_SCRAMBLER_IMPL_H

#include <swifi/scrambler.h>

namespace gr {
  namespace swifi {

    class scrambler_impl : public scrambler
    {
     private:
      enum scrambler_mode_t { MODE_SCRAMBLE, MODE_PASS_THROUGH };
      pmt::pmt_t d_phy_tag_key;
      bool d_is_scrambler;
      char d_shiftreg;
      int d_mode;
      bool d_require_seed;
      long d_tail_pos;
      bool d_random_seed;
      int d_fixed_seed;

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);
      char getSeed(const char* data);

     public:
      scrambler_impl(bool is_scrambler, const std::string &phy_tag_key);
      ~scrambler_impl();

      void set_seed(bool random, int fixed_seed);

      // Where all the action really happens
      int work(int noutput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_SCRAMBLER_IMPL_H */

