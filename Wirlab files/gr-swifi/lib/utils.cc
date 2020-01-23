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

#include "utils.h"
#include <string.h>
#include <math.h>
#include <iostream>
#include <stdio.h>

namespace gr {
  namespace swifi {

    std::string hexstr(const char *bytes, int len) {
      char buf[3+1];
      std::string s;
      for (int i = 0; i < len; i++) {
        sprintf(buf, "%02x ", int(bytes[i]) & 0xFF);
        s.append(buf, 3);
      }
      return s;
    }

    void print_hextable(const char *bytes, int len, int cols) {
      for (int i = 0; i < len; i++) {
        printf("%02X ", bytes[i]);
        if ((i + 1) % cols == 0) printf("\n");
      }
      if (len % cols != 0) printf("\n");
    }

    std::string hextable(const char *data, int len, int cols) {
      char line[3*cols];
      char *p;
      int nrows = len / cols + (len % cols == 0 ? 0 : 1);
      int i;
      std::string s;
      for (int row = 0; row < nrows; row++) {
        p = line; // point to the beginning of line
        i = row * cols;
        for (int col = 0; col < cols; col++) {
          if (i < len) {
            sprintf(p, "%02x ", int(data[i++]) & 0xFF);
            p += 3;
          }
          else break;
        }
        s.append(line, size_t(p - line));
        s.append(1, '\n');
      }
      return s;
    }

    std::string raw_data_hextable(const char *data, int len, int cols) {
      char line[4*cols+1];
      char *p;
      std::string s;
      int nrows = len / cols + (len % cols == 0 ? 0 : 1);
      int row, col, i;
      for (row = 0; row < nrows; row++) {
        p = line;  // point to the beginning of line

        // print hex numbers on left half of the line
        i = row * cols;
        for (col = 0; col < cols; col++) {
          if (i < len) sprintf(p, "%02x ", int(data[i++]) & 0xFF);
          else sprintf(p, "   ");
          p += 3;
        }

        // print raw data on right half of the line
        // Note that only characters with ascii code >= 32 are printed out
        // ('.' will replace for those < 32)
        i = row * cols;
        for (col = 0; col < cols; col++) {
          if (i < len) {
            if (int(data[i] & 0xFF) >= 32) sprintf(p, "%c", data[i++]);
            else sprintf(p, ".");
            p++;
          }
        }

        // add the line to final string
        s.append(line, size_t(p - line));
        s.append(1, '\n');
      }
      return s;
    }

    std::vector<std::string> strsplit(const std::string &s, char delim) {
      std::vector<std::string> results;
      int start = 0, i = 0;
      for (i = 0; i < s.size(); i++) {
        if (s[i] == delim) {
          std::string newpart(&s[start], i - start);
          results.push_back(newpart);
          while (s[i+1] == delim) i++;
          start = i+1;
        }
      }
      if (i > start) {  // only add non-empty part
        results.push_back(std::string(&s[start], i - start));
      }
      return results;
    }

  } /* namespace swifi */
} /* namespace gr */

