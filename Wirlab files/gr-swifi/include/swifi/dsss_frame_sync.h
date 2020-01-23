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


#ifndef INCLUDED_SWIFI_DSSS_FRAME_SYNC_H
#define INCLUDED_SWIFI_DSSS_FRAME_SYNC_H

#include <swifi/api.h>
#include <gnuradio/block.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace swifi {

    /*!
     * \brief Frame synchronization.
     *
     * \details
     *
     */
    class SWIFI_API dsss_frame_sync : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<dsss_frame_sync> sptr;

      /*!
       * \brief Create an instance of dsss_frame_sync.
       *
       * \param samprate_mhz Sample rate in MHz
       * \param taps Filter taps
       */
      static sptr make(int samprate_mhz, 
                const std::vector<float> &taps = std::vector<float>());

      /*!
       * \brief Set key of reset tag for demodulator.
       * \param demod_reset_tag_key key of reset tag for demodulator
       */
      virtual void set_demod_reset_tag_key(
        const std::string &demod_reset_tag_key = "demod_reset") = 0;

      /*!
       * \brief Set key of reset tag for descrambler.
       * \param descrambler_reset_tag_key key of reset tag for descrambler
       */
      virtual void set_descrambler_reset_tag_key(
        const std::string &descrambler_reset_tag_key = "descrambler_reset") = 0;

      /*!
       * \brief Set control message queue.
       * \param msgq control message queue
       */
      virtual void set_control_message_queue(gr::msg_queue::sptr msgq) = 0;
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_DSSS_FRAME_SYNC_H */

