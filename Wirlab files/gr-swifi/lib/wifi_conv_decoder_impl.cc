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

#include "wifi_conv_decoder_impl.h"
#include "utils.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    wifi_conv_decoder::sptr
    wifi_conv_decoder::make(const std::string &phy_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new wifi_conv_decoder_impl(phy_tag_key));
    }

    wifi_conv_decoder::sptr
    wifi_conv_decoder::make(int blocksize, int rate)
    {
      return gnuradio::get_initial_sptr
        (new wifi_conv_decoder_impl(blocksize, rate));
    }

    /*
     * The private constructor
     */
    wifi_conv_decoder_impl::wifi_conv_decoder_impl(const std::string &phy_tag_key)
      : gr::block("wifi_conv_decoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
      d_phy_tag_key(pmt::intern(phy_tag_key)),
      d_nitems_stored(0),
      d_start_offset_read(0), d_start_offset_write(0)
    {
      set_tag_propagation_policy(TPP_DONT);
      set_blocksize(1); // minimum 1 bit for an uncoded block
      set_stopsize(2);  // minimum 2 bits for a coded block
      d_puncture_map = wifi_conv_1_2::NO_PUNCTURE_MAP;
    }

    wifi_conv_decoder_impl::wifi_conv_decoder_impl(int blocksize, int rate)
      : gr::block("wifi_conv_decoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
      d_nitems_stored(0),
      d_start_offset_read(0), d_start_offset_write(0)
    {
      set_tag_propagation_policy(TPP_DONT);
      set_blocksize(blocksize);
      set_stopsize(blocksize * 2);
      if (rate == -1)
        d_puncture_map = wifi_conv_1_2::NO_PUNCTURE_MAP;
      else
        set_puncture_map(rate);
    }

    /*
     * Our virtual destructor.
     */
    wifi_conv_decoder_impl::~wifi_conv_decoder_impl()
    {
    }

    void wifi_conv_decoder_impl::set_blocksize(int blocksize) {
      if (blocksize == 0) {
        throw std::runtime_error("Block size cannot be zero");
      }
      d_blocksize = blocksize;  // uncoded block size in bits
      d_codedblocksize = d_blocksize * 2; // coded block size in bits
      if (d_buffer.size() < d_codedblocksize)
        d_buffer.resize(d_codedblocksize);
      d_stopsize = d_codedblocksize;
    }

    void wifi_conv_decoder_impl::set_stopsize(int stopsize) {
      d_stopsize = stopsize;
    }

    void wifi_conv_decoder_impl::set_puncture_map(int rate) {
      d_puncture_map = phy_utils::create_puncture_map(rate);
    }

    void wifi_conv_decoder_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      int nreqd = d_codedblocksize - d_nitems_stored;
      ninput_items_required[0] = nreqd;
    }

    int
    wifi_conv_decoder_impl::general_work(int noutput_items,
        gr_vector_int& ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      static int d_rate;
      const char *in = (const char*) input_items[0];
      char *out = (char*) output_items[0];
      int produced = 0, consumed = 0;
      int n = std::min(noutput_items * 2, ninput_items[0]);
      int m;
      int j = 0;
      std::vector<tag_t> tags, phy_tags;
      int offset;

      // get tags
      get_tags_in_window(tags, 0, 0, n);
      std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
      for (int k = 0; k < tags.size(); k++) {
        if (pmt::eqv(tags[k].key, d_phy_tag_key))
          phy_tags.push_back(tags[k]);
      }
      tag_range tr(nitems_read(0), n, phy_tags);

      for (int k = 0; k < tr.size(); k++) {
        phy_tag t;
        if (phy_tag::parse(tr.tag(k).value, t)) {
          if (t.flag(PHY_HEADER_FLAG)) {
            set_blocksize(t.nitems() * 8);
            set_stopsize(t.nitems() * 8 * 2);
          }
          else {
            set_blocksize(phy_utils::compute_uncoded_service_payload_tail_pad_size(t.rate(), t.nitems()) * 8); // set blocksize as number of uncoded bits per block
            set_stopsize((t.nitems() * 8 + 22) * 2);
            set_stopsize(d_codedblocksize);
          }
          set_puncture_map(t.rate());
          d_start_offset_read = nitems_read(0) + consumed;
          d_start_offset_write = nitems_written(0) + produced;
          d_nitems_stored = 0;  // reset number of items in the block
          d_rate = t.rate();
          d_stored_tags.clear();
        }

        n = tr.range_nitems(k); // number of items in this range
        while (n) {
          // copy items (bits) to buffer
          m = std::min(n, d_codedblocksize - d_nitems_stored); // number of items that will be copied to buffer
          memcpy(d_buffer.data() + d_nitems_stored, &in[consumed], sizeof(char) * m);
          d_nitems_stored += m; // update number of items already copied to the block
          n -= m; // update number of items in this range
          consumed += m;  // update number of read items

          // store tags on copied items
          while (j < tags.size() && tags[j].offset < nitems_read(0) + consumed) {
            offset = tags[j].offset - d_start_offset_read;
            tags[j].offset = d_start_offset_write + offset / 2;
            d_stored_tags.push_back(tags[j]);
            j++;
          }

          // enough items in block --> decode
          if (d_nitems_stored >= d_codedblocksize) {
            // zero out output
            memset(out, 0, sizeof(*out) * d_blocksize);

            // decode and produce output
            // code.decode(out, d_buffer.data(), d_stopsize, 0, 
            //   0, d_puncture_map);
            code.decode(out, d_buffer.data(), d_stopsize, 0, 
              wifi_conv_1_2::STATE_NOT_SPECIFIED, d_puncture_map);
            // printf("tail %d %d %d %d %d %d\n", 
            //   d_buffer[d_stopsize-1],
            //   d_buffer[d_stopsize-2],
            //   d_buffer[d_stopsize-3],
            //   d_buffer[d_stopsize-4],
            //   d_buffer[d_stopsize-5],
            //   d_buffer[d_stopsize-6]
            //   );

            // propagate tags
            for (int i = 0; i < d_stored_tags.size(); i++) {
              add_item_tag(0, d_stored_tags[i]);
            }
            d_stored_tags.clear();

            d_nitems_stored = 0;  // reset number of items in the block
            out += d_blocksize; // move pointer to the next output block
            produced += d_blocksize; // count number of items decoded
            d_start_offset_read = nitems_read(0) + consumed;
            d_start_offset_write = nitems_written(0) + produced;
          }
        }
      }

      consume(0, consumed);
      return produced;
    }

  } /* namespace swifi */
} /* namespace gr */

