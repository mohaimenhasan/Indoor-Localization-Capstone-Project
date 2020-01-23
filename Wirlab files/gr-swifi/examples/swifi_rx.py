#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: SWiFi RX
# Generated: Sun Jun  9 16:15:55 2019
##################################################

from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import fft
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from optparse import OptionParser
import numpy
import swifi


class swifi_rx(gr.top_block):

    def __init__(self, freq=2.412e9, samp_rate=20e6, alpha=0.15, gain=30, beta=0.3, filename="", antenna="TX/RX", nskip=20000000, ff="", dump_filename="<<stdout>>", equalization_method="ppt_dd"):
        gr.top_block.__init__(self, "SWiFi RX")

        ##################################################
        # Parameters
        ##################################################
        self.freq = freq
        self.samp_rate = samp_rate
        self.alpha = alpha
        self.gain = gain
        self.beta = beta
        self.filename = filename
        self.antenna = antenna
        self.nskip = nskip
        self.ff = ff
        self.dump_filename = dump_filename
        self.equalization_method = equalization_method

        ##################################################
        # Variables
        ##################################################
        self.header_mod = header_mod = swifi.wifi_constellation_bpsk()
        self.header_formatter_2 = header_formatter_2 = swifi.ofdm_phy_header_parser("frame_len", "phy")

        ##################################################
        # Blocks
        ##################################################
        self.swifi_wifi_conv_decoder_0_0 = swifi.wifi_conv_decoder(24, -1)
        self.swifi_wifi_conv_decoder_0 = swifi.wifi_conv_decoder("phy")
        self.swifi_scrambler_0 = swifi.scrambler(False,"phy")
        self.swifi_sample_marker_0 = swifi.sample_marker(gr.sizeof_gr_complex,"phy")
        self.swifi_payload_parser_0 = swifi.payload_parser("phy",dump_filename,"xml",ff,True)
        self.swifi_ofdm_frame_sync_0 = swifi.ofdm_frame_sync(0.9,64,0.4,0.05,0.2,"trigger")
        self.swifi_ofdm_equalizer_1 = swifi.ofdm_equalizer("",True,"channel_states",digital.constellation_bpsk().base())
        self.swifi_ofdm_equalizer_1.set_equalization_method(equalization_method)
        self.swifi_ofdm_equalizer_1.set_alpha(alpha)
        self.swifi_ofdm_equalizer_1.set_beta(beta)
        self.swifi_ofdm_equalizer_1.enable_foc(False)
        self.swifi_ofdm_equalizer_0 = swifi.ofdm_equalizer("phy",False,"channel_states")
        self.swifi_ofdm_equalizer_0.set_equalization_method(equalization_method)
        self.swifi_ofdm_equalizer_0.set_alpha(alpha)
        self.swifi_ofdm_equalizer_0.set_beta(beta)
        self.swifi_ofdm_equalizer_0.enable_foc(False)
        self.swifi_ofdm_chan_est_0 = swifi.ofdm_chan_est(64,((0, 0j, 0, 0j, 0, 0j, -1, 1j, -1, 1j, -1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, (-0-1j), 1, -1j, -1, 1j, 0, -1j, 1, (-0-1j), 1, -1j, 1, 1j, -1, -1j, 1, (-0-1j), -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 0j, 0, 0j, 0, 0j), (0, 0, 0, 0, 0, 0, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 1, -1, -1, 1, 1, -1, 1, -1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 0, 0, 0, 0)),0.5,
            "channel_states","frame_len",3,False)
        self.swifi_ofdm_chan_est_0.set_estimation_method(0)
        self.swifi_interleaver_0_0 = swifi.interleaver(False,11)
        self.swifi_interleaver_0 = swifi.interleaver(False,"phy")
        self.swifi_header_payload_demux_0 = swifi.header_payload_demux(
                3,
                64,
                16,
                "frame_len",
                "trigger",
                True,
                gr.sizeof_gr_complex,
                "phy")
        self.swifi_constellation_decoder_0 = swifi.constellation_decoder("phy")
        self.swifi_bit_unpacker_0 = swifi.bit_unpacker("phy",1)
        self.swifi_bit_inserter_0 = swifi.bit_inserter("phy")
        self.fft_vxx_0_0 = fft.fft_vcc(64, True, (window.rectangular(64)), True, 1)
        self.fft_vxx_0 = fft.fft_vcc(64, True, (window.rectangular(64)), True, 1)
        self.digital_packet_headerparser_b_0_0 = digital.packet_headerparser_b(header_formatter_2.base())
        self.digital_constellation_decoder_cb_0 = digital.constellation_decoder_cb(header_mod.base())
        self.blocks_vector_to_stream_1_0_0_0 = blocks.vector_to_stream(gr.sizeof_gr_complex*1, 48)
        self.blocks_vector_to_stream_1_0 = blocks.vector_to_stream(gr.sizeof_gr_complex*1, 48)
        self.blocks_unpacked_to_packed_xx_0_0 = blocks.unpacked_to_packed_bb(1, gr.GR_LSB_FIRST)
        self.blocks_unpacked_to_packed_xx_0 = blocks.unpacked_to_packed_bb(1, gr.GR_LSB_FIRST)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, filename, False)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.digital_packet_headerparser_b_0_0, 'header_data'), (self.swifi_header_payload_demux_0, 'header_data'))    
        self.connect((self.blocks_file_source_0, 0), (self.swifi_ofdm_frame_sync_0, 0))    
        self.connect((self.blocks_unpacked_to_packed_xx_0, 0), (self.swifi_payload_parser_0, 0))    
        self.connect((self.blocks_unpacked_to_packed_xx_0_0, 0), (self.digital_packet_headerparser_b_0_0, 0))    
        self.connect((self.blocks_vector_to_stream_1_0, 0), (self.swifi_sample_marker_0, 0))    
        self.connect((self.blocks_vector_to_stream_1_0_0_0, 0), (self.digital_constellation_decoder_cb_0, 0))    
        self.connect((self.digital_constellation_decoder_cb_0, 0), (self.swifi_interleaver_0_0, 0))    
        self.connect((self.fft_vxx_0, 0), (self.swifi_ofdm_chan_est_0, 0))    
        self.connect((self.fft_vxx_0_0, 0), (self.swifi_ofdm_equalizer_0, 0))    
        self.connect((self.swifi_bit_inserter_0, 0), (self.swifi_wifi_conv_decoder_0, 0))    
        self.connect((self.swifi_bit_unpacker_0, 0), (self.swifi_interleaver_0, 0))    
        self.connect((self.swifi_constellation_decoder_0, 0), (self.swifi_bit_unpacker_0, 0))    
        self.connect((self.swifi_header_payload_demux_0, 0), (self.fft_vxx_0, 0))    
        self.connect((self.swifi_header_payload_demux_0, 1), (self.fft_vxx_0_0, 0))    
        self.connect((self.swifi_interleaver_0, 0), (self.swifi_bit_inserter_0, 0))    
        self.connect((self.swifi_interleaver_0_0, 0), (self.swifi_wifi_conv_decoder_0_0, 0))    
        self.connect((self.swifi_ofdm_chan_est_0, 0), (self.swifi_ofdm_equalizer_1, 0))    
        self.connect((self.swifi_ofdm_equalizer_0, 0), (self.blocks_vector_to_stream_1_0, 0))    
        self.connect((self.swifi_ofdm_equalizer_1, 0), (self.blocks_vector_to_stream_1_0_0_0, 0))    
        self.connect((self.swifi_ofdm_frame_sync_0, 0), (self.swifi_header_payload_demux_0, 0))    
        self.connect((self.swifi_sample_marker_0, 0), (self.swifi_constellation_decoder_0, 0))    
        self.connect((self.swifi_scrambler_0, 0), (self.blocks_unpacked_to_packed_xx_0, 0))    
        self.connect((self.swifi_wifi_conv_decoder_0, 0), (self.swifi_scrambler_0, 0))    
        self.connect((self.swifi_wifi_conv_decoder_0_0, 0), (self.blocks_unpacked_to_packed_xx_0_0, 0))    

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_alpha(self):
        return self.alpha

    def set_alpha(self, alpha):
        self.alpha = alpha

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain

    def get_beta(self):
        return self.beta

    def set_beta(self, beta):
        self.beta = beta

    def get_filename(self):
        return self.filename

    def set_filename(self, filename):
        self.filename = filename
        self.blocks_file_source_0.open(self.filename, False)

    def get_antenna(self):
        return self.antenna

    def set_antenna(self, antenna):
        self.antenna = antenna

    def get_nskip(self):
        return self.nskip

    def set_nskip(self, nskip):
        self.nskip = nskip

    def get_ff(self):
        return self.ff

    def set_ff(self, ff):
        self.ff = ff

    def get_dump_filename(self):
        return self.dump_filename

    def set_dump_filename(self, dump_filename):
        self.dump_filename = dump_filename

    def get_equalization_method(self):
        return self.equalization_method

    def set_equalization_method(self, equalization_method):
        self.equalization_method = equalization_method

    def get_header_mod(self):
        return self.header_mod

    def set_header_mod(self, header_mod):
        self.header_mod = header_mod

    def get_header_formatter_2(self):
        return self.header_formatter_2

    def set_header_formatter_2(self, header_formatter_2):
        self.header_formatter_2 = header_formatter_2


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "-f", "--freq", dest="freq", type="eng_float", default=eng_notation.num_to_str(2.412e9),
        help="Set Center Frequency [default=%default]")
    parser.add_option(
        "-s", "--samp-rate", dest="samp_rate", type="eng_float", default=eng_notation.num_to_str(20e6),
        help="Set Sampling Rate [default=%default]")
    parser.add_option(
        "", "--alpha", dest="alpha", type="eng_float", default=eng_notation.num_to_str(0.15),
        help="Set Alpha [default=%default]")
    parser.add_option(
        "-g", "--gain", dest="gain", type="eng_float", default=eng_notation.num_to_str(30),
        help="Set Gain [default=%default]")
    parser.add_option(
        "", "--beta", dest="beta", type="eng_float", default=eng_notation.num_to_str(0.3),
        help="Set Beta [default=%default]")
    parser.add_option(
        "-F", "--filename", dest="filename", type="string", default="",
        help="Set Filename [default=%default]")
    parser.add_option(
        "-A", "--antenna", dest="antenna", type="string", default="TX/RX",
        help="Set Antenna [default=%default]")
    parser.add_option(
        "-n", "--nskip", dest="nskip", type="long", default=20000000,
        help="Set Skip [default=%default]")
    parser.add_option(
        "", "--ff", dest="ff", type="string", default="",
        help="Set Frame Filter [default=%default]")
    parser.add_option(
        "-O", "--dump-filename", dest="dump_filename", type="string", default="<<stdout>>",
        help="Set Dump Filename [default=%default]")
    parser.add_option(
        "-e", "--equalization-method", dest="equalization_method", type="string", default="ppt_dd",
        help="Set Equalization Method [default=%default]")
    return parser


def main(top_block_cls=swifi_rx, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(freq=options.freq, samp_rate=options.samp_rate, alpha=options.alpha, gain=options.gain, beta=options.beta, filename=options.filename, antenna=options.antenna, nskip=options.nskip, ff=options.ff, dump_filename=options.dump_filename, equalization_method=options.equalization_method)
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
