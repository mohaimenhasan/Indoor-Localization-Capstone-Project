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
import numpy, sys

class qa_wifi_conv_endecoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None


    ####################################################################
    ### WIFI_CONV_1_2 ENCODING TEST ####################################
    ####################################################################

    def test_encoding_001_t (self):
        # example data is the SIGNAL field content in L.1.4.1 Table L-7
        wifi_conv = swifi.wifi_conv_1_2()
        data = swifi.binstr2binv('101100010011000000000000')
        result = swifi.binv2binstr(wifi_conv.encode(data, 0))
        expected = '110100011010000100000010001111100111000000000000'
        self.assertEqual(result, expected)

    def test_encoding_002_t (self):
        # example data is the SIGNAL field content in L.1.4.1 Table L-7
        # In this test, we break example data into 2 parts and encode
        # The successful test indicates the encoder maintains correctly the encoding state
        wifi_conv = swifi.wifi_conv_1_2()
        data1 = swifi.binstr2binv('101100010011')
        result1 = swifi.binv2binstr(wifi_conv.encode(data1, 0)) # initial state 0
        expected1 = '110100011010000100000010'

        data2 = swifi.binstr2binv('000000000000')
        result2 = swifi.binv2binstr(wifi_conv.encode(data2)) # don't specify state here
        expected2 = '001111100111000000000000'

        self.assertEqual([result1, result2], [expected1, expected2])

    def test_encoding_003_t (self):
        # example data is the DATA field content in L.1.5.2 Table L-15
        wifi_conv = swifi.wifi_conv_1_2()
        data = swifi.binstr2binv('\
011011000001100110001001\
100011110110100000100001\
111101001010010101100001\
010011111101011110101110\
001001000000110011110011\
001110101110010010111100\
010100111001100011000000\
000111100011010110110011\
111000111111100000100101\
011000001101011000100101\
001101010011001111111110\
111100000100000100101011\
100011110101001100011100\
100000110100000110111110\
001110010010100001100110\
010001000110011011001101\
111101101010001111011000\
000011011101010010000001\
001110110010111111011111\
110000110101100011110111\
110001100101001011101011\
011100001000111110011110\
011010101001000010000001\
111111010111110010101001\
110100010101010100010010\
000001000111010011011001\
111010010011101111001101\
100100111000110101111011\
011111000111000000000010\
001000001001100110100001\
011111011000101000100111\
000101110011100100010101\
101000001110110010010101\
000101101001000100010000\
000000001101110001111111\
000011101111001011001001\
')

        result = swifi.binv2binstr(wifi_conv.encode(data, 0)) # initial state 0

        expected = '\
001110101101000100100100\
101000000011111101000000\
100011110111101001010011\
100101101110000111110001\
010110101110111001101001\
111010100100110110001010\
100010111001110110000010\
110100100010011101001011\
110110000000010100001011\
111010000000000101100111\
111100000010110001001011\
111000101101101001001010\
101000111011100111100010\
111111110111010011100111\
000000111001100110101011\
011110100001011000011100\
000110110110101110101000\
110001100101000111000110\
010011011000100100101011\
101000011000010000000110\
011110010000101011001001\
001100000001100011111100\
100000111010100101000111\
110010000111000110010011\
101111011011101001011101\
110010010011110111100100\
100010000010001010111001\
101110001010110111110111\
011010111001001000101101\
100111011101011000000011\
110011100011110011110001\
000000110001000111001100\
011111011110001110011011\
000100001010010100110100\
100111001110101101110000\
000111001010010000101111\
011100101110111111011111\
011001000100001000001101\
001100011001100110111010\
000110000111101001010000\
110111011010110100001101\
011110101010011100000100\
010110110011010110101111\
110010101011010111000101\
100111110010101111000010\
100100000110111100101111\
100110100011001011010010\
000111010100010100000010\
011110110010100011111000\
001100110000010011000001\
010000101111011111110010\
011100100101000001000111\
000001010111100110011110\
111011110011101001111100\
010001111110111011100001\
101101111010001000100100\
111010111101110110101110\
010001010111000000001101\
111111110011001000011100\
100011000011001001101101\
100001010001111000110100\
010011101101001100001011\
100001100011010111111011\
001000100010001111111000\
110101010110111000100101\
111111011111101101100100\
000001001111011001010101\
100100111100111111001100\
101100000000000011101011\
011100110110111010100011\
000110011001010111110011\
101001000110101011111011\
'

        self.assertEqual(result, expected)

    ####################################################################
    ### WIFI_CONV_1_2 DECODING TEST ####################################
    ####################################################################


    def test_decoding_001_t (self):
        # example encoded data is the SIGNAL field content in L.1.4.1 Table L-8
        wifi_conv = swifi.wifi_conv_1_2()
        data = swifi.binstr2binv('110100011010000100000010001111100111000000000000')
        result = swifi.binv2binstr(wifi_conv.decode(data, 0))
        expected = '101100010011000000000000'
        self.assertEqual(result, expected)

    def test_decoding_003_t (self):
        # this is the reverse test of test_encoding_003_t
        wifi_conv = swifi.wifi_conv_1_2()
        data = swifi.binstr2binv('\
001110101101000100100100\
101000000011111101000000\
100011110111101001010011\
100101101110000111110001\
010110101110111001101001\
111010100100110110001010\
100010111001110110000010\
110100100010011101001011\
110110000000010100001011\
111010000000000101100111\
111100000010110001001011\
111000101101101001001010\
101000111011100111100010\
111111110111010011100111\
000000111001100110101011\
011110100001011000011100\
000110110110101110101000\
110001100101000111000110\
010011011000100100101011\
101000011000010000000110\
011110010000101011001001\
001100000001100011111100\
100000111010100101000111\
110010000111000110010011\
101111011011101001011101\
110010010011110111100100\
100010000010001010111001\
101110001010110111110111\
011010111001001000101101\
100111011101011000000011\
110011100011110011110001\
000000110001000111001100\
011111011110001110011011\
000100001010010100110100\
100111001110101101110000\
000111001010010000101111\
011100101110111111011111\
011001000100001000001101\
001100011001100110111010\
000110000111101001010000\
110111011010110100001101\
011110101010011100000100\
010110110011010110101111\
110010101011010111000101\
100111110010101111000010\
100100000110111100101111\
100110100011001011010010\
000111010100010100000010\
011110110010100011111000\
001100110000010011000001\
010000101111011111110010\
011100100101000001000111\
000001010111100110011110\
111011110011101001111100\
010001111110111011100001\
101101111010001000100100\
111010111101110110101110\
010001010111000000001101\
111111110011001000011100\
100011000011001001101101\
100001010001111000110100\
010011101101001100001011\
100001100011010111111011\
001000100010001111111000\
110101010110111000100101\
111111011111101101100100\
000001001111011001010101\
100100111100111111001100\
101100000000000011101011\
011100110110111010100011\
000110011001010111110011\
101001000110101011111011\
')

        result = swifi.binv2binstr(wifi_conv.decode(data, 0))    # explicitly specify end state = 0
        expected = '\
011011000001100110001001\
100011110110100000100001\
111101001010010101100001\
010011111101011110101110\
001001000000110011110011\
001110101110010010111100\
010100111001100011000000\
000111100011010110110011\
111000111111100000100101\
011000001101011000100101\
001101010011001111111110\
111100000100000100101011\
100011110101001100011100\
100000110100000110111110\
001110010010100001100110\
010001000110011011001101\
111101101010001111011000\
000011011101010010000001\
001110110010111111011111\
110000110101100011110111\
110001100101001011101011\
011100001000111110011110\
011010101001000010000001\
111111010111110010101001\
110100010101010100010010\
000001000111010011011001\
111010010011101111001101\
100100111000110101111011\
011111000111000000000010\
001000001001100110100001\
011111011000101000100111\
000101110011100100010101\
101000001110110010010101\
000101101001000100010000\
000000001101110001111111\
000011101111001011001001\
'
        self.assertEqual(result, expected)


    ####################################################################
    ### WIFI_CONV_ENCODER and WIFI_CONV_DECODER TEST ###################
    ####################################################################

    def run_encoder_decoder_block_test(self, binary_data, bytes_per_block, nitems, flag):
        """
        binary_data: sequence of bits
        bytes_per_block: number of bytes per block for encoding
        mode: True: use PHY tag, False: don't create tag, create encoder using fixed block size
        """
        bits_per_block = bytes_per_block * 8
        length = int(len(binary_data) / bits_per_block) * bits_per_block
        binary_data = binary_data[0:length]   # truncate binary_data to a multiple of bits_per_block
        nblocks = len(binary_data) / bits_per_block

        # set source with tags.
        tags = []
        for i in xrange(nblocks):
            tags.append(swifi.phy_tag(key='phy',
                                      offset=i*bits_per_block,
                                      nitems=nitems,
                                      flag=flag))

        source = blocks.vector_source_b(binary_data, repeat=False, vlen=1, tags=tags)
        encoder = swifi.wifi_conv_encoder('phy')
        decoder = swifi.wifi_conv_decoder('phy')

        # vector sink
        sink = blocks.vector_sink_b()

        # connecting blocks together
        self.tb.connect(source, encoder, decoder, sink)
        self.tb.run()

        self.assertEqual(binary_data, list(sink.data()))

    def test_encoder_decoder_block_modeTrue_001_t(self):
        data = swifi.binstr2binv('101100010011000000000000')
        self.run_encoder_decoder_block_test(binary_data=data, 
            bytes_per_block=3, nitems=3, flag=swifi.PHY_HEADER_FLAG)

    def test_encoder_decoder_block_modeTrue_002_t(self):
        data = swifi.binstr2binv('101100010011000000000000101100010011000000000000')
        self.run_encoder_decoder_block_test(binary_data=data, 
            bytes_per_block=3, nitems=3, flag=swifi.PHY_HEADER_FLAG)



    ####################################################################
    ### WIFI_CONV_ENCODER + PUNCTURING TEST ########
    ####################################################################

    def run_encoder_puncturing_test(self, binary_data, expected, bytes_per_block, nitems, rate, flag):
        """
        binary_data: sequence of bits
        bytes_per_block: number of bytes per block for encoding
        rate: OFDM rate
        """
        bits_per_block = bytes_per_block * 8
        length = int(len(binary_data) / bits_per_block) * bits_per_block
        binary_data = binary_data[0:length]   # truncate binary_data to a multiple of bits_per_block
        nblocks = len(binary_data) / bits_per_block

        #data = numpy.random.randint(0, 2, length).tolist()

        # set source with tags.
        tags = []
        for i in xrange(nblocks):
            tags.append(swifi.phy_tag(key='phy', 
                                      offset=i*bits_per_block,
                                      nitems=nitems,
                                      rate=rate,
                                      flag=flag))

        source = blocks.vector_source_b(binary_data, repeat=False, vlen=1, tags=tags)
        encoder = swifi.wifi_conv_encoder('phy')
        puncturing = swifi.bit_stealer('phy')

        # vector sink
        sink = blocks.vector_sink_b()

        # connecting blocks together
        self.tb.connect(source, encoder, puncturing, sink)
        self.tb.run()

        self.assertEqual(list(sink.data()), expected)

    def test_encoder_puncturing_001_t(self):
        data = swifi.binstr2binv('101100010011000000000000')
        expected = swifi.binstr2binv('110100011010000100000010001111100111000000000000')
        self.run_encoder_puncturing_test(binary_data=data, 
            expected=expected,
            bytes_per_block=3,
            nitems=3,
            flag=swifi.PHY_HEADER_FLAG,
            rate=swifi.phy_utils.BPSK_1_2)

    def test_encoder_puncturing_002_t(self):
        data = swifi.str2binv_msb('\x6C\x19\x89\x8F\x68\x21\xF4\xA5\x61\x4F\xD7\xAE\x24\x0C\xF3\x3A\xE4\xBC\x53\x98\xC0\x1E\x35\xB3\xE3\xF8\x25\x60\xD6\x25\x35\x33\xFE\xF0\x41\x2B\x8F\x53\x1C\x83\x41\xBE\x39\x28\x66\x44\x66\xCD\xF6\xA3\xD8\x0D\xD4\x81\x3B\x2F\xDF\xC3\x58\xF7\xC6\x52\xEB\x70\x8F\x9E\x6A\x90\x81\xFD\x7C\xA9\xD1\x55\x12\x04\x74\xD9\xE9\x3B\xCD\x93\x8D\x7B\x7C\x70\x02\x20\x99\xA1\x7D\x8A\x27\x17\x39\x15\xA0\xEC\x95\x16\x91\x10\x00\xDC\x7F\x0E\xF2\xC9')
        expected = swifi.str2binv_msb('\x2B\x08\xA1\xF0\x9D\xB5\x9A\x1D\x4A\xFB\xE8\xC2\x8F\xC0\xC8\x73\xC0\x43\xE0\x19\xE0\xD3\xEB\xB2\xAF\x98\xFD\x59\x0F\x8B\x69\x66\x0C\xAA\xD9\x10\x56\x8B\xA6\x40\x64\xB3\x21\x9E\x8E\x91\xC1\x05\xB7\xB7\xC5\xD8\x80\x2F\xA2\xDD\x6F\x2B\x97\x61\xD9\xDD\x0D\x12\x76\x27\x02\x4C\x92\xBC\x12\x4B\x6A\xF7\x70\x23\x27\x8E\x01\xB4\xD6\xC3\x6A\x60\x4D\x4B\xCB\x51\x9C\xB0\x80\xEB\x89\x34\x14\x40\x6C\x9E\x2C\x51\x4B\x7C\x69\x11\x15\x86\xFD\xBE\x5E\xF9\xBE\x28\xEF\xCA\x55\x03\xFD\x26\x91\x3B\x95\xEC\x5B\x23\x99\x5F\x28\x3E\xD4\xE9\xF7\xB8\x13\x75\x8E\xF2\xA0\x1B\x6C\xE9\x07\x5D\xB0\xBF')
        self.run_encoder_puncturing_test(binary_data=data, 
            expected=expected,
            bytes_per_block=108,
            nitems=100,
            flag=0,
            rate=swifi.phy_utils.QAM16_3_4)

    ####################################################################
    ### WIFI_CONV_ENCODER + WIFI_CONV_DECODER + PUNCTURING TEST ########
    ####################################################################

    def run_encoder_decoder_puncturing_test(self, binary_data, bytes_per_block, nitems, rate, flag):
        """
        binary_data: sequence of bits
        bytes_per_block: number of bytes per block for encoding
        rate: OFDM rate
        """
        bits_per_block = bytes_per_block * 8
        length = int(len(binary_data) / bits_per_block) * bits_per_block
        binary_data = binary_data[0:length]   # truncate binary_data to a multiple of bits_per_block
        nblocks = len(binary_data) / bits_per_block

        #data = numpy.random.randint(0, 2, length).tolist()

        # set source with tags.
        tags = []
        for i in xrange(nblocks):
            tags.append(swifi.phy_tag(key='phy', 
                                      offset=i*bits_per_block,
                                      nitems=nitems,
                                      rate=rate,
                                      flag=flag))

        source = blocks.vector_source_b(binary_data, repeat=False, vlen=1, tags=tags)
        encoder = swifi.wifi_conv_encoder('phy')
        puncturing = swifi.bit_stealer('phy')
        depuncturing = swifi.bit_inserter('phy')
        decoder = swifi.wifi_conv_decoder('phy')

        # vector sink
        sink = blocks.vector_sink_b()

        # connecting blocks together
        self.tb.connect(source, encoder, puncturing, depuncturing, decoder, sink)
        self.tb.run()

        self.assertEqual(binary_data, list(sink.data()))

    def test_encoder_decoder_puncturing_001_t(self):
        data = swifi.binstr2binv('101100010011000000000000')
        self.run_encoder_decoder_puncturing_test(binary_data=data,
            bytes_per_block=3,
            nitems=3,
            rate=swifi.phy_utils.BPSK_1_2,
            flag=swifi.PHY_HEADER_FLAG)

    def test_encoder_decoder_puncturing_002_t(self):
        data = '\
000000000000000000000100\
000000100000000000101110\
000000000110000000001000\
110011010011011110100110\
000000000010000011010110\
000000010011110011110001\
000000000110000000001000\
101011010011101110101111\
000000000000000001001010\
011011110111100100101100\
001000000110001001110010\
011010010110011101101000\
011101000010000001110011\
011100000110000101110010\
011010110010000001101111\
011001100010000001100100\
011010010111011001101001\
011011100110100101110100\
011110010010110000001010\
010001000110000101110101\
011001110110100001110100\
011001010111001000100000\
011011110110011000100000\
010001010110110001111001\
011100110110100101110101\
011011010010110000001010\
010001100110100101110010\
011001010010110101101001\
011011100111001101101001\
011100100110010101100100\
001000000111011101100101\
001000000111010001110010\
011001010110000101100111\
001100110010000110110110\
000000000000000000000000\
000000000000000000000000\
'
        data = swifi.binstr2binv(data)
        self.run_encoder_decoder_puncturing_test(binary_data=data, 
            bytes_per_block=108,
            nitems=100,
            rate=swifi.phy_utils.QAM16_3_4,
            flag=0)


if __name__ == '__main__':
    gr_unittest.run(qa_wifi_conv_endecoder, "qa_wifi_conv_endecoder.xml")
