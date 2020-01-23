# Copyright 2011,2012 Free Software Foundation, Inc.
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

########################################################################
# Install public header files
########################################################################
install(FILES
    api.h
    bit_inserter.h
    bit_packer.h
    bit_stealer.h
    bit_unpacker.h
    bitmetrics_computer.h
    bitmetrics_inserter.h
    bitmetrics_interleaver.h
    chunks_to_symbols.h
    constellation_decoder.h
    constellation_metrics.h
    dsss_frame_sync.h
    dynamic_rate_block.h
    fd_noise_adder.h
    frame_filter.h
    frame_samples_extractor.h
    header_payload_demux.h
    header_payload_demux_2.h
    interleaver.h
    mac_frame_generator.h
    mac_frame_info.h
    mac_parser.h
    mac_utils.h
    ofdm_chan_est.h
    ofdm_equalizer.h
    ofdm_frame_sync.h
    ofdm_phy_header_parser.h
    payload_parser.h
    peak_gate.h
    phy_frame_generator.h
    phy_utils.h
    sample_adder.h
    sample_marker.h
    scrambler.h
    stream_pack.h
    tag_utils.h
    tx_time_tag_adder.h
    unpack_lsb.h
    vector_valve.h
    wifi_constellation.h
    wifi_conv_1_2.h
    wifi_conv_decoder.h
    wifi_conv_encoder.h
    wifi_conv_soft_decoder.h
    wifi_dump_factory.h
    DESTINATION include/swifi
)

