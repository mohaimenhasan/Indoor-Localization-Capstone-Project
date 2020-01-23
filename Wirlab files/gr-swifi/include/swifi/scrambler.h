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



#ifndef INCLUDED_SWIFI_SCRAMBLER_H
#define INCLUDED_SWIFI_SCRAMBLER_H

#include <swifi/api.h>
#include <gnuradio/sync_block.h>
#include <vector>

namespace gr {
  namespace swifi {

    /*!
     * \brief Scrambling the input stream with IEEE 802.11a scrambling
     * construction.
     * \ingroup swifi
     *
     */
    class SWIFI_API scrambler : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<scrambler> sptr;

      /*!
       * \brief Create a new instance of swifi::scrambler
       * in tag-processing mode.
       *
       * \param is_scrambler true if scrambler, false if descrambler.
       * \param phy_tag_key Tag key for PHY processing.
       */
      static sptr make(bool is_scrambler, 
        const std::string &phy_tag_key);

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_SCRAMBLER_H */

