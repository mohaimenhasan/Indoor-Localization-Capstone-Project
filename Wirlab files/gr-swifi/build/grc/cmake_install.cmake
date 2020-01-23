# Install script for directory: /home/wirlab/gr-swifi/grc

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gnuradio/grc/blocks" TYPE FILE FILES
    "/home/wirlab/gr-swifi/grc/swifi_bit_inserter.xml"
    "/home/wirlab/gr-swifi/grc/swifi_bit_packer.xml"
    "/home/wirlab/gr-swifi/grc/swifi_bit_stealer.xml"
    "/home/wirlab/gr-swifi/grc/swifi_bit_unpacker.xml"
    "/home/wirlab/gr-swifi/grc/swifi_bitmetrics_computer.xml"
    "/home/wirlab/gr-swifi/grc/swifi_bitmetrics_inserter.xml"
    "/home/wirlab/gr-swifi/grc/swifi_bitmetrics_interleaver.xml"
    "/home/wirlab/gr-swifi/grc/swifi_chunks_to_symbols.xml"
    "/home/wirlab/gr-swifi/grc/swifi_constellation_decoder.xml"
    "/home/wirlab/gr-swifi/grc/swifi_dsss_frame_sync.xml"
    "/home/wirlab/gr-swifi/grc/swifi_dsss_frame_sync_2.xml"
    "/home/wirlab/gr-swifi/grc/swifi_fd_noise_adder.xml"
    "/home/wirlab/gr-swifi/grc/swifi_frame_filter.xml"
    "/home/wirlab/gr-swifi/grc/swifi_frame_samples_extractor.xml"
    "/home/wirlab/gr-swifi/grc/swifi_header_payload_demux.xml"
    "/home/wirlab/gr-swifi/grc/swifi_header_payload_demux_2.xml"
    "/home/wirlab/gr-swifi/grc/swifi_interleaver.xml"
    "/home/wirlab/gr-swifi/grc/swifi_mac_frame_generator.xml"
    "/home/wirlab/gr-swifi/grc/swifi_ofdm_chan_est.xml"
    "/home/wirlab/gr-swifi/grc/swifi_ofdm_equalizer.xml"
    "/home/wirlab/gr-swifi/grc/swifi_ofdm_frame_sync.xml"
    "/home/wirlab/gr-swifi/grc/swifi_payload_parser.xml"
    "/home/wirlab/gr-swifi/grc/swifi_peak_gate.xml"
    "/home/wirlab/gr-swifi/grc/swifi_phy_frame_generator.xml"
    "/home/wirlab/gr-swifi/grc/swifi_sample_adder.xml"
    "/home/wirlab/gr-swifi/grc/swifi_sample_marker.xml"
    "/home/wirlab/gr-swifi/grc/swifi_scrambler.xml"
    "/home/wirlab/gr-swifi/grc/swifi_stream_pack.xml"
    "/home/wirlab/gr-swifi/grc/swifi_tx_time_tag_adder.xml"
    "/home/wirlab/gr-swifi/grc/swifi_unpack_lsb.xml"
    "/home/wirlab/gr-swifi/grc/swifi_vector_valve.xml"
    "/home/wirlab/gr-swifi/grc/swifi_wifi_conv_decoder.xml"
    "/home/wirlab/gr-swifi/grc/swifi_wifi_conv_encoder.xml"
    "/home/wirlab/gr-swifi/grc/swifi_wifi_conv_soft_decoder.xml"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

