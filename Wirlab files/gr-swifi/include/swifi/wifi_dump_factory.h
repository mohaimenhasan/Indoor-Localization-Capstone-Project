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


#ifndef INCLUDED_SWIFI_WIFI_DUMP_FACTORY_H
#define INCLUDED_SWIFI_WIFI_DUMP_FACTORY_H

#include <swifi/api.h>
#include <swifi/mac_frame_info.h>
#include <swifi/wifi_dump_formatter.h>

namespace gr {
  namespace swifi {

    /*!
     * \brief WiFi Dump.
     */
    class SWIFI_API wifi_dump_factory
    {
     protected:
      typedef std::map<std::string, wifi_dump_formatter::sptr> FormatterMap;
      static FormatterMap formatter;

     public:

      static wifi_dump_formatter::sptr make(
        const std::string &filename = "<<stdout>>",
        const std::string &format_id = "text",
        const std::string &filter_string = "",
        bool is_print_raw = false);

      static bool add(const std::string &format_id,
        wifi_dump_formatter::sptr instance);

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_WIFI_DUMP_FACTORY_H */

