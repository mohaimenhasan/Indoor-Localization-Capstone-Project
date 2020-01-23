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

#include <swifi/wifi_conv_1_2.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>




namespace gr {
  namespace swifi {

    wifi_conv_1_2::sptr wifi_conv_1_2::make()
    {
      return wifi_conv_1_2::sptr(new wifi_conv_1_2());
    }

    // number of states in Wi-Fi convolutional code
    static const char NUM_STATES = 64;

    // number of inputs of the code
    static const char NUM_INPUTS = 2;

    // number of outputs of the code
    static const char NUM_OUTPUTS = 4;

    // infinity
    static const int INFTY = 999999;

    wifi_conv_1_2::wifi_conv_1_2()
      : d_encode_state(0), d_decode_state(0)
    {
      d_outputA = new char[NUM_STATES * NUM_INPUTS];
      d_outputB = new char[NUM_STATES * NUM_INPUTS];
      d_nextState = new int[NUM_STATES * NUM_INPUTS];
      d_inByStatePair = new int[NUM_STATES * NUM_STATES];
      for (int s = 0; s < NUM_STATES; s++) {
        for (int i = 0; i < NUM_INPUTS; i++) {
          // shift registers are numbered 0 to 5 from left to right (Figure 18-8)
          // and new input enters from left
          d_outputA[s*NUM_INPUTS+i] = (i ^ (s >> 1) ^ (s >> 2) ^ (s >> 4) ^ (s >> 5)) & 1;
          d_outputB[s*NUM_INPUTS+i] = (i ^ (s) ^ (s >> 1) ^ (s >> 2) ^ (s >> 5)) & 1;
          d_nextState[s*NUM_INPUTS+i] = ((s << 1) | i) & 0b111111;
          d_inByStatePair[s * NUM_STATES + d_nextState[s*NUM_INPUTS+i]] = i;
        }
      }
    }

    wifi_conv_1_2::~wifi_conv_1_2() {
      delete[] d_outputA;
      delete[] d_outputB;
      delete[] d_nextState;
      delete[] d_inByStatePair;
    }

    void wifi_conv_1_2::set_encode_state(int encode_state) {
      d_encode_state = encode_state;
    }

    int wifi_conv_1_2::get_encode_state() {
      return d_encode_state;
    }

    void wifi_conv_1_2::set_decode_state(int decode_state) {
      d_decode_state = decode_state;
    }

    int wifi_conv_1_2::get_decode_state() {
      return d_decode_state;
    }

    void wifi_conv_1_2::encode(char *out, const char *in, int in_len,
      int initial_state)
    {
      char *outputA = d_outputA;
      char *outputB = d_outputB;
      const char *in_end = in + in_len;
      int *nextState = d_nextState;
      int s = d_encode_state;
      int j;

      if (initial_state != STATE_NOT_SPECIFIED)
        s = initial_state;

      for (; in != in_end; in++) {
        j = s * NUM_INPUTS + (*in);
        *out++ = outputA[j];  // caller must ensure out buffer has enough space
        *out++ = outputB[j];
        s = nextState[j];
      }
      d_encode_state = s;
    }

    std::vector<char> wifi_conv_1_2::encode(const std::vector<char> &input,
      int initial_state)
    {
      std::vector<char> result(input.size() * 2);
      encode(result.data(), input.data(), input.size(), initial_state);
      return result;
    }

    void wifi_conv_1_2::decode(char *out, const char *in, int in_len,
      int initial_state, int end_state, const std::vector<int> &puncture_map)
    {
      // accumulative distance corresponding to each state on path tracking
      int stateDistance[NUM_STATES * 2];  // use 2 copies for current and previous records
      int *D = stateDistance;
      int *newD = &stateDistance[NUM_STATES];
      int *tmpD;
      int trace[NUM_STATES * in_len];
      int s, j, i, nextS, minS;
      int k, d;
      const int *punc_map = puncture_map.data();
      int punc_map_size = puncture_map.size();
      int mask_A, mask_B;

      // initialize state distances
      if (initial_state == STATE_NOT_SPECIFIED) {
        memset(D, 0, sizeof(*D) * NUM_STATES);
      }
      else {
        for (s = 0; s < NUM_STATES; s++) D[s] = INFTY;
        D[initial_state] = 0;
      }

      for (s = 0; s < NUM_STATES; s++) newD[s] = INFTY;

      // compute and update state distances
      for (k = 0; k < in_len; k += 2) {
        mask_A = punc_map[k % punc_map_size];
        mask_B = punc_map[(k+1) % punc_map_size];
        for (s = 0; s < NUM_STATES; s++) {
          j = s * NUM_INPUTS;
          for (i = 0; i < NUM_INPUTS; i++, j++) {
            d = D[s] + ((d_outputA[j] ^ in[k]) & mask_A) + ((d_outputB[j] ^ in[k+1]) & mask_B);  // Hamming distance
            nextS = d_nextState[j]; // next state

            // for some reason, using >= results in less error probability than using >
            if (newD[nextS] >= d) {
              newD[nextS] = d; // update Hamming distance
              trace[k * NUM_STATES + nextS] = s; // store current state as best candidate
              // printf("k %d s %d D[%d] %d A %d B %d in0 %d in1 %d nextS %d newD[%d] %d d %d trace[%d] %d\n",
              //   k, s, s, D[s],
              //   d_outputA[j], d_outputB[j], in[k], in[k+1],
              //   nextS, nextS, newD[nextS], d,
              //   k * NUM_STATES + nextS,
              //   s);
            }
          }
        }

        tmpD = newD; newD = D; D = tmpD;  // swap pointer
        for (s = 0; s < NUM_STATES; s++) newD[s] = INFTY; // reset newD
      }

      // determine final state
      if (end_state != STATE_NOT_SPECIFIED) {
        minS = end_state;
      }
      else {
        // find final state with minimum distance
        minS = 0;
        d = INFTY;
        for (s = 0; s < NUM_STATES; s++) {
          if (D[s] < d) {
            d = D[s];
            minS = s;
          }
        }
      }

      // trace back
      for (k = in_len - 2; k >= 0; k -= 2) {
        j = k * NUM_STATES + minS;
        out[k/2] = d_inByStatePair[trace[j] * NUM_STATES + minS];
        minS = trace[j];
      }
    }

