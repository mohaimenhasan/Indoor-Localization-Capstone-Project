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


#ifndef INCLUDED_SWIFI_DIFFMODEM_H
#define INCLUDED_SWIFI_DIFFMODEM_H

#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/blocks/control_loop.h>

namespace gr {
  namespace swifi {

#define NON_COHERENT_DETECTION  0

    class diffmodem : public gr::blocks::control_loop
    {
     public:
      enum MODULATION_TYPE { MODULATION_DBPSK, MODULATION_DQPSK };

      diffmodem(float phase_bw = 2*M_PI/100, float freq_max = 0.25, float freq_min = -0.25);
      virtual ~diffmodem();

      /*!
       * \brief Demodulate complex sample to symbol.
       *
       * \param sample complex sample
       */
      virtual char demodulate(const gr_complex &sample);

      /*!
       * \brief Change modulation.
       * \param modulation modulation to change.
       */
      virtual void change_modulation(int modulation);

      /*!
       * \brief DBPSK Demodulation.
       * \param sample complex sample
       */
      virtual char demodulate_dbpsk(const gr_complex &sample);

      /*!
       * \brief DQPSK Demodulation.
       * \param sample complex sample
       */
      virtual char demodulate_dqpsk(const gr_complex &sample);

     private:
      gr_complex d_last_sample;
      char (diffmodem::*demod)(const gr_complex &sample);


    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_DIFFMODEM_H */

