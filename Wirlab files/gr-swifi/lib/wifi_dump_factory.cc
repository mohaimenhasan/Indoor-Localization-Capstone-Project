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

#include <swifi/wifi_dump_factory.h>
#include <swifi/wifi_dump_formatter.h>
#include <swifi/phy_utils.h>
#include <string>
#include <stdio.h>

namespace gr {
  namespace swifi {

    wifi_dump_factory::FormatterMap wifi_dump_factory::formatter = 
      wifi_dump_factory::FormatterMap();

    wifi_dump_formatter::sptr wifi_dump_factory::make(
        const std::string &filename,
        const std::string &format_id,
        const std::string &filter_string,
        bool is_print_raw)
    {
      if (formatter.count(format_id))
        // create a new instance from the specified formatter
        return formatter.at(format_id)->make(filename,
          filter_string, is_print_raw); 

      // specified formatter not found
      throw std::runtime_error(
        std::string("WiFi dump ") + format_id + std::string(" not found"));
    }

    bool wifi_dump_factory::add(const std::string &format_id,
      wifi_dump_formatter::sptr instance)
    {
      formatter.insert(std::pair<std::string,wifi_dump_formatter::sptr>(
        format_id, instance));
      return true;
    }
  } /* namespace swifi */
} /* namespace gr */

