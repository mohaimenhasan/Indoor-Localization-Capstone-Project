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

#ifndef INCLUDED_SWIFI_SAMPLE_ADDER_IMPL_H
#define INCLUDED_SWIFI_SAMPLE_ADDER_IMPL_H

#include <swifi/sample_adder.h>

namespace gr {
  namespace swifi {

    class sample_adder_impl : public sample_adder
    {
     private:
      enum STATE { ADD_FRONT, COPY, ADD_TAIL };

      int d_state;
      int d_num_zeros, d_samps;
      uint64_t d_length;
      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

     public:
      sample_adder_impl(const uint64_t packet_length,
                        const uint8_t rate,
                        const int num_zeros);

      ~sample_adder_impl();

      // Where all the action really happens
      int general_work(int noutput_items,
                        gr_vector_int& ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_SAMPLE_ADDER_IMPL_H */

