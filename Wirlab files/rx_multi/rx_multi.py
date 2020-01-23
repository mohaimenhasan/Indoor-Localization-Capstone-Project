#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: RX Multi
# Generated: Fri Jan 10 13:19:51 2020
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
import limesdr
import numpy
import spectrum_plotter
import os.path
print(os.path.abspath(spectrum_plotter.__file__))
import swifi


class rx_multi(gr.top_block):

    def __init__(self, alpha=0.15, antenna="TX/RX", beta=0.3, dump_filename="<<stdout>>", equalization_method="ppt_dd", ff="", filename="", freq=2.412e9, gain=30, nskip=20000000, samp_rate=20e6):
        gr.top_block.__init__(self, "RX Multi")

        ##################################################
        # Parameters
        ##################################################
        self.alpha = alpha
        self.antenna = antenna
        self.beta = beta
        self.dump_filename = dump_filename
        self.equalization_method = equalization_method
        self.ff = ff
        self.filename = filename
        self.freq = freq
        self.gain = gain
        self.nskip = nskip
        self.samp_rate = samp_rate

        ##################################################
        # Variables
        ##################################################
        self.header_mod = header_mod = swifi.wifi_constellation_bpsk()
        self.header_formatter_2 = header_formatter_2 = swifi.ofdm_phy_header_parser("frame_len", "phy")

        ##################################################
        # Blocks
        ##################################################
        self.swifi_wifi_conv_decoder_0_1 = swifi.wifi_conv_decoder("phy")
        self.swifi_wifi_conv_decoder_0_0_0 = swifi.wifi_conv_decoder(24, -1)
        self.swifi_wifi_conv_decoder_0_0 = swifi.wifi_conv_decoder(24, -1)
        self.swifi_wifi_conv_decoder_0 = swifi.wifi_conv_decoder("phy")
        self.swifi_scrambler_0_0 = swifi.scrambler(False,"phy")
        self.swifi_scrambler_0 = swifi.scrambler(False,"phy")
        self.swifi_sample_marker_0_0 = swifi.sample_marker(gr.sizeof_gr_complex,"phy")
        self.swifi_sample_marker_0 = swifi.sample_marker(gr.sizeof_gr_complex,"phy")
        self.swifi_payload_parser_0_0 = swifi.payload_parser("phy",dump_filename,"text",ff,True)
        self.swifi_payload_parser_0 = swifi.payload_parser("phy",dump_filename,"text",ff,True)
        self.swifi_ofdm_frame_sync_0_0 = swifi.ofdm_frame_sync(0.9,64,0.4,0.05,0.2,"trigger")
        self.swifi_ofdm_frame_sync_0 = swifi.ofdm_frame_sync(0.9,64,0.4,0.05,0.2,"trigger")
        self.swifi_ofdm_equalizer_1_0 = swifi.ofdm_equalizer("",True,"channel_states",digital.constellation_bpsk().base())
        self.swifi_ofdm_equalizer_1_0.set_equalization_method(equalization_method)
        self.swifi_ofdm_equalizer_1_0.set_alpha(alpha)
        self.swifi_ofdm_equalizer_1_0.set_beta(beta)
        self.swifi_ofdm_equalizer_1_0.enable_foc(False)
        self.swifi_ofdm_equalizer_1 = swifi.ofdm_equalizer("",True,"channel_states",digital.constellation_bpsk().base())
        self.swifi_ofdm_equalizer_1.set_equalization_method(equalization_method)
        self.swifi_ofdm_equalizer_1.set_alpha(alpha)
        self.swifi_ofdm_equalizer_1.set_beta(beta)
        self.swifi_ofdm_equalizer_1.enable_foc(False)
        self.swifi_ofdm_equalizer_0_0 = swifi.ofdm_equalizer("phy",False,"channel_states")
        self.swifi_ofdm_equalizer_0_0.set_equalization_method(equalization_method)
        self.swifi_ofdm_equalizer_0_0.set_alpha(alpha)
        self.swifi_ofdm_equalizer_0_0.set_beta(beta)
        self.swifi_ofdm_equalizer_0_0.enable_foc(False)
        self.swifi_ofdm_equalizer_0 = swifi.ofdm_equalizer("phy",False,"channel_states")
        self.swifi_ofdm_equalizer_0.set_equalization_method(equalization_method)
        self.swifi_ofdm_equalizer_0.set_alpha(alpha)
        self.swifi_ofdm_equalizer_0.set_beta(beta)
        self.swifi_ofdm_equalizer_0.enable_foc(False)
        self.swifi_ofdm_chan_est_0_0 = swifi.ofdm_chan_est(64,((0, 0j, 0, 0j, 0, 0j, -1, 1j, -1, 1j, -1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, (-0-1j), 1, -1j, -1, 1j, 0, -1j, 1, (-0-1j), 1, -1j, 1, 1j, -1, -1j, 1, (-0-1j), -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 0j, 0, 0j, 0, 0j), (0, 0, 0, 0, 0, 0, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 1, -1, -1, 1, 1, -1, 1, -1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 0, 0, 0, 0)),0.5,
            "channel_states","frame_len",3,False)
        self.swifi_ofdm_chan_est_0_0.set_estimation_method(0)
        self.swifi_ofdm_chan_est_0 = swifi.ofdm_chan_est(64,((0, 0j, 0, 0j, 0, 0j, -1, 1j, -1, 1j, -1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, (-0-1j), 1, -1j, -1, 1j, 0, -1j, 1, (-0-1j), 1, -1j, 1, 1j, -1, -1j, 1, (-0-1j), -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 0j, 0, 0j, 0, 0j), (0, 0, 0, 0, 0, 0, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 1, -1, -1, 1, 1, -1, 1, -1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 0, 0, 0, 0)),0.5,
            "channel_states","frame_len",3,False)
        self.swifi_ofdm_chan_est_0.set_estimation_method(0)
        self.swifi_interleaver_0_1 = swifi.interleaver(False,"phy")
        self.swifi_interleaver_0_0_0 = swifi.interleaver(False,11)
        self.swifi_interleaver_0_0 = swifi.interleaver(False,11)
        self.swifi_interleaver_0 = swifi.interleaver(False,"phy")
        self.swifi_header_payload_demux_0_0 = swifi.header_payload_demux(
                3,
                64,
                16,
                "frame_len",
                "trigger",
                True,
                gr.sizeof_gr_complex,
                "phy")
        self.swifi_header_payload_demux_0 = swifi.header_payload_demux(
                3,
                64,
                16,
                "frame_len",
                "trigger",
                True,
                gr.sizeof_gr_complex,
                "phy")
        self.swifi_constellation_decoder_0_0 = swifi.constellation_decoder("phy")
        self.swifi_constellation_decoder_0 = swifi.constellation_decoder("phy")
        self.swifi_bit_unpacker_0_0 = swifi.bit_unpacker("phy",1)
        self.swifi_bit_unpacker_0 = swifi.bit_unpacker("phy",1)
        self.swifi_bit_inserter_0_0 = swifi.bit_inserter("phy")
        self.swifi_bit_inserter_0 = swifi.bit_inserter("phy")
        self.spectrum_plotter_two_input_plotter_0 = spectrum_plotter.two_input_plotter(-8, 105, 3)
        self.spectrum_plotter_csi_collector_1 = spectrum_plotter.csi_collector(gr.sizeof_gr_complex*1)
        self.spectrum_plotter_csi_collector_0 = spectrum_plotter.csi_collector(gr.sizeof_gr_complex*1)
        self.limesdr_source_0 = limesdr.source("", 2, "")
        self.limesdr_source_0.set_sample_rate(10e6)
        self.limesdr_source_0.set_center_freq(2.472e9, 0)
        self.limesdr_source_0.set_bandwidth(20e6,0)
        self.limesdr_source_0.set_bandwidth(20e6,1)
        self.limesdr_source_0.set_gain(60,0)
        self.limesdr_source_0.set_gain(60,1)
        self.limesdr_source_0.set_antenna(1,0)
        self.limesdr_source_0.set_antenna(1,1)
        self.limesdr_source_0.calibrate(5e6, 0)
        self.limesdr_source_0.calibrate(5e6, 1)
            
        (self.limesdr_source_0).set_max_output_buffer(100)
        self.fft_vxx_0_1 = fft.fft_vcc(64, True, (window.rectangular(64)), True, 1)
        self.fft_vxx_0_0_0 = fft.fft_vcc(64, True, (window.rectangular(64)), True, 1)
        self.fft_vxx_0_0 = fft.fft_vcc(64, True, (window.rectangular(64)), True, 1)
        self.fft_vxx_0 = fft.fft_vcc(64, True, (window.rectangular(64)), True, 1)
        self.digital_packet_headerparser_b_0_0_0 = digital.packet_headerparser_b(header_formatter_2.base())
        self.digital_packet_headerparser_b_0_0 = digital.packet_headerparser_b(header_formatter_2.base())
        self.digital_constellation_decoder_cb_0_0 = digital.constellation_decoder_cb(header_mod.base())
        self.digital_constellation_decoder_cb_0 = digital.constellation_decoder_cb(header_mod.base())
        self.blocks_vector_to_stream_1_0_0_0_0 = blocks.vector_to_stream(gr.sizeof_gr_complex*1, 48)
        self.blocks_vector_to_stream_1_0_0_0 = blocks.vector_to_stream(gr.sizeof_gr_complex*1, 48)
        self.blocks_vector_to_stream_1_0_0 = blocks.vector_to_stream(gr.sizeof_gr_complex*1, 48)
        self.blocks_vector_to_stream_1_0 = blocks.vector_to_stream(gr.sizeof_gr_complex*1, 48)
        self.blocks_unpacked_to_packed_xx_0_1 = blocks.unpacked_to_packed_bb(1, gr.GR_LSB_FIRST)
        self.blocks_unpacked_to_packed_xx_0_0_0 = blocks.unpacked_to_packed_bb(1, gr.GR_LSB_FIRST)
        self.blocks_unpacked_to_packed_xx_0_0 = blocks.unpacked_to_packed_bb(1, gr.GR_LSB_FIRST)
        self.blocks_unpacked_to_packed_xx_0 = blocks.unpacked_to_packed_bb(1, gr.GR_LSB_FIRST)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.digital_packet_headerparser_b_0_0, 'header_data'), (self.swifi_header_payload_demux_0, 'header_data'))    
        self.msg_connect((self.digital_packet_headerparser_b_0_0_0, 'header_data'), (self.swifi_header_payload_demux_0_0, 'header_data'))    
        self.msg_connect((self.spectrum_plotter_csi_collector_0, 'csi_out'), (self.spectrum_plotter_two_input_plotter_0, 'pdus1'))    
        self.msg_connect((self.spectrum_plotter_csi_collector_1, 'csi_out'), (self.spectrum_plotter_two_input_plotter_0, 'pdus2'))    
        self.connect((self.blocks_unpacked_to_packed_xx_0, 0), (self.swifi_payload_parser_0, 0))    
        self.connect((self.blocks_unpacked_to_packed_xx_0_0, 0), (self.digital_packet_headerparser_b_0_0, 0))    
        self.connect((self.blocks_unpacked_to_packed_xx_0_0_0, 0), (self.digital_packet_headerparser_b_0_0_0, 0))    
        self.connect((self.blocks_unpacked_to_packed_xx_0_1, 0), (self.swifi_payload_parser_0_0, 0))    
        self.connect((self.blocks_vector_to_stream_1_0, 0), (self.swifi_sample_marker_0, 0))    
        self.connect((self.blocks_vector_to_stream_1_0_0, 0), (self.swifi_sample_marker_0_0, 0))    
        self.connect((self.blocks_vector_to_stream_1_0_0_0, 0), (self.digital_constellation_decoder_cb_0, 0))    
        self.connect((self.blocks_vector_to_stream_1_0_0_0, 0), (self.spectrum_plotter_csi_collector_0, 0))    
        self.connect((self.blocks_vector_to_stream_1_0_0_0_0, 0), (self.digital_constellation_decoder_cb_0_0, 0))    
        self.connect((self.blocks_vector_to_stream_1_0_0_0_0, 0), (self.spectrum_plotter_csi_collector_1, 0))    
        self.connect((self.digital_constellation_decoder_cb_0, 0), (self.swifi_interleaver_0_0, 0))    
        self.connect((self.digital_constellation_decoder_cb_0_0, 0), (self.swifi_interleaver_0_0_0, 0))    
        self.connect((self.fft_vxx_0, 0), (self.swifi_ofdm_chan_est_0, 0))    
        self.connect((self.fft_vxx_0_0, 0), (self.swifi_ofdm_equalizer_0, 0))    
        self.connect((self.fft_vxx_0_0_0, 0), (self.swifi_ofdm_equalizer_0_0, 0))    
        self.connect((self.fft_vxx_0_1, 0), (self.swifi_ofdm_chan_est_0_0, 0))    
        self.connect((self.limesdr_source_0, 0), (self.swifi_ofdm_frame_sync_0, 0))    
        self.connect((self.limesdr_source_0, 1), (self.swifi_ofdm_frame_sync_0_0, 0))    
        self.connect((self.swifi_bit_inserter_0, 0), (self.swifi_wifi_conv_decoder_0, 0))    
        self.connect((self.swifi_bit_inserter_0_0, 0), (self.swifi_wifi_conv_decoder_0_1, 0))    
        self.connect((self.swifi_bit_unpacker_0, 0), (self.swifi_interleaver_0, 0))    
        self.connect((self.swifi_bit_unpacker_0_0, 0), (self.swifi_interleaver_0_1, 0))    
        self.connect((self.swifi_constellation_decoder_0, 0), (self.swifi_bit_unpacker_0, 0))    
        self.connect((self.swifi_constellation_decoder_0_0, 0), (self.swifi_bit_unpacker_0_0, 0))    
        self.connect((self.swifi_header_payload_demux_0, 0), (self.fft_vxx_0, 0))    
        self.connect((self.swifi_header_payload_demux_0, 1), (self.fft_vxx_0_0, 0))    
        self.connect((self.swifi_header_payload_demux_0_0, 1), (self.fft_vxx_0_0_0, 0))    
        self.connect((self.swifi_header_payload_demux_0_0, 0), (self.fft_vxx_0_1, 0))    
        self.connect((self.swifi_interleaver_0, 0), (self.swifi_bit_inserter_0, 0))    
        self.connect((self.swifi_interleaver_0_0, 0), (self.swifi_wifi_conv_decoder_0_0, 0))    
        self.connect((self.swifi_interleaver_0_0_0, 0), (self.swifi_wifi_conv_decoder_0_0_0, 0))    
        self.connect((self.swifi_interleaver_0_1, 0), (self.swifi_bit_inserter_0_0, 0))    
        self.connect((self.swifi_ofdm_chan_est_0, 0), (self.swifi_ofdm_equalizer_1, 0))    
        self.connect((self.swifi_ofdm_chan_est_0_0, 0), (self.swifi_ofdm_equalizer_1_0, 0))    
        self.connect((self.swifi_ofdm_equalizer_0, 0), (self.blocks_vector_to_stream_1_0, 0))    
        self.connect((self.swifi_ofdm_equalizer_0_0, 0), (self.blocks_vector_to_stream_1_0_0, 0))    
        self.connect((self.swifi_ofdm_equalizer_1, 0), (self.blocks_vector_to_stream_1_0_0_0, 0))    
        self.connect((self.swifi_ofdm_equalizer_1_0, 0), (self.blocks_vector_to_stream_1_0_0_0_0, 0))    
        self.connect((self.swifi_ofdm_frame_sync_0, 0), (self.swifi_header_payload_demux_0, 0))    
        self.connect((self.swifi_ofdm_frame_sync_0_0, 0), (self.swifi_header_payload_demux_0_0, 0))    
        self.connect((self.swifi_sample_marker_0, 0), (self.swifi_constellation_decoder_0, 0))    
        self.connect((self.swifi_sample_marker_0_0, 0), (self.swifi_constellation_decoder_0_0, 0))    
        self.connect((self.swifi_scrambler_0, 0), (self.blocks_unpacked_to_packed_xx_0, 0))    
        self.connect((self.swifi_scrambler_0_0, 0), (self.blocks_unpacked_to_packed_xx_0_1, 0))    
        self.connect((self.swifi_wifi_conv_decoder_0, 0), (self.swifi_scrambler_0, 0))    
        self.connect((self.swifi_wifi_conv_decoder_0_0, 0), (self.blocks_unpacked_to_packed_xx_0_0, 0))    
        self.connect((self.swifi_wifi_conv_decoder_0_0_0, 0), (self.blocks_unpacked_to_packed_xx_0_0_0, 0))    
        self.connect((self.swifi_wifi_conv_decoder_0_1, 0), (self.swifi_scrambler_0_0, 0))    

    def get_alpha(self):
        return self.alpha

    def set_alpha(self, alpha):
        self.alpha = alpha

    def get_antenna(self):
        return self.antenna

    def set_antenna(self, antenna):
        self.antenna = antenna

    def get_beta(self):
        return self.beta

    def set_beta(self, beta):
        self.beta = beta

    def get_dump_filename(self):
        return self.dump_filename

    def set_dump_filename(self, dump_filename):
        self.dump_filename = dump_filename

    def get_equalization_method(self):
        return self.equalization_method

    def set_equalization_method(self, equalization_method):
        self.equalization_method = equalization_method

    def get_ff(self):
        return self.ff

    def set_ff(self, ff):
        self.ff = ff

    def get_filename(self):
        return self.filename

    def set_filename(self, filename):
        self.filename = filename

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain

    def get_nskip(self):
        return self.nskip

    def set_nskip(self, nskip):
        self.nskip = nskip

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

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
        "", "--alpha", dest="alpha", type="eng_float", default=eng_notation.num_to_str(0.15),
        help="Set Alpha [default=%default]")
    parser.add_option(
        "-A", "--antenna", dest="antenna", type="string", default="TX/RX",
        help="Set Antenna [default=%default]")
    parser.add_option(
        "", "--beta", dest="beta", type="eng_float", default=eng_notation.num_to_str(0.3),
        help="Set Beta [default=%default]")
    parser.add_option(
        "-O", "--dump-filename", dest="dump_filename", type="string", default="<<stdout>>",
        help="Set Dump Filename [default=%default]")
    parser.add_option(
        "-e", "--equalization-method", dest="equalization_method", type="string", default="ppt_dd",
        help="Set Equalization Method [default=%default]")
    parser.add_option(
        "", "--ff", dest="ff", type="string", default="",
        help="Set Frame Filter [default=%default]")
    parser.add_option(
        "-F", "--filename", dest="filename", type="string", default="",
        help="Set Filename [default=%default]")
    parser.add_option(
        "-f", "--freq", dest="freq", type="eng_float", default=eng_notation.num_to_str(2.412e9),
        help="Set Center Frequency [default=%default]")
    parser.add_option(
        "-g", "--gain", dest="gain", type="eng_float", default=eng_notation.num_to_str(30),
        help="Set Gain [default=%default]")
    parser.add_option(
        "-n", "--nskip", dest="nskip", type="long", default=20000000,
        help="Set Skip [default=%default]")
    parser.add_option(
        "-s", "--samp-rate", dest="samp_rate", type="eng_float", default=eng_notation.num_to_str(20e6),
        help="Set Sampling Rate [default=%default]")
    return parser


def main(top_block_cls=rx_multi, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(alpha=options.alpha, antenna=options.antenna, beta=options.beta, dump_filename=options.dump_filename, equalization_method=options.equalization_method, ff=options.ff, filename=options.filename, freq=options.freq, gain=options.gain, nskip=options.nskip, samp_rate=options.samp_rate)
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
