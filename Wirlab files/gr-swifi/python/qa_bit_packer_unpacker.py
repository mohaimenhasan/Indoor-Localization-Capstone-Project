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
import pmt
import swifi
import numpy

class qa_bit_packer_unpacker (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def run_test(self, rate, qary):
        # create random q-ary data
        length = 1*2*4*6*10000
        data = numpy.random.randint(0, qary, length).tolist()

        # set source with tags.
        # Note: Two tags are inserted, one at beginning, one at the middle
        tags=[swifi.phy_tag(offset=0, key='phy', nitems=length/2, rate=rate),
              swifi.phy_tag(offset=length/2, key='phy', nitems=length/2, rate=rate)]
        source = blocks.vector_source_b(data, repeat=False, vlen=1, tags=tags)

        # unpacker and packer
        unpacker = swifi.bit_unpacker("phy")
        packer = swifi.bit_packer("phy")

        # vector sink
        sink = blocks.vector_sink_b()

        # connecting blocks together
        self.tb.connect(source,
            unpacker,
            packer,
            sink)

        self.tb.run()

        self.assertEqual(data, list(sink.data()))

    def test_001_t (self):
        self.run_test(swifi.phy_utils.BPSK_1_2, 2)

    def test_002_t (self):
        self.run_test(swifi.phy_utils.BPSK_3_4, 2)

    def test_003_t (self):
        self.run_test(swifi.phy_utils.QPSK_1_2, 4)

    def test_004_t (self):
        self.run_test(swifi.phy_utils.QPSK_3_4, 4)

    def test_005_t (self):
        self.run_test(swifi.phy_utils.QAM16_1_2, 16)

    def test_006_t (self):
        self.run_test(swifi.phy_utils.QAM16_3_4, 16)

    def test_007_t (self):
        self.run_test(swifi.phy_utils.QAM64_2_3, 64)

    def test_008_t (self):
        self.run_test(swifi.phy_utils.QAM64_3_4, 64)

if __name__ == '__main__':
    gr_unittest.run(qa_bit_packer_unpacker, "qa_bit_packer_unpacker.xml")
