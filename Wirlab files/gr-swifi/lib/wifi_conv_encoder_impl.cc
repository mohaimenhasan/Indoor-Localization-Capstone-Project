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

#include "wifi_conv_encoder_impl.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>

namespace gr {
  namespace swifi {

    wifi_conv_encoder::sptr
    wifi_conv_encoder::make(const std::string &phy_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new wifi_conv_encoder_impl(phy_tag_key));
    }

    wifi_conv_encoder::sptr
    wifi_conv_encoder::make(int blocksize)
    {
      return gnuradio::get_initial_sptr
        (new wifi_conv_encoder_impl(blocksize));
    }

    /*
     * The private constructor
     */
    wifi_conv_encoder_impl::wifi_conv_encoder_impl(const std::string &phy_tag_key)
      : gr::sync_interpolator("wifi_conv_encoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)),2),
      d_phy_tag_key(pmt::intern(phy_tag_key)),
      d_n_processed_bits(0)
    {
      set_blocksize(999999);  // default very long block size
      code.set_encode_state(0); // reset state to 0
    }

    wifi_conv_encoder_impl::wifi_conv_encoder_impl(int blocksize)
      : gr::sync_interpolator("wifi_conv_encoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char)),2),
      d_n_processed_bits(0)
    {
      set_blocksize(blocksize);
      code.set_encode_state(0); // reset state to 0
    }

    /*
     * Our virtual destructor.
     */
    wifi_conv_encoder_impl::~wifi_conv_encoder_impl()
    {
    }

    void wifi_conv_encoder_impl::set_blocksize(int blocksize) {
      if (blocksize == 0) {
        throw std::runtime_error("Block size cannot be zero");
      }
      d_blocksize = blocksize;
    }

    int
    wifi_conv_encoder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char*) output_items[0];
      int n = noutput_items / 2;
      std::vector<tag_t> tags;
      int m;

      // get length tag
      get_tags_in_window(tags, 0, 0, n, d_phy_tag_key);
      std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
      tag_range tr(nitems_read(0), n, tags);

      for (int k = 0; k < tr.size(); k++) {
        phy_tag t;
        if (phy_tag::parse(tr.tag(k).value, t)) {
          if (t.flag(PHY_HEADER_FLAG)) {
            set_blocksize(t.nitems() * 8); // set blocksize
          }
          else {
            set_blocksize(phy_utils::compute_uncoded_service_payload_tail_pad_size(t.rate(), t.nitems()) * 8); // set blocksize as number of uncoded bits per block
          }
          d_n_processed_bits = 0; // start new block, reset number of processed bits
          code.set_encode_state(0); // reset state to 0
        }

        n = tr.range_nitems(k);
        while (n) {
          m = std::min(n, d_blocksize - d_n_processed_bits);  // don't encode more than blocksize
          code.encode(out, in, m);
          in += m;
          out += m * 2;
          d_n_processed_bits += m;
          n -= m;
          if (d_n_processed_bits >= d_blocksize) {
            d_n_processed_bits = 0; // move to new block, reset number of processed bits
            code.set_encode_state(0); // reset state to 0
          }
        }
      }

      return noutput_items;
    }

  } /* namespace swifi */
} /* namespace gr */

