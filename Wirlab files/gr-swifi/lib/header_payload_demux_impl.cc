/* -*- c++ -*- */
/* Copyright 2012,2013 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <swifi/tag_utils.h>
#include <climits>
#include <boost/format.hpp>
#include <gnuradio/io_signature.h>
#include "header_payload_demux_impl.h"
#include <stdio.h>

namespace gr {
  namespace swifi {

    enum demux_states_t {
      STATE_FIND_TRIGGER,       // "Idle" state (waiting for burst)
      STATE_HEADER,             // Copy header
      STATE_WAIT_FOR_MSG,       // Null state (wait until msg from header demod)
      STATE_HEADER_RX_SUCCESS,  // Header processing
      STATE_HEADER_RX_FAIL,     //       "
      STATE_PAYLOAD             // Copy payload
    };

    enum out_port_indexes_t {
      PORT_HEADER = 0,
      PORT_PAYLOAD = 1
    };

#define msg_port_id pmt::mp("header_data")

    header_payload_demux::sptr
    header_payload_demux::make(int header_len,
        int output_items_per_symbol, int guard_interval,
        const std::string &length_tag_key,
        const std::string &trigger_tag_key,
        bool output_symbols, size_t itemsize,
        const std::string &phy_tag_key)
    {
      return gnuradio::get_initial_sptr (
          new header_payload_demux_impl(header_len, output_items_per_symbol, guard_interval,
            length_tag_key, trigger_tag_key, output_symbols, itemsize,
            phy_tag_key));
    }

    header_payload_demux_impl::header_payload_demux_impl(int header_len,
        int output_items_per_symbol, int guard_interval,
        const std::string &length_tag_key, const std::string &trigger_tag_key,
        bool output_symbols,
        size_t itemsize,
        const std::string &phy_tag_key)
    : block("header_payload_demux",
            io_signature::make(1, 1, itemsize),
            io_signature::make(2, 2, (output_symbols ? itemsize * output_items_per_symbol : itemsize))),
      d_header_len(header_len),
      d_output_items_per_symbol(output_items_per_symbol),
      d_gi(guard_interval),
      d_input_items_per_symbol(output_items_per_symbol + guard_interval),
      d_input_symbol_size((output_items_per_symbol + guard_interval) * itemsize),
      d_output_symbol_size(output_items_per_symbol * itemsize),
      d_len_tag_key(pmt::string_to_symbol(length_tag_key)),
      d_trigger_tag_key(pmt::string_to_symbol(trigger_tag_key)),
      d_output_symbols(output_symbols),
      d_itemsize(itemsize),
      d_state(STATE_FIND_TRIGGER),
      d_curr_payload_len(0),
      d_header_ncopied(0),
      d_payload_tag_keys(0),
      d_payload_tag_values(0),
      d_uses_phy_tag(!phy_tag_key.empty()),
      d_phy_tag_key(pmt::intern(phy_tag_key))
    {
      if (d_header_len < 1) {
        throw std::invalid_argument("Header length must be at least 1 symbol.");
      }

      if (d_output_items_per_symbol < 1 || d_gi < 0 || d_itemsize < 1) {
        throw std::invalid_argument("Items and symbol sizes must be at least 1.");
      }

      if (d_output_symbols) {
        set_relative_rate(1.0 / (d_input_items_per_symbol));
      } else {
        set_relative_rate((double)d_output_items_per_symbol / d_input_items_per_symbol);
        set_output_multiple(d_output_items_per_symbol);
      }
      set_tag_propagation_policy(TPP_DONT);
      message_port_register_in(msg_port_id);
      set_msg_handler(msg_port_id, boost::bind(&header_payload_demux_impl::parse_header_data_msg, this, _1));
    }

    header_payload_demux_impl::~header_payload_demux_impl()
    {
    }

    void
    header_payload_demux_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      int n_items_reqd = 1;
      if (d_state == STATE_HEADER || d_state == STATE_PAYLOAD) {
        n_items_reqd = d_input_items_per_symbol *
          (d_output_symbols ? noutput_items : noutput_items / d_output_items_per_symbol);
      }

      ninput_items_required[0] = n_items_reqd;
    }


    int
    header_payload_demux_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out_header = (unsigned char *) output_items[PORT_HEADER];
      unsigned char *out_payload = (unsigned char *) output_items[PORT_PAYLOAD];

      int navail = ninput_items[0];
      int trigger_offset = 0;
      int n;
      std::vector<tag_t> tags;

      switch (d_state) {
        case STATE_WAIT_FOR_MSG:
          // In an ideal world, this would never be called
          return 0;

        case STATE_HEADER_RX_FAIL:
          d_state = STATE_FIND_TRIGGER;
          break;
          // Fall through

        case STATE_FIND_TRIGGER:
          get_tags_in_window(tags, 0, 0, navail, d_trigger_tag_key);
          trigger_offset = -1;
          for (int i = 0; i < tags.size(); i++) {
            remove_item_tag(0, tags[i]);  // remove no matter what it is ON or OFF trigger
            if (trigger_tag::parse(tags[i].value, d_trigger_tag) && d_trigger_tag.state()) {
              trigger_offset = tags[i].offset - nitems_read(0);
              d_header_ncopied = 0;  // number of symbols copied to header port
              d_state = STATE_HEADER;
              break;
            }
          }

          if (trigger_offset == -1) {
            consume(0, navail);
            break;
          }
          else {
            consume(0, trigger_offset);
            break;
          }

        case STATE_HEADER:
          // number of symbols to copy to header port
          n = std::min(navail / d_input_items_per_symbol, d_header_len - d_header_ncopied);
          n = std::min(n, d_output_symbols ? noutput_items : noutput_items / d_output_items_per_symbol);

          if (n > 0 && d_header_ncopied == 0) {
            // add rx time tag
            add_item_tag(PORT_HEADER, nitems_written(PORT_HEADER),
              d_phy_tag_key, 
              phy_tag::create()
                .set_rx_time(d_trigger_tag.sample_index())
                .set_freq_offset(d_trigger_tag.freq_offset()));
          }

          copy_n_symbols(in, out_header, PORT_HEADER, n);
          consume(0, n * d_input_items_per_symbol);
          produce(PORT_HEADER, d_output_symbols ? n : n * d_output_items_per_symbol);
          d_header_ncopied += n;
          if (d_header_ncopied >= d_header_len)
            d_state = STATE_WAIT_FOR_MSG;

          break;

        case STATE_HEADER_RX_SUCCESS:
          for (unsigned i = 0; i < d_payload_tag_keys.size(); i++) {
            add_item_tag(PORT_PAYLOAD, nitems_written(PORT_PAYLOAD),
                d_payload_tag_keys[i], d_payload_tag_values[i]
            );
          }
          d_state = STATE_PAYLOAD;
          // Fall through

        case STATE_PAYLOAD:
          {
            // find OFF trigger. This is to prevent the case of the parsed header
            // contains wrong LENGTH information while the checksum/parity check is still correct
            get_tags_in_window(tags, 0, 0, navail,
              // std::min(d_curr_payload_len * d_input_items_per_symbol, navail),
              d_trigger_tag_key);
            if (tags.size()) {
              trigger_offset = tags[0].offset - nitems_read(0);
              // if a trigger tag is found during this state, stop copying to payload
              int nsyms_until_frame_end = trigger_offset / d_input_items_per_symbol;
              if (d_curr_payload_len > nsyms_until_frame_end) {
                d_curr_payload_len = nsyms_until_frame_end;
              }

              // remove if it is a pmt::PMT_F trigger
              if (trigger_tag::parse(tags[0].value, d_trigger_tag) && !d_trigger_tag.state()) {
                remove_item_tag(0, tags[0]);
              }
            }

            // number of symbols to copy to payload port
            n = std::min(navail / d_input_items_per_symbol, d_curr_payload_len);
            n = std::min(n, d_output_symbols ? noutput_items : noutput_items / d_output_items_per_symbol);

            // copy
            copy_n_symbols(in, out_payload, PORT_PAYLOAD, n);
            produce(PORT_PAYLOAD, d_output_symbols ? n : n * d_output_items_per_symbol);
            consume(0, n * d_input_items_per_symbol);
            d_curr_payload_len -= n;
            if (d_curr_payload_len <= 0) {
              d_state = STATE_FIND_TRIGGER;
            }
          }
          break;

        default:
          throw std::runtime_error("invalid state");
      } /* switch */

      return WORK_CALLED_PRODUCE;
    } /* general_work() */


    void
    header_payload_demux_impl::parse_header_data_msg(pmt::pmt_t header_data)
    {
      d_payload_tag_keys.clear();
      d_payload_tag_values.clear();
      d_state = STATE_HEADER_RX_FAIL;

      if (pmt::is_integer(header_data)) {
        d_curr_payload_len = pmt::to_long(header_data);
        d_payload_tag_keys.push_back(d_len_tag_key);
        d_payload_tag_values.push_back(header_data);
        d_state = STATE_HEADER_RX_SUCCESS;
      } else if (pmt::is_dict(header_data)) {
        pmt::pmt_t dict_items(pmt::dict_items(header_data));
        while (!pmt::is_null(dict_items)) {
          pmt::pmt_t this_item(pmt::car(dict_items));
          d_payload_tag_keys.push_back(pmt::car(this_item));
          d_payload_tag_values.push_back(pmt::cdr(this_item));
          if (pmt::equal(pmt::car(this_item), d_len_tag_key)) {
            d_curr_payload_len = pmt::to_long(pmt::cdr(this_item));
            d_state = STATE_HEADER_RX_SUCCESS;
          }
          dict_items = pmt::cdr(dict_items);
        }
        if (d_state == STATE_HEADER_RX_FAIL) {
          GR_LOG_CRIT(d_logger, "no length tag passed from header data");
        }
      } else if (header_data == pmt::PMT_F || pmt::is_null(header_data)) {
        GR_LOG_INFO(d_logger, boost::format("Parser returned %1%") % pmt::write_string(header_data));
      } else {
        GR_LOG_ALERT(d_logger, boost::format("Received illegal header data (%1%)") % pmt::write_string(header_data));
      }
    } /* parse_header_data_msg() */


    void
    header_payload_demux_impl::copy_n_symbols(const unsigned char *in,
        unsigned char *out, int port, int n_symbols)
    {
      // Copy samples
      if (d_gi) {
        for (int i = 0; i < n_symbols; i++) {
          memcpy(out, in + d_gi * d_itemsize, d_output_symbol_size);
          out += d_output_symbol_size;
          in += d_input_symbol_size;
        }
      } else {
        memcpy(out, in, n_symbols * d_output_symbol_size);
      }

      // Copy tags
      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0),
          nitems_read(0) + n_symbols * d_input_items_per_symbol);
      for (unsigned t = 0; t < tags.size(); t++) {
        int new_offset = tags[t].offset - nitems_read(0);
        if (d_output_symbols) {
          new_offset /= d_input_items_per_symbol;
        }
        else if (d_gi) {
          int pos_on_symbol = (new_offset % d_input_items_per_symbol) - d_gi;
          if (pos_on_symbol < 0) pos_on_symbol = 0;
          new_offset = (new_offset / d_input_items_per_symbol) * d_output_items_per_symbol + pos_on_symbol;
        }

        add_item_tag(port, nitems_written(port) + new_offset,
            tags[t].key, tags[t].value);
      }
    } /* copy_n_symbols() */

  } /* namespace swifi */
} /* namespace gr */

