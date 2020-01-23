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

#include <swifi/mac_parser.h>
#include <swifi/phy_utils.h>
#include <endian.h>
#include <string.h>
#include <stdio.h>

namespace gr {
  namespace swifi {


    // NOTE: keep in mind that the frame is parsed without
    // assuming that the frame is correctly received.
    // So when implementing parsing for new frame, field, information element, etc.
    // please take care of the case the input might be incorrect
    // (e.g., size of information element, ...)


    mac_parser::sptr
    mac_parser::make()
    {
      return mac_parser::sptr(new mac_parser());
    }

    mac_parser::mac_parser()
    {
    }

    mac_parser::~mac_parser()
    {
    }


    mac_frame_info& mac_parser::parse(const char *recv, int mpdu_size, 
      const phy_frame_rx_info &rx_info) 
    {
      try {
        // copy raw MPDU
        info.mpdu_size = mpdu_size;
        info.mpdu.reset(new char[mpdu_size]);
        memcpy(info.mpdu.get(), recv, mpdu_size);
        read_pos = 0;

        info.malformed = false;
        info.rx_info = rx_info;

        // check FCS
        info.correct = check_fcs(recv, mpdu_size);

        // get required fields
        next_chunk(2, &info.fc);
        info.duration = next_uint16();
        next_chunk(MAC_ADDR_LEN, info.addr[0]);

        // parse specific frame
        switch (info.fc.type) {
          case FRAME_TYPE_MANAGEMENT:
            parse_management();
            break;
          case FRAME_TYPE_CONTROL:
            parse_control();
            break;
          case FRAME_TYPE_DATA:
            parse_data();
            break;
        }
      }
      catch (...) {
        info.malformed = true;
      }

      return info;
    }


    bool mac_parser::check_fcs(const char *recv, int mpdu_size) {
      // check fcs
      boost::crc_32_type result;
      result.process_bytes(recv, mpdu_size - 4);
      uint32_t* fcs = (uint32_t*)(recv + mpdu_size - 4);
      return (le32toh((*fcs)) == result.checksum());
    }


/**************************************************************/
/************* CONTROL FRAME PARSING **************************/
/**************************************************************/

    void mac_parser::parse_control() {
      // Note: FC, Duration, Address 1 should have been parsed before this method is called

      info.control_frame.reset(new control_frame_info);

      switch (info.fc.subtype) {
        case FRAME_SUBTYPE_CONTROL_RTS:
        case FRAME_SUBTYPE_CONTROL_PSPOLL:
        case FRAME_SUBTYPE_CONTROL_CFEND:
        case FRAME_SUBTYPE_CONTROL_CFEND_CFACK:
          next_chunk(MAC_ADDR_LEN, info.addr[1]); // Address 2
          break;
        case FRAME_SUBTYPE_CONTROL_CTS:
        case FRAME_SUBTYPE_CONTROL_ACK:
          // no more additional fields to parse
          break;
        case FRAME_SUBTYPE_CONTROL_BLOCK_ACK_REQ:
          parse_control_block_ack_req();
          break;
        case FRAME_SUBTYPE_CONTROL_BLOCK_ACK:
          parse_control_block_ack();
          break;
      }
    }


    void mac_parser::parse_control_block_ack_req() {
        // Note: FC, Duration, Address 1 should have been parsed before this method is called
        next_chunk(MAC_ADDR_LEN, info.addr[1]); // Address 2
        info.control_frame->bar_info.reset(new control_bar_info);
        info.control_frame->bar_info->bar_control = next_uint16();
        info.control_frame->bar_info->bar_info_len = get_remaining_size();
        info.control_frame->bar_info->bar_info.reset(new char[info.control_frame->bar_info->bar_info_len]);
        next_chunk(info.control_frame->bar_info->bar_info_len,
          info.control_frame->bar_info->bar_info.get());
    }

    void mac_parser::parse_control_block_ack() {
        // Note: FC, Duration, Address 1 should have been parsed before this method is called
        next_chunk(MAC_ADDR_LEN, info.addr[1]); // Address 2
        info.control_frame->ba_info.reset(new control_ba_info);
        info.control_frame->ba_info->ba_control = next_uint16();
        info.control_frame->ba_info->ba_info_len = get_remaining_size();
        info.control_frame->ba_info->ba_info.reset(new char[info.control_frame->ba_info->ba_info_len]);
        next_chunk(info.control_frame->ba_info->ba_info_len,
          info.control_frame->ba_info->ba_info.get());
    }




/**************************************************************/
/****************** MANAGEMENT FRAME PARSING ******************/
/**************************************************************/

    void mac_parser::parse_management()
    {
      // Management frame format: IEEE802.11, Section 8.3.3.1
      // Note: FC, Duration, Address 1 should have been parsed before this method is called

      next_chunk(MAC_ADDR_LEN, info.addr[1]); // Address 2
      next_chunk(MAC_ADDR_LEN, info.addr[2]); // Address 3
      next_chunk(2, &info.seq_control);

      // check HT Control
      if (info.fc.order)
        next_chunk(4, &info.ht_control);

      info.management_frame.reset(new management_frame_info);
      printf("mac_parser management_frame\n");fflush(stdout);

      switch (info.fc.subtype) {
        case FRAME_SUBTYPE_MANAGEMENT_BEACON:
          parse_management_beacon();
          break;
      }
    }

    void mac_parser::parse_management_beacon() {
      info.management_frame->timestamp = next_uint64();
      info.management_frame->beacon_interval = next_uint16();
      next_uint16(&info.management_frame->capability);
      parse_ies();
    }

