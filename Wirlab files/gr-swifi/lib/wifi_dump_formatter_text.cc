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

#include "wifi_dump_formatter_text.h"
#include <swifi/wifi_dump_factory.h>
#include <swifi/phy_utils.h>
#include <string.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    // add "text" to the wifi dump factory
    static bool added = wifi_dump_factory::add("text",
      wifi_dump_formatter_text::sptr(new wifi_dump_formatter_text()));



    wifi_dump_formatter::sptr wifi_dump_formatter_text::make(
      const std::string &filename,
      const std::string &filter_string,
      bool is_print_raw)
    {
      return wifi_dump_formatter_text::sptr(new wifi_dump_formatter_text(
        filename, filter_string, is_print_raw));
    }

    wifi_dump_formatter_text::wifi_dump_formatter_text(
          const std::string &filename,
          const std::string &filter_string,
          bool is_print_raw)
      : f(NULL), is_print_raw(is_print_raw)
    {
      if (filename.empty() || filename == "<<stdout>>")
        f = stdout;
      else if (filename == "<<stderr>>")
        f = stderr;
      else
        f = fopen(filename.c_str(), "w");

      // set unbuffered mode
      // if (f) setvbuf(f, NULL, _IONBF, 0);
      if (!f) f = stdout; // default output

      if (!filter_string.empty()) {
        frame_filter.reset(new simple_filter(filter_string));
        xml_formatter.reset(new wifi_dump_formatter_xml("", "", false));
      }
    }

    wifi_dump_formatter_text::~wifi_dump_formatter_text()
    {
      if (f && f != stdout && f != stderr) fclose(f);
    }


    void wifi_dump_formatter_text::dump(const mac_frame_info &info) {
      if (frame_filter) {
        // XMLNode node = xml_formatter->xmlnode_frame(info);
        // printf("Node XML:\n%s\n", node.str().c_str());
        if (!frame_filter->filter(xml_formatter->xmlnode_frame(info)))
          return; // frame not matched
      }

      fprintf(f, "\n*******************************************************************\n");
      fprintf(f, "[Start %lu End %lu] MAC Checksum: %s\tRate: %s\tLength %d\n",
        info.rx_info.time_start, info.rx_info.time_end,
        info.correct ? "Correct" : "Incorrect",
        phy_utils::rate_string(info.rx_info.rate, info.rx_info.mode).c_str(),
        info.mpdu_size);
      fprintf(f, "PHY Info: freq_offset %f, scrambling_seed %d\n",
        info.rx_info.freq_offset, info.rx_info.scrambling_seed);

      print_frame_type(info.fc);
      print_frame_control(info.fc);
      fprintf(f, "\tDuration: %d\n", info.duration);

      switch (info.fc.type) {
        case FRAME_TYPE_MANAGEMENT:
          print_management_frame(info);
          break;
        case FRAME_TYPE_CONTROL:
          print_control_frame(info);
          break;
        case FRAME_TYPE_DATA:
          print_data_frame(info);
          break;
      }

      if (is_print_raw) {
        fprintf(f, "Raw content:\n");
        fprintf(f, "%s", raw_data_hextable(
          info.mpdu.get(), info.mpdu_size).c_str());
      }
    }

    void wifi_dump_formatter_text::print_frame_type(const mac80211_frame_control_field &fc) {
      fprintf(f, "Frame Type: %s/", mac80211_frame_type_str[fc.type]);
      switch (fc.type) {
        case FRAME_TYPE_MANAGEMENT: 
          fprintf(f, "%s", mac80211_frame_management_subtype_str[fc.subtype]);
          break;
        case FRAME_TYPE_CONTROL: 
          fprintf(f, "%s", mac80211_frame_control_subtype_str[fc.subtype]);
          break;
        case FRAME_TYPE_DATA: 
          fprintf(f, "%s", mac80211_frame_data_subtype_str[fc.subtype]);
          break;
      }
    }

    void wifi_dump_formatter_text::print_frame_control(const mac80211_frame_control_field &fc) {
      if (fc.toDS) fprintf(f, " [ToDS]");
      if (fc.fromDS) fprintf(f, " [FromDS]");
      if (fc.more_frmts) fprintf(f, " [MoreFrags]");
      if (fc.retry) fprintf(f, " [Retx]");
      if (fc.more_data) fprintf(f, " [MoreData]");
      if (fc.protected_frame) fprintf(f, " [Encrypted]");
      if (fc.order) {
        if (fc.is_qos_data()) fprintf(f, " [HTControl]");
        else fprintf(f, " [Ordered]");
      }
      fprintf(f, "\n");
    }


