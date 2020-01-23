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

#include "tx_time_tag_adder_impl.h"
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <stdio.h>

namespace gr {
  namespace swifi {

    tx_time_tag_adder::sptr
    tx_time_tag_adder::make(const uint64_t full_secs,
                            const double frac_secs,
                            const std::string &length_tag_key,
                            const long length)
    {
      return gnuradio::get_initial_sptr
        (new tx_time_tag_adder_impl(full_secs, frac_secs, length_tag_key, length));
    }

    /*
     * The private constructor
     */
    tx_time_tag_adder_impl::tx_time_tag_adder_impl(const uint64_t full_secs,
                                                  const double frac_secs,
                                                  const std::string &length_tag_key,
                                                  const long length)
      : gr::sync_block("tx_time_tag_adder",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_tx_time_tag_key(pmt::string_to_symbol("tx_time")),
        d_time_stamp(1),
        d_length_tag_key(pmt::string_to_symbol(length_tag_key)),
        d_length(length)
    {
      d_cycle_time = full_secs + frac_secs;
      // set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    tx_time_tag_adder_impl::~tx_time_tag_adder_impl()
    {
    }

    int
    tx_time_tag_adder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex*) output_items[0];

      for (int i = 0; i < noutput_items; i++) {
        out[i] = in[i];
        if ((nitems_written(0)+i) % d_length == 0) {
          d_full_secs = (uint64_t)floor(d_time_stamp);
          d_frac_secs = d_time_stamp - d_full_secs;
          d_tx_time = pmt::make_tuple(pmt::from_uint64(d_full_secs),
                                    pmt::from_double(d_frac_secs));
          add_item_tag(0, nitems_written(0)+i, d_tx_time_tag_key, d_tx_time, pmt::PMT_F);
          add_item_tag(0, nitems_written(0)+i, d_length_tag_key, pmt::from_long(d_length), pmt::PMT_F);

          d_time_stamp += d_cycle_time;
        }
      }

      return noutput_items;



    }

  } /* namespace swifi */
} /* namespace gr */

