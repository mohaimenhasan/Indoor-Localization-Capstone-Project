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


#ifndef INCLUDED_SWIFI_MAC_FRAME_INFO_H
#define INCLUDED_SWIFI_MAC_FRAME_INFO_H

#include <swifi/api.h>
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
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





    enum mac80211_frame_type {
      FRAME_TYPE_MANAGEMENT,
      FRAME_TYPE_CONTROL,
      FRAME_TYPE_DATA,
      FRAME_TYPE_RESERVED,
      NUM_FRAME_TYPES
    };

    static const char *mac80211_frame_type_str[NUM_FRAME_TYPES] = {
      "MANAGEMENT", "CONTROL", "DATA", "RESERVED"
    };

    enum mac80211_frame_management_subtype {
      FRAME_SUBTYPE_MANAGEMENT_ASSOC_REQ,
      FRAME_SUBTYPE_MANAGEMENT_ASSOC_RES,
      FRAME_SUBTYPE_MANAGEMENT_REASSOC_REQ,
      FRAME_SUBTYPE_MANAGEMENT_REASSOC_RES,
      FRAME_SUBTYPE_MANAGEMENT_PROBE_REQ,
      FRAME_SUBTYPE_MANAGEMENT_PROBE_RES,
      FRAME_SUBTYPE_MANAGEMENT_TIMING_ADV,
      FRAME_SUBTYPE_MANAGEMENT_RESERVED1,
      FRAME_SUBTYPE_MANAGEMENT_BEACON,
      FRAME_SUBTYPE_MANAGEMENT_ATIM,
      FRAME_SUBTYPE_MANAGEMENT_DISASSOC,
      FRAME_SUBTYPE_MANAGEMENT_AUTH,
      FRAME_SUBTYPE_MANAGEMENT_DEAUTH,
      FRAME_SUBTYPE_MANAGEMENT_ACTION,
      FRAME_SUBTYPE_MANAGEMENT_ACTION_NOACK,
      FRAME_SUBTYPE_MANAGEMENT_RESERVED2,
      NUM_FRAME_SUBTYPE_MANAGEMENT
    };

    static const char *mac80211_frame_management_subtype_str[NUM_FRAME_SUBTYPE_MANAGEMENT] = {
      "ASSOC_REQ",
      "ASSOC_RES",
      "REASSOC_REQ",
      "REASSOC_RES",
      "PROBE_REQ",
      "PROBE_RES",
      "TIMING_ADV",
      "RESERVED1",
      "BEACON",
      "ATIM",
      "DISASSOC",
      "AUTH",
      "DEAUTH",
      "ACTION",
      "ACTION_NOACK",
      "RESERVED2",
    };

    enum mac80211_frame_control_subtype {
      FRAME_SUBTYPE_CONTROL_RESERVED0,
      FRAME_SUBTYPE_CONTROL_RESERVED1,
      FRAME_SUBTYPE_CONTROL_RESERVED2,
      FRAME_SUBTYPE_CONTROL_RESERVED3,
      FRAME_SUBTYPE_CONTROL_RESERVED4,
      FRAME_SUBTYPE_CONTROL_RESERVED5,
      FRAME_SUBTYPE_CONTROL_RESERVED6,
      FRAME_SUBTYPE_CONTROL_WRAPPER,
      FRAME_SUBTYPE_CONTROL_BLOCK_ACK_REQ,
      FRAME_SUBTYPE_CONTROL_BLOCK_ACK,
      FRAME_SUBTYPE_CONTROL_PSPOLL,
      FRAME_SUBTYPE_CONTROL_RTS,
      FRAME_SUBTYPE_CONTROL_CTS,
      FRAME_SUBTYPE_CONTROL_ACK,
      FRAME_SUBTYPE_CONTROL_CFEND,
      FRAME_SUBTYPE_CONTROL_CFEND_CFACK,
      NUM_FRAME_SUBTYPE_CONTROL
    };

    static const char *mac80211_frame_control_subtype_str[NUM_FRAME_SUBTYPE_CONTROL] = {
      "RESERVED0",
      "RESERVED1",
      "RESERVED2",
      "RESERVED3",
      "RESERVED4",
      "RESERVED5",
      "RESERVED6",
      "WRAPPER",
      "BLOCK_ACK_REQ",
      "BLOCK_ACK",
      "PSPOLL",
      "RTS",
      "CTS",
      "ACK",
      "CFEND",
      "CFEND_CFACK",
    };

    enum mac80211_frame_data_subtype {
      FRAME_SUBTYPE_DATA_DATA,
      FRAME_SUBTYPE_DATA_DATA_CFACK,
      FRAME_SUBTYPE_DATA_DATA_CFPOLL,
      FRAME_SUBTYPE_DATA_DATA_CFACKPOLL,
      FRAME_SUBTYPE_DATA_NULL,
      FRAME_SUBTYPE_DATA_CFACK,
      FRAME_SUBTYPE_DATA_CFPOLL,
      FRAME_SUBTYPE_DATA_CFACKPOLL,
      FRAME_SUBTYPE_DATA_QOS_DATA,
      FRAME_SUBTYPE_DATA_QOS_DATA_CFACK,
      FRAME_SUBTYPE_DATA_QOS_DATA_CFPOLL,
      FRAME_SUBTYPE_DATA_QOS_DATA_CFACKPOLL,
      FRAME_SUBTYPE_DATA_QOS_NULL,
      FRAME_SUBTYPE_DATA_RESERVED,
      FRAME_SUBTYPE_DATA_QOS_CFPOLL,
      FRAME_SUBTYPE_DATA_QOS_CFACKPOLL,
      NUM_FRAME_SUBTYPE_DATA
    };

    static const char *mac80211_frame_data_subtype_str[NUM_FRAME_SUBTYPE_DATA] = {
      "DATA",
      "DATA_CFACK",
      "DATA_CFPOLL",
      "DATA_CFACKPOLL",
      "NULL",
      "CFACK",
      "CFPOLL",
      "CFACKPOLL",
      "QOS_DATA",
      "QOS_DATA_CFACK",
      "QOS_DATA_CFPOLL",
      "QOS_DATA_CFACKPOLL",
      "QOS_NULL",
      "RESERVED",
      "QOS_CFPOLL",
      "QOS_CFACKPOLL",
    };

    enum mac80211_information_element_id {
      IE_SSID = 0,
      IE_SUPPORTED_RATES,
      IE_FH_PARAMETER_SET,
      IE_DSSS_PARAMETER_SET,
      IE_CF_PARAMETER_SET,
      IE_TIM,
      IE_IBSS_PARAMETER_SET,
      IE_COUNTRY,
      IE_HOPPING_PATTERN_PARAMETERS,
      IE_HOPPING_PATTERN_TABLE,
      IE_REQUEST,
      IE_BSS_LOAD,
      IE_EDCA_PARAMETER_SET,
      IE_TSPEC,
      IE_TCLAS,
      IE_SCHEDULE,
      IE_CHALLENGE_TEXT,
      IE_POWER_CONSTRAINT = 32,
      IE_POWER_CAPABILITY,
      IE_TPC_REQUEST,
      IE_TPC_REPORT,
      IE_SUPPORTED_CHANNELS,
      IE_CHANNEL_SWITCH_ANNOUNCEMENT,
      IE_MEASUREMENT_REQUEST,
      IE_MEASUREMENT_REPORT,
      IE_QUIET,
      IE_IBSS_DFS,
      IE_ERP,
      IE_TS_DELAY,
      IE_TCLAS_PROCESSING,
      IE_HT_CAPABILITIES,
      IE_QOS_CAPABILITY,
      IE_RSN = 48,
      IE_EXTENDED_SUPPORTED_RATES = 50,
      IE_AP_CHANNEL_REPORT,
      IE_NEIGHBOR_REPORT,
      IE_RCPI,
      IE_MOBILITY_DOMAIN,
      IE_FAST_BSS_TRANSITION,
      IE_TIMEOUT_INTERVAL,
      IE_RIC_DATA,
      IE_DSE_REGISTERED_LOCATION,
      IE_SUPPORTED_OPERATING_CLASSES,
      IE_EXTENDED_CHANNEL_SWITCH_ANNOUNCEMENT,
      IE_HT_OPERATION,
      IE_SECONDARY_CHANNEL_OFFSET,
      IE_BSS_AVERAGE_ACCESS_DELAY,
      IE_ANTENNA,
      IE_RSNI,
      IE_MEASUREMENT_PILOT_TRANSMISSION,
      IE_BSS_AVAILABLE_ADMISSION_CAPACITY,
      IE_BSS_AC_ACCESS_DELAY,
      IE_TIME_ADVERTISEMENT,
      IE_RM_ENABLED_CAPABILITIES,
      IE_MULTIPLE_BSSID,
      IE_20_40_BSS_COEXISTENCE,
      IE_20_40_BSS_INTOLERANT_CHANNEL_REPORT,
      IE_OVERLAPPING_BSS_SCAN_PARAMETERS,
      IE_RIC_DESCRIPTOR,
      IE_MANAGEMENT_MIC,
      IE_EVENT_REQUEST = 78,
      IE_EVENT_REPORT,
      IE_DIAGNOSTIC_REQUEST,
      IE_DIAGNOSTIC_REPORT,
      IE_LOCATION_PARAMETERS,
      IE_NONTRANSMITTED_BSSID_CAPABILITY,
      IE_SSID_LIST,
      IE_MULTIPLE_BSSID_INDEX,
      IE_FMS_DESCRIPTOR,
      IE_FMS_REQUEST,
      IE_FMS_RESPONSE,
      IE_QOS_TRAFFICE_CAPABILITY,
      IE_BSS_MAX_IDLE_PERIOD,
      IE_TFS_REQUEST,
      IE_TFS_RESPONSE,
      IE_WNM_SLEEP_MODE,
      IE_TIM_BROADCAST_REQUEST,
      IE_TIM_BROADCAST_RESPONSE,
      IE_COLLOCATED_INTERFERENCE_REPORT,
      IE_CHANNEL_USAGE,
      IE_TIME_ZONE,
      IE_DMS_REQUEST,
      IE_DMS_RESPONSE,
      IE_LINK_IDENTIFIER,
      IE_WAKEUP_SCHEDULE,
      IE_CHANNEL_SWITCH_TIMING = 104,
      IE_PTI_CONTROL,
      IE_TPU_BUFFER_STATUS,
      IE_INTERWORKING,
      IE_ADVERTISEMENT_PROTOCOL,
      IE_EXPEDITED_BANDWIDTH_REQUEST,
      IE_QOS_MAP_SET,
      IE_ROAMING_CONSORTIUM,
      IE_EMERGENCY_ALERT_IDENTIFIER,
      IE_MESH_CONFIGURATION,
      IE_MESH_ID,
      IE_MESH_LINK_METRIC_REPORT,
      IE_CONGESTION_NOTIFICATION,
      IE_MESH_PEERING_MANAGEMENT,
      IE_MESH_CHANNEL_SWITCH_PARAMETERS,
      IE_MESH_AWAKE_WINDOW,
      IE_BEACON_TIMING,
      IE_MCCAOP_SETUP_REQUEST,
      IE_MCCAOP_SETUP_REPLY,
      IE_MCCAOP_ADVERTISEMENT,
      IE_MCCAOP_TEARDOWN,
      IE_GANN,
      IE_RANN,
      IE_EXTENDED_CAPABILITIES,
      IE_PREQ = 130,
      IE_PREP,
      IE_PERR,
      IE_PXU = 137,
      IE_PXUC,
      IE_AUTHENTICATED_MESH_PEERING_EXCHANGE,
      IE_MIC,
      IE_DESTINATION_URI,
      IE_UAPSD_COEXISTENCE,
      IE_MCCAOP_ADVERTISEMENT_OVERVIEW = 174,
      IE_VENDOR_SPECIFIC = 221
    };

    static std::map<int,const char*> init_ie_id_str() {
      std::map<int,const char*> s;
      int id = 0;
      s[id++] = "SSID";
      s[id++] = "SUPPORTED_RATES";
      s[id++] = "FH_PARAMETER_SET";
      s[id++] = "DSSS_PARAMETER_SET";
      s[id++] = "CF_PARAMETER_SET";
      s[id++] = "TIM";
      s[id++] = "IBSS_PARAMETER_SET";
      s[id++] = "COUNTRY";
      s[id++] = "HOPPING_PATTERN_PARAMETERS";
      s[id++] = "HOPPING_PATTERN_TABLE";
      s[id++] = "REQUEST";
      s[id++] = "BSS_LOAD";
      s[id++] = "EDCA_PARAMETER_SET";
      s[id++] = "TSPEC";
      s[id++] = "TCLAS";
      s[id++] = "SCHEDULE";
      s[id++] = "CHALLENGE_TEXT";

      id = 32;
      s[id++] = "POWER_CONSTRAINT";
      s[id++] = "POWER_CAPABILITY";
      s[id++] = "TPC_REQUEST";
      s[id++] = "TPC_REPORT";
      s[id++] = "SUPPORTED_CHANNELS";
      s[id++] = "CHANNEL_SWITCH_ANNOUNCEMENT";
      s[id++] = "MEASUREMENT_REQUEST";
      s[id++] = "MEASUREMENT_REPORT";
      s[id++] = "QUIET";
      s[id++] = "IBSS_DFS";
      s[id++] = "ERP";
      s[id++] = "TS_DELAY";
      s[id++] = "TCLAS_PROCESSING";
      s[id++] = "HT_CAPABILITIES";
      s[id++] = "QOS_CAPABILITY";

      id = 48;
      s[id++] = "RSN";

      id = 50;
      s[id++] = "EXTENDED_SUPPORTED_RATES";
      s[id++] = "AP_CHANNEL_REPORT";
      s[id++] = "NEIGHBOR_REPORT";
      s[id++] = "RCPI";
      s[id++] = "MOBILITY_DOMAIN";
      s[id++] = "FAST_BSS_TRANSITION";
      s[id++] = "TIMEOUT_INTERVAL";
      s[id++] = "RIC_DATA";
      s[id++] = "DSE_REGISTERED_LOCATION";
      s[id++] = "SUPPORTED_OPERATING_CLASSES";
      s[id++] = "EXTENDED_CHANNEL_SWITCH_ANNOUNCEMENT";
      s[id++] = "HT_OPERATION";
      s[id++] = "SECONDARY_CHANNEL_OFFSET";
      s[id++] = "BSS_AVERAGE_ACCESS_DELAY";
      s[id++] = "ANTENNA";
      s[id++] = "RSNI";
      s[id++] = "MEASUREMENT_PILOT_TRANSMISSION";
      s[id++] = "BSS_AVAILABLE_ADMISSION_CAPACITY";
      s[id++] = "BSS_AC_ACCESS_DELAY";
      s[id++] = "TIME_ADVERTISEMENT";
      s[id++] = "RM_ENABLED_CAPABILITIES";
      s[id++] = "MULTIPLE_BSSID";
      s[id++] = "20_40_BSS_COEXISTENCE";
      s[id++] = "20_40_BSS_INTOLERANT_CHANNEL_REPORT";
      s[id++] = "OVERLAPPING_BSS_SCAN_PARAMETERS";
      s[id++] = "RIC_DESCRIPTOR";
      s[id++] = "MANAGEMENT_MIC";

      id = 78;
      s[id++] = "EVENT_REQUEST";
      s[id++] = "EVENT_REPORT";
      s[id++] = "DIAGNOSTIC_REQUEST";
      s[id++] = "DIAGNOSTIC_REPORT";
      s[id++] = "LOCATION_PARAMETERS";
      s[id++] = "NONTRANSMITTED_BSSID_CAPABILITY";
      s[id++] = "SSID_LIST";
      s[id++] = "MULTIPLE_BSSID_INDEX";
      s[id++] = "FMS_DESCRIPTOR";
      s[id++] = "FMS_REQUEST";
      s[id++] = "FMS_RESPONSE";
      s[id++] = "QOS_TRAFFICE_CAPABILITY";
      s[id++] = "BSS_MAX_IDLE_PERIOD";
      s[id++] = "TFS_REQUEST";
      s[id++] = "TFS_RESPONSE";
      s[id++] = "WNM_SLEEP_MODE";
      s[id++] = "TIM_BROADCAST_REQUEST";
      s[id++] = "TIM_BROADCAST_RESPONSE";
      s[id++] = "COLLOCATED_INTERFERENCE_REPORT";
      s[id++] = "CHANNEL_USAGE";
      s[id++] = "TIME_ZONE";
      s[id++] = "DMS_REQUEST";
      s[id++] = "DMS_RESPONSE";
      s[id++] = "LINK_IDENTIFIER";
      s[id++] = "WAKEUP_SCHEDULE";

      id = 104;
      s[id++] = "CHANNEL_SWITCH_TIMING";
      s[id++] = "PTI_CONTROL";
      s[id++] = "TPU_BUFFER_STATUS";
      s[id++] = "INTERWORKING";
      s[id++] = "ADVERTISEMENT_PROTOCOL";
      s[id++] = "EXPEDITED_BANDWIDTH_REQUEST";
      s[id++] = "QOS_MAP_SET";
      s[id++] = "ROAMING_CONSORTIUM";
      s[id++] = "EMERGENCY_ALERT_IDENTIFIER";
      s[id++] = "MESH_CONFIGURATION";
      s[id++] = "MESH_ID";
      s[id++] = "MESH_LINK_METRIC_REPORT";
      s[id++] = "CONGESTION_NOTIFICATION";
      s[id++] = "MESH_PEERING_MANAGEMENT";
      s[id++] = "MESH_CHANNEL_SWITCH_PARAMETERS";
      s[id++] = "MESH_AWAKE_WINDOW";
      s[id++] = "BEACON_TIMING";
      s[id++] = "MCCAOP_SETUP_REQUEST";
      s[id++] = "MCCAOP_SETUP_REPLY";
      s[id++] = "MCCAOP_ADVERTISEMENT";
      s[id++] = "MCCAOP_TEARDOWN";
      s[id++] = "GANN";
      s[id++] = "RANN";
      s[id++] = "EXTENDED_CAPABILITIES";

      id = 130;
      s[id++] = "PREQ";
      s[id++] = "PREP";
      s[id++] = "PERR";

      id = 137;
      s[id++] = "PXU";
      s[id++] = "PXUC";
      s[id++] = "AUTHENTICATED_MESH_PEERING_EXCHANGE";
      s[id++] = "MIC";
      s[id++] = "DESTINATION_URI";
      s[id++] = "UAPSD_COEXISTENCE";

      id = 174;
      s[id++] = "MCCAOP_ADVERTISEMENT_OVERVIEW";

      id = 221;
      s[id++] = "VENDOR_SPECIFIC";

      return s;
    }

    static std::map<int,const char*> mac80211_information_element_id_str = init_ie_id_str();





