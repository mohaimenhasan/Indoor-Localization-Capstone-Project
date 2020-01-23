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


#ifndef INCLUDED_SWIFI_OFDM_FRAME_SYNC_H
#define INCLUDED_SWIFI_OFDM_FRAME_SYNC_H

#include <swifi/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace swifi {

    /*!
     * \brief OFDM Frame synchronization.
     *
     * \details We use state machine for frame synchronization:
     * 1. First, we are in SHORT_SEARCH state, where we look for
     *    two repeated short symbols (16 samples)
     *    by Schmidt-Cox method: computing the correlation between them
     *    and comparing to a threshold.
     * 2. When the threshold is exceeded, we move to LONG_SEARCH state,
     *    in which we correct the frequency offset
     *    for the incoming samples with the coarse estimated value
     *    obtained in SHORT_SEARCH. We start searching for two repeated long
     *    symbols (64 symbols). Again, Schmidt-Cox mechanism is applied.
     * 3. When the threshold is exceeded, we compute the correlation of
     *    these long symbols with the predefined long preamble sequence.
     *    If this exceeds a threshold, we believe that this is the start
     *    of the new frame and move to SYNC state.
     * 4. In SYNC state, we modify the signal with fine estimated value
     *    of frequency offset and send samples to the next block for processing.
     *    During SYNC state, if we see a drop in energy, we think that
     *    the frame has ended, then we go back to SHORT SEARCH state.
     *
     */
    class SWIFI_API ofdm_frame_sync : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<ofdm_frame_sync> sptr;

      /*!
       * \brief Create an instance of ofdm_frame_sync.
       * \param short_threshold threshold for short symbols.
       * \param min_short_plateau_len Minimum plateau length for short symbols.
       * \param long_threshold threshold for long symbols.
       * \param avg_alpha moving average coefficient.
       * \param drop_ratio_threshold ratio between current signal power an
       *        preamble power, below which the frame is thought to be ended.
       * \param trigger_tag_key Key of trigger tag.
       */
      static sptr make(float short_threshold, int min_short_plateau_len,
        float long_threshold, float avg_alpha, float drop_ratio_threshold,
        const std::string &trigger_tag_key = "trigger");

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_OFDM_FRAME_SYNC_H */

