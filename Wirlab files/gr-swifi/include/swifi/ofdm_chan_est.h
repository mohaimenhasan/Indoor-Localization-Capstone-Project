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


#ifndef INCLUDED_SWIFI_OFDM_CHAN_EST_H
#define INCLUDED_SWIFI_OFDM_CHAN_EST_H

#include <swifi/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace swifi {

    class SWIFI_API ofdm_chan_est : virtual public block
    {
    public:
      enum EstimationMethod {
        ESTIMATE_LS,
        ESTIMATE_LS_FOC,
        ESTIMATE_LPI
      };

      typedef boost::shared_ptr<ofdm_chan_est> sptr;
      static sptr make(int fft_len,
        const std::vector<std::vector<gr_complex> > &sync_ofdm_symbols,
        float alpha,
        const std::string &channel_states_tag_key = "channel_states",
        const std::string &len_tag_key = "length",
        int total_ofdm_symbols = 0,
        bool copy_sync_symbols = false);

      virtual void set_estimation_method(int method) = 0;
    };

  }  // namespace swifi
}  // namespace gr

#endif /* INCLUDED_SWIFI_OFDM_CHAN_EST_H */