#pragma pack(push, 1)
    struct mac80211_frame_control_field {
      unsigned char prot_ver : 2;  // protocol version
      unsigned char type : 2;  // type
      unsigned char subtype : 4; // subtype
      unsigned char toDS : 1;  // to DS
      unsigned char fromDS : 1; // from DS
      unsigned char more_frmts : 1; // more fragments
      unsigned char retry : 1; // retry
      unsigned char pwr_mngt : 1; // power management
      unsigned char more_data : 1;  // more data
      unsigned char protected_frame : 1;  // protected (encrypted) frame
      unsigned char order : 1;  // order

      bool is_qos_data() const {
        return (type == FRAME_TYPE_DATA &&
          ((subtype >> 3) & 1) && subtype != FRAME_SUBTYPE_DATA_RESERVED);
      }

    };

    struct mac80211_sequence_control_field {
      uint8_t fragment_no : 4;
      uint16_t seq_no : 12;
    };

    struct mac80211_qos_control_field {
      uint8_t tid : 4;
      uint8_t eosp : 1;
      uint8_t ack_policy : 2;
      uint8_t amsdu : 1;
      union {
        uint8_t txop;
        uint8_t ap_ps_buf_state;
        uint8_t queue_size;
        struct {
          uint8_t mesh_control_present : 1;
          uint8_t mesh_power_save_level : 1;
          uint8_t rspi : 1;
          uint8_t reserved : 4;
        } mesh_control;
      };
    };

    struct mac80211_ht_control_field {
      struct {
        uint8_t reserved : 1;
        uint8_t trq : 1;
        uint8_t mai : 4;
        uint8_t mfsi : 3;
        uint8_t mfb_aselc : 7;
      } link_adaptation;
      uint8_t calibration_position : 2;
      uint8_t calibration_sequence : 2;
      uint8_t reserved1 : 2;
      uint8_t csi : 2;
      uint8_t ndp : 1;
      uint8_t reserved2 : 5;
      uint8_t ac_constraint : 1;
      uint8_t rdg_more_ppdu : 1;
    };

