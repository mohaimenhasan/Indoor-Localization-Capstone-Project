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


#ifndef INCLUDED_SWIFI_MAC_UTILS_H
#define INCLUDED_SWIFI_MAC_UTILS_H

#include <swifi/api.h>
#include <swifi/mac_frame_info.h>
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/crc.hpp>
#include <stdio.h>


// disable SWIG warning for not supporting nested union/struct
#pragma SWIG nowarn=312



namespace gr {
  namespace swifi {

#define MAC_ADDR_LEN  6

#define MAC_ADDR1 0
#define MAC_ADDR2 1
#define MAC_ADDR3 2
#define MAC_MAX_ADDRS 3

#pragma pack(push, 1)
    struct mac80211_frame_header {
      union {
        uint16_t fc16;
        mac80211_frame_control_field fc;
      };  // anonymous union
      uint16_t duration;
      unsigned char addr[3][MAC_ADDR_LEN];
      uint16_t seq; // sequence control
    };

    struct mac80211_beacon_frame {
      uint64_t timestamp;
      uint16_t beacon_interval;
      uint16_t capability;
    };

    struct ie_ht_cap {
      uint16_t info;
      uint8_t ampdu_params;
      uint8_t mcs_set[16];
      uint16_t extended_cap;
      uint32_t tx_beamforming;
      uint8_t asel_cap;
    };
#pragma pack(pop)


    /*!
     * \brief MAC Header utility for IEEE 802.11a.
     */
    class SWIFI_API mac_utils
    {
     public:
      typedef boost::shared_ptr<mac_utils> sptr;

      static sptr make();

      mac_utils();
      ~mac_utils();

      /*!
       * Embed header into MPDU and return pointer to the header.
       * The MPDU contained at this pointer is in proper order
       * specified by IEEE 802.11 and ready to be transmitted.
       */
      char* generate_header();

      /*!
       * Embed tail into MPDU and return point to the tail
       * which contains the proper byte-order FCS.
       */
      char* generate_fcs();

      /*!
       * Return the current value of address.
       * \param index which address to query.
       * \return address as a vector of 6 numbers.
       */
      std::vector<int> get_addr(int index);

      /*!
       * Get pointer to the MPDU's data.
       */
      char *get_data();

      /*!
       * Returns the size of MPDU's frame body.
       */
      int get_datalen();

      /*!
       * Return value of Duration field.
       */
      int get_duration();

      /*!
       * Return value of From_DS field.
       * \return From_DS value.
       */
      int get_fromDS();

      /*!
       * Get pointer to the MPDU's header. The frame contained at
       * the returned pointer is already in proper byte-order
       * specified by IEEE 802.11, and therefore, is ready to be transmitted.
       */
      char *get_header();

      /*!
       * Returns the size of MPDU's header.
       */
      int get_headerlen();

      /*!
       * Return value of Sequence Control field.
       */
      int get_seq();

      /*!
       * Get pointer to the MPDU's tail.
       */
      char *get_fcs();

      /*!
       * Return value of To_DS field.
       * \return To_DS value.
       */
      int get_toDS();

      /*!
       * Returns frame type.
       */
      int get_type();

      /*!
       * Returns frame subtype.
       */
      int get_subtype();

      


      /*!
       * Returns value of Duration field.
       * \param bytes receive bytes
       */
      uint16_t get_duration(const char *bytes);
      
      /*!
       * Returns Frame Control field.
       */
      mac80211_frame_control_field get_control_field(const char *bytes);

      /*!
       * Parse header from a received array of bytes whose
       * order is specified by IEEE 802.11.
       * \param recv received bytes.
       * \param len number of bytes of received MAC frame.
       * \return true if checksum is correct, false otherwise.
       */
      bool check_recv_bytes(const char *recv, int len);

      /*!
       * Parse header from a received array of bytes whose
       * order is specified by IEEE 802.11.
       * \param recv received bytes.
       * \param len number of bytes of received MAC frame.
       * \param info MAC frame information.
       * \return true if checksum is correct, false otherwise.
       */
      bool parse_recv_bytes(const char *recv, int len, 
        const phy_frame_rx_info &info);

