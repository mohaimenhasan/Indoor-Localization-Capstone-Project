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


#ifndef INCLUDED_SWIFI_BITMETRICS_COMPUTER_H
#define INCLUDED_SWIFI_BITMETRICS_COMPUTER_H

#include <swifi/api.h>
#include <swifi/dynamic_rate_block.h>
#include <gnuradio/digital/constellation.h>
#include <map>

namespace gr {
  namespace swifi {

    /*!
     * \brief Bit metrics computer with modulation changed by tags.
     *
     * \details Compute metrics with respect to each individual bit
     * from a received sample based on the constellation given by the tag.
     */
    class SWIFI_API bitmetrics_computer
      : virtual public dynamic_rate_block
    {
    public:
      // gr::swifi::bitmetrics_computer::sptr
      typedef boost::shared_ptr<bitmetrics_computer> sptr;

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

#endif /* INCLUDED_SWIFI_BITMETRICS_COMPUTER_H */