#pragma pack(pop)


    struct phy_frame_rx_info {
      int mode; // OFDM or DSSS mode
      int rate; // rate used to transmit this frame
      int psdu_size;  // size of PSDU
      unsigned long time_start;  // time when received this frame
      unsigned long time_end;  // time when finish this frame
      unsigned long sample_start;  // index of the first sample of the frame
      unsigned long sample_end;  // index of the last sample of the frame
      unsigned long payload_sample_start;  // first sample's index in payload stream
      unsigned long payload_sample_end; // last sample's index in payload stream
      float freq_offset;  // carrier frequency offset
      float sfo;  // sampling frequency offset
      int scrambling_seed;
      boost::shared_array<char> data_tail_padding_bytes;

      phy_frame_rx_info() {}
      
      phy_frame_rx_info(int mode, int rate, 
            unsigned long time_start,
            unsigned long time_end)
        : mode(mode), rate(rate), 
          time_start(time_start), 
          time_end(time_end)
      {
      }

    };




    struct data_frame_info {
      char *data;
      int len;
    };

    struct control_bar_info {
      uint16_t bar_control;
      boost::shared_array<char> bar_info;
      int bar_info_len;
    };

    struct control_ba_info {
      uint16_t ba_control;
      boost::shared_array<char> ba_info;
      int ba_info_len;
    };

    struct control_frame_info {
      boost::shared_ptr<control_bar_info> bar_info;
      boost::shared_ptr<control_ba_info> ba_info;
    };

    struct ie_dsss_parameter_set_info {
      int channel;
    };

    struct ie_country_info {
      std::string code;
      std::vector<uint8_t> first_channel, num_channels, max_txpwr;
    };

