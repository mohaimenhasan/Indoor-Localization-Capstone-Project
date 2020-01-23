/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_SWIFI_HEADER_PAYLOAD_DEMUX_H
#define INCLUDED_SWIFI_HEADER_PAYLOAD_DEMUX_H

#include <swifi/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace swifi {

    /*!
     * \brief Header/Payload demuxer.
     * \ingroup packet_operators_blk
     *
     * \details
     * This block is designed to handle packets from a bursty transmission.
     * The input takes a continuous transmission of samples.
     * Unless a tag with the key specified in \p trigger_tag_key
     * is found, samples from the input are dropped.
     * The trigger tag can have pmt::PMT_T or pmt::PMT_F, where
     * the former indicates the frame start, while the latter indicates
     * the frame end.
     *
     * Once a pmt::PMT_T trigger is found, a total of \p header_len items are copied to output 0.
     * The block then stalls until it receives a message on the message port
     * \p header_data. The message must be a PMT dictionary; all key/value pairs are
     * copied as tags to the first item of the payload (which is assumed to be the
     * first item after the header).
     * The value corresponding to the key specified in \p length_tag_key is read
     * and taken as the payload length. The payload, together with the header data
     * as tags, is then copied to output 1.
     * The copying of symbols to payload is stopped once the number of copied
     * symbols reaches the payload length value specified in \p length_tag_key,
     * or once the pmt::PMT_F trigger tag is found.
     *
     * If specified, \p guard_interval items are discarded before every symbol.
     * This is useful for demuxing bursts of OFDM signals.
     *
     * Any tags on the input stream are copied to the corresponding output *if* they're
     * on an item that is propagated. Note that a tag on the header items is copied to the
     * header stream; that means the header-parsing block must handle these tags if they
     * should go on the payload.
     * A special case are tags on items that make up the guard interval. These are copied
     * to the first item of the following symbol.
     * If a tag is situated very close to the end of the payload, it might be unclear if
     * it belongs to this packet or the following. In this case, the tag might be propagated
     * twice.
     *
     * Tags outside of packets are generally discarded.
     *
     */
    class SWIFI_API header_payload_demux : virtual public block
    {
     public:
      typedef boost::shared_ptr<header_payload_demux> sptr;

      /*!
       * \param header_len Number of symbols per header
       * \param output_items_per_symbol Number of items per output symbol
       * \param guard_interval Number of items between two consecutive symbols
       * \param length_tag_key Key of the frame length tag
       * \param trigger_tag_key Key of the trigger tag
       * \param output_symbols Output symbols (true) or items (false)?
       * \param itemsize Item size (bytes per item)
       * \param phy_tag_key Key of PHY tag
       */
      static sptr make(int header_len, int output_items_per_symbol, int guard_interval=0,
          const std::string &length_tag_key="frame_len",
          const std::string &trigger_tag_key="trigger",
          bool output_symbols=false,
          size_t itemsize=sizeof(gr_complex),
          const std::string &phy_tag_key="phy");
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_HEADER_PAYLOAD_DEMUX_H */

