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

#include "phy_frame_generator_impl.h"
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <stdio.h>

namespace gr {
  namespace swifi {

    phy_frame_generator::sptr
    phy_frame_generator::make(const std::string &phy_tag_key,
      const std::string &len_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new phy_frame_generator_impl(phy_tag_key, len_tag_key));
    }

    /*
     * The private constructor
     */
    phy_frame_generator_impl::phy_frame_generator_impl(
        const std::string &phy_tag_key,
        const std::string &len_tag_key)
      : gr::block("phy_frame_generator",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
        d_phy_tag_key(pmt::intern(phy_tag_key)),
        d_len_tag_key(pmt::intern(len_tag_key)),
        d_state(GENERATE_HEADER),
        d_nitems_to_copy(1), d_nitems_copied(0)
    {
      set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    phy_frame_generator_impl::~phy_frame_generator_impl()
    {
    }

    void phy_frame_generator_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      int nreqd;
      switch (d_state) {
        case GENERATE_HEADER:
          nreqd = d_nitems_copied == 0 ? 1 : 0;// noutput_items;
          break;
        case COPY_DATA:
          nreqd = std::min(d_nitems_to_copy - d_nitems_copied, noutput_items);//noutput_items;
          break;
        case GENERATE_TAIL_PAD:
          nreqd = 0;
          break;
      }
      for (unsigned i = 0; i < ninput_items_required.size(); i++)
        ninput_items_required[i] = nreqd;
    }

    int
    phy_frame_generator_impl::general_work(int noutput_items,
        gr_vector_int& ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char*) output_items[0];
      std::vector<tag_t> tags;
      int consumed = 0;
      int produced = 0;
      int n, k;
      phy_tag t;

      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + noutput_items,
        d_phy_tag_key);
      std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
      k = 0;  // tag index

      while (produced < noutput_items) {
        switch (d_state) {
          // copy 5 bytes of PHY header: SIGNAL(3 bytes) + SERVICE(2 bytes)
          case GENERATE_HEADER:
            // d_nitems_copied == 0 means the beginning of the header
            if (d_nitems_copied == 0) {
              // a valid tag must be found on input stream at the beginning of the header
              if (k < tags.size() && tags[k].offset == nitems_read(0) + consumed) {
                if (!phy_tag::parse(tags[k].value, t))
                  throw std::runtime_error("Invalid PHY tag\n");
                d_added = false;
                d_rate = t.rate();
                d_length = t.nitems();
                phy_utils::create_header(d_rate, d_length, d_header);
                // d_nitems_to_copy = sizeof(d_header);
                d_nitems_to_copy = 3;
                k++;

                // add PHY tag for processing PHY header (the first 3 bytes)
                // - scrambling: no
                add_item_tag(0, nitems_written(0) + produced, d_phy_tag_key,
                  phy_tag::create()
                    .set_nitems(phy_utils::compute_uncoded_header_no_service_size())
                    .set_rate(phy_utils::BPSK_1_2)
                    .set_flag(PHY_HEADER_FLAG));

                // add length tag: contains total number of symbols required for the whole
                // PHY frame, used by OFDM allocator and cyclic prefixer block
                add_item_tag(0, nitems_written(0) + produced, d_len_tag_key,
                  pmt::from_long(phy_utils::compute_n_syms(d_rate, d_length) + 48));
                // printf("add len tag %lu\n", nitems_written(0) + produced);
              }
              else {
                goto endwhile; // No SIGNAL tag found
              }
            }
            n = std::min(d_nitems_to_copy - d_nitems_copied, noutput_items - produced);
            memcpy(out + produced, ((char*)&d_header) + d_nitems_copied, n);
            // if (d_nitems_copied + n > 3) {
            //   if (!d_added) {
            //     // add PHY tag for processing PHY payload (including SERVICE and tail padding fields)
            //     // + scrambling: yes
            //     add_item_tag(0, nitems_written(0) + produced - d_nitems_copied + 3,
            //       d_phy_tag_key,
            //       phy_tag_create(phy_utils::compute_uncoded_service_payload_tail_pad_size(d_rate, d_length),
            //         d_rate, PHY_SCRAMBLER_BIT));
            //     d_added = true;
            //   }
            // }
            break;

          case GENERATE_SERVICE:
            if (d_nitems_copied == 0) {
              d_nitems_to_copy = 2;
              add_item_tag(0, nitems_written(0) + produced, d_phy_tag_key,
                phy_tag::create()
                  .set_nitems(d_length)
                  .set_rate(d_rate)
                  .set_flag(PHY_SCRAMBLER_FLAG | PHY_TAIL_SET_FLAG));
            }
            n = std::min(d_nitems_to_copy - d_nitems_copied, noutput_items - produced);
            memcpy(out + produced, ((char*)&d_header) + 3 + d_nitems_copied, n);
            break;

          // copy data payload (what received from MAC)
          case COPY_DATA:
            if (d_nitems_copied == 0)
              d_nitems_to_copy = d_length;
            n = std::min(std::min(d_nitems_to_copy - d_nitems_copied, noutput_items - produced),
              ninput_items[0] - consumed);
            if (n == 0) // not enough input items --> exit general_work
              goto endwhile;

            memcpy(out + produced, in, n);
            in += n;
            consumed += n;
            break;

          // copy tail plus padding bytes
          case GENERATE_TAIL_PAD:
            if (d_nitems_copied == 0)
              d_nitems_to_copy = phy_utils::compute_uncoded_tail_pad_size(d_rate, d_length);
            n = std::min(d_nitems_to_copy - d_nitems_copied, noutput_items - produced);
            memset(out + produced, 0, n);
            break;
        }

        d_nitems_copied += n;
        produced += n;

        // go to next part
        if (d_nitems_copied >= d_nitems_to_copy) {
          d_state = (d_state + 1) % NUM_STATES;
          d_nitems_copied = 0;  // marking the beginning of each part
        }
      }

      endwhile:

      consume(0, consumed);
      return produced;
    }

  } /* namespace swifi */
} /* namespace gr */

