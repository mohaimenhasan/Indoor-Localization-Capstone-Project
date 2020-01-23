/* -*- c++ -*- */

#define SWIFI_API
#define DIGITAL_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "swifi_swig_doc.i"


%{
#include "gnuradio/digital/constellation.h"
#include "gnuradio/digital/packet_header_default.h"
#include "pmt/pmt.h"

#include "swifi/wifi_constellation.h"
#include "swifi/tag_utils.h"
#include "swifi/dynamic_rate_block.h"

#include "swifi/bit_inserter.h"
#include "swifi/bit_packer.h"
#include "swifi/bit_stealer.h"
#include "swifi/bit_unpacker.h"
#include "swifi/bitmetrics_computer.h"
#include "swifi/bitmetrics_inserter.h"
#include "swifi/bitmetrics_interleaver.h"
#include "swifi/chunks_to_symbols.h"
#include "swifi/constellation_decoder.h"
#include "swifi/dsss_frame_sync.h"
#include "swifi/dsss_frame_sync_2.h"
#include "swifi/fd_noise_adder.h"
#include "swifi/frame_filter.h"
#include "swifi/frame_samples_extractor.h"
#include "swifi/header_payload_demux.h"
#include "swifi/header_payload_demux_2.h"
#include "swifi/interleaver.h"
#include "swifi/mac_frame_info.h"
#include "swifi/mac_utils.h"
#include "swifi/mac_frame_generator.h"
#include "swifi/ofdm_chan_est.h"
#include "swifi/ofdm_equalizer.h"
#include "swifi/ofdm_frame_sync.h"
#include "swifi/ofdm_phy_header_parser.h"
#include "swifi/payload_parser.h"
#include "swifi/peak_gate.h"
#include "swifi/phy_frame_generator.h"
#include "swifi/phy_utils.h"
#include "swifi/sample_adder.h"
#include "swifi/sample_marker.h"
#include "swifi/scrambler.h"
#include "swifi/stream_pack.h"
#include "swifi/tx_time_tag_adder.h"
#include "swifi/unpack_lsb.h"
#include "swifi/vector_valve.h"
#include "swifi/wifi_conv_1_2.h"
#include "swifi/wifi_conv_decoder.h"
#include "swifi/wifi_conv_encoder.h"
#include "swifi/wifi_conv_soft_decoder.h"

%}

%include "gnuradio/digital/constellation.h"
%include "gnuradio/digital/packet_header_default.h"
%include "swifi/wifi_constellation.h"
%include "swifi/tag_utils.h"
%include "swifi/dynamic_rate_block.h"


%include "swifi/bit_inserter.h"
%include "swifi/bit_packer.h"
%include "swifi/bit_stealer.h"
%include "swifi/bit_unpacker.h"
%include "swifi/bitmetrics_computer.h"
%include "swifi/bitmetrics_inserter.h"
%include "swifi/bitmetrics_interleaver.h"
%include "swifi/chunks_to_symbols.h"
%include "swifi/constellation_decoder.h"
%include "swifi/dsss_frame_sync.h"
%include "swifi/dsss_frame_sync_2.h"
%include "swifi/fd_noise_adder.h"
%include "swifi/frame_filter.h"
%include "swifi/frame_samples_extractor.h"
%include "swifi/header_payload_demux.h"
%include "swifi/header_payload_demux_2.h"
%include "swifi/interleaver.h"
%include "swifi/mac_frame_info.h"
%include "swifi/mac_utils.h"
%include "swifi/mac_frame_generator.h"
%include "swifi/ofdm_chan_est.h"
%include "swifi/ofdm_equalizer.h"
%include "swifi/ofdm_frame_sync.h"
%include "swifi/ofdm_phy_header_parser.h"
%include "swifi/payload_parser.h"
%include "swifi/peak_gate.h"
%include "swifi/phy_frame_generator.h"
%include "swifi/phy_utils.h"
%include "swifi/sample_adder.h"
%include "swifi/sample_marker.h"
%include "swifi/scrambler.h"
%include "swifi/stream_pack.h"
%include "swifi/tx_time_tag_adder.h"
%include "swifi/unpack_lsb.h"
%include "swifi/vector_valve.h"
%include "swifi/wifi_conv_1_2.h"
%include "swifi/wifi_conv_decoder.h"
%include "swifi/wifi_conv_encoder.h"
%include "swifi/wifi_conv_soft_decoder.h"




