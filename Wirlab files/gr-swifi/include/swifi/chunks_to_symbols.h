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


#ifndef INCLUDED_SWIFI_CHUNKS_TO_SYMBOLS_H
#define INCLUDED_SWIFI_CHUNKS_TO_SYMBOLS_H

#include <swifi/api.h>
#include <swifi/dynamic_rate_block.h>
// #include <gnuradio/sync_block.h>  // used for old implementation
#include <vector>

namespace gr {
  namespace swifi {

    /*!
     * \brief Mapping q-ary symbols into complex constellation symbols
     * with capability of changing constellation via tags.
     * \details This block currently does not support multidimensional
     * modulations.
     * \ingroup swifi
     *
     */
    class SWIFI_API chunks_to_symbols :
      virtual public dynamic_rate_block
      // virtual public gr::sync_block // used for old implementation
    {
     public:
      typedef boost::shared_ptr<chunks_to_symbols> sptr;

      /*!
       * \brief Create a new instance of swifi::chunks_to_symbols
       * in tag-processing mode.
       *
       * \param phy_tag_key Tag key for PHY properties.
       */
      static sptr make(const std::string &phy_tag_key);

      /*!
       * \brief Create a new instance of swifi::chunks_to_symbols
       * in normal mode with fixed constellation.
       *
       * \param points vector of constellation points defining the constellationm.
       */
      static sptr make(const std::vector<gr_complex> &points);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_CHUNKS_TO_SYMBOLS_H */

