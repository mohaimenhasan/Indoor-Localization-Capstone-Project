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

#include <swifi/wifi_constellation.h>
#include <gnuradio/math.h>
#include <gnuradio/gr_complex.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

#define SQRT_TWO 0.707107

    wifi_constellation_bpsk::sptr wifi_constellation_bpsk::make() {
      return wifi_constellation_bpsk::sptr(new wifi_constellation_bpsk());
    }

    wifi_constellation_bpsk::wifi_constellation_bpsk() {
      d_constellation.resize(2);
      d_constellation[0] = gr_complex(-1, 0);
      d_constellation[1] = gr_complex(1, 0);
      d_dimensionality = 1;
      d_rotational_symmetry = 2;
      calc_arity();
    }

    wifi_constellation_bpsk::~wifi_constellation_bpsk() {
    }

    unsigned int wifi_constellation_bpsk::decision_maker(const gr_complex *sample) {
      return sample->real() > 0;
    }

    void wifi_constellation_bpsk::bitmetrics(const gr_complex *sample, float *metrics) {
      metrics[0] = std::norm(d_constellation[0] - *sample);
      metrics[1] = std::norm(d_constellation[1] - *sample);
    }


    /********************************************************************/

    wifi_constellation_qpsk::sptr wifi_constellation_qpsk::make() {
      return wifi_constellation_qpsk::sptr(new wifi_constellation_qpsk());
    }

    wifi_constellation_qpsk::wifi_constellation_qpsk() {
      d_constellation.resize(4);
      d_constellation[0] = gr_complex(-SQRT_TWO, -SQRT_TWO);
      d_constellation[1] = gr_complex(SQRT_TWO, -SQRT_TWO);
      d_constellation[2] = gr_complex(-SQRT_TWO, SQRT_TWO);
      d_constellation[3] = gr_complex(SQRT_TWO, SQRT_TWO);
      d_dimensionality = 1;
      d_rotational_symmetry = 4;
      calc_arity();
    }

    wifi_constellation_qpsk::~wifi_constellation_qpsk() {
    }

    unsigned int wifi_constellation_qpsk::decision_maker(const gr_complex *sample) {
      return sample->real() > 0 | ((sample->imag() > 0) << 1);
    }

    void wifi_constellation_qpsk::bitmetrics(const gr_complex *sample, float *metrics) {
      // the metrics computation is based on the constellation defined in IEEE 802.11/Figure 18-10.
      unsigned int m;

      m = (sample->imag() > 0) << 1;
      metrics[0] = std::norm(d_constellation[m] - *sample);
      metrics[1] = std::norm(d_constellation[m+1] - *sample);
      m = (sample->real() > 0);
      metrics[2] = std::norm(d_constellation[m] - *sample);
      metrics[3] = std::norm(d_constellation[m+2] - *sample);
    }

    /********************************************************************/

    // For creating QAM constellations, the bit - real/imag mappings
    // from IEEE 802.11a are used
    // (see Figure 18.10 and Table 18.8, Table 18.9, Table 18.10, Table 18.11)
    int qam16_2bits_mapping[4] = {-3, 3, -1, 1};
    int qam64_3bits_mapping[8] = {-7, 7, -1, 1, -5, 5, -3, 3};

    wifi_constellation_16qam::sptr 
    wifi_constellation_16qam::make()
    {
      return wifi_constellation_16qam::sptr(new wifi_constellation_16qam());
    }

    wifi_constellation_16qam::wifi_constellation_16qam()
    {
      d_constellation.resize(16);
      for (int i = 0; i < 16; i++) {
        // create each point with scaling factor
        d_constellation[i] = gr_complex(
          1.0 / sqrt(10) * qam16_2bits_mapping[i & 3],
          1.0 / sqrt(10) * qam16_2bits_mapping[i >> 2]);
      }
      d_midvalue = 2.0 / sqrt(10);
      d_dimensionality = 1;
      d_rotational_symmetry = 4;
      calc_arity();
    }

    wifi_constellation_16qam::~wifi_constellation_16qam()
    {
    }

    unsigned int
    wifi_constellation_16qam::decision_maker(const gr_complex *sample)
    {
      int re = int(floor(sample->real() / d_midvalue)) + 2;
      int im = int(floor(sample->imag() / d_midvalue)) + 2;

      if (re <= 0) re = 0;
      else if (re >= 3) re = 1;
      else re++;

      if (im <= 0) im = 0;
      else if (im >= 3) im = 1;
      else im++;

      return (im << 2) | re;
    }


    void wifi_constellation_16qam::bitmetrics(const gr_complex *sample, float *metrics) {
      // the metrics computation is based on the constellation defined in IEEE 802.11/Figure 18-10.
      unsigned int a = decision_maker(sample);
      unsigned int m[8]; // 8 floats corresponding to 4 bits of 16-QAM

      // bit 0: compute m00 and m01
      if (a & 1) {
        m[0] = (a & 0b1110) | 0b0010;  // m00
        m[1] = a; // m01
      }
      else {
        m[0] = a;  // m00
        m[1] = a | 0b0011;  // m01
      }

      // bit 1: compute m10 and m11
      if (a & 2) {
        m[2] = a & 0b1101;  // m10
        m[3] = a; // m11
      }
      else {
        m[2] = a; // m10
        m[3] = a | 0b0010;  // m11
      }

      // bit 2: compute m20 and m21
      if (a & 4) {
        m[4] = (a & 0b1011) | 0b1000; // m20
        m[5] = a; // m21
      }
      else {
        m[4] = a; // m20
        m[5] = a & 0b1100;  // m21
      }

      // bit 3: compute m30 and m31
      if (a & 8) {
        m[6] = a & 0b0111;  // m30
        m[7] = a; // m31
      }
      else {
        m[6] = a; // m30
        m[7] = a | 0b1000;  // m31
      }

      // compute distances to closest points
      for (int i = 0; i < 8; i++)
        metrics[i] = std::norm(d_constellation[m[i]] - *sample);
    }

    /********************************************************************/


    wifi_constellation_64qam::sptr 
    wifi_constellation_64qam::make()
    {
      return wifi_constellation_64qam::sptr(new wifi_constellation_64qam());
    }

    wifi_constellation_64qam::wifi_constellation_64qam()
    {
      d_constellation.resize(64);
      for (int i = 0; i < 64; i++) {
        // create each point with scaling factor
        d_constellation[i] = gr_complex(
          1.0 / sqrt(42) * qam64_3bits_mapping[i & 7],
          1.0 / sqrt(42) * qam64_3bits_mapping[i >> 3]);
      }
      d_midvalue = 2.0 / sqrt(42);
      d_dimensionality = 1;
      d_rotational_symmetry = 4;

      char regular_to_gray[] = { 0, 4, 6, 2, 3, 7, 5, 1}; 
      d_regular_to_gray = new char[8];  // use for decoding
      memcpy(d_regular_to_gray, regular_to_gray, 8);

      calc_arity();
    }

    wifi_constellation_64qam::~wifi_constellation_64qam()
    {
      delete[] d_regular_to_gray;
    }

    unsigned int
    wifi_constellation_64qam::decision_maker(const gr_complex *sample)
    {
      int re = int(floor(sample->real() / d_midvalue)) + 4;
      int im = int(floor(sample->imag() / d_midvalue)) + 4;

      if (re <= 0) re = 0;
      else if (re >= 7) re = 7;

      if (im <= 0) im = 0;
      else if (im >= 7) im = 7;

      return (d_regular_to_gray[im] << 3) | d_regular_to_gray[re];
    }

    void wifi_constellation_64qam::bitmetrics(const gr_complex *sample, float *metrics) {
      // the metrics computation is based on the constellation defined in IEEE 802.11/Figure 18-10.

      unsigned int a = decision_maker(sample);
      unsigned int m[12]; // 12 floats corresponding to 6 bits of 64-QAM

      // bit 0: compute m00 and m01
      if (a & 1) {
        m[0] = (a & 0b111010) | 0b000010;  // m00: set 3 lower bits to 010 (LSB)
        m[1] = a; // m01
      }
      else {
        m[0] = a;  // m00
        m[1] = (a & 0b111011) | 0b000011;  // m01: set 3 lower bits to 110 (LSB)
      }

      // bit 1: compute m10 and m11
      if (a & 2) {
        m[2] = (a & 0b111101) | 0b000100;  // m10: set bits 1 and 2 to 01 (LSB)
        m[3] = a; // m11
      }
      else {
        m[2] = a; // m10
        m[3] = a | 0b000110;  // m11: set bits 1 and 2 to 11 (LSB)
      }

      // bit 2: compute m20 and m21
      if (a & 4) {
        m[4] = a & 0b111011; // m20: set bit 2 to 0
        m[5] = a; // m21
      }
      else {
        m[4] = a; // m20
        m[5] = a | 0b000100;  // m21: set bit 2 to 1
      }

      // bit 3: compute m30 and m31
      if (a & 8) {
        m[6] = (a & 0b010111) | 0b010000;  // m30: set 3 higher bits to 010 (LSB)
        m[7] = a; // m31
      }
      else {
        m[6] = a; // m30
        m[7] = (a & 0b011111) | 0b011000;  // m31: set 3 higher bits to 110 (LSB)
      }

      // bit 4: compute m40 and m41
      if (a & 16) {
        m[8] = (a & 0b101111) | 0b100000; // m40: set bit 4 and 5 to 01 (LSB)
        m[9] = a; // m41
      }
      else {
        m[8] = a; // m40
        m[9] = a | 0b110000;  // m41: set bit 4 and 5 to 11 (LSB)
      }

      // bit 5: compute m50 and m51
      if (a & 32) {
        m[10] = a & 0b011111; // m50: set bit 5 to 0
        m[11] = a;  // m51
      }
      else {
        m[10] = a;  // m51
        m[11] = a | 0b100000;  // m50: set bit 5 to 1
      }

      // compute distances to closest points
      for (int i = 0; i < 12; i++)
        metrics[i] = std::norm(d_constellation[m[i]] - *sample);
    }



    /********************************************************************/




  } /* namespace digital */
} /* namespace gr */
