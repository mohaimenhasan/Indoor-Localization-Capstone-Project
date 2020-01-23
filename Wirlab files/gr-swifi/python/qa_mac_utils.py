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
import swifi#_swig as swifi
import sys

class qa_mac_utils (gr_unittest.TestCase):

    def setUp (self):
        pass

    def tearDown (self):
        pass

    def test_001_t (self):
        m = swifi.mac_utils()

        # example taken from IEEE 802.11's Annex L
        m.set_type(swifi.FRAME_TYPE_CONTROL, swifi.FRAME_SUBTYPE_CONTROL_RESERVED0)
        m.set_toDS_fromDS(0, 1)
        m.set_duration(11776) #  // 11776us (0x2E00)
        m.set_addr(swifi.MAC_ADDR1, [0x00, 0x60, 0x08, 0xCD, 0x37, 0xA6])
        m.set_addr(swifi.MAC_ADDR2, [0x00, 0x20, 0xD6, 0x01, 0x3C, 0xF1])
        m.set_addr(swifi.MAC_ADDR3, [0x00, 0x60, 0x08, 0xAD, 0x3B, 0xAF])
        m.set_seq(0)
        m.set_data("""Joy, bright spark of divinity,
Daughter of Elysium,
Fire-insired we trea""", 72)
        mpdu = m.serialize()

        expected_mpdu = "\
\x04\x02\x00\x2E\x00\
\x60\x08\xCD\x37\xA6\
\x00\x20\xD6\x01\x3C\
\xF1\x00\x60\x08\xAD\
\x3B\xAF\x00\x00\x4A\
\x6F\x79\x2C\x20\x62\
\x72\x69\x67\x68\x74\
\x20\x73\x70\x61\x72\
\x6B\x20\x6F\x66\x20\
\x64\x69\x76\x69\x6E\
\x69\x74\x79\x2C\x0A\
\x44\x61\x75\x67\x68\
\x74\x65\x72\x20\x6F\
\x66\x20\x45\x6C\x79\
\x73\x69\x75\x6D\x2C\
\x0A\x46\x69\x72\x65\
\x2D\x69\x6E\x73\x69\
\x72\x65\x64\x20\x77\
\x65\x20\x74\x72\x65\
\x61\x67\x33\x21\xB6"
        # for k in xrange(len(mpdu)):
        #     if k % 5 == 0:
        #         print ""
        #     print "0x{0:02X}".format(ord(mpdu[k])),
        # print mpdu
        # print m.parse_recv_bytes(expected_mpdu, 100)

        self.assertEqual(mpdu, tuple(expected_mpdu))

    def test_002_t (self):
        m = swifi.mac_utils()

        # example taken from IEEE 802.11's Annex L
        mpdu = "\
\x04\x02\x00\x2E\x00\
\x60\x08\xCD\x37\xA6\
\x00\x20\xD6\x01\x3C\
\xF1\x00\x60\x08\xAD\
\x3B\xAF\x00\x00\x4A\
\x6F\x79\x2C\x20\x62\
\x72\x69\x67\x68\x74\
\x20\x73\x70\x61\x72\
\x6B\x20\x6F\x66\x20\
\x64\x69\x76\x69\x6E\
\x69\x74\x79\x2C\x0A\
\x44\x61\x75\x67\x68\
\x74\x65\x72\x20\x6F\
\x66\x20\x45\x6C\x79\
\x73\x69\x75\x6D\x2C\
\x0A\x46\x69\x72\x65\
\x2D\x69\x6E\x73\x69\
\x72\x65\x64\x20\x77\
\x65\x20\x74\x72\x65\
\x61\x67\x33\x21\xB6"

        self.assertTrue(m.check_recv_bytes(mpdu, 100))

if __name__ == '__main__':
    gr_unittest.run(qa_mac_utils, "qa_mac_utils.xml")
