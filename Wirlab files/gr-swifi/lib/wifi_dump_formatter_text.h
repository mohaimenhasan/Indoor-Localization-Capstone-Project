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

#ifndef INCLUDED_SWIFI_WIFI_DUMP_FORMATTER_TEXT_H
#define INCLUDED_SWIFI_WIFI_DUMP_FORMATTER_TEXT_H

#include "simple_filter.h"
#include "wifi_dump_formatter_xml.h"
#include <swifi/api.h>
#include <swifi/mac_frame_info.h>
#include <swifi/wifi_dump_formatter.h>

namespace gr {
  namespace swifi {

    /*!
     * \brief WiFi Dump Formatter.
     */
    class SWIFI_API wifi_dump_formatter_text : public wifi_dump_formatter
    {
     public:
      typedef boost::shared_ptr<wifi_dump_formatter_text> sptr;

      wifi_dump_formatter::sptr make(const std::string &filename = "<<stdout>>",
        const std::string &filter_string = "",
        bool is_print_raw = false);

      wifi_dump_formatter_text(const std::string &filename = "<<stdout>>",
        const std::string &filter_string = "",
        bool is_print_raw = false);
      ~wifi_dump_formatter_text();

      void dump(const mac_frame_info &info);

     protected:
      FILE *f;
      bool is_print_raw;
      boost::shared_ptr<simple_filter> frame_filter;
      boost::shared_ptr<wifi_dump_formatter_xml> xml_formatter;

      void print_frame_type(const mac80211_frame_control_field &fc);
      void print_frame_control(const mac80211_frame_control_field &fc);
      void print_management_frame(const mac_frame_info &info);
      void print_control_frame(const mac_frame_info &info);
      void print_data_frame(const mac_frame_info &info);

      void print_management_beacon(const mac_frame_info &info);
      void print_ies(const mac_frame_info &info);
      void print_ie_ht_capabilities(const ie_ht_capabilities_info &ht);

      void print_sequence_control(const mac_frame_info &info);
      void print_qos_control(const mac_frame_info &info);
      void print_ht_control(const mac_frame_info &info);

      std::string addr_str(const char *byte);
      std::string yesno_str(bool v, const std::string &caption = "");
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_WIFI_DUMP_FORMATTER_TEXT_H */

