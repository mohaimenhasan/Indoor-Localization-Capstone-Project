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

#ifndef INCLUDED_SWIFI_PEAK_DETECTOR_H
#define INCLUDED_SWIFI_PEAK_DETECTOR_H

#include <gnuradio/gr_complex.h>
#include <vector>

namespace gr {
  namespace swifi {

    class peak_detector
    {
     public:

      /*!
       * \brief Create an instance with given block size and averaging coefficient.
       * \param blocksize block size
       * \param avg_alpha averaging coefficient
       */
      peak_detector(int blocksize, float avg_alpha = 0.2);

      /*!
       * \brief Virtual destructor.
       */
      virtual ~peak_detector();

      /*!
       * \brief Process the detection.
       *
       * \param sample complex sample
       */
      void process(const gr_complex &sample);

      /*!
       * \brief Return the peak.
       *
       * \param peak store the peak value
       * \return true if a peak was found, false otherwise
       */
      bool get_peak(gr_complex &peak);

      /*!
       * \brief Lock/unlock peak search.
       * \details When peak detector is locked, it always selects the
       * correlation value at the same index as in previous period.
       * In unlocked mode, it detects the peak by compare power at
       * all indices in the same period.
       */
      void lock(bool locked) {
        d_locked = locked;
      }

      /*!
       * \brief Return block size.
       */
      float get_blocksize() const {
        return d_blocksize;
      }

      /*!
       * \brief Set block size.
       * \param blocksize new block size
       */
      void set_blocksize(int blocksize) {
        d_blocksize = blocksize;
      }

      /*!
       * \brief Return running average coefficient.
       */
      float get_avg_alpha() const {
        return d_corr_avg_alpha;
      }

      /*!
       * \brief Set running average coefficient.
       * \param alpha running average coefficient
       */
      void set_avg_alpha(float alpha) {
        d_corr_avg_alpha = alpha;
        d_corr_avg_one_minus_alpha = 1 - d_corr_avg_alpha;
      }

      /*!
       * \brief Return correlation power array.
       */
      std::vector<float> corr_pwr() const {
        return std::vector<float>(d_corr_pwr, d_corr_pwr + d_blocksize);
      }

      /*!
       * \brief Return the current index in the block.
       */
      int get_index() const {
        return d_corr_idx;
      }

      /*!
       * \brief Set the current index in the block.
       * \param index the new index
       */
      void set_corr_index(int index) {
        d_corr_idx = index;
      }

      /*!
       * \brief Return the index corresponding to the maximum correlation.
       */
      int get_max_index() const {
        return d_max_corr_idx;
      }

     private:
      int d_blocksize;
      int d_corr_idx;
      float *d_corr_pwr;
      float d_max_corr_pwr;
      gr_complex d_max_corr;
      int d_max_corr_idx;
      float d_corr_avg_alpha, d_corr_avg_one_minus_alpha;
      bool d_locked;

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_PEAK_DETECTOR_H */

