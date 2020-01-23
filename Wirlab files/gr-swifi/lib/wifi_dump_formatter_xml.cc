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

#include "wifi_dump_formatter_xml.h"
#include "xml_reader.h"
#include "simple_filter.h"
#include <swifi/wifi_dump_factory.h>
#include <swifi/phy_utils.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

namespace gr {
  namespace swifi {

    // add "text" to the wifi dump factory
    static bool added = wifi_dump_factory::add("xml",
      wifi_dump_formatter_xml::sptr(new wifi_dump_formatter_xml()));



    wifi_dump_formatter::sptr wifi_dump_formatter_xml::make(
      const std::string &filename,
      const std::string &filter_string,
      bool is_print_raw)
    {
      return wifi_dump_formatter_xml::sptr(new wifi_dump_formatter_xml(
        filename, filter_string, is_print_raw));
    }

    wifi_dump_formatter_xml::wifi_dump_formatter_xml(
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
      }
    }

    wifi_dump_formatter_xml::~wifi_dump_formatter_xml()
    {
      if (f && f != stdout && f != stderr) fclose(f);
    }


    void wifi_dump_formatter_xml::dump(const mac_frame_info &info) {
      XMLNode root = xmlnode_frame(info);
      if (frame_filter) {
        if (!frame_filter->filter(root))
          return;
      }

      fprintf(f, "%s", root.str().c_str());
      fflush(f);
    }

    XMLNode wifi_dump_formatter_xml::xmlnode_frame(const mac_frame_info &info) {
      XMLNode root("frame");
      root.set_attr("correct", info.correct);

      root.add(xmlnode_phy_info(info));
      root.add(xmlnode_frame_control(info.fc));
      root.add("duration", info.duration);

      switch (info.fc.type) {
        case FRAME_TYPE_MANAGEMENT:
          root.add(xmlnode_management_frame(info));
          break;
        case FRAME_TYPE_CONTROL:
          root.add(xmlnode_control_frame(info));
          break;
        case FRAME_TYPE_DATA:
          root.add(xmlnode_data_frame(info));
          break;
      }

      if (is_print_raw) {
        // add raw data by hex representation
      }
      return root;
    }

    XMLNode wifi_dump_formatter_xml::xmlnode_phy_info(const mac_frame_info &info) {
      XMLNode phy("phy");
      phy.add("time").get_last_child()
        .set_attr("start", info.rx_info.time_start)
        .set_attr("end", info.rx_info.time_end);
      phy.add("sample").get_last_child()
        .set_attr("frame_start", info.rx_info.sample_start)
        .set_attr("frame_end", info.rx_info.sample_end)
        .set_attr("preamble_end", info.rx_info.sample_start + 320)
        .set_attr("payload_start", info.rx_info.payload_sample_start)
        .set_attr("payload_end", info.rx_info.payload_sample_end);
      phy.add("rate", phy_utils::rate_string(info.rx_info.rate, info.rx_info.mode))
        .get_last_child()
          .set_attr("code", (int)info.rx_info.rate)
          .set_attr("mode", (int)info.rx_info.mode);
      phy.add("mpdu_size", info.mpdu_size);
      phy.add("freq_offset", info.rx_info.freq_offset);
      phy.add("sfo", info.rx_info.sfo);
      phy.add("scrambling_seed", info.rx_info.scrambling_seed);

      // set padding string
      int padsize = phy_utils::compute_uncoded_tail_pad_size(info.rx_info.rate, info.rx_info.psdu_size);
      phy.add("data_padding", hexstr(info.rx_info.data_tail_padding_bytes.get(), padsize))
        .get_last_child().set_attr("size", padsize);

      return phy;
    }

