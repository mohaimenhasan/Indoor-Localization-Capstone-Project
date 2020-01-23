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

#include "chunks_to_symbols_impl.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
  namespace swifi {

    chunks_to_symbols::sptr
    chunks_to_symbols::make(const std::string &phy_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new chunks_to_symbols_impl(phy_tag_key));
    }

    chunks_to_symbols::sptr
    chunks_to_symbols::make(const std::vector<gr_complex> &points)
    {
      return gnuradio::get_initial_sptr
        (new chunks_to_symbols_impl(points));
    }

    /*
     * The private constructor
     */
    chunks_to_symbols_impl::chunks_to_symbols_impl(
        const std::string &phy_tag_key)
      : dynamic_rate_block("chunks_to_symbols",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              phy_tag_key)
    {
      // default constellation points
      d_points = std::vector<gr_complex>(2);
      d_points[0] = gr_complex(-1, 0);
      d_points[1] = gr_complex(1, 0);
    }

    chunks_to_symbols_impl::chunks_to_symbols_impl(
        const std::vector<gr_complex> &points)
      : dynamic_rate_block("chunks_to_symbols",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              ""),
        d_points(points)
    {
      if (points.empty()) {
        throw std::runtime_error("Constellation shared pointer must be specified in normal mode");
      }
    }

    void chunks_to_symbols_impl::update_rate(const tag_t &tag) {
      phy_tag t;
      if (phy_tag::parse(tag.value, t)) {
        if (phy_utils::WIFI_CONSTELLATIONS[t.rate()]) {
          // change rate
          digital::constellation_sptr c = phy_utils::WIFI_CONSTELLATIONS[t.rate()];
          d_points = c->points();
        }
        else {
          throw std::runtime_error("Specified RATE value invalid\n");
        }
      }
    }

    int
    chunks_to_symbols_impl::dynamic_rate_work(int noutput_items,
        int ninput_items, const void* input, void* output)
    {
      const char *in = (const char *) input;
      gr_complex *out = (gr_complex*) output;

      // mapping
      for (int i = 0; i < noutput_items; i++)
        out[i] = d_points[in[i]];

      return noutput_items;
    }

  } /* namespace swifi */
} /* namespace gr */

