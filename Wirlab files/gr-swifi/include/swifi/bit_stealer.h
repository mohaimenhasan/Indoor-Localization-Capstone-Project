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

#ifndef INCLUDED_SWIFI_BIT_STEALER_H
#define INCLUDED_SWIFI_BIT_STEALER_H

#include <swifi/api.h>
#include <swifi/dynamic_rate_block.h>
// #include <gnuradio/block.h>  // used for old implementation

namespace gr {
  namespace swifi {

    /*!
     * \brief Steal selected bits on input stream with
     * support of phy tag.
     */
    class SWIFI_API bit_stealer : 
      virtual public dynamic_rate_block
      // virtual public block  // used for old implementation
    {
    public:
      typedef boost::shared_ptr<bit_stealer> sptr;

      /*!
       * \brief Create an instance.
       * \param phy_tag_key Key of PHY tag.
       */
      static sptr make(const std::string &phy_tag_key);

      /*!
       * \brief Create an instance.
       * \param rate Wi-Fi OFDM rate.
       */
      static sptr make(int rate);

    };

  } /* namespace swifi */
} /* namespace gr */

#endif /* INCLUDED_SWIFI_BIT_STEALER_H */