    XMLNode wifi_dump_formatter_xml::xmlnode_frame_control(const mac80211_frame_control_field &fc) {
      XMLNode node("frame_control");
      node.add("type", mac80211_frame_type_str[fc.type]);
      switch (fc.type) {
        case FRAME_TYPE_MANAGEMENT: 
          node.add("subtype", mac80211_frame_management_subtype_str[fc.subtype]);
          break;
        case FRAME_TYPE_CONTROL: 
          node.add("subtype", mac80211_frame_control_subtype_str[fc.subtype]);
          break;
        case FRAME_TYPE_DATA: 
          node.add("subtype", mac80211_frame_data_subtype_str[fc.subtype]);
          break;
      }

      // node.add("toDS", fc.toDS).add("fromDS", fc.fromDS);
      node.add("more_frags", (int)fc.more_frmts).add("retx", (int)fc.retry);
      node.add("more_data", (int)fc.more_data).add("encrypted", (int)fc.protected_frame);
      node.add("order", (int)fc.order);
      return node;
    }



/***********************************************************************/
/***************** PRINT MANAGEMENT FRAME ******************************/
/***********************************************************************/

    XMLNode wifi_dump_formatter_xml::xmlnode_management_frame(const mac_frame_info &info) {
      XMLNode node("management_frame");

      // addresses
      node.add("addresses");
      node.get_last_child().add("ta", addr_str(info.addr[1]))
        .add("ra", addr_str(info.addr[0]))
        .add("bssid", addr_str(info.addr[2]));

      node.add(xmlnode_sequence_control(info));

      if (info.fc.order)
        node.add(xmlnode_ht_control(info));

      switch (info.fc.subtype) {
        case FRAME_SUBTYPE_MANAGEMENT_BEACON:
          node.add(xmlnode_management_beacon(info));
          break;
      }
      return node;
    }

    XMLNode wifi_dump_formatter_xml::xmlnode_management_beacon(const mac_frame_info &info) {
      management_frame_info *frame = info.management_frame.get();
      XMLNode node("beacon_frame");
      node.add("timestamp", frame->timestamp);
      node.add("interval", float(frame->beacon_interval * 1024.0 / 1000));

      node.add("capability");
      node.get_last_child().add("ess", (int)frame->capability.ess)
        .add("ibss", (int)frame->capability.ibss)
        .add("cf_pollable", (int)frame->capability.cf_pollable)
        .add("cf_pollreq", (int)frame->capability.cf_pollreq)
        .add("privacy", (int)frame->capability.privacy)
        .add("short_preamble", (int)frame->capability.short_preamble)
        .add("pbcc", (int)frame->capability.pbcc)
        .add("channel_agility", (int)frame->capability.channel_agility)
        .add("spectrum_mgmt", (int)frame->capability.spectrum_mgmt)
        .add("qos", (int)frame->capability.qos)
        .add("short_slot", (int)frame->capability.short_slot)
        .add("apsd", (int)frame->capability.apsd)
        .add("radio_measurement", (int)frame->capability.radio_measurement)
        .add("dsss_ofdm", (int)frame->capability.dsss_ofdm)
        .add("delayed_block_ack", (int)frame->capability.delayed_block_ack)
        .add("immediate_block_ack", (int)frame->capability.immediate_block_ack);

      node.add(xmlnode_ies(info));
      return node;
    }