    void mac_parser::parse_ies() {
      unsigned char eid, elen;
      bool set_has_ie; 
      while (true) {
        eid = next_uint8();
        elen = next_uint8();
        set_has_ie = true;
        switch (eid) {
          case IE_SSID:
            parse_ie_ssid(elen);
            break;
          case IE_SUPPORTED_RATES:
          case IE_EXTENDED_SUPPORTED_RATES:
            parse_ie_supported_rates(elen);
            break;
          case IE_DSSS_PARAMETER_SET:
            parse_ie_dsss_parameter_set(elen);
            break;
          case IE_COUNTRY:
            parse_ie_country(elen);
            break;
          case IE_ERP:
            parse_ie_erp(elen);
            break;
          case IE_HT_CAPABILITIES:
            parse_ie_ht_capabilities(elen);
            break;
          default:
            set_has_ie = false;
            break;
        }
        if (set_has_ie)
          info.management_frame->ie.has_ie[eid] = true;
      }
    }

    void mac_parser::parse_ie_ssid(int elen) {
      info.management_frame->ie.ssid.reset(new std::string(next_string(elen)));
    }

    void mac_parser::parse_ie_supported_rates(int elen) {
      info.management_frame->ie.supported_rates.reset(new std::vector<float>());
      for (int j = 0; j < elen; j++) {
        unsigned char rate_id = next_uint8();
        float rate = 0.5f * (rate_id & 0x7F);
        info.management_frame->ie.supported_rates->push_back(rate);
      }
    }

    void mac_parser::parse_ie_dsss_parameter_set(int elen) {
      info.management_frame->ie.dsss_parameter_set.reset(
        new ie_dsss_parameter_set_info);
      info.management_frame->ie.dsss_parameter_set->channel = next_uint8();
    }

    void mac_parser::parse_ie_country(int elen) {
      info.management_frame->ie.country.reset(new ie_country_info);
      info.management_frame->ie.country->code = next_string(3);
      info.management_frame->ie.country->first_channel.clear();
      info.management_frame->ie.country->num_channels.clear();
      info.management_frame->ie.country->max_txpwr.clear();
      if (elen > 3) {
        for (int j = 0; j < (elen - 3) / 3; j++) {
          info.management_frame->ie.country->first_channel.push_back(next_uint8());
          info.management_frame->ie.country->num_channels.push_back(next_uint8());
          info.management_frame->ie.country->max_txpwr.push_back(next_uint8());
        }
      }
    }

    void mac_parser::parse_ie_erp(int elen) {
      info.management_frame->ie.erp.reset(new ie_erp_info);
      next_chunk(1, info.management_frame->ie.erp.get());
    }

    void mac_parser::parse_ie_ht_capabilities(int elen) {
      info.management_frame->ie.ht.reset(new ie_ht_capabilities_info);
      next_chunk(26, info.management_frame->ie.ht.get());
    }


/**************************************************************/
/**************** DATA FRAME PARSING **************************/
/**************************************************************/


    void mac_parser::parse_data() {
      // Data frame format: IEEE802.11, Section 8.3.2.1
      // Note: FC, Duration, Address 1 should have been parsed before this method is called

      next_chunk(MAC_ADDR_LEN, info.addr[1]); // Address 2
      next_chunk(MAC_ADDR_LEN, info.addr[2]); // Address 3
      next_chunk(2, &info.seq_control);

      // check Address 4
      if (info.fc.fromDS && info.fc.toDS) {
        next_chunk(MAC_ADDR_LEN, info.addr[3]); // Address 4
      }

      // check QOS Control and HT Control fields (both present only for QOS DATA)
      if (info.fc.subtype == FRAME_SUBTYPE_DATA_QOS_DATA) {
        next_chunk(2, &info.qos_control);
        if (info.fc.order)
          next_chunk(4, &info.ht_control);
      }

      // set frame body pointer
      info.data_frame.reset(new data_frame_info);
      info.data_frame->data = info.mpdu.get() + read_pos;
      info.data_frame->len = get_remaining_size();
    }



/**************************************************************/
/****************** HELPER FUNCTIONS **************************/
/**************************************************************/

    int mac_parser::get_remaining_size() {
      return std::max(info.mpdu_size - 4 - read_pos, 0);
    }

    char* mac_parser::next(int size) {
      if (read_pos + size > info.mpdu_size - 4)  { // 4 last bytes used for FCS
        printf("throw mpdu_size %d\n", info.mpdu_size);fflush(stdout);
        throw false;  // buffer not enough
      }
      char *p = info.mpdu.get() + read_pos;
      read_pos += size;
      return p;
    }

    uint64_t mac_parser::next_uint64() {
      return le64toh(*((uint64_t*)next(8)));
    }

    void mac_parser::next_uint64(void *dest) {
      uint64_t v = next_uint64();
      memcpy(dest, &v, 8);
    }

    uint32_t mac_parser::next_uint32() {
      return le32toh(*((uint32_t*)next(4)));
    }

    void mac_parser::next_uint32(void *dest) {
      uint32_t v = next_uint32();
      memcpy(dest, &v, 4);
    }
    
    uint16_t mac_parser::next_uint16() {
      return le16toh(*((uint16_t*)next(2)));
    }

    void mac_parser::next_uint16(void *dest) {
      uint16_t v = next_uint16();
      memcpy(dest, &v, 2);
    }

    uint8_t mac_parser::next_uint8() {
      return *next(1);
    }

    void mac_parser::next_chunk(int size, void *dest) {
      memcpy(dest, next(size), size);
    }

    std::string mac_parser::next_string(int size) {
      char buf[size+1];
      next_chunk(size, buf);
      buf[size] = 0;
      std::string s(buf, strlen(buf));
      return s;
    }

  } /* namespace swifi */
} /* namespace gr */

