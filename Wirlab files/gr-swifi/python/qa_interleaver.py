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

class qa_interleaver (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None


    ###################################################################################
    ### INTERLEAVING TEST #############################################################
    ###################################################################################

    def run_interleaving_test(self, data, expected, rate):
        # set source block
        tags = [swifi.phy_tag(offset=0, key='phy', rate=rate)]
        source = blocks.vector_source_b(data, repeat=False, vlen=1, tags=tags)

        # set interleaving
        interleaver = swifi.interleaver(True, 'phy')

        # set sink for comparing result
        sink = blocks.vector_sink_b()

        # connect blocks together
        self.tb.connect(source, interleaver, sink)

        self.tb.run()

        # check
        self.assertEqual(sink.data(), tuple(expected))

    def test_interleaving_001_t(self):
        # data is encoded SIGNAL field taken from IEEE 802.11 Table L-8
        data = swifi.binstr2binv('110100011010000100000010001111100111000000000000')
        expected = swifi.binstr2binv('100101001101000000010100100000110010010010010100')
        self.run_interleaving_test(data, expected, rate=swifi.phy_utils.BPSK_1_2)

    def test_interleaving_002_t(self):
        # data is the first symbol of encoded DATA field taken from IEEE 802.11 Table L-16
        data = swifi.str2binv_msb('\x2B\x08\xA1\xF0\x9D\xB5\x9A\x1D\x4A\xFB\xE8\xC2\x8F\xC0\xC8\x73\xC0\x43\xE0\x19\xE0\xD3\xEB\xB2')
        expected = swifi.binstr2binv('\
01110111111100001110111111000100\
01110011000000001011111100010001\
00010000100110100001110100010010\
01101110001110001111010101101001\
00011011011010111001100001000011\
00000000000011011011001101101101\
')
        self.run_interleaving_test(data, expected, rate=swifi.phy_utils.QAM16_3_4)


if __name__ == '__main__':
    gr_unittest.run(qa_interleaver, "qa_interleaver.xml")
