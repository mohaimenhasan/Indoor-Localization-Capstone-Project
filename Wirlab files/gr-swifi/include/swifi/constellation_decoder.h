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


#ifndef INCLUDED_SWIFI_CONSTELLATION_DECODER_H
#define INCLUDED_SWIFI_CONSTELLATION_DECODER_H

#include <swifi/api.h>
#include <swifi/dynamic_rate_block.h>
#include <gnuradio/digital/constellation.h>
// #include <gnuradio/sync_block.h> // used for old implementation
#include <map>

namespace gr {
  namespace swifi {

    /*!
     * \brief Constellation Decoder with modulation changed by tags.
     *
     * \details
     * Decode a constellation's points from a complex space to
     * (unpacked) bits based on the map of the \p consetllation
     * object.
     */
    class SWIFI_API constellation_decoder
      : virtual public dynamic_rate_block
      // : virtual public sync_block  // used for old implementation
    {
    public:
      // gr::swifi::constellation_decoder::sptr
      typedef boost::shared_ptr<constellation_decoder> sptr;

      /*!
       * \brief Create a constellation decoder block in tag-processing mode.
       *
       * \param phy_tag_key Tag key for SIGNAL field information.
       */
      static sptr make(const std::string& phy_tag_key="phy");

      /*!
       * \brief Create a constellation decoder block in normal mode.
       *
       * \param constellation Shared pointer to constellation object.
       */
      static sptr make(digital::constellation_sptr constellation);
    };

  } /* namespace swifi */
} /* namespace gr */

#endif /* INCLUDED_SWIFI_CONSTELLATION_DECODER_H */