GR_SWIG_BLOCK_MAGIC2(swifi, bit_inserter);
GR_SWIG_BLOCK_MAGIC2(swifi, bit_packer);
GR_SWIG_BLOCK_MAGIC2(swifi, bit_stealer);
GR_SWIG_BLOCK_MAGIC2(swifi, bit_unpacker);
GR_SWIG_BLOCK_MAGIC2(swifi, bitmetrics_computer);
GR_SWIG_BLOCK_MAGIC2(swifi, bitmetrics_inserter);
GR_SWIG_BLOCK_MAGIC2(swifi, bitmetrics_interleaver);
GR_SWIG_BLOCK_MAGIC2(swifi, chunks_to_symbols);
GR_SWIG_BLOCK_MAGIC2(swifi, constellation_decoder);
GR_SWIG_BLOCK_MAGIC2(swifi, dsss_frame_sync);
GR_SWIG_BLOCK_MAGIC2(swifi, dsss_frame_sync_2);
GR_SWIG_BLOCK_MAGIC2(swifi, fd_noise_adder);
GR_SWIG_BLOCK_MAGIC2(swifi, frame_filter);
GR_SWIG_BLOCK_MAGIC2(swifi, frame_samples_extractor);
GR_SWIG_BLOCK_MAGIC2(swifi, header_payload_demux);
GR_SWIG_BLOCK_MAGIC2(swifi, header_payload_demux_2);
GR_SWIG_BLOCK_MAGIC2(swifi, interleaver);
GR_SWIG_BLOCK_MAGIC2(swifi, mac_frame_generator);
GR_SWIG_BLOCK_MAGIC2(swifi, ofdm_chan_est);
GR_SWIG_BLOCK_MAGIC2(swifi, ofdm_equalizer);
GR_SWIG_BLOCK_MAGIC2(swifi, ofdm_frame_sync);
GR_SWIG_BLOCK_MAGIC2(swifi, payload_parser);
GR_SWIG_BLOCK_MAGIC2(swifi, peak_gate);
GR_SWIG_BLOCK_MAGIC2(swifi, phy_frame_generator);
GR_SWIG_BLOCK_MAGIC2(swifi, sample_adder);
GR_SWIG_BLOCK_MAGIC2(swifi, sample_marker);
GR_SWIG_BLOCK_MAGIC2(swifi, scrambler);
GR_SWIG_BLOCK_MAGIC2(swifi, stream_pack);
GR_SWIG_BLOCK_MAGIC2(swifi, tx_time_tag_adder);
GR_SWIG_BLOCK_MAGIC2(swifi, unpack_lsb);
GR_SWIG_BLOCK_MAGIC2(swifi, vector_valve);
GR_SWIG_BLOCK_MAGIC2(swifi, wifi_conv_decoder);
GR_SWIG_BLOCK_MAGIC2(swifi, wifi_conv_encoder);
GR_SWIG_BLOCK_MAGIC2(swifi, wifi_conv_soft_decoder);


/*
%template(mac_utils_sptr) boost::shared_ptr<gr::swifi::mac_utils>;
%pythoncode %{
mac_utils_sptr.__repr__ = lambda self: "<mac_utils>"
mac_utils = mac_utils .make;
%}
*/


/*
%template(phy_utils_sptr) boost::shared_ptr<gr::swifi::phy_utils>;
%pythoncode %{
phy_utils_sptr.__repr__ = lambda self: "<phy_utils>"
phy_utils = phy_utils .make;
%}
*/


%template(ofdm_phy_header_parser_sptr) boost::shared_ptr<gr::swifi::ofdm_phy_header_parser>;
%pythoncode %{
ofdm_phy_header_parser_sptr.__repr__ = lambda self: "<ofdm_phy_header_parser>"
ofdm_phy_header_parser = ofdm_phy_header_parser .make;
%}



%template(wifi_constellation_bpsk_sptr) boost::shared_ptr<gr::swifi::wifi_constellation_bpsk>;
%pythoncode %{
wifi_constellation_bpsk_sptr.__repr__ = lambda self: "<constellation BPSK soft>"
wifi_constellation_bpsk = wifi_constellation_bpsk.make;
%}


%template(wifi_constellation_qpsk_sptr) boost::shared_ptr<gr::swifi::wifi_constellation_qpsk>;
%pythoncode %{
wifi_constellation_qpsk_sptr.__repr__ = lambda self: "<constellation QPSK soft>"
wifi_constellation_qpsk = wifi_constellation_qpsk.make;
%}

%template(wifi_constellation_16qam_sptr) boost::shared_ptr<gr::swifi::wifi_constellation_16qam>;
%pythoncode %{
wifi_constellation_16qam_sptr.__repr__ = lambda self: "<constellation QAM16>"
wifi_constellation_16qam = wifi_constellation_16qam.make;
%}

%template(wifi_constellation_64qam_sptr) boost::shared_ptr<gr::swifi::wifi_constellation_64qam>;
%pythoncode %{
wifi_constellation_64qam_sptr.__repr__ = lambda self: "<constellation QAM64>"
wifi_constellation_64qam = wifi_constellation_64qam.make;
%}