#pragma pack(push, 1)
    struct ie_erp_info {
      uint8_t nonerp : 1;
      uint8_t use_protection : 1;
      uint8_t barker : 1;
      uint8_t reserved : 5;
    };

    struct ie_ht_capabilities_info {
      struct {
        unsigned char ldpc : 1;
        unsigned char channel40mhz : 1; // 0: 20MHz, 1: 40MHz
        unsigned char sm_pwr_save : 2;  // 0:static, 1:dynamic, 2:reserved, 3:disabled
        unsigned char ht_green : 1;
        unsigned char short_gi_20mhz : 1;
        unsigned char short_gi_40mhz : 1;
        unsigned char tx_stbc : 1;
        unsigned char rx_stbc : 2;  // n=0..3: up to n streams
        unsigned char ht_delayed_block_ack : 1;
        unsigned char max_amsdu_len : 1;  // 0 for 3839 octets, 1 for 7935 octets
        unsigned char dsss40mhz : 1;
        unsigned char reserved : 1;
        unsigned char intorant40mhz : 1;
        unsigned char lsig_txop : 1;
      } ht; // 2 bytes
      struct {
        unsigned char max_exponent : 2;
        unsigned char min_spacing : 3;
        unsigned char reserved : 3;
      } ampdu_params; // 1 byte
      struct {
        unsigned char rx_mcs_bitmask[10];  // 77 bitmask bits + 3 reserved bits
        uint16_t rx_highest_rate : 10;  // 0-1023 Mbps
        unsigned char reserved1 : 6;
        unsigned char tx_mcs_defined : 1;
        unsigned char tx_rx_mcs_not_equal : 1;
        unsigned char tx_max_streams : 2; // n=0..3: n+1 streams
        unsigned char tx_unequal_modulation : 1;
        uint32_t reserved2 : 27;
        std::vector<int> get_mcs_set() const {
          std::vector<int> mcs;
          for (int i = 0; i < 77; i++) {
            if (rx_mcs_bitmask[i/8] & (i << (i % 8)))
              mcs.push_back(i);
          }
          return mcs;
        }
      } mcs;  // 16 bytes
      struct {
        unsigned char pco : 1;
        unsigned char pco_transition_time : 2;
        unsigned char reserved1 : 5;
        unsigned char mcs_feedback : 2;
        unsigned char htc_support : 1;
        unsigned char rd_responder : 1;
        unsigned char reserved2 : 4;
      } ht_extended;  // 2 bytes
      uint32_t tx_beamforming;  // 4 bytes
      unsigned char asel_cap; // 1 byte
    };
