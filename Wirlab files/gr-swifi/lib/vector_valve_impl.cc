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

#include "vector_valve_impl.h"
#include <swifi/tag_utils.h>
#include <swifi/phy_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <stdio.h>

namespace gr {
  namespace swifi {

    vector_valve::sptr
    vector_valve::make(int itemsize, const std::vector<int> &valve_map)
    {
      return gnuradio::get_initial_sptr(new vector_valve_impl(itemsize, valve_map));
    }

    /*
     * The private constructor
     */
    vector_valve_impl::vector_valve_impl(int itemsize, const std::vector<int> &valve_map)
      : gr::sync_block("vector_valve",
              gr::io_signature::make(1, 1, itemsize * valve_map.size()),
              gr::io_signature::make(1, 1, itemsize * valve_map.size())),
        d_itemsize(itemsize),
        d_valve_map(valve_map)
    {
    }

    vector_valve_impl::~vector_valve_impl()
    {
    }

    int
    vector_valve_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char*) output_items[0];
      size_t N = d_valve_map.size();

      memcpy(out, in, N * noutput_items * d_itemsize);
      for (int k = 0; k < N; k++) {
        if (!d_valve_map[k]) {
          for (int i = 0; i < noutput_items; i++) {
            memset(&out[(i * N + k) * d_itemsize], 0, d_itemsize);
          }
        }
      }

      return noutput_items;
    }

  } /* namespace swifi */
} /* namespace gr */

