# Install script for directory: /home/wirlab/gr-swifi/include/swifi

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/swifi" TYPE FILE FILES
    "/home/wirlab/gr-swifi/include/swifi/api.h"
    "/home/wirlab/gr-swifi/include/swifi/bit_inserter.h"
    "/home/wirlab/gr-swifi/include/swifi/bit_packer.h"
    "/home/wirlab/gr-swifi/include/swifi/bit_stealer.h"
    "/home/wirlab/gr-swifi/include/swifi/bit_unpacker.h"
    "/home/wirlab/gr-swifi/include/swifi/bitmetrics_computer.h"
    "/home/wirlab/gr-swifi/include/swifi/bitmetrics_inserter.h"
    "/home/wirlab/gr-swifi/include/swifi/bitmetrics_interleaver.h"
    "/home/wirlab/gr-swifi/include/swifi/chunks_to_symbols.h"
    "/home/wirlab/gr-swifi/include/swifi/constellation_decoder.h"
    "/home/wirlab/gr-swifi/include/swifi/constellation_metrics.h"
    "/home/wirlab/gr-swifi/include/swifi/dsss_frame_sync.h"
    "/home/wirlab/gr-swifi/include/swifi/dynamic_rate_block.h"
    "/home/wirlab/gr-swifi/include/swifi/fd_noise_adder.h"
    "/home/wirlab/gr-swifi/include/swifi/frame_filter.h"
    "/home/wirlab/gr-swifi/include/swifi/frame_samples_extractor.h"
    "/home/wirlab/gr-swifi/include/swifi/header_payload_demux.h"
    "/home/wirlab/gr-swifi/include/swifi/header_payload_demux_2.h"
    "/home/wirlab/gr-swifi/include/swifi/interleaver.h"
    "/home/wirlab/gr-swifi/include/swifi/mac_frame_generator.h"
    "/home/wirlab/gr-swifi/include/swifi/mac_frame_info.h"
    "/home/wirlab/gr-swifi/include/swifi/mac_parser.h"
    "/home/wirlab/gr-swifi/include/swifi/mac_utils.h"
    "/home/wirlab/gr-swifi/include/swifi/ofdm_chan_est.h"
    "/home/wirlab/gr-swifi/include/swifi/ofdm_equalizer.h"
    "/home/wirlab/gr-swifi/include/swifi/ofdm_frame_sync.h"
    "/home/wirlab/gr-swifi/include/swifi/ofdm_phy_header_parser.h"
    "/home/wirlab/gr-swifi/include/swifi/payload_parser.h"
    "/home/wirlab/gr-swifi/include/swifi/peak_gate.h"
    "/home/wirlab/gr-swifi/include/swifi/phy_frame_generator.h"
    "/home/wirlab/gr-swifi/include/swifi/phy_utils.h"
    "/home/wirlab/gr-swifi/include/swifi/sample_adder.h"
    "/home/wirlab/gr-swifi/include/swifi/sample_marker.h"
    "/home/wirlab/gr-swifi/include/swifi/scrambler.h"
    "/home/wirlab/gr-swifi/include/swifi/stream_pack.h"
    "/home/wirlab/gr-swifi/include/swifi/tag_utils.h"
    "/home/wirlab/gr-swifi/include/swifi/tx_time_tag_adder.h"
    "/home/wirlab/gr-swifi/include/swifi/unpack_lsb.h"
    "/home/wirlab/gr-swifi/include/swifi/vector_valve.h"
    "/home/wirlab/gr-swifi/include/swifi/wifi_constellation.h"
    "/home/wirlab/gr-swifi/include/swifi/wifi_conv_1_2.h"
    "/home/wirlab/gr-swifi/include/swifi/wifi_conv_decoder.h"
    "/home/wirlab/gr-swifi/include/swifi/wifi_conv_encoder.h"
    "/home/wirlab/gr-swifi/include/swifi/wifi_conv_soft_decoder.h"
    "/home/wirlab/gr-swifi/include/swifi/wifi_dump_factory.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

