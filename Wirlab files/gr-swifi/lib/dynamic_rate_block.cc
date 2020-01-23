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

#include <swifi/dynamic_rate_block.h>
#include <swifi/tag_utils.h>
#include <swifi/phy_utils.h>
#include <gnuradio/io_signature.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    dynamic_rate_block::
    dynamic_rate_block(const std::string &name,
            gr::io_signature::sptr input_signature,
            gr::io_signature::sptr output_signature,
            const std::string &tag_key)
      : block(name, input_signature, output_signature),
        d_tag_key(pmt::intern(tag_key)),
        d_input_itemsize(input_signature->sizeof_stream_item(0)),
        d_output_itemsize(output_signature->sizeof_stream_item(0)),
        d_start_read_idx(0), d_start_write_idx(0)
    {
      set_tag_propagation_policy(TPP_DONT);
    }

    void dynamic_rate_block::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = int(floor(noutput_items / relative_rate() + 0.5));
    }

    void dynamic_rate_block::propagate_tags() {
      std::vector<tag_t> tags;
      std::vector<tag_t>::iterator i;
      get_tags_in_range(tags, 0, d_start_read_idx, nitems_read(0));
      for (i = tags.begin(); i != tags.end(); ++i) {
        int offset = i->offset - d_start_read_idx;  // relative offset in input stream
        offset = int(float(offset * relative_rate() + 0.5)); // compute relative offset in output stream
        add_item_tag(0, d_start_write_idx + offset, i->key, i->value);
      }
      d_start_read_idx = nitems_read(0);
      d_start_write_idx = nitems_written(0);
    }

    int
    dynamic_rate_block::general_work(int noutput_items,
        gr_vector_int& ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char* in = (const char*)input_items[0];
      char* out = (char*)output_items[0];
      std::vector<tag_t> dynamic_rate_tags;
      int r, navail_items = ninput_items[0];

      // get dynamic rate tags
      get_tags_in_window(dynamic_rate_tags, 0, 0, navail_items, d_tag_key);
      std::sort(dynamic_rate_tags.begin(), dynamic_rate_tags.end(), tag_t::offset_compare);

      // get tag range for dynamic rate tags
      tag_range tr(nitems_read(0), navail_items, dynamic_rate_tags);

      // process items in each range, whose rate may change based on the tag
      for (int k = 0; k < tr.size(); k++) {
        tag_t& tag = tr.tag(k);
        if (!pmt::is_null(tag.key) && !pmt::is_null(tag.value))
          update_rate(tr.tag(k)); // update relative rate based on the received tag

        // compute number of available items on input and check with number of items
        // to produce on output ensuring that the number of produced items is a multiple
        // of output_multiple()
        int navail_items = tr.range_nitems(k);
        int ntoproduce = (int(floor(navail_items * relative_rate() + 0.5)) / output_multiple()) * output_multiple();
        if (ntoproduce > noutput_items) {
          ntoproduce = (noutput_items / output_multiple()) * output_multiple();
          if (ntoproduce == 0)
            return WORK_CALLED_PRODUCE; // wait for next general_work call
        }

        else if (ntoproduce == 0) {
          // if the current range is not the last range, we throw it away because we will never get enough input to produce non-zero items
          if (k + 1 < tr.size()) {
            consume(0, navail_items);
            d_start_read_idx = nitems_read(0);  // we don't propagate tags belonging to this range
            continue; // continue for loop
          }

          // if this is the last range, we wait for next general_work call
          return WORK_CALLED_PRODUCE;
        }

        navail_items = (int)floor(1.0 * ntoproduce / relative_rate() + 0.5);

        // virtual work function
        r = dynamic_rate_work(ntoproduce, navail_items, in, out); // r is returned as number of produced output items
        if (r >= 0) {
          int s = int(floor(1.0 * r / relative_rate() + 0.5));  // number of consumed input items
          in += d_input_itemsize * s;
          consume(0, s);

          out += d_output_itemsize * r;
          noutput_items -= r;
          produce(0, r);

          propagate_tags();

          // if we have not yet consumed all items in the current range,
          // we return what have been produced so far
          if (s < tr.range_nitems(k)) {
            return WORK_CALLED_PRODUCE;
          }
        }
        else {
          propagate_tags();
          return r; // WORK_CALLED_PRODUCE or WORK_DONE
        }
      }

      return WORK_CALLED_PRODUCE;
    }

  } /* namespace swifi */
} /* namespace gr */
