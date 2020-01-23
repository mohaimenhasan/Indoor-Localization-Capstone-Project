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

#include "wifi_conv_soft_decoder_impl.h"
#include "utils.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    #define METRICS_PER_ITEM  2
    #define CODE_EXPAND_RATIO 2

    wifi_conv_soft_decoder::sptr
    wifi_conv_soft_decoder::make(const std::string &phy_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new wifi_conv_soft_decoder_impl(phy_tag_key));
    }

    wifi_conv_soft_decoder::sptr
    wifi_conv_soft_decoder::make(int blocksize)
    {
      return gnuradio::get_initial_sptr
        (new wifi_conv_soft_decoder_impl(blocksize));
    }

    /*
     * The private constructor
     */
    wifi_conv_soft_decoder_impl::wifi_conv_soft_decoder_impl(const std::string &phy_tag_key)
      : dynamic_rate_block("wifi_conv_soft_decoder",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(char)),
              phy_tag_key),
      d_nitems_stored(0)
    {
      set_relative_rate(1.0 / (CODE_EXPAND_RATIO * METRICS_PER_ITEM));
      set_blocksize(1 * 8 + 22 + 2);  // 8 data bits + 16 service bits + 6 tail bits + 2 padding bits
      set_stopsize((1 * 8 + 22) * CODE_EXPAND_RATIO); // 8 data bits + 16 service bits + 6 tail bits
    }

    wifi_conv_soft_decoder_impl::wifi_conv_soft_decoder_impl(int blocksize)
      : dynamic_rate_block("wifi_conv_soft_decoder",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(char)),
              ""),
      d_nitems_stored(0)
    {
      set_relative_rate(1.0 / (CODE_EXPAND_RATIO * METRICS_PER_ITEM));
      set_blocksize(blocksize);
      set_stopsize(blocksize * CODE_EXPAND_RATIO);
    }

    void wifi_conv_soft_decoder_impl::set_blocksize(int blocksize) {
      if (blocksize == 0) {
        throw std::runtime_error("Block size cannot be zero");
      }
      d_blocksize = blocksize;  // uncoded block size in bits
      d_codedblocksize = d_blocksize * CODE_EXPAND_RATIO; // coded block size in bits
      if (d_buffer.size() < d_codedblocksize * METRICS_PER_ITEM)
        d_buffer.resize(d_codedblocksize * METRICS_PER_ITEM);
      d_stopsize = d_codedblocksize;
    }

    void wifi_conv_soft_decoder_impl::set_stopsize(int stopsize) {
      d_stopsize = stopsize;
    }

    void wifi_conv_soft_decoder_impl::update_rate(const tag_t &tag) {
      phy_tag t;
      if (phy_tag::parse(tag.value, t)) {
        if (t.flag(PHY_HEADER_FLAG)) {
          set_blocksize(t.nitems() * 8);
          set_stopsize(t.nitems() * 8 * CODE_EXPAND_RATIO);
        }
        else {
          set_blocksize(phy_utils::compute_uncoded_service_payload_tail_pad_size(t.rate(), t.nitems()) * 8); // set blocksize as number of uncoded bits per block
          set_stopsize((t.nitems() * 8 + 22) * CODE_EXPAND_RATIO);
          set_stopsize(d_codedblocksize);
        }
        d_nitems_stored = 0;  // reset number of items in the block

        // reset d_start_read_idx and d_stored_tags
        // no matter what if tags were propagated.
        // Note: If tags were not propagated, the block size (implied from RATE info in PHY tag)
        // did not match the size of the range, and therefore, no decoding was
        // performed and no tags in the corresponding range should be propagated (instead, tags are dropped)
        d_start_read_idx = nitems_read(0);  // new start read index
        d_stored_tags.clear();  // remove all stored tags
      }
    }

    // overriding method: only propagate tags if we produce something
    void wifi_conv_soft_decoder_impl::propagate_tags() {
      if (d_start_write_idx != nitems_written(0)) {
        for (std::vector<tag_t>::iterator i = d_stored_tags.begin();
          i != d_stored_tags.end(); ++i)
        {
          int offset = i->offset - d_start_read_idx;  // relative offset in input stream
          offset = int(float(offset * relative_rate() + 0.5)); // compute relative offset in output stream
          add_item_tag(0, d_start_write_idx + offset, i->key, i->value);
        }
        d_start_read_idx = nitems_read(0);
        d_start_write_idx = nitems_written(0);
        d_stored_tags.clear();
      }
    }

    int
    wifi_conv_soft_decoder_impl::dynamic_rate_work(int noutput_items, int ninput_items,
        const void* input, void* output)
    {
      const float *in = (const float*) input;
      char *out = (char*) output;
      int n;
      int m;

      // number of items available on input
      n = std::min(noutput_items * CODE_EXPAND_RATIO * METRICS_PER_ITEM, ninput_items);

      // number of items that will be copied to buffer
      m = std::min(n, d_codedblocksize * METRICS_PER_ITEM - d_nitems_stored);

      memcpy(d_buffer.data() + d_nitems_stored, in, sizeof(*in) * m);
      d_nitems_stored += m; // update number of items already copied to the block

      // store tags
      std::vector<tag_t> tags;
      get_tags_in_window(tags, 0, 0, m);
      d_stored_tags.insert(d_stored_tags.end(), tags.begin(), tags.end());

      consume(0, m);

      // enough items in block --> decode
      // Note: this condition might not occur at all if the blocksize
      // implied from the RATE info in PHY tag does not match the size
      // of the tag range. In this case, no decoding is performed,
      // eventually new tag range will trigger update_rate() and
      // everything will be restarted
      if (d_nitems_stored >= d_codedblocksize * METRICS_PER_ITEM) {
        // zero out output
        memset(out, 0, sizeof(*out) * d_blocksize);

        // decode and produce output
        code.soft_decode(out, d_buffer.data(), d_stopsize, 0, 
          wifi_conv_1_2::STATE_NOT_SPECIFIED);

        d_nitems_stored = 0;  // reset number of items in the block

        produce(0, d_blocksize);
      }

      return WORK_CALLED_PRODUCE;
    }

  } /* namespace swifi */
} /* namespace gr */