/***********************************************************************/
/***************** PRINT MANAGEMENT FRAME ******************************/
/***********************************************************************/

    void wifi_dump_formatter_text::print_management_frame(const mac_frame_info &info) {
      // addresses
      fprintf(f, "\tAddresses: TA %s --> RA %s    BSSID %s\n",
        addr_str(info.addr[1]).c_str(),
        addr_str(info.addr[0]).c_str(),
        addr_str(info.addr[2]).c_str());

      print_sequence_control(info);

      if (info.fc.order)
        print_ht_control(info);

      switch (info.fc.subtype) {
        case FRAME_SUBTYPE_MANAGEMENT_BEACON:
          print_management_beacon(info);
          break;
      }
    }

    void wifi_dump_formatter_text::print_management_beacon(const mac_frame_info &info) {
      management_frame_info *frame = info.management_frame.get();
      fprintf(f, "Beacon Frame:\n");
      fprintf(f, "\tTimestamp: %lu\n", frame->timestamp);
      fprintf(f, "\tInterval: %0.2fms\n", frame->beacon_interval * 1024.0 / 1000);

      fprintf(f, "\tCapability:");
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.ess, "ESS").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.ibss, "IBSS").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.cf_pollable, "CF Pollable").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.cf_pollreq, "CF Poll Request").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.privacy, "Privacy").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.short_preamble, "Short Preamble").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.pbcc, "PBCC").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.channel_agility, "Channel Agility").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.spectrum_mgmt, "Spectrum Management").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.qos, "QoS").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.short_slot, "Short Slot Time").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.apsd, "APSD").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.radio_measurement, "Radio Measurement").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.dsss_ofdm, "DSSS-OFDM").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.delayed_block_ack, "Delayed Block Ack").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(frame->capability.immediate_block_ack, "Immediate Block Ack").c_str());

      print_ies(info);
    }

    void wifi_dump_formatter_text::print_ies(const mac_frame_info &info) {
      if (!info.management_frame)
        return;

      int i;
      const ie_info &ie = info.management_frame->ie;
      for (std::map<int,bool>::const_iterator it = ie.has_ie.begin();
        it != ie.has_ie.end(); ++it) {
        if (!it->second) continue;

        int eid = it->first;
        fprintf(f, "\t%s: ", mac80211_information_element_id_str[eid]);
        switch (eid) {
          case IE_SSID:
            fprintf(f, "%s\n", ie.ssid->c_str());
            break;
          case IE_SUPPORTED_RATES:
            for (i = 0; i < ie.supported_rates->size(); i++) {
              if (i > 0) fprintf(f, ", %0.1f", (*ie.supported_rates)[i]);
              else fprintf(f, "%0.1f, ", (*ie.supported_rates)[i]);
            }
            fprintf(f, " Mbps\n");
            break;
          case IE_DSSS_PARAMETER_SET:
            fprintf(f, "Channel %d\n", ie.dsss_parameter_set->channel);
            break;
          case IE_COUNTRY:
            fprintf(f, "%s", ie.country->code.c_str());
            for (i = 0; i < ie.country->first_channel.size(); i++) {
              fprintf(f, ", Channels %d-%d (max. %udBm)",
                ie.country->first_channel[i],
                ie.country->first_channel[i] + ie.country->num_channels[i] - 1,
                ie.country->max_txpwr[i]);
            }
            break;
          case IE_ERP:
            fprintf(f, "\n\t\t%s\n", yesno_str(ie.erp->nonerp, "Non-ERP").c_str());
            fprintf(f, "\t\t%s\n", yesno_str(ie.erp->use_protection, "Use Protection").c_str());
            fprintf(f, "\t\t%s\n", yesno_str(ie.erp->barker, "Barker Preamble").c_str());
            break;
          case IE_HT_CAPABILITIES:
            print_ie_ht_capabilities(*ie.ht);
            break;
        }
      }
    }

    void wifi_dump_formatter_text::print_ie_ht_capabilities(const ie_ht_capabilities_info &ht) {
      fprintf(f, "\n\t\t%s\n", yesno_str(ht.ht.ldpc, "LDPC").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(ht.ht.channel40mhz, "Channel 40MHz").c_str());
      fprintf(f, "\t\tSpatial Multiplexing Power Save: ");
      switch (ht.ht.sm_pwr_save) {
        case 0: fprintf(f, "static\n"); break;
        case 1: fprintf(f, "dynamic\n");break;
        case 3: fprintf(f, "disabled\n");break;
        default: fprintf(f, "reserved\n");break;
      }
      fprintf(f, "\t\t%s\n", yesno_str(ht.ht.ht_green, "HT-Greenfield").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(ht.ht.short_gi_20mhz, "Short GI for 20MHz").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(ht.ht.short_gi_40mhz, "Short GI for 40MHz").c_str());
      fprintf(f, "\t\t%s\n", yesno_str(ht.ht.tx_stbc, "Tx STBC").c_str());
      fprintf(f, "\t\tRx STBC: ");
      switch (ht.ht.rx_stbc) {
        case 0: fprintf(f, "No\n");break;
        case 1: case 2: case 3: 
          fprintf(f, "up to %d streams\n", ht.ht.rx_stbc);
          break;
      }
      fprintf(f, "\t\t%s\n", yesno_str(ht.ht.dsss40mhz, "DSSS in 40Mhz").c_str());
      fprintf(f, "\t\tSupported MCS: ");
      {
        std::vector<int> mcs_set = ht.mcs.get_mcs_set();
        for (int i = 0; i < mcs_set.size(); i++)
          fprintf(f, "%d ", mcs_set[i]);
        fprintf(f, "\t\t\tRx Highest Rate: %dMbps\n", ht.mcs.rx_highest_rate);
        fprintf(f, "\t\t\t%s\n", yesno_str(ht.mcs.tx_mcs_defined, "Tx MCS Set Defined").c_str());
        fprintf(f, "\t\t\t%s\n", yesno_str(ht.mcs.tx_rx_mcs_not_equal, "Tx Rx MCS Set Not Equal").c_str());
        fprintf(f, "\t\t\t%s\n", yesno_str(ht.mcs.tx_unequal_modulation, "Tx Unequal Modulation Support").c_str());
        fprintf(f, "\t\t\tTx Max Spatial Streams: %d\n", ht.mcs.tx_max_streams);
      }
    }


