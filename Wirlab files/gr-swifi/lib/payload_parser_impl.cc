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

#include "payload_parser_impl.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <swifi/mac_parser.h>
#include <swifi/wifi_dump_factory.h>
#include <gnuradio/io_signature.h>

namespace gr {
  namespace swifi {


    payload_parser::sptr
    payload_parser::make(const std::string &phy_tag_key,
      const std::string &filename,
      const std::string &wifi_dump_format,
      const std::string &filter_string,
      bool is_print_raw)
    {
      return gnuradio::get_initial_sptr
        (new payload_parser_impl(phy_tag_key,
          filename, wifi_dump_format, filter_string, is_print_raw));
    }

    /*
     * The private constructor
     */
    payload_parser_impl::payload_parser_impl(const std::string &phy_tag_key,
      const std::string &filename,
      const std::string &wifi_dump_format,
      const std::string &filter_string,
      bool is_print_raw)
      : gr::sync_block("payload_parser",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(0, 0, 0)),
      d_phy_tag_key(pmt::intern(phy_tag_key)),
      d_info_tag_key(pmt::intern("info")),
      d_n_data_bytes(1), d_n_bytes_copied(0),
      d_mac_utils(mac_utils::make()),
      d_wifi_dump(wifi_dump_factory::make(filename, wifi_dump_format,
        filter_string, is_print_raw))
    {
      d_rx_info.payload_sample_end = 0;
    }

    /*
     * Our virtual destructor.
     */
    payload_parser_impl::~payload_parser_impl()
    {
    }

    int
    payload_parser_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      std::vector<tag_t> tags;
      int n, m, consumed = 0;

      // get phy tags
      get_tags_in_window(tags, 0, 0, noutput_items, d_phy_tag_key);
      std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
      tag_range tr(nitems_read(0), noutput_items, tags);

      // for each range
      for (int k = 0; k < tr.size(); k++) {
        process_tag_value(tr.tag(k).value);

        n = tr.range_nitems(k);
        while (n) {
          m = std::min(n, d_n_data_bytes - d_n_bytes_copied);
          memcpy(d_buffer.data() + d_n_bytes_copied, &in[consumed], m);

          // get info tags
          std::vector<tag_t> info_tags;
          get_tags_in_window(info_tags, 0, consumed, consumed + m, d_info_tag_key);
          for (int i = 0; i < info_tags.size(); i++) {
            info_tag t;
            if (info_tag::parse(info_tags[i].value, t)) {
              d_rx_info.sfo = t.sfo();
            }
          }

          consumed += m;
          d_n_bytes_copied += m;
          n -= m;
          if (d_n_bytes_copied >= d_n_data_bytes) {

            // parsing
            // d_mac_utils->parse_recv_bytes(
            //   d_buffer.data() + 2,  // omit the first 2 bytes of SERVICE field
            //   d_psdu_size, // size of MAC frame
            //   rx_info);
            // // debug only
            // print_output(d_buffer.data(), d_psdu_size + 2);


            // copy tail + padding bytes to d_rx_info
            int padsize = phy_utils::compute_uncoded_tail_pad_size(d_rx_info.rate, d_rx_info.psdu_size);
            d_rx_info.data_tail_padding_bytes.reset(new char[padsize]);
            memcpy(d_rx_info.data_tail_padding_bytes.get(),
              d_buffer.data() + 2 + d_rx_info.psdu_size, padsize);

            // parsing
            mac_frame_info& mac_info = d_mac_parser.parse(
              d_buffer.data() + 2, d_rx_info.psdu_size, d_rx_info);

            if (!mac_info.malformed)
              d_wifi_dump->dump(mac_info);

            d_n_bytes_copied = 0;
          }
        }
      }

      return noutput_items;
    }

    void payload_parser_impl::process_tag_value(pmt::pmt_t v) {
      int n_ofdm_syms;

      phy_tag t;
      if (phy_tag::parse(v, t)) {
        // get information from tag
        d_rx_info.mode = phy_utils::MODE_OFDM;
        d_rx_info.rate = t.rate();
        d_rx_info.psdu_size = t.nitems();
        d_rx_info.freq_offset = t.freq_offset();
        d_rx_info.scrambling_seed = t.scrambling_seed();
        d_n_data_bytes = phy_utils::compute_uncoded_service_payload_tail_pad_size(
          d_rx_info.rate, d_rx_info.psdu_size);
        if (d_n_data_bytes > d_buffer.size())
          d_buffer.resize(d_n_data_bytes);
        d_n_bytes_copied = 0;

        // derive number of OFDM symbols for the DATA field
        n_ofdm_syms = phy_utils::compute_n_ofdm_syms(d_rx_info.rate, d_rx_info.psdu_size);

        // get sample index of frame start and frame end
        d_rx_info.sample_start = t.rx_time();
        d_rx_info.sample_end = d_rx_info.sample_start + 80 * (5 + n_ofdm_syms); // each OFDM symbol contains 80 samples including CP and 64 subcarriers

        // convert number of samples to microsecs
        d_rx_info.time_start = d_rx_info.sample_start / 20;
        d_rx_info.time_end = d_rx_info.sample_end / 20;

        // payload sample index is kept track inside this block
        // d_rx_info.payload_sample_start = d_rx_info.payload_sample_end;
        // d_rx_info.payload_sample_end += n_ofdm_syms * 48; // 48 due to sample after equalization
        d_rx_info.payload_sample_start = t.payload_sample_index();
        d_rx_info.payload_sample_end = d_rx_info.payload_sample_start + n_ofdm_syms * 48;
      }
    }

    void payload_parser_impl::print_output(const char *data, int len) {
      static int width = 16;
      int nrows = len / width + (len % width == 0 ? 0 : 1);
      int row, col, i = 0;
      printf("Raw content:\n");
      for (row = 0; row < nrows; row++) {
        i = row * width;
        for (col = 0; col < width; col++) {
          if (i < len) printf("%02x ", int(data[i++]) & 0xFF);
          else printf("   ");
        }

        i = row * width;
        for (col = 0; col < width; col++) {
          if (i < len) {
            if (int(data[i] & 0xFF) >= 32) printf("%c", data[i++]);
            else printf(".");
          }
        }
        printf("\n");
      }
    }

  } /* namespace swifi */
} /* namespace gr */

