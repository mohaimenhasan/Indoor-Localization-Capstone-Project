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
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND swifi_sources
    bit_inserter_impl.cc
    bit_packer_impl.cc
    bit_stealer_impl.cc
    bit_unpacker_impl.cc
    bitmetrics_computer_impl.cc
    bitmetrics_inserter_impl.cc
    bitmetrics_interleaver_impl.cc
    chunks_to_symbols_impl.cc
    constellation_decoder_impl.cc
    diffmodem.cc
    dsss_frame_sync_impl.cc
    dsss_frame_sync_2_impl.cc
    dynamic_rate_block.cc
    fd_noise_adder_impl.cc
    frame_filter_impl.cc
    frame_samples_extractor_impl.cc
    header_payload_demux_impl.cc
    header_payload_demux_2_impl.cc
    interleaver_impl.cc
    mac_frame_generator_impl.cc
    mac_parser.cc
    mac_utils.cc
    ofdm_chan_est_impl.cc
    ofdm_equalizer_impl.cc
    ofdm_frame_sync_impl.cc
    ofdm_phy_header_parser.cc
    payload_parser_impl.cc
    peak_detector.cc
    peak_gate_impl.cc
    phy_frame_generator_impl.cc
    phy_utils.cc
    query_parser.cc
    sample_adder_impl.cc
    sample_marker_impl.cc
    scrambling.cc
    scrambler_impl.cc
    simple_filter.cc
    stream_pack_impl.cc
    tag_utils.cc
    tx_time_tag_adder_impl.cc
    unpack_lsb_impl.cc
    utils.cc
    vector_valve_impl.cc
    wifi_constellation.cc
    wifi_conv_1_2.cc
    wifi_conv_decoder_impl.cc
    wifi_conv_encoder_impl.cc
    wifi_conv_soft_decoder_impl.cc
    wifi_dump_factory.cc
    wifi_dump_formatter_text.cc
    wifi_dump_formatter_xml.cc
    xml_node.cc
    xml_reader.cc
)


set(swifi_sources "${swifi_sources}" PARENT_SCOPE)
if(NOT swifi_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT swifi_sources)

add_library(gnuradio-swifi SHARED ${swifi_sources})
target_link_libraries(gnuradio-swifi ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES}
)
set_target_properties(gnuradio-swifi PROPERTIES DEFINE_SYMBOL "gnuradio_swifi_EXPORTS")



add_executable(framefilter framefilter.cc 
    query_parser.cc
    simple_filter.cc
    utils.cc
    xml_reader.cc 
    xml_node.cc)
target_link_libraries(
    framefilter
    ${Boost_LIBRARIES}
)

add_executable(extract extract.cc 
    query_parser.cc
    simple_filter.cc
    utils.cc
    xml_reader.cc 
    xml_node.cc)
target_link_libraries(
    extract
    ${Boost_LIBRARIES}
)

if(APPLE)
    set_target_properties(gnuradio-swifi PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-swifi
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_swifi_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_swifi.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_swifi.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_phy_utils.cc
)

add_executable(test-swifi ${test_swifi_sources})

target_link_libraries(
  test-swifi
  ${GNURADIO_TRELLIS_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-swifi
)

GR_ADD_TEST(test_swifi test-swifi)
