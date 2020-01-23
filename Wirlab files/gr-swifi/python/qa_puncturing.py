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

class qa_puncturing (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None


    #####################################################################
    ### PUNCTURING TESTS ################################################
    #####################################################################

    def run_puncturing_test(self, rate, data, expected):
        # set source block
        tags = [swifi.phy_tag(offset=0, key='phy', rate=rate)]
        source = blocks.vector_source_b(data, repeat=False, vlen=1, tags=tags)

        # set scrambler and descrambler
        puncture = swifi.bit_stealer('phy')

        # set sink for comparing result
        sink = blocks.vector_sink_b()

        # connect blocks together
        self.tb.connect(source, puncture, sink)

        self.tb.run()

        # check
        self.assertEqual(sink.data(), tuple(expected))

    def test_puncturing_001_t(self):
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
        expected = data
        self.run_puncturing_test(swifi.phy_utils.BPSK_1_2, data, expected)

    def test_puncturing_002_t(self):
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
        expected = swifi.binstr2binv('\
0000000000001000\
0100000000011100\
0000000001100100\
1010111011011001\
0000000001011011\
1000000111000011\
0000000001100100\
1011011111011101\
')
        self.run_puncturing_test(swifi.phy_utils.BPSK_3_4, data, expected)



    #####################################################################
    ### DEPUNCTURING TESTS ##############################################
    #####################################################################

    def run_depuncturing_test(self, rate, data, expected):
        # set source block
        tags = [swifi.phy_tag(offset=0, key='phy', rate=rate)]
        source = blocks.vector_source_b(data, repeat=False, vlen=1, tags=tags)

        # set scrambler and descrambler
        depuncture = swifi.bit_inserter('phy')

        # set sink for comparing result
        sink = blocks.vector_sink_b()

        # connect blocks together
        self.tb.connect(source, depuncture, sink)

        self.tb.run()

        # check
        self.assertEqual(sink.data(), tuple(expected))

    def test_depuncturing_001_t(self):
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
        expected = data
        self.run_depuncturing_test(swifi.phy_utils.BPSK_1_2, data, expected)

    def test_depuncturing_002_t(self):
        data = swifi.binstr2binv('\
0000000000001000\
0100000000011100\
0000000001100100\
1010111011011001\
0000000001011011\
1000000111000011\
0000000001100100\
1011011111011101\
')
        expected = swifi.binstr2binv('\
000000000000000000100000\
010000000000000001110000\
000000000000011000010000\
101000111000110001100001\
000000000000010001101001\
100000000001110000001001\
000000000000011000010000\
101001011001110001110001\
')
        self.run_depuncturing_test(swifi.phy_utils.BPSK_3_4, data, expected)

if __name__ == '__main__':
    gr_unittest.run(qa_puncturing, "qa_puncturing.xml")
