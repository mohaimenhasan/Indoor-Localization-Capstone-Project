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

#include <swifi/ofdm_phy_header_parser.h>
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/digital/lfsr.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    ofdm_phy_header_parser::sptr
    ofdm_phy_header_parser::make(const std::string &frame_len_tag_key,
      const std::string& phy_tag_key)
    {
      return ofdm_phy_header_parser::sptr(
          new ofdm_phy_header_parser(frame_len_tag_key, phy_tag_key)
      );
    }

    ofdm_phy_header_parser::ofdm_phy_header_parser(const std::string &frame_len_tag_key,
      const std::string& phy_tag_key)
    : packet_header_default(3, "", "", 8),
      d_frame_len_tag_key(pmt::string_to_symbol(frame_len_tag_key)),
      d_phy_tag_key(pmt::intern(phy_tag_key))
    {
    }

    ofdm_phy_header_parser::~ofdm_phy_header_parser()
    {
    }

    bool ofdm_phy_header_parser::header_parser(
        const unsigned char *in,
        std::vector<tag_t> &tags)
    {
      static int correct = 0;
      static int wrong = 0;
      bool result;
      bool parity = false;
      phy80211_frame_header* hdr;
      uint32_t decoded;

      // decoding
      hdr = (phy80211_frame_header*)in;
      decoded = *((uint32_t*)in);

      printf("Header: %02X %02X %02X\n", in[0], in[1], in[2]);
      // checking parity
      for (int i = 0; i < 17; i++)
        parity ^= (bool)((decoded >> i) & 1);

      printf("OFDM PHY Header: RATE 0x%02X LENGTH %u RESERVED %d TAIL %d --> ",
        hdr->rate, hdr->length, hdr->reserved, hdr->tail);
      result = (parity == hdr->parity);
      if (!result) {
        printf("Parity check failed. ");
      }

      result = result && (hdr->rate >= 8);
      if (!result) 
      {
        printf("RATE not supported. ");
      }

      result = result && (hdr->length >= 0 && hdr->length < 4096);
      if (!result) {
        printf("Length invalid. ");
      }

      if (result) {
        correct++;
        printf("n_ofdm_syms %d", phy_utils::compute_n_ofdm_syms(hdr->rate, hdr->length));

        // process incoming tags
        long flag = PHY_SCRAMBLER_FLAG | PHY_TAIL_SET_FLAG;

        // create PHY tag
        phy_tag pt;
        pt.set_rx_time(0);

        for (int i = 0; i < tags.size(); i++) {
          // get PHY tag 
          if (pmt::eqv(tags[i].key, d_phy_tag_key)) {
            phy_tag::parse(tags[i].value, pt);
          }
        }

        // add new tags
        tag_t t;
        t.key = d_frame_len_tag_key;
        t.value = pmt::from_long(phy_utils::compute_n_ofdm_syms(hdr->rate, hdr->length));
        tags.push_back(t);

        t.key = d_phy_tag_key;
        t.value = pt.set_nitems(hdr->length).set_rate(hdr->rate).set_flag(flag);
        tags.push_back(t);
      }
      else {
        wrong++;
      }

      end:

      printf(" [Correct: %d/%d]\n", correct, correct + wrong);

      return result;
    }


  } /* namespace swifi */
} /* namespace gr */

