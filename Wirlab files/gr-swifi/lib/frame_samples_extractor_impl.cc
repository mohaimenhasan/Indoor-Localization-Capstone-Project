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

#include "frame_samples_extractor_impl.h"
#include "xml_reader.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <stdio.h>

namespace gr {
  namespace swifi {

    frame_samples_extractor::sptr
    frame_samples_extractor::make(const std::string &xml_dump_file,
      const std::string &filter_string,
      const std::string &start_expr,
      const std::string &end_expr)
    {
      return gnuradio::get_initial_sptr
        (new frame_samples_extractor_impl(xml_dump_file, filter_string, 
          start_expr, end_expr));
    }


    /*
     * The private constructor
     */
    frame_samples_extractor_impl::frame_samples_extractor_impl(const std::string &xml_dump_file,
          const std::string &filter_string,
          const std::string &start_expr,
          const std::string &end_expr)
      : gr::block("frame_samples_extractor",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_frames(XMLNode("allframes"))
    {
      // check file existence
      FILE *f = fopen(xml_dump_file.c_str(), "r");
      if (!f) {
        throw std::runtime_error("Cannot read XML dump file\n");
      }
      fclose(f);

      // read frames as XML nodes (might take time if file is big)
      d_frames = XMLNode(XMLReader::read(xml_dump_file, "allframes"));
      d_frame_idx = -1;
      d_start_sample = d_end_sample = -1;

      // create frame filter and start/end evaluators
      d_filter.reset(new simple_filter(filter_string));
      d_start_evaluator.reset(new simple_filter(start_expr));
      d_end_evaluator.reset(new simple_filter(end_expr));
    }


    /*
     * Our virtual destructor.
     */
    frame_samples_extractor_impl::~frame_samples_extractor_impl()
    {
    }

    // void frame_samples_extractor_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    // {
    //   int ideal_noutput_items = ceil(1.0 * noutput_items / d_blocksize);
    //   int nreqd = ideal_noutput_items * 2 - d_nitems_stored;
    //   ninput_items_required[0] = nreqd;
    // }

    int
    frame_samples_extractor_impl::general_work(int noutput_items,
        gr_vector_int& ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex*) output_items[0];
      int n;

      if (d_frame_idx >= d_frames.num_children()) {
        return -1;  // end flowgraph
      }
      else if (d_start_sample > (long)nitems_read(0)) {
        // this case happens when the first sample of the requested frame
        // is behind the current input sample, so we just consume input until
        // the first sample of requested frame
        n = std::min(int(d_start_sample - nitems_read(0)), ninput_items[0]);
        consume(0, n);
      }
      else {
        if (d_end_sample > (long)nitems_read(0)) {
          // this case happens when we are in the middle of copying
          // samples for the current requested frame
          n = std::min(int(d_end_sample - nitems_read(0)), noutput_items);
          n = std::min(n, ninput_items[0]);
          memcpy(out, in, sizeof(gr_complex) * n);
          consume(0, n);
          produce(0, n);
        }
        else {
          // this case happens when we just finish copying the requested frame,
          // so we proceed to find the next frame that meets the filter condition
          d_frame_idx++;
          while (d_frame_idx < d_frames.num_children()) {
            XMLNode &frame = d_frames.get_child(d_frame_idx);

            if (d_filter->filter(frame)) {
              std::string value;

              // get start position
              d_start_sample = atol(d_start_evaluator->evaluate(frame).c_str());

              // get end position
              d_end_sample = atol(d_end_evaluator->evaluate(frame).c_str());

              if (d_end_sample <= d_start_sample) {
                // something wrong
                d_start_sample = d_end_sample = -1;
              }
              else break; // valid sample positions
            } // end if

            d_frame_idx++;
          } // end while
        }
      }

      return WORK_CALLED_PRODUCE;
    }

  } /* namespace swifi */
} /* namespace gr */

