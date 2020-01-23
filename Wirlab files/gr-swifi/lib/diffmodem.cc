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

#include "diffmodem.h"
#include <gnuradio/expj.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    diffmodem::diffmodem(float phase_bw, float freq_max, float freq_min)
      : gr::blocks::control_loop(phase_bw, freq_max, freq_min),
        d_last_sample(gr_complex(1, 0))
    {
      // set default modulation
      change_modulation(MODULATION_DBPSK);
    }

    diffmodem::~diffmodem()
    {
    }

    void diffmodem::change_modulation(int modulation) {
      switch (modulation) {
        case MODULATION_DBPSK:
          demod = &diffmodem::demodulate_dbpsk;
          break;
        case MODULATION_DQPSK:
          demod = &diffmodem::demodulate_dqpsk;
          break;
      }
    }

    char diffmodem::demodulate(const gr_complex &sample) {
      return (this->*demod)(sample);
    }

#if NON_COHERENT_DETECTION
    char diffmodem::demodulate_dbpsk(const gr_complex &sample) {
      gr_complex diff = sample * conj(d_last_sample);
      d_last_sample = sample;
      return diff.real() < 0;
    }

    char diffmodem::demodulate_dqpsk(const gr_complex &sample) {
      gr_complex diff = sample * conj(d_last_sample);
      d_last_sample = sample;
      return (bool(diff.real() < diff.imag()) << 1) |
              bool(diff.real() < -diff.imag());
    }
#else
    char diffmodem::demodulate_dbpsk(const gr_complex &sample) {
      char symbol;
      float phase_error;
      gr_complex corrected = sample * gr_expj(d_phase);
      gr_complex diff = corrected * conj(d_last_sample);

      if (diff.real() > 0) {
        symbol = 0;
        phase_error = -arg(diff * conj(gr_complex(1, 0)));
      }
      else {
        symbol = 1;
        phase_error = -arg(diff * conj(gr_complex(-1, 0)));
      }

      // PLL
      advance_loop(phase_error);
      phase_wrap();
      frequency_limit();

      d_last_sample = corrected;

      return symbol;
    }

    char diffmodem::demodulate_dqpsk(const gr_complex &sample) {
      char symbol;
      float phase_error;
      gr_complex corrected = sample * gr_expj(d_phase);
      gr_complex diff = corrected * conj(d_last_sample);

      if (diff.real() < diff.imag()) {
        // 2 or 3
        if (diff.real() < -diff.imag()) {
          symbol = 3;
          phase_error = -arg(diff * conj(gr_complex(-1, 0)));
        }
        else {
          symbol = 2;
          phase_error = -arg(diff * conj(gr_complex(0, 1)));
        }
      }
      else {
        // 0 or 1
        if (diff.real() < -diff.imag()) {
          symbol = 1;
          phase_error = -arg(diff * conj(gr_complex(0, -1)));
        }
        else {
          symbol = 0;
          phase_error = -arg(diff * conj(gr_complex(1, 0)));
        }
      }

      // PLL
      advance_loop(phase_error);
      phase_wrap();
      frequency_limit();

      d_last_sample = corrected;

      return symbol;
    }
#endif

  } /* namespace swifi */
} /* namespace gr */

