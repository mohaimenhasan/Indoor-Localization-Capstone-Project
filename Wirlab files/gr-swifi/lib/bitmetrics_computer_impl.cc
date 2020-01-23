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

#include "bitmetrics_computer_impl.h"
#include <swifi/tag_utils.h>
#include <swifi/phy_utils.h>
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    bitmetrics_computer::sptr
    bitmetrics_computer::make(const std::string& phy_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new bitmetrics_computer_impl(phy_tag_key));
    }

    bitmetrics_computer::sptr
    bitmetrics_computer::make(digital::constellation_sptr constellation)
    {
      return gnuradio::get_initial_sptr
        (new bitmetrics_computer_impl(constellation));
    }

    bitmetrics_computer_impl::
    bitmetrics_computer_impl(const std::string& phy_tag_key)
      : dynamic_rate_block("bitmetrics_computer",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(float) /* metric values are floats */),
                 phy_tag_key)
    {
      // set default constellation
      set_constellation(phy_utils::BPSK_1_2);
    }

    bitmetrics_computer_impl::
    bitmetrics_computer_impl(digital::constellation_sptr constellation)
      : dynamic_rate_block("bitmetrics_computer",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(float)),
                 ""),
        d_constellation(constellation)
    {
      set_constellation(constellation);
    }

    void bitmetrics_computer_impl::update_settings() {
      if (!d_constellation)
        throw std::runtime_error("The specified rate is not supported");

      d_bpsc = d_constellation->bits_per_symbol();
      set_output_multiple(d_bpsc * 2); // each bit produces 2 metric values, one for 0 and one for 1
      set_relative_rate(d_bpsc * 2);

      d_metrics = boost::dynamic_pointer_cast<constellation_metrics>(d_constellation);
      if (!d_metrics)
        throw std::runtime_error("The given constellation object does not support metrics computation");
    }

    void bitmetrics_computer_impl::set_constellation(int rate) {
      d_constellation = phy_utils::WIFI_CONSTELLATIONS[rate];
      update_settings();
    }

    void bitmetrics_computer_impl::set_constellation(digital::constellation_sptr c) {
      d_constellation = c;
      update_settings();
    }

    void bitmetrics_computer_impl::update_rate(const tag_t &tag) {
      phy_tag t;
      if (phy_tag::parse(tag.value, t)) {
        set_constellation(t.rate());
      }
    }

    int
    bitmetrics_computer_impl::dynamic_rate_work(int noutput_items,
        int ninput_items, const void* input, void* output)
    {
      gr_complex const *in = (const gr_complex*)input;
      float *out = (float*)output;
      std::vector<tag_t> tags;

      int m = d_bpsc * 2; // number of metric values to produce per input sample
      int n = noutput_items / m; // number of input samples to process
      for (int i = 0; i < n; i++, out += m) {
        d_metrics->bitmetrics(&in[i], out);
      }

      return n * m;
    }

  } /* namespace swifi */
} /* namespace gr */