    std::vector<char> wifi_conv_1_2::decode(const std::vector<char> &input,
      int initial_state, int end_state, const std::vector<int> &puncture_map)
    {
      std::vector<char> result(input.size() / 2);
      decode(result.data(), input.data(), input.size(),
        initial_state, end_state, puncture_map);
      return result;
    }




    void wifi_conv_1_2::soft_decode(char *out, const float *in, int in_len,
      int initial_state, int end_state)
    {
      // accumulative distance corresponding to each state on path tracking
      float stateDistance[NUM_STATES * 2];  // use 2 copies for current and previous records
      float *D = stateDistance;
      float *newD = &stateDistance[NUM_STATES];
      float *tmpD;
      float d;
      int trace[NUM_STATES * in_len];
      int s, j, i, nextS, minS;
      int k;

      // initialize state distances
      if (initial_state == STATE_NOT_SPECIFIED) {
        memset(D, 0, sizeof(*D) * NUM_STATES);
      }
      else {
        for (s = 0; s < NUM_STATES; s++) D[s] = INFTY;
        D[initial_state] = 0;
      }

      for (s = 0; s < NUM_STATES; s++) newD[s] = INFTY;

      // compute and update state distances
      for (k = 0; k < in_len; k += 2) {
        for (s = 0; s < NUM_STATES; s++) {
          j = s * NUM_INPUTS;
          for (i = 0; i < NUM_INPUTS; i++, j++) {
            // d = D[s] + std::min(std::min(in[k * 2 + d_outputA[j]], 0.3f) + std::min(in[(k+1) * 2 + d_outputB[j]], 0.3f), 0.5f);
            d = D[s] + std::min(in[k * 2 + d_outputA[j]] + in[(k+1) * 2 + d_outputB[j]], 0.5f);
            // d = D[s] + std::min(sqrt(in[k * 2 + d_outputA[j]]) + sqrt(in[(k+1) * 2 + d_outputB[j]]), 1.0);
            // d = D[s] + sqrt(in[k * 2 + d_outputA[j]]) + sqrt(in[(k+1) * 2 + d_outputB[j]]);
            // d = D[s] + sqrt(std::min(in[k * 2 + d_outputA[j]], 2.0f)) + sqrt(std::min(in[(k+1) * 2 + d_outputB[j]], 2.0f));
            // d = D[s] + std::min(in[k * 2 + d_outputA[j]], 4.0f) + std::min(in[(k+1) * 2 + d_outputB[j]], 4.0f);
            // d = D[s] + in[k * 2 + d_outputA[j]] + in[(k+1) * 2 + d_outputB[j]];
            nextS = d_nextState[j]; // next state

            // for some reason, using >= results in less error probability than using >
            if (newD[nextS] >= d) {
              newD[nextS] = d; // update distance
              trace[k * NUM_STATES + nextS] = s; // store current state as best candidate
              // printf("k %d s %d D[%d] %d A %d B %d in0 %d in1 %d nextS %d newD[%d] %d d %d trace[%d] %d\n",
              //   k, s, s, D[s],
              //   d_outputA[j], d_outputB[j], in[k], in[k+1],
              //   nextS, nextS, newD[nextS], d,
              //   k * NUM_STATES + nextS,
              //   s);
            }
          }
        }

        tmpD = newD; newD = D; D = tmpD;  // swap pointer
        for (s = 0; s < NUM_STATES; s++) newD[s] = INFTY; // reset newD
      }

      // determine final state
      if (end_state != STATE_NOT_SPECIFIED) {
        minS = end_state;
      }
      else {
        // find final state with minimum distance
        minS = 0;
        d = INFTY;
        for (s = 0; s < NUM_STATES; s++) {
          if (D[s] < d) {
            d = D[s];
            minS = s;
          }
        }
      }

      // trace back
      for (k = in_len - 2; k >= 0; k -= 2) {
        j = k * NUM_STATES + minS;
        out[k/2] = d_inByStatePair[trace[j] * NUM_STATES + minS];
        minS = trace[j];
      }
    }



    static std::vector<int> initialize_no_puncture_map() {
      std::vector<int> m;
      m.push_back(1);
      return m;
    }

    const std::vector<int> wifi_conv_1_2::NO_PUNCTURE_MAP = initialize_no_puncture_map();

  } /* namespace swifi */
} /* namespace gr */

