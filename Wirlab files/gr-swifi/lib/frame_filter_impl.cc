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

#include "frame_filter_impl.h"
#include "xml_reader.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <stdio.h>

namespace gr {
  namespace swifi {

    frame_filter::sptr
    frame_filter::make(const std::string &xml_dump_file,
      const std::string &filter_string,
      const std::string &start_sample_tagattrname,
      const std::string &end_sample_tagattrname,
      const std::string &phy_tag_key,
      const std::string &xml_out_file)
    {
      return gnuradio::get_initial_sptr
        (new frame_filter_impl(xml_dump_file, filter_string, 
          start_sample_tagattrname, end_sample_tagattrname,
          phy_tag_key, xml_out_file));
    }


    /*
     * The private constructor
     */
    frame_filter_impl::frame_filter_impl(const std::string &xml_dump_file,
          const std::string &filter_string,
          const std::string &start_sample_tagattrname,
          const std::string &end_sample_tagattrname,
          const std::string &phy_tag_key,
          const std::string &xml_out_file)
      : gr::block("frame_filter",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_start_sample_tagattrname(start_sample_tagattrname),
        d_end_sample_tagattrname(end_sample_tagattrname),
        d_phy_tag_key(pmt::intern(phy_tag_key)),
        d_frames(XMLNode("allframes"))
    {
      // check file existence
      FILE *f = fopen(xml_dump_file.c_str(), "r");
      if (!f) {
        throw std::runtime_error("Cannot read XML dump file\n");
      }
      fclose(f);

      // open XML output file
      if (!xml_out_file.empty())
        xml_out_f = fopen(xml_out_file.c_str(), "w");
      else
        xml_out_f = NULL;


      // read frames as XML nodes
      d_frames = XMLNode(XMLReader::read(xml_dump_file, "allframes"));
      d_frame_idx = -1;
      d_start_sample = d_end_sample = -1;

      // create frame filter
      d_filter.reset(new simple_filter(filter_string));
    }


    /*
     * Our virtual destructor.
     */
    frame_filter_impl::~frame_filter_impl()
    {
    }

    int
    frame_filter_impl::general_work(int noutput_items,
        gr_vector_int& ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex*) output_items[0];
      int n;

      if (d_frame_idx >= d_frames.num_children()) {
        // no more frames available, consume everything, ignore output
        consume(0, ninput_items[0]);
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

          if (d_start_sample == nitems_read(0)) {
            add_item_tag(0, nitems_written(0), d_phy_tag_key,
              phy_tag::create()
                .set_nitems(d_psdu_size)
                .set_rate(d_rate)
                .set_flag(PHY_SCRAMBLER_FLAG | PHY_TAIL_SET_FLAG)
                .set_rx_time(d_rx_time)
                .set_payload_sample_index(d_start_sample));
          }
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
              d_start_sample = d_end_sample = -1;
              bool ok = true;

              // get attribute payload_start
              ok = ok && frame.retrieve_grandchild_or_attr(value, d_start_sample_tagattrname);
              if (ok) d_start_sample = atol(value.c_str()); // the first sample of requested frame

              // get attribute payload_end
              if (ok) ok = ok && frame.retrieve_grandchild_or_attr(value, d_end_sample_tagattrname);
              if (ok) d_end_sample = atol(value.c_str()); // the last sample (exclusive) of requested frame
              ok = ok && (d_start_sample < d_end_sample);

              // get rate
              if (ok) ok = ok && frame.retrieve_grandchild_or_attr(value, "phy.rate@code");
              if (ok) ok = ok &&
                        ((bool)phy_utils::WIFI_CONSTELLATIONS[d_rate = atoi(value.c_str())]);

              // get psdu_size
              if (ok) ok = ok && frame.retrieve_grandchild_or_attr(value, "phy.mpdu_size");
              if (ok) ok = ok && ((d_psdu_size = atoi(value.c_str())) > 0);

              // get rx_time
              if (ok) ok = ok && frame.retrieve_grandchild_or_attr(value, "phy.sample@frame_start");
              if (ok) d_rx_time = atol(value.c_str());

              // verify the obtained attributes
              if (ok) {
                // dump to XML output
                if (xml_out_f) {
                  fprintf(xml_out_f, "%s", frame.str().c_str());
                }

                break; // break while
              }
            } // end if

            d_frame_idx++;
          } // end while
        }
      }

      return WORK_CALLED_PRODUCE;
    }

  } /* namespace swifi */
} /* namespace gr */

