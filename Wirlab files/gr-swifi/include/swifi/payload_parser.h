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

 
#ifndef INCLUDED_SWIFI_PAYLOAD_PARSER_H
#define INCLUDED_SWIFI_PAYLOAD_PARSER_H

#include <swifi/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace swifi {

  class SWIFI_API payload_parser : virtual public sync_block
  {
  public:

    typedef boost::shared_ptr<payload_parser> sptr;
    static sptr make(const std::string &phy_tag_key="phy",
        const std::string &filename = "<<stdout>>",
        const std::string &wifi_dump_format = "text",
        const std::string &filter_string = "",
        bool is_print_raw = false);

  };

  }  // namespace swifi
}  // namespace gr

#endif /* INCLUDED_SWIFI_PAYLOAD_PARSER_H */


