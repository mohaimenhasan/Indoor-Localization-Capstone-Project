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



#ifndef INCLUDED_SWIFI_INTERLEAVER_H
#define INCLUDED_SWIFI_INTERLEAVER_H

#include <swifi/api.h>
#include <swifi/dynamic_rate_block.h>
// #include <gnuradio/sync_block.h> // used for old implementation
#include <vector>

namespace gr {
  namespace swifi {

    /*!
     * \brief IEEE 802.11a interleaver.
     * \ingroup swifi
     *
     */
    class SWIFI_API interleaver : 
      virtual public dynamic_rate_block
      // virtual public gr::sync_block  // used for old implementation
    {
     public:
      typedef boost::shared_ptr<interleaver> sptr;

      /*!
       * \brief Create a new instance of swifi::interleaver
       * in tag-processing mode.
       *
       * \param is_interleaver true if interleaver, false if deinterleaver.
       * \param phy_tag_key Tag key for PHY processing.
       */
      static sptr make(bool is_interleaver, 
        const std::string &phy_tag_key);

      /*!
       * \brief Create a new instance of swifi::interleaver
       * in tag-processing mode.
       *
       * \param is_interleaver true if interleaver, false if deinterleaver.
       * \param rate OFDM rate.
       */
      static sptr make(bool is_interleaver, int rate);

      /*!
       * \brief Set new interleaving/deinterleaving map.
       * \param newmap new mapping.
       */
      virtual void set_interleaving_map(const std::vector<int> &newmap) = 0;

      /*!
       * \brief Set new interleaving/deinterleaving map according to the given Wi-Fi rate.
       * \param rate Wi-Fi rate value.
       */
      virtual void set_interleaving_map(int rate) = 0;
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_INTERLEAVER_H */

