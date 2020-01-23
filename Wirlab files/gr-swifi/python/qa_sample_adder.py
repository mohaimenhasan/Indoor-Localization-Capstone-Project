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
from gnuradio import digital
import swifi
import sys

class qa_sample_adder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None


    def run_sample_adder_test_no_frame(self, data, data_len, num_zeros, expected):
        # set source blocks
        source = blocks.vector_source_b(data, repeat=False, vlen=1)

        # set modulator
        modulator = swifi.chunks_to_symbols(swifi.wifi_constellation_bpsk().points())

        # set sample_adder
        sample_adder = swifi.sample_adder(data_len, 0, num_zeros)

        # set demodulator
        demodulator = swifi.constellation_decoder(swifi.wifi_constellation_bpsk().base())

        # set sink(s) for comparing result
        sink = blocks.vector_sink_b()

        # connect blocks together
        self.tb.connect(source, modulator)
        self.tb.connect(modulator, sample_adder)
        self.tb.connect(sample_adder, demodulator)
        self.tb.connect(demodulator, sink)

        self.tb.run()

        # check
        result = [int(x) for x in sink.data()]
        self.assertEqual(result, expected)


    #################################################################################
    #Test
    #################################################################################
    def test_sample_adder_001_t(self):
        data =          [1,0,0,1,0,1,0,1,0,        1,0,0,1,0,1,0,1,0]
        expected =  [0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,1,0,1,0,1,0]
        data_len = 9
        num_zeros = 2

        self.run_sample_adder_test_no_frame(data=data,
                                data_len=data_len,
                                num_zeros=num_zeros,
                                expected=expected)



if __name__ == '__main__':
    gr_unittest.run(qa_sample_adder, "qa_sample_adder.xml")
