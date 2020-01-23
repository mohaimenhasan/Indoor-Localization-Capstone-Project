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


#ifndef INCLUDED_SWIFI_OFDM_EQUALIZER_H
#define INCLUDED_SWIFI_OFDM_EQUALIZER_H

#include <swifi/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/digital/constellation.h>
#include <map>

namespace gr {
  namespace swifi {

    class SWIFI_API ofdm_equalizer : virtual public sync_block
    {
    public:

      static std::map<int, std::string> EQUALIZER_NAME;

      enum EqualizeMethod {
        EQUALIZE_STATIC,  // assume static channel over packet period
        EQUALIZE_PPT, // pilot phase tracking with constant channel amplitude
        EQUALIZE_STA, // spectral temporal averaging
        EQUALIZE_DD, // decision-directed: demap and remap to track channel
        EQUALIZE_LI, // linear interpolation based on pilot channel states
        EQUALIZE_LPI, // low-pass interpolation based on pilot channel states
        EQUALIZE_PPT_LPI, // select best among PPT and LPI
        EQUALIZE_PPT_DD, // combine PPT and DD
        EQUALIZE_LPI2 = 98,
        EQUALIZE_TEST = 99,
      };

      typedef boost::shared_ptr<ofdm_equalizer> sptr;

      static sptr make(const std::string& phy_tag_key,
        bool propagate_channel_states,
        const std::string &channel_states_tag_key,
        digital::constellation_sptr header_constellation = digital::constellation_sptr());

      virtual void enable_foc(bool enabled) = 0;
      virtual void set_equalization_method(int method) = 0;
      virtual void set_equalization_method(const std::string &method) = 0;
      virtual void set_alpha(float alpha) = 0;
      virtual void set_beta(float beta) = 0;
      virtual void set_debug_file(const std::string &filename) = 0;

    };

  }  // namespace swifi
}  // namespace gr

#endif /* INCLUDED_SWIFI_OFDM_EQUALIZER_H*/

