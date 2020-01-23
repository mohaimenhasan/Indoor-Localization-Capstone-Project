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


#ifndef INCLUDED_SWIFI_FD_NOISE_ADDER_IMPL_H
#define INCLUDED_SWIFI_FD_NOISE_ADDER_IMPL_H

#include <swifi/fd_noise_adder.h>
#include <gnuradio/random.h>

namespace gr {
  namespace swifi {

    class fd_noise_adder_impl : public fd_noise_adder
    {
     private:
      pmt::pmt_t d_phy_tag_key;
      std::vector<int> d_subcarrier_jamming_pattern;
      std::vector<int> d_ofdmsym_jamming_pattern;
      gr::random d_rng;
      float d_ampl;
      int d_idx;

     public:
      fd_noise_adder_impl(
        const std::string &phy_tag_key,
        const std::vector<int> &subcarrier_jamming_pattern,
        const std::vector<int> &ofdmsym_jamming_pattern,
        float ampl = 1.0f);
      ~fd_noise_adder_impl();

      // Where all the action really happens
      int work(int noutput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_FD_NOISE_ADDER_IMPL_H */