    XMLNode wifi_dump_formatter_xml::xmlnode_ies(const mac_frame_info &info) {
      XMLNode node("information_element");

      int i;
      const ie_info &ie = info.management_frame->ie;
      for (std::map<int,bool>::const_iterator it = ie.has_ie.begin();
        it != ie.has_ie.end(); ++it) {
        if (!it->second) continue;

        int eid = it->first;
        std::string ie_id = mac80211_information_element_id_str[eid];
        std::transform(ie_id.begin(), ie_id.end(), ie_id.begin(), ::tolower);
        node.add(ie_id);
        XMLNode &subnode = node.get_last_child();

        switch (eid) {
          case IE_SSID:
            subnode.set_value(*ie.ssid);
            break;
          case IE_SUPPORTED_RATES:
            for (i = 0; i < ie.supported_rates->size(); i++) {
              subnode.add("rate", (*ie.supported_rates)[i]);
            }
            break;
          case IE_DSSS_PARAMETER_SET:
            subnode.set_value(ie.dsss_parameter_set->channel);
            break;
          case IE_COUNTRY:
            subnode.set_attr("code", ie.country->code);
            for (i = 0; i < ie.country->first_channel.size(); i++) {
              subnode.add("channel_set");
              subnode.get_last_child()
                .set_attr("first_channel", (int)ie.country->first_channel[i])
                .set_attr("num_channels", (int)ie.country->num_channels[i])
                .set_attr("max_txpwr", (int)ie.country->max_txpwr[i]);
            }
            break;
          case IE_ERP:
            subnode.add("non_erp", (int)ie.erp->nonerp)
              .add("use_protection", (int)ie.erp->use_protection)
              .add("barker_preamble", (int)ie.erp->barker);
            break;
          case IE_HT_CAPABILITIES:
            xmlnode_ie_ht_capabilities(*ie.ht, subnode);
            break;
        }
      }

      return node;
    }

    void wifi_dump_formatter_xml::xmlnode_ie_ht_capabilities(
        const ie_ht_capabilities_info &ht,
        XMLNode &node) 
    {
      node.add("ldpc", (int)ht.ht.ldpc);
      node.add("channel40mhz", (int)ht.ht.channel40mhz);
      node.add("sm_pwr_save");
      switch (ht.ht.sm_pwr_save) {
        case 0: node.get_last_child().set_value("static"); break;
        case 1: node.get_last_child().set_value("dynamic");break;
        case 3: node.get_last_child().set_value("disabled");break;
        default: node.get_last_child().set_value("reserved");break;
      }
      node.add("ht_green", (int)ht.ht.ht_green);
      node.add("short_gi_20mhz", (int)ht.ht.short_gi_20mhz);
      node.add("short_gi_40mhz", (int)ht.ht.short_gi_40mhz);
      node.add("tx_stbc", (int)ht.ht.tx_stbc);
      node.add("rx_stbc", (int)ht.ht.rx_stbc);
      node.add("dsss40mhz", (int)ht.ht.dsss40mhz);
      XMLNode mcsnode("supported_mcs");
      {
        std::vector<int> mcs_set = ht.mcs.get_mcs_set();
        for (int i = 0; i < mcs_set.size(); i++)
          mcsnode.add("mcs", mcs_set[i]);
        mcsnode.add("rx_highest_rate", (int)ht.mcs.rx_highest_rate);
        mcsnode.add("tx_mcs_set_defined", (int)ht.mcs.tx_mcs_defined);
        mcsnode.add("tx_rx_mcs_not_equal", (int)ht.mcs.tx_rx_mcs_not_equal);
        mcsnode.add("tx_unequal_modulation", (int)ht.mcs.tx_unequal_modulation);
        mcsnode.add("tx_max_spatial_streams", (int)ht.mcs.tx_max_streams);
      }
      node.add(mcsnode);
    }


/***********************************************************************/
/***************** PRINT CONTROL FRAME *********************************/
/***********************************************************************/

    XMLNode wifi_dump_formatter_xml::xmlnode_control_frame(const mac_frame_info &info) {
      XMLNode node("control_frame");
      switch (info.fc.subtype) {
        case FRAME_SUBTYPE_CONTROL_RTS:
        case FRAME_SUBTYPE_CONTROL_BLOCK_ACK_REQ:
        case FRAME_SUBTYPE_CONTROL_BLOCK_ACK:
          node.add(XMLNode("addresses").add("ta", addr_str(info.addr[1]))
            .add("ra", addr_str(info.addr[0])));
          break;

        case FRAME_SUBTYPE_CONTROL_CTS:
        case FRAME_SUBTYPE_CONTROL_ACK:
          node.add(XMLNode("addresses").add("ra", addr_str(info.addr[0])));
          break;

        case FRAME_SUBTYPE_CONTROL_PSPOLL:
          node.add(XMLNode("addresses").add("ta", addr_str(info.addr[1]))
            .add(XMLNode("ra", addr_str(info.addr[0])).set_attr("bssid", true)));
          break;

        case FRAME_SUBTYPE_CONTROL_CFEND:
        case FRAME_SUBTYPE_CONTROL_CFEND_CFACK:
          node.add(XMLNode("addresses").add(XMLNode("ta", addr_str(info.addr[1])).set_attr("bssid", true))
            .add("ra", addr_str(info.addr[0])));
          break;
      }
      return node;
    }




/***********************************************************************/
/***************** PRINT DATA FRAME ************************************/
/***********************************************************************/

