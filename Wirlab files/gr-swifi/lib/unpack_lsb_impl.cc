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

#include "unpack_lsb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace swifi {

    unpack_lsb::sptr
    unpack_lsb::make(int bits_per_input_item)
    {
      return gnuradio::get_initial_sptr
        (new unpack_lsb_impl(bits_per_input_item));
    }

    /*
     * The private constructor
     */
    unpack_lsb_impl::unpack_lsb_impl(int bits_per_input_item)
      : gr::sync_interpolator("unpack_lsb",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)),
              bits_per_input_item),
        d_bits(bits_per_input_item)
    {
    }

    /*
     * Our virtual destructor.
     */
    unpack_lsb_impl::~unpack_lsb_impl()
    {
    }

    int
    unpack_lsb_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char*) output_items[0];
      int ninputs = noutput_items / d_bits;
      char s;

      // unpack in LSB order
      switch (d_bits) {
        case 1:
          memcpy(out, in, ninputs);
          break;
        case 2:
          for (int i = 0; i < ninputs; i++) {
            s = *in++;
            *out++ = s & 1; s >>= 1;
            *out++ = s;
          }
          break;
        case 4:
          for (int i = 0; i < ninputs; i++) {
            s = *in++;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s;
          }
          break;
        case 6:
          for (int i = 0; i < ninputs; i++) {
            s = *in++;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s;
          }
          break;
        case 8:
          for (int i = 0; i < ninputs; i++) {
            s = *in++;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s & 1; s >>= 1;
            *out++ = s;
          }
          break;
      }

      return ninputs * d_bits;
    }

  } /* namespace swifi */
} /* namespace gr */

