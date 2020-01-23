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


#ifndef INCLUDED_SWIFI_CONSTELLATION_METRICS_H
#define INCLUDED_SWIFI_CONSTELLATION_METRICS_H

#include <swifi/api.h>
#include <gnuradio/gr_complex.h>
#include <boost/shared_ptr.hpp>

namespace gr {
  namespace swifi {

    /*!
     * \brief Constellation metrics interface.
     */
    class SWIFI_API constellation_metrics
    {
     public:

      typedef boost::shared_ptr<constellation_metrics> sptr;


      /*!
       * \brief Compute metrics of a given complex sample with respect to
       * each individual bit. The output metrics is an array of 2k float values,
       * corresponding to k bits determined by the constellation, e.g., k=2 for QPSK,
       * k=3 for 8-PSK, k=4 for 16-QAM, k=6 for 64-QAM, etc.
       * For every pair of floats, the first float indicates the metrics
       * corresponding to value 0 of the bit, and the second float indicates the
       * metrics corresponding to value 1 of the bit.
       *
       * Example 1: For BPSK constellation, the output contains 2 floats, where
       * the first float is the computed metrics corresponding to 0, while the
       * second is corresponding to 1.
       *
       * Example 2: For QPSK constellation, the output contains 4 floats in the
       * following order: m00, m01, m10, m11, where m_ij indicates the metrics
       * corresponding to the value j of the i-th bit.
       *
       * Example 3: For 8-PSK constellation, the output contains 6 floats in the
       * following order: m00, m01, m10, m11, m20, m21, where m_ij indicates the metrics
       * corresponding to the value j of the i-th bit.
       */
      virtual void bitmetrics(const gr_complex *sample, float *metrics) = 0;

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_CONSTELLATION_METRICS_H */

