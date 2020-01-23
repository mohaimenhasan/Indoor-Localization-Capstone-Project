#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2015 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import swifi_swig as swifi

class qa_phy_utils (gr_unittest.TestCase):

    def setUp (self):
        pass

    def tearDown (self):
        pass

    def test_001_t (self):
        #self.tb.run ()
        p = swifi.phy_utils()
        rate = p.QAM16_1_2
        content = "hello 1"
        ppdu = p.create_frame(rate, content)
        expected ="\
\xE9\x00\x02\x00\x00\
\x68\x65\x6C\x6C\x6F\
\x20\x31\x00\x00\x00"
        # for k in xrange(len(ppdu)):
        #     if k % 5 == 0:
        #         print ""
        #     print "0x{0:02X}".format(ord(ppdu[k])),
        # print ppdu

        self.assertEqual(ppdu, tuple(expected))


if __name__ == '__main__':
    gr_unittest.run(qa_phy_utils, "qa_phy_utils.xml")