      /*!
       * Parse management frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_management(const char *recv, int len);

      /*!
       * Parse management beacon frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_management_beacon(const char *recv, int len);

      /*!
       * Parse Control frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_control(const char *recv, int len);

      /*!
       * Parse Control RTS frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_control_rts(const char *recv, int len);

      /*!
       * Parse Control CTS frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_control_cts(const char *recv, int len);

      /*!
       * Parse Control ACK frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_control_ack(const char *recv, int len);

      /*!
       * Parse Control PS-Poll frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_control_pspoll(const char *recv, int len);

      /*!
       * Parse Control CF-End frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_control_cfend(const char *recv, int len);

      /*!
       * Parse Control CF-End+CF-Ack frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_control_cfend_cfack(const char *recv, int len);

      /*!
       * Parse Control BlockAckReq frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_control_block_ack_req(const char *recv, int len);

      /*!
       * Parse Data frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_data(const char *recv, int len);

      /*!
       * Parse Data Data frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_data_data(const char *recv, int len);

      /*!
       * Parse data QoS Data frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */
      void parse_data_qos_data(const char *recv, int len);





      /*!
       * Parse ies in management frame from received bytes (lowest-order byte first).
       * \param recv received bytes.
       * \param len number of bytes.
       */  
      void parse_ies(const char *recv, int len);

      /*!
       * Put data of a given size into the payload at a given position.
       * \param data contains the user data.
       * \param len size of the data chunk to put in.
       * \param where_to_put where in the payload to put data in.
       */
      void put_data(const char *data, int len, int where_to_put);

      /*!
       * Reset FCS part.
       */
      void reset_fcs();

      /*!
       * Return MPDU as a sequence of bytes in correct order
       * specified by IEEE 802.11. The returned sequence is
       * ready to embed into PHY layer.
       */
      std::vector<char> serialize();

      /*!
       * Set MAC address fields. See IEEE80211-8.2.4.3.
       * \param index which address to set. Currently only 3 addresses are
       *              supported. To specify which address, use
       *              MAC_ADDR1, MAC_ADDR2, or MAC_ADDR3.
       * \param addr contains 6 number elements of the address, each should have
       *             value between 0-255.
       */
      void set_addr(int index, const std::vector<int>& addr);

      /*!
       * Set MAC address from a string type address.
       * \param index which address to set. Currently only 3 addresses are
       *              supported. To specify which address, use
       *              MAC_ADDR1, MAC_ADDR2, or MAC_ADDR3.
       * \param addr address in format "xx:xx:xx:xx:xx:xx".
       */
      void set_addr(int index, const std::string& addr);

      /*!
       * Set MAC frame's body.
       * \param data user data.
       * \param len data size in bytes.
       */
      void set_data(const char *data, int len);

      /*!
       * Prepare MPDU buffer to contain the data with given size.
       * \param len size of the data.
       */
      void set_datalen(int len);

      /*!
       * Set Duration field.
       * \param duration value of Duration field.
       */
      void set_duration(int duration);

      /*!
       * Set Sequence Control field.
       * \param seq value of Sequence Control field.
       */
      void set_seq(int seq);

      /*!
       * Set To_DS and From_DS fields. See IEEE80211-8.2.4.1.4.
       * \param toDS value of To_DS (0 or 1).
       * \param fromDS value of From_DS (0 or 1).
       */
      void set_toDS_fromDS(int toDS, int fromDS);

      /*!
       * Set frame type and subtype.
       * \param type frame type.
       * \param subtype frame subtype.
       */
      void set_type(int type, int subtype);

      /*!
       * Update the tail part of the MPDU with new user data
       * appending to the body.
       * \param data data appending to the body.
       * \param len size of the data.
       * \return the pointer to the MPDU tail part.
       */
      char* update_fcs(const char *data, int len);

      //void collect_statistics(mac80211_frame_header d_hdr, unsigned long time_rx_frame);


     protected:
      mac80211_frame_header d_hdr;
      std::vector<char> d_mpdu;
      int d_data_len;
      boost::crc_32_type d_crc;
      FILE *d_fstat;
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_MAC_UTILS_H */

