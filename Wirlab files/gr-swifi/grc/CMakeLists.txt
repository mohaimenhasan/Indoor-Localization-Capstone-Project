# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

install(FILES
    swifi_bit_inserter.xml
    swifi_bit_packer.xml
    swifi_bit_stealer.xml
    swifi_bit_unpacker.xml
    swifi_bitmetrics_computer.xml
    swifi_bitmetrics_inserter.xml
    swifi_bitmetrics_interleaver.xml
    swifi_chunks_to_symbols.xml
    swifi_constellation_decoder.xml
    swifi_dsss_frame_sync.xml
    swifi_dsss_frame_sync_2.xml
    swifi_fd_noise_adder.xml
    swifi_frame_filter.xml
    swifi_frame_samples_extractor.xml
    swifi_header_payload_demux.xml
    swifi_header_payload_demux_2.xml
    swifi_interleaver.xml
    swifi_mac_frame_generator.xml
    swifi_ofdm_chan_est.xml
    swifi_ofdm_equalizer.xml
    swifi_ofdm_frame_sync.xml
    swifi_payload_parser.xml
    swifi_peak_gate.xml
    swifi_phy_frame_generator.xml
    swifi_sample_adder.xml
    swifi_sample_marker.xml
    swifi_scrambler.xml
    swifi_stream_pack.xml
    swifi_tx_time_tag_adder.xml
    swifi_unpack_lsb.xml
    swifi_vector_valve.xml
    swifi_wifi_conv_decoder.xml
    swifi_wifi_conv_encoder.xml
    swifi_wifi_conv_soft_decoder.xml
    DESTINATION share/gnuradio/grc/blocks
)
