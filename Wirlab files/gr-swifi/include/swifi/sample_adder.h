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


#ifndef INCLUDED_SWIFI_SAMPLE_ADDER_H
#define INCLUDED_SWIFI_SAMPLE_ADDER_H

#include <swifi/api.h>
#include <gnuradio/block.h>
#include <uhd/types/time_spec.hpp>

namespace gr {
  namespace swifi {

    /*!
     * \brief Add zero samples to front and tail of each frame.
     * \ingroup swifi
     *
     */
    class SWIFI_API sample_adder : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<sample_adder> sptr;

      /*!
       * \brief Create a new instance of swifi::sample_adder
       *
       * \param packet_length length in bytes of payload (must be same as packet length indicated in MAC Frame Generator block)
       * \param rate encoding rate of PHY frame
       * \param num_zeros number of zero samples to add to front and tail
       */
      static sptr make(const uint64_t packet_length,
                      const uint8_t rate,
                      const int num_zeros);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_SAMPLE_ADDER_H */

