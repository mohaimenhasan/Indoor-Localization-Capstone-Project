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

#ifndef INCLUDED_SWIFI_UTILS_H
#define INCLUDED_SWIFI_UTILS_H

#include <swifi/api.h>
#include <gnuradio/tags.h>
#include <vector>
#include <string>
#include <stdint.h>

namespace gr {
  namespace swifi {

    std::string hexstr(const char *bytes, int len);

    void print_hextable(const char *bytes, int len, int cols = 16);

    std::string hextable(const char *data, int len, int cols = 16);

    std::string raw_data_hextable(const char *data, int len, int cols = 16);

    std::vector<std::string> strsplit(const std::string &s, char delim);


  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_UTILS_H */

