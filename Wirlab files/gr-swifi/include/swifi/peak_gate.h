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


#ifndef INCLUDED_SWIFI_PEAK_GATE_H
#define INCLUDED_SWIFI_PEAK_GATE_H

#include <swifi/api.h>
#include <gnuradio/sync_block.h>
#include <vector>

namespace gr {
  namespace swifi {

    /*!
     * Input stream should contain peak/non-peak values, which
     * usually are the correlation results.
     * If the number of peaks (whose values greater than a threshold)
     * reaches a given value, this block outputs 1, otherwise outputs 0.
     * \ingroup swifi
     *
     */
    class SWIFI_API peak_gate : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<peak_gate> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of swifi::peak_gate.
       *
       * \param threshold If an input item passes over this threshold, it adds one more count.
       * \param num_peaks If the number of peaks reaches this value, the block outputs 1, otherwise 0.
       * \param min_distance Required min. distance between two consecutive peaks.
       *                     If the current peak is too close to the previous peak, it is not counted.
       * \param max_distance Required max. distance between two consecutive peaks.
       *                     If the current peak is too far to the previous peak,
       *                     peak count is restarted from 1 (i.e., starting from the current peak).
       */
      static sptr make(float threshold, int num_peaks,
        int min_distance, int max_distance);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_PEAK_GATE_H */