    XMLNode wifi_dump_formatter_xml::xmlnode_data_frame(const mac_frame_info &info) {
      XMLNode node("data_frame");

      // print addresses, note: we don't take care of the A-MSDU case
      node.add("addresses");
      node.get_last_child().add("ta", addr_str(info.addr[1]))
        .add("ra", addr_str(info.addr[0]));
      switch (info.fc.toDS + info.fc.fromDS * 2) {
        case 0: // ToDS = 0, FromDS = 0
          node.get_last_child().add("bssid", addr_str(info.addr[2]));
          break;
        case 1: // ToDS = 1, FromDS = 0
          node.get_last_child().add("da", addr_str(info.addr[2]));
          break;
        case 2: // ToDS = 0, FromDS = 1
          node.get_last_child().add("sa", addr_str(info.addr[2]));
          break;
        case 3: // ToDS = 1, FromDS = 1
          node.get_last_child().add("da", addr_str(info.addr[2]))
            .add("sa", addr_str(info.addr[3]));
          break;
      }

      node.add(xmlnode_sequence_control(info));

      if (info.fc.is_qos_data()) {
        node.add(xmlnode_qos_control(info));
        if (info.fc.order)
          node.add(xmlnode_ht_control(info));
      }
      return node;
    }

    XMLNode wifi_dump_formatter_xml::xmlnode_sequence_control(const mac_frame_info &info) {
      return XMLNode("sequence_control")
                .add("seq_no", (int)info.seq_control.seq_no)
                .add("frag_no", (int)info.seq_control.fragment_no);
    }


    XMLNode wifi_dump_formatter_xml::xmlnode_qos_control(const mac_frame_info &info) {
      XMLNode node("qos_control");

      node.add("tid", (int)info.qos_control.tid);
      node.add("eosp", (int)info.qos_control.eosp);

      // ack policy
      switch (info.qos_control.ack_policy) {
        case 0:
          node.add("ack_policy", "Normal Ack / Implicit Block Ack Request");
          break;
        case 1:
          node.add("ack_policy", "No Ack");
          break;
        case 2:
          node.add("ack_policy", "No explicit Ack / PSMP Ack");
          break;
        case 3:
          node.add("ack_policy", "Block Ack");
          break;
      }
      node.get_last_child().set_attr("code", (int)info.qos_control.ack_policy);

      // A-MSDU
      if (info.fc.subtype >= FRAME_SUBTYPE_DATA_QOS_DATA && 
        info.fc.subtype <= FRAME_SUBTYPE_DATA_QOS_DATA_CFACKPOLL) {
        node.add("amsdu", (int)info.qos_control.amsdu);
      }

      // TXOP Limit
      node.add("txop", (int)info.qos_control.txop);

      return node;
    }

    XMLNode wifi_dump_formatter_xml::xmlnode_ht_control(const mac_frame_info &info) {
      return XMLNode("ht_control");
    }



/***********************************************************************/
/***************** HELPER FUNCTIONS ************************************/
/***********************************************************************/

    std::string wifi_dump_formatter_xml::addr_str(const char *byte) {
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

    std::string wifi_dump_formatter_xml::yesno_str(bool v, const std::string &caption) {
      return v ? caption + ": Yes" : caption + ": No";
    }


  } /* namespace swifi */
} /* namespace gr */

