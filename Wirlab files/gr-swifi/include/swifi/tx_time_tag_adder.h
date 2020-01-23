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



#ifndef INCLUDED_SWIFI_TX_TIME_TAG_ADDER_H
#define INCLUDED_SWIFI_TX_TIME_TAG_ADDER_H

#include <swifi/api.h>
#include <gnuradio/sync_block.h>
#include <uhd/types/time_spec.hpp>

namespace gr {
  namespace swifi {

    /*!
     * \brief Unpack every input byte into a number of invidual bits
     * on output in LSB order (lowest-order bit transmitted first).
     * Only 1, 2, 4, 6, 8 are supported.
     * \ingroup swifi
     *
     */
    class SWIFI_API tx_time_tag_adder : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<tx_time_tag_adder> sptr;

      /*!
       * \brief Create a new instance of swifi::tx_time_tag_adder
       *
       * \param full_secs seconds count of tx_time of the burst
       * \param frac_secs fractional seconds count of tx_time of the burst
       * \param length_tag_key length tag key of the burst
       * \param length length of the burst
       */
      static sptr make(const uint64_t full_secs,
                      const double frac_secs,
                      const std::string &length_tag_key,
                      const long length);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_TX_TIME_TAG_ADDER_H */

