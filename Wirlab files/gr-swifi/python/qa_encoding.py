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
import sys

class qa_encoding (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None


    #########################################################################################
    ### ENCODING TEST #######################################################################
    #########################################################################################

    def run_encoding_test(self, binary_data, expected, bytes_per_block, nitems, rate, flag):
        """
        binary_data: sequence of bits
        expected: must be a list/tuple of binary integers 0 and 1
        """

        bits_per_block = bytes_per_block * 8
        length = int(len(binary_data) / bits_per_block) * bits_per_block
        binary_data = binary_data[0:length]   # truncate binary_data to a multiple of bits_per_block
        nblocks = len(binary_data) / bits_per_block

        # set source with tag
        tags = []
        for i in xrange(nblocks):
            tags.append(swifi.phy_tag(key='phy', 
                                      offset=i*bits_per_block, 
                                      nitems=nitems,
                                      rate=rate,
                                      flag=flag))
        source = blocks.vector_source_b(binary_data, repeat=False, vlen=1, tags=tags)
        
        # scrambler
        scrambler = swifi.scrambler(True, 'phy')

        # convolutional encoder
        conv = swifi.wifi_conv_encoder('phy')

        # puncturing
        bit_stealer = swifi.bit_stealer('phy')

        # interleaving
        interleaver = swifi.interleaver(True, 'phy')

        # vector sink
        sink = blocks.vector_sink_b()

        # connecting blocks together
        self.tb.connect(source,
            scrambler,
            conv,
            bit_stealer,
            interleaver,
            sink)

        self.tb.run()

        self.assertEqual(sink.data(), tuple(expected))

    def test_encode_signal_field_t (self):
        # data source is SIGNAL field taken from IEEE 802.11 standard's Table L-7
        # expected result is taken from Table L-9
        self.run_encoding_test(
            binary_data=swifi.binstr2binv('101100010011000000000000'),
            expected=swifi.binstr2binv('100101001101000000010100100000110010010010010100'),
            bytes_per_block=3,
            nitems=3,
            rate=swifi.phy_utils.BPSK_1_2,
            flag=swifi.PHY_HEADER_FLAG)

    def test_encode_data_field_t (self):
        # data source is the DATA field taken from IEEE 802.11's Table L-13
        data = swifi.str2binv('\x00\x00\x04\x02\x00\x2E\x00\x60\x08\xCD\x37\xA6\x00\x20\xD6\x01\x3C\xF1\x00\x60\x08\xAD\x3B\xAF\x00\x00\x4A\x6F\x79\x2C\x20\x62\x72\x69\x67\x68\x74\x20\x73\x70\x61\x72\x6B\x20\x6F\x66\x20\x64\x69\x76\x69\x6E\x69\x74\x79\x2C\x0A\x44\x61\x75\x67\x68\x74\x65\x72\x20\x6F\x66\x20\x45\x6C\x79\x73\x69\x75\x6D\x2C\x0A\x46\x69\x72\x65\x2D\x69\x6E\x73\x69\x72\x65\x64\x20\x77\x65\x20\x74\x72\x65\x61\x67\x33\x21\xB6\x00\x00\x00\x00\x00\x00')

        # expected result is generated below
        expected = swifi.binstr2binv('\
01110111111100001110111111000100\
01110011000000001011111100010001\
00010000100110100001110100010010\
01101110001110001111010101101001\
00011011011010111001100001000011\
00000000000011011011001101101101\
11000101001110101001011011010001\
11001000100100001111110000101101\
01110101101000110010111110001010\
10101010111010100010000000011000\
10001100100011011110101001000010\
00001010001110101100100100010111\
11110110001010000101010010011000\
10000100100101110000101100001100\
11100100100111001111110011110000\
11010010001010101001101010101100\
10101110001100010111101001110111\
00011101101010011001011111010110\
00001001111011001111000011100100\
00001000010001101000011110100001\
01010110101011010000001100110001\
10111000110001001111101011010100\
11100110001001010010001001000111\
00000001111010100100110001111000\
00000101101101111001110111010101\
10100000111110111111011110101100\
11111101001000111000001111001111\
10001110100010010001010000100111\
00111111100100101010011110010101\
11000001100011001111101000011010\
10101101100110001100100000010100\
11011101110100100111000101000100\
11101001010001110010110110010001\
10001101010101010111100111011111\
01010011101101111011110011110111\
00010011011100011001100101011111\
')

        self.run_encoding_test(
            binary_data=data,
            expected=expected,
            bytes_per_block=108,
            nitems=100,
            rate=swifi.phy_utils.QAM16_3_4,
            flag=swifi.PHY_SCRAMBLER_FLAG | swifi.PHY_TAIL_SET_FLAG)




    #########################################################################################
    ### DECODING TEST #######################################################################
    #########################################################################################

    def run_decoding_test(self, binary_data, expected, bytes_per_block, nitems, rate, flag, ntruncate=0):
        """
        binary_data: sequence of bits
        expected: must be a list/tuple of binary integers 0 and 1
        """

        bits_per_block = bytes_per_block * 8
        length = int(len(binary_data) / bits_per_block) * bits_per_block
        binary_data = binary_data[0:length]   # truncate binary_data to a multiple of bits_per_block
        nblocks = len(binary_data) / bits_per_block

        # set source with tag
        tags = []
        for i in xrange(nblocks):
            tags.append(swifi.phy_tag(key='phy', 
                                      offset=i*bits_per_block, 
                                      nitems=nitems,
                                      rate=rate,
                                      flag=flag))
        source = blocks.vector_source_b(binary_data, repeat=False, vlen=1, tags=tags)
        
        # scrambler
        descrambler = swifi.scrambler(False, 'phy')

        # convolutional encoder
        dec = swifi.wifi_conv_decoder('phy')

        # puncturing
        depuncture = swifi.bit_inserter('phy')

        # interleaving
        deinterleaver = swifi.interleaver(False, 'phy')

        # vector sink
        sink = blocks.vector_sink_b()

        # connecting blocks together
        self.tb.connect(source,
            deinterleaver,
            depuncture,
            dec,
            descrambler,
            sink)

        self.tb.run()

        result = [x for x in sink.data()]
        if ntruncate > 0:
            result = result[0:len(result)-ntruncate]
            expected = expected[0:len(expected)-ntruncate]
        self.assertEqual(result, expected)

    def test_decode_signal_field_t (self):
        # data source is encoded SIGNAL field taken from IEEE 802.11 standard's Table L-9
        # expected result is taken from Table L-7
        self.run_decoding_test(
            binary_data=swifi.binstr2binv('100101001101000000010100100000110010010010010100'),
            expected=swifi.binstr2binv('101100010011000000000000'),
            bytes_per_block=6,
            nitems=3,
            rate=swifi.phy_utils.BPSK_1_2,
            flag=swifi.PHY_HEADER_FLAG)

    def test_decode_data_field_t (self):
        # expected result is the DATA field taken from IEEE 802.11's Table L-13
        expected = swifi.str2binv('\x00\x00\x04\x02\x00\x2E\x00\x60\x08\xCD\x37\xA6\x00\x20\xD6\x01\x3C\xF1\x00\x60\x08\xAD\x3B\xAF\x00\x00\x4A\x6F\x79\x2C\x20\x62\x72\x69\x67\x68\x74\x20\x73\x70\x61\x72\x6B\x20\x6F\x66\x20\x64\x69\x76\x69\x6E\x69\x74\x79\x2C\x0A\x44\x61\x75\x67\x68\x74\x65\x72\x20\x6F\x66\x20\x45\x6C\x79\x73\x69\x75\x6D\x2C\x0A\x46\x69\x72\x65\x2D\x69\x6E\x73\x69\x72\x65\x64\x20\x77\x65\x20\x74\x72\x65\x61\x67\x33\x21\xB6\x00\x00\x00\x00\x00\x00')

        # data is taken from test_encode_data_field
        data = swifi.binstr2binv('\
01110111111100001110111111000100\
01110011000000001011111100010001\
00010000100110100001110100010010\
01101110001110001111010101101001\
00011011011010111001100001000011\
00000000000011011011001101101101\
11000101001110101001011011010001\
11001000100100001111110000101101\
01110101101000110010111110001010\
10101010111010100010000000011000\
10001100100011011110101001000010\
00001010001110101100100100010111\
11110110001010000101010010011000\
10000100100101110000101100001100\
11100100100111001111110011110000\
11010010001010101001101010101100\
10101110001100010111101001110111\
00011101101010011001011111010110\
00001001111011001111000011100100\
00001000010001101000011110100001\
01010110101011010000001100110001\
10111000110001001111101011010100\
11100110001001010010001001000111\
00000001111010100100110001111000\
00000101101101111001110111010101\
10100000111110111111011110101100\
11111101001000111000001111001111\
10001110100010010001010000100111\
00111111100100101010011110010101\
11000001100011001111101000011010\
10101101100110001100100000010100\
11011101110100100111000101000100\
11101001010001110010110110010001\
10001101010101010111100111011111\
01010011101101111011110011110111\
00010011011100011001100101011111\
')

        self.run_decoding_test(
            binary_data=data,
            expected=expected,
            bytes_per_block=144,
            nitems=100,
            rate=swifi.phy_utils.QAM16_3_4,
            flag=swifi.PHY_SCRAMBLER_FLAG | swifi.PHY_TAIL_SET_FLAG,
            ntruncate=48)   # ignore the last 48 bits of tail and padding for correct comparison with expected result


if __name__ == '__main__':
    gr_unittest.run(qa_encoding, "qa_encoding.xml")