#pragma pack(pop)


    struct ie_info {
      std::map<int,bool> has_ie;
      
      boost::shared_ptr<std::string> ssid;
      boost::shared_ptr<std::vector<float> > supported_rates;
      boost::shared_ptr<ie_dsss_parameter_set_info> dsss_parameter_set;
      boost::shared_ptr<ie_country_info> country;
      boost::shared_ptr<ie_erp_info> erp;
      boost::shared_ptr<ie_ht_capabilities_info> ht;
    };

    struct management_frame_info {
      uint64_t timestamp;
      uint16_t beacon_interval;
      struct {
        uint8_t ess : 1;
        uint8_t ibss : 1;
        uint8_t cf_pollable : 1;
        uint8_t cf_pollreq : 1;
        uint8_t privacy : 1;
        uint8_t short_preamble : 1;
        uint8_t pbcc : 1;
        uint8_t channel_agility : 1;
        uint8_t spectrum_mgmt : 1;
        uint8_t qos : 1;
        uint8_t short_slot : 1;
        uint8_t apsd : 1;
        uint8_t radio_measurement : 1;
        uint8_t dsss_ofdm : 1;
        uint8_t delayed_block_ack : 1;
        uint8_t immediate_block_ack : 1;
      } capability;
      ie_info ie;
    };

    /*!
     * \brief MAC Frame Info.
     */
    struct mac_frame_info {
      boost::shared_array<char> mpdu;
      int mpdu_size;

      mac80211_frame_control_field fc;
      int duration;
      char addr[4][MAC_ADDR_LEN];
      mac80211_sequence_control_field seq_control;
      mac80211_qos_control_field qos_control;
      mac80211_ht_control_field ht_control;
      int fcs;
      bool correct;
      bool malformed;

      boost::shared_ptr<management_frame_info> management_frame;
      boost::shared_ptr<control_frame_info> control_frame;
      boost::shared_ptr<data_frame_info> data_frame;

      phy_frame_rx_info rx_info;
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_MAC_FRAME_INFO_H */

