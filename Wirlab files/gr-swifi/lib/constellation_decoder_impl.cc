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

#include "constellation_decoder_impl.h"
#include <swifi/tag_utils.h>
#include <swifi/phy_utils.h>
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    constellation_decoder::sptr
    constellation_decoder::make(const std::string& phy_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new constellation_decoder_impl(phy_tag_key));
    }

    constellation_decoder::sptr
    constellation_decoder::make(digital::constellation_sptr constellation)
    {
      return gnuradio::get_initial_sptr
        (new constellation_decoder_impl(constellation));
    }

    constellation_decoder_impl::
    constellation_decoder_impl(const std::string& phy_tag_key)
      : dynamic_rate_block("constellation_decoder",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(unsigned char)),
                 phy_tag_key)
    {
      // set default constellation
      d_constellation = phy_utils::WIFI_CONSTELLATIONS[phy_utils::BPSK_1_2];
    }

    constellation_decoder_impl::
    constellation_decoder_impl(digital::constellation_sptr constellation)
      : dynamic_rate_block("constellation_decoder",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(unsigned char)),
                 ""),
        d_constellation(constellation)
    {
    }

    void constellation_decoder_impl::update_rate(const tag_t &tag) {
      phy_tag t;
      if (phy_tag::parse(tag.value, t)) {
        digital::constellation_sptr c = phy_utils::WIFI_CONSTELLATIONS[t.rate()];
        if (c) d_constellation = c;
      }
    }

    int
    constellation_decoder_impl::dynamic_rate_work(int noutput_items,
        int ninput_items, const void* input, void* output)
    {
      gr_complex const *in = (const gr_complex*)input;
      unsigned char *out = (unsigned char*)output;

      for (int i = 0; i < noutput_items; i++) {
        out[i] = d_constellation->decision_maker(&in[i]);
      }

      return noutput_items;
    }

  } /* namespace swifi */
} /* namespace gr */
