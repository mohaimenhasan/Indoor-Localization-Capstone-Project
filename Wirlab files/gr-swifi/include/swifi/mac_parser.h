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


#ifndef INCLUDED_SWIFI_MAC_PARSER_H
#define INCLUDED_SWIFI_MAC_PARSER_H

#include <swifi/api.h>
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/crc.hpp>
#include <stdio.h>
#include <swifi/mac_frame_info.h>

namespace gr {
  namespace swifi {

    /*!
     * \brief MAC Header utility for IEEE 802.11a.
     */
    class SWIFI_API mac_parser
    {
     protected:
      mac_frame_info info;
      int read_pos;

      char* next(int size);
      uint64_t next_uint64();
      uint32_t next_uint32();
      uint16_t next_uint16();
      uint8_t next_uint8();
      void next_uint64(void *dest);
      void next_uint32(void *dest);
      void next_uint16(void *dest);
      void next_chunk(int size, void *dest);
      std::string next_string(int size);
      int get_remaining_size();


      virtual void parse_management();
      virtual void parse_management_beacon();
      virtual void parse_ies();
      virtual void parse_ie_ssid(int elen);
      virtual void parse_ie_supported_rates(int elen);
      virtual void parse_ie_dsss_parameter_set(int elen);
      virtual void parse_ie_country(int elen);
      virtual void parse_ie_erp(int elen);
      virtual void parse_ie_ht_capabilities(int elen);

      virtual void parse_control();
      virtual void parse_control_block_ack_req();
      virtual void parse_control_block_ack();

      virtual void parse_data();



     public:
      typedef boost::shared_ptr<mac_parser> sptr;

      static sptr make();


      mac_parser();
      ~mac_parser();


      /*!
       * Parse header from a received array of bytes whose
       * order is specified by IEEE 802.11.
       * \param recv received bytes.
       * \param len number of bytes of received MAC frame.
       * \return true if checksum is correct, false otherwise.
       */
      virtual bool check_fcs(const char *recv, int len);

      /*!
       * Parse header from a received array of bytes whose
       * order is specified by IEEE 802.11.
       * \param recv received bytes.
       * \param len number of bytes of received MAC frame.
       * \param info MAC frame information.
       * \return true if checksum is correct, false otherwise.
       */
      virtual mac_frame_info& parse(const char *recv, int len, 
                          const phy_frame_rx_info &info);

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_MAC_PARSER_H */

