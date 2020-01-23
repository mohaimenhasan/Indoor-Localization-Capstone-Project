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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "bit_stealer_impl.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <iostream>
#include <stdio.h>

namespace gr {
  namespace swifi {

    bit_stealer::sptr
    bit_stealer::make(const std::string &phy_tag_key)
    {
      return gnuradio::get_initial_sptr(
        new bit_stealer_impl(phy_tag_key));
    }

    bit_stealer::sptr
    bit_stealer::make(int rate)
    {
      return gnuradio::get_initial_sptr(
        new bit_stealer_impl(rate));
    }

    bit_stealer_impl::bit_stealer_impl(const std::string &phy_tag_key)
    : dynamic_rate_block("bit_stealer",
            io_signature::make(1, 1, sizeof(char)),
            io_signature::make(1, 1, sizeof(char)),
            phy_tag_key)
    {
      update_puncture_map(phy_utils::BPSK_1_2);
    }

    bit_stealer_impl::bit_stealer_impl(int rate)
    : dynamic_rate_block("bit_stealer",
            io_signature::make(1, 1, sizeof(char)),
            io_signature::make(1, 1, sizeof(char)),
            "")
    {
      update_puncture_map(rate);
    }

    void bit_stealer_impl::update_puncture_map(int rate) {
      d_puncture_map = phy_utils::create_puncture_map(rate);
      d_num_ones = 0; // used for calculing relative output/input rate
      for (int i = 0; i < d_puncture_map.size(); i++)
        if (d_puncture_map[i]) d_num_ones++;
      set_output_multiple(d_num_ones);
      set_relative_rate(1.0 * d_num_ones / d_puncture_map.size());
    }

    void bit_stealer_impl::update_rate(const tag_t &tag) {
      phy_tag t;
      if (phy_tag::parse(tag.value, t)) {
        update_puncture_map(t.rate());
      }
    }

    int
    bit_stealer_impl::dynamic_rate_work(int noutput_items, int ninput_items,
        const void* input, void* output)
    {
      const char *in = (const char*)input;
      char *out = (char *)output;
      int m, l;
      int *punc_map;

      // preparation
      punc_map = d_puncture_map.data(); // local pointer for faster access
      m = d_puncture_map.size();  // number of bits per punctured group
      l = noutput_items / d_num_ones; // number of groups of bits

      // stealing
      if (m > 1) {
        for (int i = 0; i < l; i++) {
          for (int j = 0; j < m; j++) {
            if (punc_map[j])
              *out++ = *in++;
            else
              in++;
          }
        }
      }
      // no stealing
      else {
        memcpy(out, in, noutput_items);
      }

      return noutput_items;
    }

  } /* namespace trellis */
} /* namespace gr */
