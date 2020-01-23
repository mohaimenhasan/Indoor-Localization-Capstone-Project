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

#include "mac_frame_generator_impl.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <stdio.h>

namespace gr {
  namespace swifi {

    mac_frame_generator::sptr
    mac_frame_generator::make(const std::string &phy_tag_key,
      int rate, int datalen)
    {
      return gnuradio::get_initial_sptr
        (new mac_frame_generator_impl(phy_tag_key, rate, datalen));
    }

    /*
     * The private constructor
     */
    mac_frame_generator_impl::mac_frame_generator_impl(
        const std::string &phy_tag_key, int rate, int datalen)
      : gr::block("mac_frame_generator",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
        d_phy_tag_key(pmt::intern(phy_tag_key)),
        d_rate(rate), d_datalen(datalen),
        d_state(GENERATE_HEADER),
        d_header(NULL),
        d_nitems_to_copy(0), d_nitems_copied(0),
        d_mac_utils(mac_utils::make()),
        d_seq(0)
    {
      set_header();
    }

    /*
     * Our virtual destructor.
     */
    mac_frame_generator_impl::~mac_frame_generator_impl()
    {
    }

    void mac_frame_generator_impl::set_header() {
      d_mac_utils->set_datalen(d_datalen);
      // d_mac_utils->set_type(FRAME_TYPE_CONTROL, FRAME_SUBTYPE_CONTROL_RESERVED0);
      d_mac_utils->set_type(FRAME_TYPE_DATA, FRAME_SUBTYPE_DATA_DATA);
      d_mac_utils->set_duration(0x2e00);
      d_mac_utils->set_toDS_fromDS(0, 1);
      d_mac_utils->set_addr(MAC_ADDR1, "00:60:08:CD:37:A6");
      d_mac_utils->set_addr(MAC_ADDR2, "00:20:D6:01:3C:F1");
      d_mac_utils->set_addr(MAC_ADDR3, "00:60:08:AD:3B:AF");
    }

    void mac_frame_generator_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      int nreqd;
      switch (d_state) {
        case GENERATE_HEADER:
          nreqd = d_nitems_copied == 0 ? 1 : 0;
          break;
        case COPY_DATA:
          nreqd = std::min(d_nitems_to_copy - d_nitems_copied, noutput_items);
          break;
        case GENERATE_FCS:
          nreqd = 0;
          break;
      }
      for (unsigned i = 0; i < ninput_items_required.size(); i++)
        ninput_items_required[i] = nreqd;
    }

    int
    mac_frame_generator_impl::general_work(int noutput_items,
        gr_vector_int& ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const char *in = (const char *) input_items[0];
      char *out = (char*) output_items[0];
      std::vector<tag_t> tags;
      int consumed = 0;
      int produced = 0;
      int n;

      while (produced < noutput_items) {
        switch (d_state) {
          case GENERATE_HEADER:
            if (d_nitems_copied == 0) {
              // require at least 1 input item to produce header
              if (consumed >= ninput_items[0])
                goto endwhile;
              d_mac_utils->set_seq(d_seq++);
              d_header = d_mac_utils->generate_header();
              d_nitems_to_copy = d_mac_utils->get_headerlen();
              d_mac_utils->reset_fcs();
              d_mac_utils->update_fcs(d_header, d_mac_utils->get_headerlen());
            }
            n = std::min(d_nitems_to_copy - d_nitems_copied, noutput_items - produced);
            memcpy(out + produced, d_header + d_nitems_copied, n);
            add_item_tag(0, nitems_written(0) + produced, d_phy_tag_key,
              phy_tag::create()
                .set_nitems(d_datalen + d_mac_utils->get_headerlen() + 4)
                .set_rate(d_rate));
            break;
          case COPY_DATA:
            if (d_nitems_copied == 0)
              d_nitems_to_copy = d_datalen;
            n = std::min(std::min(d_nitems_to_copy - d_nitems_copied, noutput_items - produced),
              ninput_items[0] - consumed);
            if (n == 0) // not enough input items --> exit general_work
              goto endwhile;

            memcpy(out + produced, in, n);
            d_mac_utils->put_data(in, n, d_nitems_copied);
            d_mac_utils->update_fcs(in, n);
            in += n;
            consumed += n;
            break;
          case GENERATE_FCS:
            if (d_nitems_copied == 0)
              d_nitems_to_copy = 4;
            n = std::min(d_nitems_to_copy - d_nitems_copied, noutput_items - produced);
            memcpy(out + produced, d_mac_utils->get_fcs() + d_nitems_copied, n);
            break;
        }

        d_nitems_copied += n;
        produced += n;

        if (d_nitems_copied >= d_nitems_to_copy) {
          d_state = (d_state + 1) % NUM_STATES;
          d_nitems_copied = 0;
        }
      }

      endwhile:

      consume(0, consumed);
      return produced;
    }

  } /* namespace swifi */
} /* namespace gr */

