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

#include "bitmetrics_interleaver_impl.h"
#include <swifi/tag_utils.h>
#include <swifi/phy_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <stdio.h>

namespace gr {
  namespace swifi {

    bitmetrics_interleaver::sptr
    bitmetrics_interleaver::make(bool is_interleaver, const std::string &phy_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new bitmetrics_interleaver_impl(is_interleaver, phy_tag_key));
    }

    bitmetrics_interleaver::sptr
    bitmetrics_interleaver::make(bool is_interleaver, int rate)
    {
      return gnuradio::get_initial_sptr
        (new bitmetrics_interleaver_impl(is_interleaver, rate));
    }

    /*
     * The private constructor
     */
    bitmetrics_interleaver_impl::bitmetrics_interleaver_impl(bool is_interleaver,
        const std::string &phy_tag_key)
      : dynamic_rate_block("interleaver",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(float)),
              phy_tag_key),
        d_is_interleaver(is_interleaver)
    {
      set_interleaving_map(phy_utils::BPSK_1_2);  // default interleaving map
    }

    bitmetrics_interleaver_impl::bitmetrics_interleaver_impl(bool is_interleaver, int rate)
      : dynamic_rate_block("interleaver",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(float)),
              ""),
        d_is_interleaver(is_interleaver)
    {
      set_interleaving_map(rate);
    }

    void bitmetrics_interleaver_impl::set_interleaving_map(int rate) {
      d_mapping = d_is_interleaver ? phy_utils::INTL_TABLE[rate] :
        d_mapping = phy_utils::DEINTL_TABLE[rate];
      if (d_mapping.size() == 0)
        throw std::runtime_error("Interleaving map cannot be set for invalid rate\n");
      set_output_multiple(d_mapping.size() * 2);
    }

    void bitmetrics_interleaver_impl::update_rate(const tag_t &tag) {
      phy_tag t;
      if (phy_tag::parse(tag.value, t)) {
        set_interleaving_map(t.rate());
      }
    }

    int
    bitmetrics_interleaver_impl::dynamic_rate_work(int noutput_items, int ninput_items,
        const void* input, void* output)
    {
      const float *in = (const float *)input;
      float *out = (float*)output;
      int n, s, m;
      int *mapping;

      mapping = d_mapping.data(); // get direct pointer to mapping table
      s = d_mapping.size();

      // process a multiple of interleaving table size of items
      n = noutput_items / output_multiple(); // num groups for interleaving
      for (int j = 0; j < n; j++) {
        for (int i = 0; i < s; i++) {
          memcpy(out + mapping[i] * 2, in + i * 2, 2 * sizeof(float));
        }
        out += output_multiple();
        in += output_multiple();
      }

      return n * output_multiple();
    }

  } /* namespace swifi */
} /* namespace gr */

