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

#include <swifi/mac_utils.h>
#include <swifi/phy_utils.h>
#include <endian.h>
#include <string.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    mac_utils::sptr
    mac_utils::make()
    {
      return mac_utils::sptr(new mac_utils());
    }

    mac_utils::mac_utils()
    {
      d_fstat = fopen("stat_out", "w");
      memset(&d_hdr, 0, sizeof(d_hdr));
      set_type(FRAME_TYPE_DATA, FRAME_SUBTYPE_DATA_DATA);
      d_data_len = 0;
    }

    mac_utils::~mac_utils()
    {
      fclose(d_fstat);
    }


    void print_frame_type(mac80211_frame_control_field fc) {
      printf("Frame Type: %s ", mac80211_frame_type_str[fc.type]);
      switch (fc.type) {
        case FRAME_TYPE_MANAGEMENT: 
          printf("%s", mac80211_frame_management_subtype_str[fc.subtype]);
          break;
        case FRAME_TYPE_CONTROL: 
          printf("%s", mac80211_frame_control_subtype_str[fc.subtype]);
          break;
        case FRAME_TYPE_DATA: 
          printf("%s", mac80211_frame_data_subtype_str[fc.subtype]);
          break;
      }
      printf("\n");
    }

    void print_frame_control(mac80211_frame_control_field fc) {
      uint16_t fc16 = *((uint16_t*)&fc);
      if (fc16 >> 8) {
        printf("\tFrame Control: %04X [ ", fc16);
        
        if (fc.toDS) printf("ToDS ");
        if (fc.fromDS) printf("FromDS ");
        if (fc.more_frmts) printf("MoreFragments ");
        if (fc.retry) printf("Retransmitted ");
        if (fc.more_data) printf("MoreData ");
        if (fc.protected_frame) printf("Encrypted ");
        if (fc.order) {
          if (fc.is_qos_data()) printf("QoS-has-HT-Control ");
          else printf("StrictlyOrdered ");
        }
        printf("]\n");
      }
    }

    void print_qos_control(uint16_t qos, int pair) {  //  pair 0-10 Table 8-4, P 8.2.4.5.1 IEEE802.11 Standard
      unsigned char tid = qos & 15;
      unsigned char eosp = (qos >> 4) & 1;
      unsigned char ack_policy = (qos >> 5) & 3;
      unsigned char a_msdu = (qos >> 7) & 1;

      //  TXOP Limit, AP PS Buffer State, TXOP Duration Requested, Queue Size, Reserved
    
      unsigned char last_8_bits = qos >> 8;

      unsigned char buffer_state = (last_8_bits >> 1) & 1;
      unsigned char highest_priority_buffered_ac = (last_8_bits >> 2) & 3;
      unsigned char ap_buffered_load = (last_8_bits >> 4) & 15;

      //  All frames sent by mesh STAs in mesh BSS
      unsigned char mcp = (qos >> 8) & 1;
      unsigned char mpsl = (qos >> 9) & 1;
      unsigned char rspi = (qos >> 10) & 1;
      unsigned char reserved = (qos >> 11) & 31;

      printf("\t\tTID: %d\n",tid);                        //TID
      printf("\t\tEOSP: %s\n",eosp ? "Yes" : "No ");      //EOSP
      printf("\t\tAck Policy: ");
      switch (ack_policy) {                               //Ack Policy
        case 0:
          printf("Normal Ack / Implicit Block Ack Request\n");
          break;
        case 1:
          printf("No Ack\n");
          break;
        case 2:
          printf("No explicit ack / PSMP Ack\n");
          break;
        case 3:
          printf("Block Ack\n");
          break;
        default:
          break;
      }
      if (pair == 1 || pair == 2 || pair == 4 || pair == 6 || pair == 8 || pair == 10)
        printf("\t\tA-MSDU: %s\n", a_msdu ? "Yes" : "No");      //A-MSDU present
      
      // Last 8 bit
      switch (pair) {
        case 0:
          printf("\t\tTXOP Limit: %dus\n",last_8_bits);
          break;
        case 1:
          printf("\t\tTXOP Limit: %dus\n",last_8_bits);
          break;
        case 2:
          printf("\t\tAP PS buffer state: %s\n", buffer_state ? "Specified" : "Not Specified");      
          printf("\t\tHighest Priority Buffered AC: %d\n", highest_priority_buffered_ac);
          printf("\t\tAP Buffered Load: %d\n", ap_buffered_load);
          break;
        case 3:
          printf("\t\tAP PS buffer state: %s\n", buffer_state ? "Specified" : "Not Specified");
          printf("\t\tHighest Priority Buffered AC: %d\n", highest_priority_buffered_ac);
          printf("\t\tAP Buffered Load: %d\n", ap_buffered_load);
          break;
        case 4:
          printf("\t\t%s: %d%s\n",!eosp ? "TXOP Duration Requested" : "Queue Size", last_8_bits, !eosp ? "us":'\0');
          break;
        case 5:
          printf("\t\t%s: %d%s\n",!eosp ? "TXOP Duration Requested" : "Queue Size", last_8_bits, !eosp ? "us":'\0');
          break;
        case 10:
          printf("\t\tMesh Control: ");
          if (mcp) printf("Yes\n");
          else printf("No\n");
          printf("\t\tMesh Power Save Level: %d\n",mpsl);
          printf("\t\tRSPI: ");
          if (rspi) printf("Yes\n");
          else printf("No\n");
      }
    }

    void print_management_capability_info(uint16_t capability) {
      unsigned char ees = capability & 1;
      unsigned char ibss = (capability >> 1) & 1;
      unsigned char cf_pollable = (capability >> 2) & 1;
      unsigned char cf_poll_request = (capability >> 3) & 1;
      unsigned char privacy = (capability >> 4) & 1;
      unsigned char short_preamble = (capability >> 5) & 1;
      unsigned char pbcc = (capability >> 6) & 1;
      unsigned char channel_agility = (capability >> 7) & 1;
      unsigned char spectrum_mgmt = (capability >> 8) & 1;
      unsigned char qos = (capability >> 9) & 1;
      unsigned char short_slot_time = (capability >> 10) & 1;
      unsigned char apsd = (capability >> 11) & 1;
      unsigned char radio_measure = (capability >> 12) & 1;
      unsigned char dsss_ofdm = (capability >> 13) & 1;
      unsigned char delayed_block_ack = (capability >> 14) & 1;
      unsigned char immediate_block_ack = (capability >> 15) & 1;

      printf("\t\tESS: %s\t",ees ? "Yes" : "No ");
      printf("\t\tIBSS: %s\t",ibss ? "Yes" : "No ");
      printf("\t\tCF Pollable: %s\t",cf_pollable ? "Yes" : "No ");
      printf("\tCF-Poll Request: %s\n",cf_poll_request ? "Yes" : "No ");
      printf("\t\tPrivacy: %s\t",privacy ? "Yes" : "No ");
      printf("\t\tShort Preamble: %s\t",short_preamble ? "Yes" : "No ");
      printf("\tPBCC: %s\t",pbcc ? "Yes" : "No ");
      printf("\t\tChannel Agility: %s\n",channel_agility ? "Yes" : "No ");
      printf("\t\tSpectrum Mgmt: %s\t",spectrum_mgmt ? "Yes" : "No ");
      printf("\tQoS: %s\t",qos ? "Yes" : "No ");
      printf("\t\tShort Slot Time: %s\t",short_slot_time ? "Yes" : "No ");
      printf("\tAPSD: %s\n",apsd ? "Yes" : "No ");
      printf("\t\tRadio Measurement: %s\t",radio_measure ? "Yes" : "No ");
      printf("\tDSSS-OFDM: %s\t",dsss_ofdm ? "Yes" : "No ");
      printf("\t\tDelayed Block Ack: %s\t",delayed_block_ack ? "Yes" : "No ");
      printf("\tImmediate Block Ack: %s\n",immediate_block_ack ? "Yes" : "No ");
    }

    uint64_t get_uint64(const char *netorder_bytes) {
      return le64toh(*((uint64_t*)netorder_bytes));
    }

    uint32_t get_uint32(const char *netorder_bytes) {
      return le32toh(*((uint32_t*)netorder_bytes));
    }
    
    uint16_t get_uint16(const char *netorder_bytes) {
      return le16toh(*((uint16_t*)netorder_bytes));
    }

    void get_uint16(const char *netorder_bytes, void *hostorder) {
      uint16_t a = get_uint16(netorder_bytes);
      memcpy(hostorder, &a, 2);
    }

    uint16_t mac_utils::get_duration(const char *bytes) {
      return get_uint16(bytes);
    }

    std::string get_address(const char *byte) {
      std::string address = std::string(17,':');
      unsigned char *addr = (unsigned char *)byte;
      unsigned char l,r;

      for (int i = 0; i < MAC_ADDR_LEN; i++) {
        l = *addr >> 4;
        r = *addr & 15;
        if (l < 10) address[3*i] = '0' + l;
        else address[3*i] = 'a' + l - 10;
        if (r < 10) address[3*i+1] = '0' + r;
        else address[3*i+1] = 'a' + r - 10;
        addr++;
      }
      return address;
    }

    void get_squence_control(const char *byte, int &seq, int &frag) {
      uint16_t sequence = get_uint16(byte);
      frag = sequence & 15;
      seq = sequence >> 4;
    }

    uint16_t get_qos_control(const char *byte) {
      return get_uint16(byte);
    }

    uint32_t get_ht_control(const char *byte) {
      return get_uint32(byte);
    }

    uint16_t get_bar_control(const char *byte) {
      return get_uint16(byte);
    }



    void print_addresses_to_file(FILE *d_fstat, const char *addresses, const char *addr1 = NULL,
      const char *addr2 = NULL, const char *addr3 = NULL, const char *addr4 = NULL, const char *addr4_pointer = NULL)
    {
      if (addr1) {
        fprintf(d_fstat,"%s,", get_address(addresses).c_str());
        if (addr2) {
          fprintf(d_fstat,"%s,", get_address(addresses + 6).c_str());
          if (addr3) {
            fprintf(d_fstat,"%s,", get_address(addresses + 12).c_str());
            if (addr4) {
              fprintf(d_fstat,"%s,", get_address(addr4_pointer).c_str());
            }
            else fprintf(d_fstat,",");
          }
          else fprintf(d_fstat,",,");
        }
        else fprintf(d_fstat,",,,");
      }
      else fprintf(d_fstat,",,,,");
    }


    void print_addresses(const char *addresses, const char *addr1 = NULL,
      const char *addr2 = NULL, const char *addr3 = NULL, const char *addr4 = NULL, const char *addr4_pointer = NULL)
    {
      if (addr1) {
        printf("\t\t%s: %s\n", addr1, get_address(addresses).c_str());
        if (addr2) {
          printf("\t\t%s: %s\n", addr2, get_address(addresses + 6).c_str());
          if (addr3) {
            printf("\t\t%s: %s\n", addr3, get_address(addresses + 12).c_str());
            if (addr4) {
              printf("\t\t%s: %s\n", addr4, get_address(addr4_pointer).c_str());
            }
          }
        }
      }
    }


    mac80211_frame_control_field mac_utils::get_control_field(const char *bytes) {
      mac80211_frame_control_field fc;
      get_uint16(bytes, &fc);
      return fc;
    }



    //  PARSE MANAGEMENT FRAMES    



    void mac_utils::parse_ies(const char *recv, int len) {
      // get all information elements
      int eid;
      int elen;
      int i = 0;
      while (i + 2 /* 2 bytes for EID+ELEN */ <= len) {
        eid = recv[i] & 0xFF;
        elen = recv[i+1] & 0xFF;
        if (i + 2 + elen > len)
          break;

        i += 2; // go to information part of the element

        // print element id
        if (mac80211_information_element_id_str[eid])
          printf("\t%s: ", mac80211_information_element_id_str[eid]);
        else
          printf("\tUnknown EID (%u): ", eid);

        switch (eid) {
          case IE_SSID:
            for (int j = 0; j < elen; j++)
              printf("%c", recv[i + j]);
            printf("\n");
            break;
          case IE_SUPPORTED_RATES:
          case IE_EXTENDED_SUPPORTED_RATES:
            for (int j = 0; j < elen; j++) {
              unsigned char rate = recv[i + j];
              printf("%0.1f", 0.5f * (rate & 0x7F));  // ignore bit 7
              if (j != elen - 1) printf(", ");
              else printf(" Mbps\n");
            }
            break;
          case IE_DSSS_PARAMETER_SET:
            printf("Channel %d\n", recv[i]);
            break;
          case IE_COUNTRY:
            printf("%c%c%c", recv[i], recv[i + 1], recv[i + 2]);
            // for each group of 3 fields (FirstChannelNumber|NumberChannels|MaxTxPower)
            for (int j = 0; j < (elen - 3) / 3; j++) {
              int first_channel = recv[i+3+j*3] & 0xFF;
              int num_channels = recv[i+3+j*3+1] & 0xFF;
              int max_txpwr = recv[i+3+j*3+2] & 0xFF;
              printf(", Channels %d-%d (max. %udBm)",
                first_channel, first_channel + num_channels - 1, max_txpwr);
            }
            printf("\n");
            break;
          case IE_ERP:
            {
              unsigned char info = recv[i]; // get only the 1st byte
              printf("Non-ERP: %d", info & 1);
              printf(", Protected: %d", (info & 2) >> 1);
              printf(", Barker preamble: %s\n", (info & 4) >> 2 ? "short" : "long");
            }
            break;
          case IE_HT_CAPABILITIES:
            {
              uint16_t *ht16 = (uint16_t*)recv;
              ie_ht_cap *ht = (ie_ht_cap*)recv;
              *ht16 = le16toh(*ht16);

              printf("\n");
              printf("\t\tLDPC Support: %s\n", ht->info & 1 ? "yes" : "no");
              printf("\t\tChannel Support: %s\n",
                ht->info & 2 ? "20 MHz and 40 MHz" : "20 Mhz only");
              printf("\t\tSpatial Multiplexing Power Save: ");
              switch ((ht->info >> 2) & 3) {
                case 0: printf("static\n");break;
                case 1: printf("dynamic\n");break;
                case 3: printf("disabled\n");break;
                default: printf("reserved\n");break;
              }
              printf("\t\tHT-Greenfield: %s\n", (ht->info >> 4) & 1 ? "yes" : "no");
              printf("\t\tShort GI for 20 MHz: %s\n", (ht->info >> 5) & 1 ? "yes" : "no");
              printf("\t\tShort GI for 40 MHz: %s\n", (ht->info >> 6) & 1 ? "yes" : "no");
              printf("\t\tTx STBC: %s\n", (ht->info >> 7) & 1 ? "yes" : "no");
              printf("\t\tRx STBC: ");
              switch ((ht->info >> 8) & 3) {
                case 0: printf("no\n");break;
                case 1:
                case 2:
                case 3: printf("up to %d streams\n", (ht->info >> 8) & 3);break;
              }
              printf("\t\tDSSS/CCK in 40 MHz: %s\n", (ht->info >> 12) & 1 ? "yes" : "no");
              printf("\t\tSupported MCS: ");
              {
                int bit;
                for (int j = 0; j < 77; j++) {
                  bit = ht->mcs_set[j/8] & (1 << (j % 8));
                  if (bit) printf("%d ", j);
                }
                if (ht->mcs_set[100/8] & (1 << (100 % 8)))
                  printf("TxUnequalMod-supported");
                printf("\n");
              }
            }
            break;
          default:
            printf("(%u bytes) ", elen);
            for (int j = 0; j < elen; j++) {
              printf("%02X ", recv[i + j] & 0xFF);
            }
            printf("\n");
            break;
        }

        i += elen;
      }
    }



