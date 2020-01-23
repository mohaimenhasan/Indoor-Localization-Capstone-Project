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


#ifndef INCLUDED_SWIFI_OFDM_PHY_HEADER_PARSER_H
#define INCLUDED_SWIFI_OFDM_PHY_HEADER_PARSER_H

#include <vector>
#include <swifi/api.h>
#include <gnuradio/digital/packet_header_default.h>

namespace gr {
  namespace swifi {

    /*!
     * \brief Header utility for OFDM signals.
     * \ingroup ofdm_blk
     */
    class SWIFI_API ofdm_phy_header_parser : public digital::packet_header_default
    {
     public:
      typedef boost::shared_ptr<ofdm_phy_header_parser> sptr;

      ofdm_phy_header_parser(const std::string &frame_len_tag_key,
        const std::string& phy_tag_key);
      ~ofdm_phy_header_parser();

      /*!
       * \brief Inverse function to header_formatter().
       */
      bool header_parser(
        const unsigned char *header,
        std::vector<tag_t> &tags);

      /*!
       * \param frame_len_tag_key The tag key used for the frame length (number of
       *                          OFDM symbols, this is the tag key required for the
       *                          frame equalizer etc.)
       * \param phy_tag_key The tag key used for PHY properties.
       */
      static sptr make(const std::string &frame_len_tag_key="frame_len",
        const std::string& phy_tag_key="phy");


     protected:
      pmt::pmt_t d_frame_len_tag_key; //!< Tag key of the additional frame length tag
      pmt::pmt_t d_phy_tag_key; //!< Tag key of RATE value
      static int inter[48];
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_OFDM_PHY_HEADER_PARSER_H */

