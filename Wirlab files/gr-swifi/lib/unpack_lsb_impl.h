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

#ifndef INCLUDED_SWIFI_UNPACK_LSB_IMPL_H
#define INCLUDED_SWIFI_UNPACK_LSB_IMPL_H

#include <swifi/unpack_lsb.h>

namespace gr {
  namespace swifi {

    class unpack_lsb_impl : public unpack_lsb
    {
     private:
      int d_bits;

     public:
      unpack_lsb_impl(int bits_per_input_item);
      ~unpack_lsb_impl();

      // Where all the action really happens
      int work(int noutput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_UNPACK_LSB_IMPL_H */

