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

#include "scrambler_impl.h"
#include <swifi/tag_utils.h>
#include <swifi/phy_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <stdio.h>

namespace gr {
  namespace swifi {

    scrambler::sptr
    scrambler::make(bool is_scrambler, const std::string &phy_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new scrambler_impl(is_scrambler, phy_tag_key));
    }

    /*
     * The private constructor
     */
    scrambler_impl::scrambler_impl(bool is_scrambler,
        const std::string &phy_tag_key)
      : gr::sync_block("scrambler",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
        d_phy_tag_key(pmt::intern(phy_tag_key)),
        d_is_scrambler(is_scrambler),
        d_shiftreg(0), d_mode(MODE_SCRAMBLE),
        d_require_seed(false),
        d_random_seed(false),
        d_fixed_seed(0b1011101) // this seed is taken from example in standard
    {
    }

    /*
     * Our virtual destructor.
     */
    scrambler_impl::~scrambler_impl()
    {
    }

    void scrambler_impl::set_seed(bool random, int fixed_seed) {
      d_random_seed = random;
      d_fixed_seed = fixed_seed;
    }

    void scrambler_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      int nreqd = noutput_items;
      if (!d_is_scrambler && d_require_seed && nreqd < 7) nreqd = 7;
      for (unsigned i = 0; i < ninput_items_required.size(); i++) {
        ninput_items_required[i] = nreqd;
      }
    }
    

    char scrambler_impl::getSeed(const char* data) {
      char seed = 0;
      seed |= (data[6]^data[2]) & 1;
      seed |= ((data[5]^data[1]) & 1) << 1;
      seed |= ((data[4]^data[0]) & 1) << 2;
      seed |= ((data[3]^data[6]^data[2]) & 1) << 3;
      seed |= ((data[2]^data[5]^data[1]) & 1) << 4;
      seed |= ((data[1]^data[4]^data[0]) & 1) << 5;
      seed |= ((data[0]^data[3]^data[6]^data[2]) & 1) << 6;

      // std::cout << "SERVICE (scrambled): ";
      // for (int i = 0; i < 7; i++)
      //   std::cout << (int)data[i];
      // std::cout << ", ";
      // printf("Found Seed: %02x\n", seed);
      return seed;
    }




    int
    scrambler_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char*) output_items[0];
      std::vector<tag_t> tags;
      char fb;
      int start, end, produced = 0;

      get_tags_in_range(tags, 0, nitems_read(0),
        nitems_read(0) + noutput_items, d_phy_tag_key);
      std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
      tag_range tr(nitems_read(0), noutput_items, tags);

      // for each tag range
      for (int k = 0; k < tr.size(); k++) {
        phy_tag t;

        if (tr.range_nitems(k) == 0)
          printf("scrambler zero items!!!!!!!!!\n");
        
        // tag should appear at the first bit of the payload or header
        if (phy_tag::parse(tr.tag(k).value, t)) {
          if (t.flag(PHY_SCRAMBLER_FLAG)) {
            d_mode = MODE_SCRAMBLE;
            if (d_is_scrambler) {
              // reset seed
              d_shiftreg = d_random_seed ? rand() & 0xff : d_fixed_seed;

              // set 6 bits (following the message) to zeros
              if (t.flag(PHY_TAIL_SET_FLAG)) {
                // determine position of tail bits in order to
                d_tail_pos = tr.abs_first_itemidx(k) + t.nitems() * 8 + 16;
              }
              else {
                d_tail_pos = LONG_MAX;  // disable setting tail bits
              }
            }
            else {
              // need to detect seed at the beginning of payload
              d_require_seed = true;
            }
          }
          else
            d_mode = MODE_PASS_THROUGH;
        }

        // processing
        start = tr.rel_first_itemidx(k);
        end = tr.rel_last_itemidx(k);
        switch (d_mode) {
          case MODE_SCRAMBLE:
            // for descrambler, need seed detection if it was not done
            if (!d_is_scrambler) {
              if (d_require_seed) {
                if (tr.range_nitems(k) >= 7) {
                  d_shiftreg = getSeed(&in[tr.rel_first_itemidx(k)]);
                  d_require_seed = false;

                  // record scrambling seed to the PHY tag
                  remove_item_tag(0, tr.tag(k));
                  t.set_scrambling_seed(d_shiftreg);
                  tr.tag(k).value = t;
                  add_item_tag(0, tr.tag(k));
                }
                else return produced;
              }
            }

            // scrambling/descrambling
            for (int i = start; i < end; i++) {
              fb = ((d_shiftreg >> 3) & 1) ^ ((d_shiftreg >> 6) & 1);
              out[i] = in[i] ^ fb;
              d_shiftreg = (d_shiftreg << 1) | fb;
            }

            // set 6 tail bits to zeros (ignore scrambled values)
            if (d_is_scrambler) {
              if (tr.abs_last_itemidx(k) >= d_tail_pos &&
                tr.abs_first_itemidx(k) < d_tail_pos + 6) 
              {
                int left = std::max(d_tail_pos, tr.abs_first_itemidx(k)) - nitems_read(0);
                int right = std::min(d_tail_pos + 6, tr.abs_last_itemidx(k)) - nitems_read(0);
                for (int i = left; i < right; i++)
                  out[i] = 0;
              }
            }
            break;
          case MODE_PASS_THROUGH:
            // simply copy input to output
            memcpy(&out[start], &in[start], end - start);
            break;
        }
        produced += end - start;
      }

      return produced;
    }

  } /* namespace swifi */
} /* namespace gr */

