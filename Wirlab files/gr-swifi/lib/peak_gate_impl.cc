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

#include "peak_gate_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace swifi {

#define NOT_SET     -100000

    peak_gate::sptr
    peak_gate::make(float threshold, int num_peaks, 
      int min_distance, int max_distance)
    {
      return gnuradio::get_initial_sptr
        (new peak_gate_impl(threshold, num_peaks, 
          min_distance, max_distance));
    }

    /*
     * The private constructor
     */
    peak_gate_impl::peak_gate_impl(float threshold, int num_peaks,
      int min_distance, int max_distance)
      : gr::sync_block("peak_gate",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(char))),
      d_threshold(threshold), d_num_peaks(num_peaks),
      d_min_distance(min_distance), d_max_distance(max_distance),
      d_peak_count(0), d_last_peak_idx(NOT_SET)
    {

    }

    /*
     * Our virtual destructor.
     */
    peak_gate_impl::~peak_gate_impl()
    {
    }

    int
    peak_gate_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];
      char *out = (char*) output_items[0];
      long delta;
      memset(out, 0, noutput_items * sizeof(char));

      for (int i = 0; i < noutput_items; i++) {
        if (in[i] > d_threshold) {
          delta = nitems_read(0) + i - d_last_peak_idx;
          if (delta > d_max_distance)
            d_peak_count = 0; // restart counting from this current peak
          else if (delta < d_min_distance)
            continue; // don't count this too-close peak

          d_last_peak_idx += delta; // update position of the current peak
          d_peak_count++;

          // found enough peaks
          if (d_peak_count >= d_num_peaks) {
            out[i] = 1;
            d_peak_count = 0;
            d_last_peak_idx = NOT_SET;
          }
        }
      }
      return noutput_items;
    }

  } /* namespace swifi */
} /* namespace gr */

