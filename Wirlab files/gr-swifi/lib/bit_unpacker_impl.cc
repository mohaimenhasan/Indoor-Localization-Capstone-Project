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

#include "bit_unpacker_impl.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    bit_unpacker::sptr
    bit_unpacker::make(const std::string &phy_tag_key, int bpsc)
    {
      return gnuradio::get_initial_sptr
        (new bit_unpacker_impl(phy_tag_key, bpsc));
    }

    /*
     * The private constructor
     */
    bit_unpacker_impl::bit_unpacker_impl(
        const std::string &phy_tag_key, int bpsc)
      : dynamic_rate_block("bit_unpacker",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)),
              phy_tag_key),
        d_bpsc(bpsc)
    {
      if (d_bpsc < 1)
        d_bpsc = 1;
      set_relative_rate(d_bpsc);
      set_output_multiple(d_bpsc);
    }

    void bit_unpacker_impl::update_rate(const tag_t &tag) {
      phy_tag t;
      if (phy_tag::parse(tag.value, t)) {
        d_bpsc = phy_utils::N_BPSC[t.rate()]; // change number of bits per subcarrier
        set_relative_rate(d_bpsc);
        set_output_multiple(d_bpsc);
      }
    }

    int
    bit_unpacker_impl::dynamic_rate_work(int noutput_items, int ninput_items,
        const void* input, void* output)
    {
      const char *in = (const char *)input;
      char *out = (char*)output;
      int n = noutput_items / d_bpsc; // number of input symbols required to produce noutput_items
      char a;

      // packing bits to symbols
      switch (d_bpsc) {
        case 1: // BPSK
          memcpy(out, in, n);
          out += n;
          in += n;
          break;
        case 2: // QPSK
          for (int i = 0; i < n; i++) {
            a = *in++;
            *out++ = a & 1; a >>= 1;
            *out++ = a & 1;
          }
          break;
        case 4: // 16QAM
          for (int i = 0; i < n; i++) {
            a = *in++;
            *out++ = a & 1; a >>= 1;
            *out++ = a & 1; a >>= 1;
            *out++ = a & 1; a >>= 1;
            *out++ = a & 1;
          }
          break;
        case 6: // 64QAM
          for (int i = 0; i < n; i++) {
            a = *in++;
            *out++ = a & 1; a >>= 1;
            *out++ = a & 1; a >>= 1;
            *out++ = a & 1; a >>= 1;
            *out++ = a & 1; a >>= 1;
            *out++ = a & 1; a >>= 1;
            *out++ = a & 1;
          }
          break;
      }

      return n * d_bpsc;
    }

  } /* namespace swifi */
} /* namespace gr */

