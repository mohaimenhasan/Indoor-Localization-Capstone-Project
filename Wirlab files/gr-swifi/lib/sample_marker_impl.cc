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

#include "sample_marker_impl.h"
#include <swifi/tag_utils.h>
#include <swifi/phy_utils.h>
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    sample_marker::sptr
    sample_marker::make(int itemsize, const std::string &phy_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new sample_marker_impl(itemsize, phy_tag_key));
    }

    /*
     * The private constructor
     */
    sample_marker_impl::sample_marker_impl(int itemsize,
        const std::string &phy_tag_key)
      : gr::sync_block("sample_marker",
              gr::io_signature::make(1, 1, itemsize),
              gr::io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize),
        d_phy_tag_key(pmt::intern(phy_tag_key))
    {
      set_tag_propagation_policy(TPP_DONT);
    }

    sample_marker_impl::~sample_marker_impl()
    {
    }

    int
    sample_marker_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char*) output_items[0];
      std::vector<tag_t> tags, phy_tags;
      tag_t tag;
      int tag_idx = 0;

      get_tags_in_range(tags, 0, nitems_read(0),
        nitems_read(0) + noutput_items);
      std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
      for (int k = 0; k < tags.size(); k++) {
        if (pmt::eqv(tags[k].key, d_phy_tag_key))
          phy_tags.push_back(tags[k]);
      }
      tag_range tr(nitems_read(0), noutput_items, phy_tags);

      // for each tag range
      for (int k = 0; k < tr.size(); k++) {
        phy_tag t;
        if (phy_tag::parse(tr.tag(k).value, t)) {
          // add modified PHY tag
          t.set_payload_sample_index(tr.abs_first_itemidx(k));
          add_item_tag(0, tr.abs_first_itemidx(k), d_phy_tag_key, t);
        }

        // other tags propagation
        while (tag_idx < tags.size() && tags[tag_idx].offset < tr.abs_last_itemidx(k)) {
          if (!pmt::eqv(tags[tag_idx].key, d_phy_tag_key))
            add_item_tag(0, tags[tag_idx]);
          tag_idx++;
        }

        int m = tr.range_nitems(k);
        memcpy(out, in, m * d_itemsize);
        out += m * d_itemsize;
        in += m * d_itemsize;
      }

      return noutput_items;
    }

  } /* namespace swifi */
} /* namespace gr */

