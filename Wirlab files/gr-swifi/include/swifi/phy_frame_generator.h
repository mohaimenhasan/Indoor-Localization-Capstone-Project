/* -*- c++ -*- */
/*
 * Copyright 2014 Triet Vo-Huu <vohuudtr@ccs.neu.edu>
 *
 * Wireless Security Lab - College of Computer and Information Science
 * Northeastern University, Boston, MA 02115, USA.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_SWIFI_PHY_FRAME_GENERATOR_H
#define INCLUDED_SWIFI_PHY_FRAME_GENERATOR_H

#include <swifi/api.h>
#include <gnuradio/block.h>
#include <vector>

namespace gr {
  namespace swifi {

    /*!
     * \brief PHY frame generator embeds data from upstream block
     * into a format specified by IEEE 802.11a.
     * \details The input stream of bytes will be read until
     * a signal tag is found or a number of read bytes reaches a predefined
     * value. Every chunk of data bytes will constitute a PHY frame,
     * with appropriate header fields.
     *
     * On the output, the PHY frame generator produces a stream of
     * IEEE 802.11a compliant frame. Note that the generated output is
     * not yet encoded. A signal tag is attached to
     * the first byte of the frame to inform the downstream block of
     * RATE and LENGTH value for the PHY frame to be constructed.
     *
     * \ingroup swifi
     *
     */
    class SWIFI_API phy_frame_generator : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<phy_frame_generator> sptr;

      /*!
       * \brief Create a new instance of swifi::phy_frame_generator.
       *
       * \param phy_tag_key Tag key for PHY properties used for
       *                    controlling encoding processing performed
       *                    by downstream blocks such as scrambling,
       *                    convolutional encoding, interleaving, modulating.
       * \param len_tag_key Tag key for number of symbols per packet.
       */
      static sptr make(const std::string &phy_tag_key,
        const std::string &len_tag_key);

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_PHY_FRAME_GENERATOR_H */