/***********************************************************************/
/***************** PRINT CONTROL FRAME *********************************/
/***********************************************************************/

    void wifi_dump_formatter_text::print_control_frame(const mac_frame_info &info) {
      switch (info.fc.subtype) {
        case FRAME_SUBTYPE_CONTROL_RTS:
        case FRAME_SUBTYPE_CONTROL_BLOCK_ACK_REQ:
        case FRAME_SUBTYPE_CONTROL_BLOCK_ACK:
          fprintf(f, "\tAddresses: TA %s --> RA %s\n",
            addr_str(info.addr[1]).c_str(),
            addr_str(info.addr[0]).c_str());
          if (info.fc.subtype != FRAME_SUBTYPE_CONTROL_RTS)
            fprintf(f, "\tBA/BAR Control Information: not implemented\n");
          break;

        case FRAME_SUBTYPE_CONTROL_CTS:
        case FRAME_SUBTYPE_CONTROL_ACK:
          fprintf(f, "\tAddress: RA %s\n", addr_str(info.addr[0]).c_str());
          break;

        case FRAME_SUBTYPE_CONTROL_PSPOLL:
          fprintf(f, "\tAddresses: TA %s --> RA/BSSID %s\n",
            addr_str(info.addr[1]).c_str(),
            addr_str(info.addr[0]).c_str());
          break;

        case FRAME_SUBTYPE_CONTROL_CFEND:
        case FRAME_SUBTYPE_CONTROL_CFEND_CFACK:
          fprintf(f, "\tAddresses: TA/BSSID %s --> RA %s\n",
            addr_str(info.addr[1]).c_str(),
            addr_str(info.addr[0]).c_str());
          break;
      }
    }




