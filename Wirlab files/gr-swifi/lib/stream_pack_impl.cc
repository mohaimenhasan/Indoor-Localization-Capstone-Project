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

#include <gnuradio/io_signature.h>
#include "stream_pack_impl.h"

namespace gr {
  namespace swifi {

    stream_pack::sptr
    stream_pack::make(const std::vector<int>& itembitsize)
    {
      return gnuradio::get_initial_sptr
        (new stream_pack_impl(itembitsize));
    }

    /*
     * The private constructor
     */
    stream_pack_impl::stream_pack_impl(const std::vector<int>& itembitsize)
      : gr::sync_block("stream_pack",
              gr::io_signature::make(itembitsize.size(), itembitsize.size(), sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
      d_itembitsize(itembitsize)
    {

    }

    /*
     * Our virtual destructor.
     */
    stream_pack_impl::~stream_pack_impl()
    {
    }

    int
    stream_pack_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char*) output_items[0];
      int m;

      memcpy(out, in, sizeof(char) * noutput_items);
      for (int k = 1; k < d_itembitsize.size(); k++) {
        m = d_itembitsize[k-1];
        in = (const char*)input_items[k];
        for (int i = 0; i < noutput_items; i++) {
          out[i] |= in[i] << m;
        }
      }
      return noutput_items;
    }

  } /* namespace swifi */
} /* namespace gr */