/*
    void mac_utils::parse_probe_res(const char *recv, int len) {
      // probe response frame is identical to beacon frame
      mac80211_beacon_frame *beacon = (mac80211_beacon_frame*)recv;
      beacon->timestamp = le64toh(beacon->timestamp);
      beacon->beacon_interval = le16toh(beacon->beacon_interval);
      beacon->capability = le16toh(beacon->capability);

      printf("Probe Response Frame:\n");
      printf("\tTimestamp: %lu\n\tInterval: %0.2fms\n\tCapability: %04x\n",
        beacon->timestamp, 
        beacon->beacon_interval * 1024.0 / 1000, 
        beacon->capability);

      parse_ies(recv + 12, len - 12);
    }



    void mac_utils::parse_beacon(const char *recv, int len) {
      mac80211_beacon_frame *beacon = (mac80211_beacon_frame*)recv;
      beacon->timestamp = le64toh(beacon->timestamp);
      beacon->beacon_interval = le16toh(beacon->beacon_interval);
      beacon->capability = le16toh(beacon->capability);

      printf("Beacon Frame:\n");
      printf("\tTimestamp: %lu\n\tInterval: %0.2fms\n\tCapability: %04x\n",
        beacon->timestamp, 
        beacon->beacon_interval * 1024.0 / 1000, 
        beacon->capability);

      parse_ies(recv + 12, len - 12);
    }
*/



    void mac_utils::parse_management_beacon(const char *recv, int mpdu_size) {
      mac80211_frame_control_field fc = get_control_field(recv);
        
        printf("\tDuration: %d\n", get_duration(recv+2));
        printf("\tRA(DA):\t%s\n", get_address(recv+4).c_str());  
        printf("\tTA(SA):\t%s\n", get_address(recv+10).c_str());
        printf("\tBSSID:\t%s\n", get_address(recv+16).c_str());
          

        int frag, seq;
        get_squence_control(recv+22,seq,frag);
        printf("\tSequence: %d\tFragment: %d\n", seq, frag);

        //  parse frame body
        int ht_length = fc.order ? 4 : 0;
        uint64_t timestamp = get_uint64(recv+24+ht_length);
        uint16_t interval = get_uint16(recv+32+ht_length);
        uint16_t capability = get_uint16(recv+34+ht_length);
        printf("Beacon Frame:\n");
        printf("\tTimestamp: %lu\n\tInterval: %0.2fms\n", timestamp, interval * 1024.0 / 1000);
        printf("\tCapability:\n");
        print_management_capability_info(capability);
        
        parse_ies(recv + 36 + ht_length, mpdu_size - 40 - ht_length);        
    }

    void mac_utils::parse_management(const char *recv, int mpdu_size) {
      mac80211_frame_control_field fc = get_control_field(recv);

      print_addresses_to_file(d_fstat, recv+4, "\0", "\0", "\0");
      switch (fc.subtype) {
        case FRAME_SUBTYPE_MANAGEMENT_BEACON:
          parse_management_beacon(recv, mpdu_size);
          break;
        
        default:
          break;
      }
    }


    //  PARSE CONTROL FRAMES

    void mac_utils::parse_control_rts(const char *recv, int mpdu_size) {
        mac80211_frame_control_field fc = get_control_field(recv);
        
        printf("\tDuration: %d\n", get_duration(recv+2));
        printf("\tRA:\t%s\n", get_address(recv+4).c_str());
        printf("\tTA:\t%s\n", get_address(recv+10).c_str());
    }

    void mac_utils::parse_control_cts(const char *recv, int mpdu_size) {
        mac80211_frame_control_field fc = get_control_field(recv);
        
        printf("\tDuration: %d\n", get_duration(recv+2));
        printf("\tRA:\t%s\n", get_address(recv+4).c_str());
    }

    void mac_utils::parse_control_ack(const char *recv, int mpdu_size) {
        mac80211_frame_control_field fc = get_control_field(recv);
        
        printf("\tDuration: %d\n", get_duration(recv+2));
        printf("\tRA:\t%s\n", get_address(recv+4).c_str());
    }

    void mac_utils::parse_control_pspoll(const char *recv, int mpdu_size) {
        mac80211_frame_control_field fc = get_control_field(recv);
        
        printf("\tAID:\t%d\n", get_duration(recv+2));
        printf("\tBSSID(RA):\t%s\n", get_address(recv+4).c_str());
        printf("\tTA:\t%s\n", get_address(recv+10).c_str());
    }

    void mac_utils::parse_control_cfend(const char *recv, int mpdu_size) {
        mac80211_frame_control_field fc = get_control_field(recv);
        
        printf("\tDuration: %d\n", get_duration(recv+2));
        printf("\tRA:\t%s\n", get_address(recv+4).c_str());
        printf("\tBSSID(TA):\t%s\n", get_address(recv+10).c_str());
    }

    void mac_utils::parse_control_cfend_cfack(const char *recv, int mpdu_size) {
        mac80211_frame_control_field fc = get_control_field(recv);
        
        printf("\tDuration: %d\n", get_duration(recv+2));
        printf("\tRA:\t%s\n", get_address(recv+4).c_str());
        printf("\tBSSID(TA):\t%s\n", get_address(recv+10).c_str());
    }

    void mac_utils::parse_control_block_ack_req(const char *recv, int mpdu_size) {
        mac80211_frame_control_field fc = get_control_field(recv);
        
        enum BLOCKACKREQ_TYPE {
          BASIC_BLOCKACKREQ, COMPRESSED_BLOCKACKREQ, RESERVED, MULTI_TID_BLOCKACKREQ
        };

        printf("\tDuration: %d\n", get_duration(recv+2));
        printf("\tRA:\t%s\n", get_address(recv+4).c_str());
        printf("\tTA:\t%s\n", get_address(recv+10).c_str());
        
        //  print BAR Control
        uint16_t bar_control = get_bar_control(recv+16);
        unsigned char bar_ack_policy = bar_control & 1;
        unsigned char multi_tid = (bar_control >> 1) & 1;
        unsigned char compressed_bitmap = (bar_control >> 2) & 1;
        unsigned char tid_info = (bar_control >> 12) & 15;

        printf("BAR Control:\n");
        printf("\tBAR Ack Policy: %s\n", (!bar_ack_policy) ? "Normal Ack" : "No Ack");

        // print BAR Information
        uint16_t bar_info_basic_blockackreq = get_uint16(recv+18);
        uint16_t bar_info_compressed_blockackreq = get_uint16(recv+18);
        int block_ack_req_type;
        if (!multi_tid) {
          if (!compressed_bitmap) block_ack_req_type = BASIC_BLOCKACKREQ;
          else block_ack_req_type = COMPRESSED_BLOCKACKREQ;
        }
        else block_ack_req_type = MULTI_TID_BLOCKACKREQ;
        switch (block_ack_req_type) {
          case BASIC_BLOCKACKREQ:
            printf("Basic BlockAckReq\n");
            printf("\tTID: %d\n",tid_info);
            printf("\tStarting Sequence: %d\tFragment: %d\n", 
              bar_info_basic_blockackreq >> 4,
              bar_info_basic_blockackreq & 15);
            break;
          case COMPRESSED_BLOCKACKREQ:
            printf("Compressed BlockAckReq\n");
            printf("\tTID: %d\n",tid_info);
            printf("\tStarting Sequence: %d\tFragment: %d\n", 
              bar_info_basic_blockackreq >> 4,
              bar_info_basic_blockackreq & 15);
            break;
          case MULTI_TID_BLOCKACKREQ:
            printf("Multi-TID BlockAckReq\n");
            printf("\tTID: %d\n",tid_info);
            break;
          default:
            break;
        }
    }


    void mac_utils::parse_control(const char *recv, int mpdu_size) {
      mac80211_frame_control_field fc = get_control_field(recv);

      switch (fc.subtype) {
        case FRAME_SUBTYPE_CONTROL_RTS:
          print_addresses_to_file(d_fstat, recv+4, "\0", "\0");
          parse_control_rts(recv, mpdu_size);
          break;
        case FRAME_SUBTYPE_CONTROL_CTS:
          print_addresses_to_file(d_fstat, recv+4, "\0");
          parse_control_cts(recv, mpdu_size);
          break;
        case FRAME_SUBTYPE_CONTROL_ACK:
          print_addresses_to_file(d_fstat, recv+4, "\0");
          parse_control_ack(recv, mpdu_size);
          break;
        case FRAME_SUBTYPE_CONTROL_PSPOLL:
          parse_control_pspoll(recv, mpdu_size);
          break;
        case FRAME_SUBTYPE_CONTROL_CFEND:
          parse_control_cfend(recv, mpdu_size);
          break;
        case FRAME_SUBTYPE_CONTROL_CFEND_CFACK:
          parse_control_cfend_cfack(recv, mpdu_size);
          break;
        case FRAME_SUBTYPE_CONTROL_BLOCK_ACK_REQ:
          parse_control_block_ack_req(recv, mpdu_size);
          break;

        default:
          break;
      }
    }

    
    //  PARSE DATA FRAMES

    void mac_utils::parse_data_data(const char *recv, int mpdu_size) {
        int seq, frag;
        mac80211_frame_control_field fc = get_control_field(recv);

        printf("\tDuration: %d\n", get_duration(recv+2));

        if (!fc.toDS) {
          if (!fc.fromDS) print_addresses(recv+4, "RA(DA):", "TA(SA):", "BSSID:");
          else {
            if (fc.order) print_addresses(recv+4, "RA(DA):", "TA(BSSID):", "SA:");
            else print_addresses(recv+4, "RA(DA):", "TA(BSSID):");
          }
        }
        else {
          if (!fc.fromDS) {
            if (fc.order) print_addresses(recv+4, "RA(BSSID):", "TA(SA):", "DA:");
            else print_addresses(recv+4, "RA(BSSID):", "TA(SA):", "BSSID:");
          }
          else {
            if (fc.order) print_addresses(recv+4, "RA:", "TA:", "DA:", "SA:", recv+24);
            else print_addresses(recv+4, "RA:", "TA:", "BSSID:", "BSSID:", recv+24);
          }
        }
        get_squence_control(recv+22,seq,frag);
        printf("\tSequence %d\tFragment %d\n", seq,frag);
        //  No QoS Control
        //  No HT Control
    }

    void mac_utils::parse_data_qos_data(const char *recv, int mpdu_size) {
        int seq, frag;
        mac80211_frame_control_field fc = get_control_field(recv);
        
        printf("\tDuration: %d\n", get_duration(recv+2));

        if (!fc.toDS) {
          if (!fc.fromDS) print_addresses(recv+4, "RA(DA):", "TA(SA):", "BSSID:");
          else {
            if (fc.order) print_addresses(recv+4, "RA(DA):", "TA(BSSID):", "SA:");
            else print_addresses(recv+4, "RA(DA):", "TA(BSSID):");
          }
        }
        else {
          if (!fc.fromDS) {
            if (fc.order) print_addresses(recv+4, "RA(BSSID):", "TA(SA):", "DA:");
            else print_addresses(recv+4, "RA(BSSID):", "TA(SA):", "BSSID:");
          }
          else {
            if (fc.order) print_addresses(recv+4, "RA:", "TA:", "DA:", "SA:", recv+24);
            else print_addresses(recv+4, "RA:", "TA:", "BSSID:", "BSSID:", recv+24);
          }
        }
        
        get_squence_control(recv+22,seq,frag);
        printf("\tSequence %d\tFragment %d\n", seq,frag);

        //  print Qos Control
        uint16_t qos_control;
        if (fc.toDS && fc.fromDS) qos_control = get_qos_control(recv+30);
        else qos_control = get_qos_control(recv+24);
        printf("\tQoS Control:\n");
        print_qos_control(qos_control,10);

        //  print HT Control, NOT IMPLEMENTED
        uint32_t ht_control;
        if (fc.order) {
          if (fc.toDS && fc.fromDS) {
            ht_control = get_ht_control(recv+32);
            printf("HT Control: %d\n",ht_control);
          }
          else {
            ht_control = get_ht_control(recv+26);
            printf("HT Control: %d\n",ht_control);
          }
        }

    }

    void mac_utils::parse_data(const char *recv, int mpdu_size) {
      mac80211_frame_control_field fc = get_control_field(recv);

      print_addresses_to_file(d_fstat, recv+4, "\0", "\0", "\0");
      switch (fc.subtype) {
        case FRAME_SUBTYPE_DATA_DATA:
          parse_data_data(recv, mpdu_size);
          break;
        case FRAME_SUBTYPE_DATA_QOS_DATA:
          parse_data_qos_data(recv, mpdu_size);
          break;
        
        default:
          break;
      }
    }


    bool mac_utils::check_recv_bytes(const char *recv, int mpdu_size) {
      // check fcs
      boost::crc_32_type result;
      result.process_bytes(recv, mpdu_size - 4);
      uint32_t* fcs = (uint32_t*)(recv + mpdu_size - 4);
      return (le32toh((*fcs)) == result.checksum());
    }

    bool mac_utils::parse_recv_bytes(const char *recv, int mpdu_size, 
      const phy_frame_rx_info &info) 
    {
      // check fcs
      bool correct = check_recv_bytes(recv, mpdu_size);

      mac80211_frame_control_field fc = get_control_field(recv);
      printf( "[Start %lu End %lu] ", info.time_start, info.time_end);
      fprintf(d_fstat,"%lu,",info.time_start);
      fprintf(d_fstat,"%lu,",info.time_end);
      fprintf(d_fstat,"%d,",info.mode);
      fprintf(d_fstat,"%d,",info.rate);
      fprintf(d_fstat,"%d,",correct);
      
      if (correct) {
        printf("MAC Checksum: Correct\tRate: %s\tLength: %d\t",
          phy_utils::rate_string(info.rate, info.mode).c_str(),
          mpdu_size);
        print_frame_type(fc);
        print_frame_control(fc);
        fprintf(d_fstat,"%d,%d,%d,%d,%d,%d,%d,",fc.toDS,fc.fromDS,fc.more_frmts,fc.retry,fc.more_data,fc.protected_frame,fc.order);
        fprintf(d_fstat,"%d,",mpdu_size);
        switch (fc.type) {
          case FRAME_TYPE_MANAGEMENT:
            fprintf(d_fstat,"%s,%s,", mac80211_frame_type_str[fc.type], mac80211_frame_management_subtype_str[fc.subtype]);
            parse_management(recv, mpdu_size);
            break;
          case FRAME_TYPE_CONTROL:
            fprintf(d_fstat,"%s,%s,", mac80211_frame_type_str[fc.type], mac80211_frame_control_subtype_str[fc.subtype]);          
            parse_control(recv, mpdu_size);
            break;
          case FRAME_TYPE_DATA:
            fprintf(d_fstat,"%s,%s,", mac80211_frame_type_str[fc.type], mac80211_frame_data_subtype_str[fc.subtype]);
            parse_data(recv, mpdu_size);
            break;
          default:
            break;
        }

      // printf("Control Field: 0x%02X\n", d_hdr.fc16);
      // copy whole frame including data
        d_mpdu.resize(mpdu_size);
        memcpy(d_mpdu.data(), recv, mpdu_size);
      }

      else {
        printf("MAC Checksum: Incorrect!\n");
        fprintf(d_fstat,",,,,,,,,,,,,");
      }
      
      fprintf(d_fstat,"\n");

      return correct;
    }

    

 

    std::vector<char> mac_utils::serialize()
    {
      // embed header to mpdu
      generate_header();

      // append fcs to mpdu
      generate_fcs();

      return d_mpdu;
    }

    char* mac_utils::generate_header() {
      mac80211_frame_header *hdr = (mac80211_frame_header*)d_mpdu.data();
      memcpy(hdr, &d_hdr, sizeof(d_hdr));
      hdr->fc16 = htole16(hdr->fc16);
      hdr->duration = htole16(hdr->duration);
      hdr->seq = htole16(hdr->seq);
      return d_mpdu.data();
    }

    char* mac_utils::generate_fcs() {
      reset_fcs();
      return update_fcs(d_mpdu.data(), d_data_len + sizeof(d_hdr));
    }

    void mac_utils::reset_fcs() {
      d_crc.reset();
    }

    char* mac_utils::update_fcs(const char *in, int len) {
      d_crc.process_bytes(in, len);
      uint32_t fcs = htole32(d_crc.checksum());
      char *p = d_mpdu.data() + d_data_len + sizeof(d_hdr);
      memcpy(p, &fcs, sizeof(fcs));
      return p;
    }

    void mac_utils::set_data(const char *data, int len) {
      set_datalen(len);
      put_data(data, len, 0);
    }

    void mac_utils::set_datalen(int len) {
      d_mpdu.resize(sizeof(d_hdr) + len + 4 /* 4 bytes of fcs */);
      d_data_len = len;
    }

    void mac_utils::put_data(const char *data, int len, int where_to_put) {
      memcpy(d_mpdu.data() + sizeof(d_hdr) + where_to_put, data, len);
    }

    char* mac_utils::get_header() {
      return d_mpdu.data();
    }

    int mac_utils::get_headerlen() {
      return sizeof(d_hdr);
    }

    char* mac_utils::get_data() {
      return d_mpdu.data() + sizeof(d_hdr);
    }

    int mac_utils::get_datalen() {
      return d_data_len;
    }

    char* mac_utils::get_fcs() {
      return d_mpdu.data() + sizeof(d_hdr) + d_data_len;
    }

    void mac_utils::set_type(int type, int subtype) {
      d_hdr.fc.type = type;
      d_hdr.fc.subtype = subtype;
    }

    int mac_utils::get_type() {
      return d_hdr.fc.type;
    }

    int mac_utils::get_subtype() {
      return d_hdr.fc.subtype;
    }

    void mac_utils::set_addr(int index, const std::vector<int>& addr) {
      assert(((unsigned int)index) < MAC_MAX_ADDRS);

      for (int i = 0; i < MAC_ADDR_LEN; i++)
        d_hdr.addr[index][i] = (unsigned char)addr[i];
    }

    void mac_utils::set_addr(int index, const std::string& addr) {
      std::vector<int> int_addr(MAC_ADDR_LEN, 0);
      int k = 0;
      for (int i = 0; i < addr.size(); i++) {
        char c = addr[i];
        if (c == ':') 
          k++;
        else if (c >= '0' && c <= '9') 
          int_addr[k] = int_addr[k] * 16 + (c - '0');
        else if (c >= 'a' && c <= 'f')
          int_addr[k] = int_addr[k] * 16 + (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F')
          int_addr[k] = int_addr[k] * 16 + (c - 'A' + 10);
      }
      set_addr(index, int_addr);
    }

    std::vector<int> mac_utils::get_addr(int index) {
      assert(((unsigned int)index) < MAC_MAX_ADDRS);

      std::vector<int> addr(6, 0);
      for (int i = 0; i < MAC_ADDR_LEN; i++)
        addr[i] = (int)d_hdr.addr[index][i];
      return addr;
    }

    void mac_utils::set_toDS_fromDS(int toDS, int fromDS) {
      d_hdr.fc.toDS = toDS;
      d_hdr.fc.fromDS = fromDS;
    }

    int mac_utils::get_toDS() {
      return d_hdr.fc.toDS;
    }

    int mac_utils::get_fromDS() {
      return d_hdr.fc.fromDS;
    }

    void mac_utils::set_seq(int seq) {
      d_hdr.seq = seq;
    }

    int mac_utils::get_seq() {
      return d_hdr.seq;
    }

    void mac_utils::set_duration(int duration) {
      d_hdr.duration = (uint16_t)duration;
    }

    int mac_utils::get_duration() {
      return d_hdr.duration;
    }


  } /* namespace swifi */
} /* namespace gr */

