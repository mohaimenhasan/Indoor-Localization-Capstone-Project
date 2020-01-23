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



#ifndef INCLUDED_SWIFI_FD_NOISE_ADDER_H
#define INCLUDED_SWIFI_FD_NOISE_ADDER_H

#include <swifi/api.h>
#include <gnuradio/sync_block.h>
#include <vector>

namespace gr {
  namespace swifi {

    /*!
     * \brief Vector valve.
     * \ingroup swifi
     *
     */
    class SWIFI_API fd_noise_adder : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<fd_noise_adder> sptr;

      /*!
       * \brief Create a new instance of swifi::fd_noise_adder.
       *
       * \param phy_tag_key Key for PHY tag.
       * \param subcarrier_jamming_pattern A vector of pattern specifiying 
       *            which locations are jammed (value 1), or not jammed (value 0).
       * \param ofdmsym_jamming_pattern A vector of pattern specifiying 
       *            which OFDM symbols are jammed (value 1), or not jammed (value 0).
       * \param ampl Amplitude of noise.
       */
      static sptr make(const std::string &phy_tag_key,
        const std::vector<int> &subcarrier_jamming_pattern,
        const std::vector<int> &ofdmsym_jamming_pattern,
        float ampl);

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_FD_NOISE_ADDER_H */

