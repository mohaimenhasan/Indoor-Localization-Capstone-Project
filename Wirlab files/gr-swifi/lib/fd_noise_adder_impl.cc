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

#include "fd_noise_adder_impl.h"
#include <swifi/tag_utils.h>
#include <swifi/phy_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <stdio.h>

namespace gr {
  namespace swifi {

    fd_noise_adder::sptr
    fd_noise_adder::make(
      const std::string &phy_tag_key,
      const std::vector<int> &subcarrier_jamming_pattern,
      const std::vector<int> &ofdmsym_jamming_pattern,
      float ampl)
    {
      return gnuradio::get_initial_sptr(new fd_noise_adder_impl(
        phy_tag_key,
        subcarrier_jamming_pattern,
        ofdmsym_jamming_pattern,
        ampl));
    }

    /*
     * The private constructor
     */
    fd_noise_adder_impl::fd_noise_adder_impl(
          const std::string &phy_tag_key,
          const std::vector<int> &subcarrier_jamming_pattern,
          const std::vector<int> &ofdmsym_jamming_pattern,
          float ampl)
      : gr::sync_block("fd_noise_adder",
              gr::io_signature::make(1, 1, sizeof(gr_complex) * subcarrier_jamming_pattern.size()),
              gr::io_signature::make(1, 2, sizeof(gr_complex) * subcarrier_jamming_pattern.size())),
        d_phy_tag_key(pmt::intern(phy_tag_key)),
        d_subcarrier_jamming_pattern(subcarrier_jamming_pattern),
        d_ofdmsym_jamming_pattern(ofdmsym_jamming_pattern),
        d_ampl(ampl),
        d_idx(0)
    {
    }

    fd_noise_adder_impl::~fd_noise_adder_impl()
    {
    }

    int
    fd_noise_adder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*) input_items[0];
      gr_complex *out = (gr_complex*) output_items[0];
      gr_complex *out_noise = NULL;
      size_t N = d_subcarrier_jamming_pattern.size();
      size_t S = d_ofdmsym_jamming_pattern.size();
      std::vector<tag_t> phy_tags;

      // there is output for noise?
      if (output_items.size() >= 2)
        out_noise = (gr_complex*)output_items[1];

      // get PHY tags
      get_tags_in_window(phy_tags, 0, 0, noutput_items, d_phy_tag_key);
      std::sort(phy_tags.begin(), phy_tags.end(), tag_t::offset_compare);
      tag_range tr(nitems_read(0), noutput_items, phy_tags);

      // process each frame
      for (int k = 0; k < tr.size(); k++) {
        phy_tag t;
        if (phy_tag::parse(tr.tag(k).value, t)) {
          d_idx = 0;
        }

        for (int n = 0; n < tr.range_nitems(k); n++) {
          memcpy(out, in, sizeof(gr_complex) * N);

          if (out_noise)
            memset(out_noise, 0, sizeof(gr_complex) * N);

          // if this OFDM symbol is to be jammed
          if (d_idx < S && d_ofdmsym_jamming_pattern[d_idx]) {
            for (int i = 0; i < N; i++) {
              if (d_subcarrier_jamming_pattern[i]) {
                // generate AWGN
                gr_complex noise = d_ampl * d_rng.rayleigh_complex();
                out[i] = in[i] + noise;

                if (out_noise)
                  out_noise[i] = noise;
              }
            }
          }

          in += N;
          out += N;
          if (out_noise)
            out_noise += N;

          d_idx++;
        }
      }

      return noutput_items;
    }

  } /* namespace swifi */
} /* namespace gr */

