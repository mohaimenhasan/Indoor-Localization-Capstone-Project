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

#include "sample_adder_impl.h"
#include <swifi/tag_utils.h>
#include <swifi/phy_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <stdio.h>

namespace gr {
  namespace swifi {

    sample_adder::sptr
    sample_adder::make(const uint64_t packet_length,
                      const uint8_t rate,
                      const int num_zeros)
    {
      return gnuradio::get_initial_sptr
        (new sample_adder_impl(packet_length, rate, num_zeros));
    }


    /*
     * The private constructor
     */
    sample_adder_impl::sample_adder_impl(const uint64_t packet_length,
                                        const uint8_t rate,
                                        const int num_zeros)
      : gr::block("sample_adder",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_num_zeros(num_zeros),
        d_samps(num_zeros),
        d_state(ADD_FRONT)
    {
      if (rate == 0) {
        d_length = packet_length;
      }
      else {
        d_length = (phy_utils::compute_n_ofdm_syms(rate, packet_length + 28) + 5) * 80;
      }
    }


    /*
     * Our virtual destructor.
     */
    sample_adder_impl::~sample_adder_impl()
    {
    }

    void sample_adder_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required) {
      for (unsigned i = 0; i < ninput_items_required.size(); i++) {
        ninput_items_required[i] = noutput_items;
      }
    }


    int
    sample_adder_impl::general_work(int noutput_items,
        gr_vector_int& ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex*) output_items[0];

      int n = ninput_items[0];
      int produced = 0, consumed = 0;
      int m;
      
      while (noutput_items > 0 && n > 0) {
        switch (d_state) {
          case ADD_FRONT:
            {
              m = std::min(d_samps, noutput_items);
              for (int k = 0; k < m; k++) {
                out[produced++] = gr_complex(0.0,0);
              }
            }
            break;
          case COPY:
            {
              m = std::min(d_samps, noutput_items);
              m = std::min(m, n);
              memcpy(&out[produced], &in[consumed], m * sizeof(gr_complex));
              produced += m;
              consumed += m;
              n -= m;
            }
            break;
          case ADD_TAIL:
            {
              m = std::min(d_samps, noutput_items);
              for (int k = 0; k < m; k++) {
                out[produced++] = gr_complex(0.0,0);
              }
            }
        }

        noutput_items -= m;
        d_samps -= m;
        if (d_samps == 0) {
          if (d_state == ADD_FRONT) d_samps = d_length;
          // else d_samps = (rand() % 10 + 1)*d_num_zeros; // add random number of d_num_zeros
          else d_samps = d_num_zeros;

          d_state = (d_state + 1) % 3;
        }
      }

      consume(0, consumed);
      return produced;
    }

  } /* namespace swifi */
} /* namespace gr */