/***********************************************************************/
/***************** PRINT DATA FRAME ************************************/
/***********************************************************************/

    void wifi_dump_formatter_text::print_data_frame(const mac_frame_info &info) {
      // print addresses, note: we don't take care of the A-MSDU case
      fprintf(f, "\tAddresses: ");
      switch (info.fc.toDS + info.fc.fromDS * 2) {
        case 0: // ToDS = 0, FromDS = 0
          fprintf(f, "TA %s --> RA %s [BSSID %s]\n",
            addr_str(info.addr[1]).c_str(),
            addr_str(info.addr[0]).c_str(),
            addr_str(info.addr[2]).c_str());
          break;
        case 1: // ToDS = 1, FromDS = 0
          fprintf(f, "TA %s --> RA/BSSID %s [DA %s]\n",
            addr_str(info.addr[1]).c_str(),
            addr_str(info.addr[0]).c_str(),
            addr_str(info.addr[2]).c_str());
          break;
        case 2: // ToDS = 0, FromDS = 1
          fprintf(f, "[SA %s] TA/BSSID %s --> RA %s\n",
            addr_str(info.addr[2]).c_str(),
            addr_str(info.addr[1]).c_str(),
            addr_str(info.addr[0]).c_str());
          break;
        case 3: // ToDS = 1, FromDS = 1
          fprintf(f, "[SA %s] TA %s --> RA %s [DA %s]\n",
            addr_str(info.addr[3]).c_str(),
            addr_str(info.addr[1]).c_str(),
            addr_str(info.addr[0]).c_str(),
            addr_str(info.addr[2]).c_str());
          break;
      }

      print_sequence_control(info);

      if (info.fc.is_qos_data()) {
        print_qos_control(info);
        if (info.fc.order)
          print_ht_control(info);
      }
    }

    void wifi_dump_formatter_text::print_sequence_control(const mac_frame_info &info) {
      fprintf(f, "\tSequence %d, Fragment %d\n", 
        info.seq_control.seq_no, info.seq_control.fragment_no);
    }


    void wifi_dump_formatter_text::print_qos_control(const mac_frame_info &info) {
      fprintf(f, "\tQoS Control:\n");

      // TID with access policy
      fprintf(f, "\t\tTID: %d (%s)\n", info.qos_control.tid,
        info.qos_control.tid <= 7 ? "EDCA" : "HCCA/HEMM");

      // end of service period
      fprintf(f, "\t\t%s\n", yesno_str(info.qos_control.eosp, "EOSP").c_str());

      // ack policy
      fprintf(f, "\t\tAck Policy: ");
      switch (info.qos_control.ack_policy) {
        case 0:
          fprintf(f, "Normal Ack / Implicit Block Ack Request\n");
          break;
        case 1:
          fprintf(f, "No Ack\n");
          break;
        case 2:
          fprintf(f, "No explicit Ack / PSMP Ack\n");
          break;
        case 3:
          fprintf(f, "Block Ack\n");
          break;
      }

      // A-MSDU
      if (info.fc.subtype >= FRAME_SUBTYPE_DATA_QOS_DATA && 
        info.fc.subtype <= FRAME_SUBTYPE_DATA_QOS_DATA_CFACKPOLL) {
        fprintf(f, "\t\t%s\n", yesno_str(info.qos_control.amsdu, "A-MSDU").c_str());
      }

      // TXOP Limit
      fprintf(f, "\t\tTXOP: %d\n", info.qos_control.txop);
    }

    void wifi_dump_formatter_text::print_ht_control(const mac_frame_info &info) {
      fprintf(f, "\tHT Control: (not implemeneted)\n");
    }



/***********************************************************************/
/***************** HELPER FUNCTIONS ************************************/
/***********************************************************************/

    std::string wifi_dump_formatter_text::addr_str(const char *byte) {
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

    std::string wifi_dump_formatter_text::yesno_str(bool v, const std::string &caption) {
      return v ? caption + ": Yes" : caption + ": No";
    }

  } /* namespace swifi */
} /* namespace gr */

