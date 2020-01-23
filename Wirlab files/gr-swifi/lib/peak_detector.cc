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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "peak_detector.h"
#include <stdio.h>
#include <string.h>

namespace gr {
  namespace swifi {

    peak_detector::peak_detector(int blocksize, float avg_alpha)
      : d_blocksize(blocksize),
        d_corr_avg_alpha(avg_alpha),
        d_corr_avg_one_minus_alpha(1-avg_alpha),
        d_corr_idx(0),
        d_corr_pwr(NULL),
        d_max_corr_pwr(0),
        d_max_corr(0, 0),
        d_max_corr_idx(0),
        d_locked(false)
    {
      d_corr_pwr = new float[d_blocksize];
      memset(d_corr_pwr, 0, sizeof(float) * d_blocksize);
    }

    peak_detector::~peak_detector()
    {
      if (d_corr_pwr)
        delete[] d_corr_pwr;
    }

    void peak_detector::process(const gr_complex &corr) {
      float pwr;

      // compute power of correlation
      pwr = corr.real() * corr.real() + corr.imag() * corr.imag();

      // averaging correlation for corresponding bin
      d_corr_pwr[d_corr_idx] = d_corr_pwr[d_corr_idx] * d_corr_avg_one_minus_alpha + pwr * d_corr_avg_alpha;

      // select max correlation
      if (!d_locked) {
        if (d_corr_pwr[d_corr_idx] > d_max_corr_pwr) {
          d_max_corr_pwr = d_corr_pwr[d_corr_idx];
          d_max_corr = corr;
          d_max_corr_idx = d_corr_idx;
        }
      }
      else {
        if (d_corr_idx == d_max_corr_idx)
          d_max_corr = corr;
      }

      d_corr_idx++;
    }

    bool peak_detector::get_peak(gr_complex &peak) {
      if (d_corr_idx < d_blocksize)
        return false;

      peak = d_max_corr;
      d_max_corr_pwr = 0;
      d_corr_idx = 0;

      return true;
    }

  } /* namespace swifi */
} /* namespace gr */

