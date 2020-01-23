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

class qa_scrambler (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None


    ###################################################################################
    ### SCRAMBLING + DESCRAMBLING TEST ################################################
    ###################################################################################

    def run_scrambling_descrambling_test(self, data, flag=0):
        # set source block
        tags = [swifi.phy_tag(offset=0, key='phy', flag=flag)]
        source = blocks.vector_source_b(data, repeat=False, vlen=1, tags=tags)

        # set scrambler and descrambler
        scrambler = swifi.scrambler(is_scrambler=True, phy_tag_key='phy')
        descrambler = swifi.scrambler(is_scrambler=False, phy_tag_key='phy')

        # set sink for comparing result
        sink = blocks.vector_sink_b()

        # connect blocks together
        self.tb.connect(source, scrambler, descrambler, sink)

        self.tb.run()

        # check
        result = [int(x) for x in sink.data()]
        self.assertEqual(data, result)

    def test_scrambling_descrambling_001_t(self):
        data = swifi.binstr2binv('\
000000000000000000100000\
010000000000000001110100\
000000000000011000010000\
101100111110110001100101\
000000000000010001101011\
100000000011110010001111\
000000000000011000010000\
101101011101110011110101\
')
        # run scrambling-descrambling without 6 tail bits set to 0 (flag=1)
        self.run_scrambling_descrambling_test(data, flag=swifi.PHY_SCRAMBLER_FLAG)


    def test_scrambling_descrambling_002_t(self):
        # data source is the DATA field taken from IEEE 802.11's Table L-13
        data = swifi.str2binv('\x00\x00\x04\x02\x00\x2E\x00\x60\x08\xCD\x37\xA6\x00\x20\xD6\x01\x3C\xF1\x00\x60\x08\xAD\x3B\xAF\x00\x00\x4A\x6F\x79\x2C\x20\x62\x72\x69\x67\x68\x74\x20\x73\x70\x61\x72\x6B\x20\x6F\x66\x20\x64\x69\x76\x69\x6E\x69\x74\x79\x2C\x0A\x44\x61\x75\x67\x68\x74\x65\x72\x20\x6F\x66\x20\x45\x6C\x79\x73\x69\x75\x6D\x2C\x0A\x46\x69\x72\x65\x2D\x69\x6E\x73\x69\x72\x65\x64\x20\x77\x65\x20\x74\x72\x65\x61\x67\x33\x21\xB6\x00\x00\x00\x00\x00\x00')

        # run scrambling-descrambling without 6 tail bits set to 0 (flag=1)
        self.run_scrambling_descrambling_test(data, flag=swifi.PHY_SCRAMBLER_FLAG)



    ###################################################################################
    ### SCRAMBLING ONLY TEST ##########################################################
    ###################################################################################

    def run_scrambling_test(self, data, expected, npayloadbytes=0, flag=0):
        # set source block
        tags = [swifi.phy_tag(offset=0, key='phy', nitems=npayloadbytes, flag=flag)]
        source = blocks.vector_source_b(data, repeat=False, vlen=1, tags=tags)

        # set scrambler and descrambler
        scrambler = swifi.scrambler(is_scrambler=True, phy_tag_key='phy')

        # set sink for comparing result
        sink = blocks.vector_sink_b()

        # connect blocks together
        self.tb.connect(source, scrambler, sink)

        self.tb.run()

        # check
        result = [int(x) for x in sink.data()]
        self.assertEqual(result, expected)


    def test_scrambling_001_t(self):
        # data source is an example of DATA field taken first 192 bits from IEEE 802.11's Table L-13, L-15 
        data = [
                0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,1,0,0,0,0,0,
                0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,1,1,1,0,1,0,0,
                0,0,0,0,0,0,0,0, 0,0,0,0,0,1,1,0, 0,0,0,1,0,0,0,0,
                1,0,1,1,0,0,1,1, 1,1,1,0,1,1,0,0, 0,1,1,0,0,1,0,1,
                0,0,0,0,0,0,0,0, 0,0,0,0,0,1,0,0, 0,1,1,0,1,0,1,1,
                1,0,0,0,0,0,0,0, 0,0,1,1,1,1,0,0, 1,0,0,0,1,1,1,1,
                0,0,0,0,0,0,0,0, 0,0,0,0,0,1,1,0, 0,0,0,1,0,0,0,0,
                1,0,1,1,0,1,0,1, 1,1,0,1,1,1,0,0, 1,1,1,1,0,1,0,1] 

        expected = [
                0,1,1,0,1,1,0,0, 0,0,0,1,1,0,0,1, 1,0,0,0,1,0,0,1,
                1,0,0,0,1,1,1,1, 0,1,1,0,1,0,0,0, 0,0,1,0,0,0,0,1,
                1,1,1,1,0,1,0,0, 1,0,1,0,0,1,0,1, 0,1,1,0,0,0,0,1,
                0,1,0,0,1,1,1,1, 1,1,0,1,0,1,1,1, 1,0,1,0,1,1,1,0,
                0,0,1,0,0,1,0,0, 0,0,0,0,1,1,0,0, 1,1,1,1,0,0,1,1,
                0,0,1,1,1,0,1,0, 1,1,1,0,0,1,0,0, 1,0,1,1,1,1,0,0,
                0,1,0,1,0,0,1,1, 1,0,0,1,1,0,0,0, 1,1,0,0,0,0,0,0,
                0,0,0,1,1,1,1,0, 0,0,1,1,0,1,0,1, 1,0,1,1,0,0,1,1]

        # run scrambling without 6 tail bits set to 0 (flag=1)
        self.run_scrambling_test(data, expected, flag=swifi.PHY_SCRAMBLER_FLAG)


    def test_scrambling_002_t(self):
        # data source is the DATA field taken from IEEE 802.11's Table L-13
        data = swifi.str2binv('\x00\x00\x04\x02\x00\x2E\x00\x60\x08\xCD\x37\xA6\x00\x20\xD6\x01\x3C\xF1\x00\x60\x08\xAD\x3B\xAF\x00\x00\x4A\x6F\x79\x2C\x20\x62\x72\x69\x67\x68\x74\x20\x73\x70\x61\x72\x6B\x20\x6F\x66\x20\x64\x69\x76\x69\x6E\x69\x74\x79\x2C\x0A\x44\x61\x75\x67\x68\x74\x65\x72\x20\x6F\x66\x20\x45\x6C\x79\x73\x69\x75\x6D\x2C\x0A\x46\x69\x72\x65\x2D\x69\x6E\x73\x69\x72\x65\x64\x20\x77\x65\x20\x74\x72\x65\x61\x67\x33\x21\xB6\x00\x00\x00\x00\x00\x00')

        # expected output taken from IEEE 802.11's Table L-15
        expected = swifi.str2binv_msb('\x6C\x19\x89\x8F\x68\x21\xF4\xA5\x61\x4F\xD7\xAE\x24\x0C\xF3\x3A\xE4\xBC\x53\x98\xC0\x1E\x35\xB3\xE3\xF8\x25\x60\xD6\x25\x35\x33\xFE\xF0\x41\x2B\x8F\x53\x1C\x83\x41\xBE\x39\x28\x66\x44\x66\xCD\xF6\xA3\xD8\x0D\xD4\x81\x3B\x2F\xDF\xC3\x58\xF7\xC6\x52\xEB\x70\x8F\x9E\x6A\x90\x81\xFD\x7C\xA9\xD1\x55\x12\x04\x74\xD9\xE9\x3B\xCD\x93\x8D\x7B\x7C\x70\x02\x20\x99\xA1\x7D\x8A\x27\x17\x39\x15\xA0\xEC\x95\x16\x91\x10\x00\xDC\x7F\x0E\xF2\xC9')

        # run scrambling with 6 tail bits set to 0 (flag=3)
        self.run_scrambling_test(data, expected, npayloadbytes=100, 
            flag=swifi.PHY_SCRAMBLER_FLAG | swifi.PHY_TAIL_SET_FLAG)



if __name__ == '__main__':
    gr_unittest.run(qa_scrambler, "qa_scrambler.xml")
