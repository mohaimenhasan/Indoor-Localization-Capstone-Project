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


#ifndef INCLUDED_SWIFI_WIFI_DUMP_FORMATTER_H
#define INCLUDED_SWIFI_WIFI_DUMP_FORMATTER_H

#include <swifi/api.h>
#include <swifi/mac_frame_info.h>
#include <stdint.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    /*!
     * \brief WiFi Dump Formatter.
     */
    class SWIFI_API wifi_dump_formatter
    {
     public:
      typedef boost::shared_ptr<wifi_dump_formatter> sptr;

      virtual sptr make(const std::string &filename = "<<stdout>>",
        const std::string &filter_string = "",
        bool is_print_raw = false) = 0;

      virtual void dump(const mac_frame_info &info) = 0;

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_WIFI_DUMP_FORMATTER_H */

