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


#include <gnuradio/attributes.h>
#include <cppunit/TestAssert.h>
#include "qa_phy_utils.h"
#include <swifi/phy_utils.h>
#include <algorithm>

namespace gr {
  namespace swifi {

    void
    qa_phy_utils::t1()
    {
      //--- Test scrambling 127-bit all-zero data with seed 0b1011101
      //--- Example taken from IEEE 802.11's Table L-14
      char data[127];
      const char *expected = "0110110000011001101010011100111101101000010101011111010010100011011100011111110000111011110010110010010000001000100110001011101";

      memset(data, 0, 127);
      phy_utils::scramble(data, 127, 0b1011101);
      for (int i = 0; i < 127; i++)
        data[i] = data[i] + '0';  // convert bit 0/1 to letter '0'/'1' for comparison
      CPPUNIT_ASSERT(std::equal(data, data + 127, expected));
    }

    void
    qa_phy_utils::t2()
    {
      //--- Test scrambling 127-bit all-zero data with seed 0b1011101
      //--- The scrambling is called for subsequence instead of whole sequence
      //--- in comparison with test t1.
      char data[127];
      const char *expected = "0110110000011001101010011100111101101000010101011111010010100011011100011111110000111011110010110010010000001000100110001011101";

      memset(data, 0, 127);
      char seed = 0b1011101;
      for (int k = 0; k < 12; k++) {
        seed = phy_utils::scramble(data + k*10, 10, seed);
      }
      seed = phy_utils::scramble(data + 120, 7, seed);
      for (int i = 0; i < 127; i++)
        data[i] = data[i] + '0';  // convert bit 0/1 to letter '0'/'1' for comparison
      CPPUNIT_ASSERT(std::equal(data, data + 127, expected));
    }

  } /* namespace swifi */
} /* namespace gr */

