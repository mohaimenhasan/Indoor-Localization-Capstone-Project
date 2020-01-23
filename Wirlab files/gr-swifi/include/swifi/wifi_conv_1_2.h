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


#ifndef INCLUDED_SWIFI_WIFI_CONV_1_2_H
#define INCLUDED_SWIFI_WIFI_CONV_1_2_H

#include <swifi/api.h>
#include <vector>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

namespace gr {
  namespace swifi {

    /*!
     * \brief Encoder and Decoder of Wi-Fi convolutional code of coding rate 1/2.
     * \details This classes provides methods for encoding and decoding
     * independently, i.e., the encoding state and decoding state are
     * maintained separately. This allows caller to use the same object
     * to encode and decode two different streams at the same time without
     * worrying about the changing states during encoding/decoding.
     */
    class SWIFI_API wifi_conv_1_2 {
    public:
      typedef boost::shared_ptr<wifi_conv_1_2> sptr;
      static const char STATE_NOT_SPECIFIED = 64;
      static const std::vector<int> NO_PUNCTURE_MAP;

      static sptr make();

      wifi_conv_1_2();
      ~wifi_conv_1_2();

      /*!
       * \brief Set encoding state.
       * \details Note that encoding state is maintained separately from decoding state.
       * \param encode_state new encoding state.
       */
      void set_encode_state(int encode_state);

      /*!
       * \brief Return the current encoding state.
       */
      int get_encode_state();

      /*!
       * \brief Set decoding state.
       * \details Note that decoding state is maintained separately from encoding state.
       * \param decode_state new decoding state.
       */
      void set_decode_state(int decode_state);

      /*!
       * \brief Return the current decoding state.
       */
      int get_decode_state();

      /*!
       * Encode the input bits and produced encoded output bits.
       * The caller must ensure the output buffer size is double
       * the input buffer size.
       * During encoding, the encoding state is maintained properly,
       * and is SEPARATE from the decoding state.
       *
       * \param output pointer to the output buffer, whose size must be 2*in_len
       * \param input pointer to input bits
       * \param in_len size of the input buffer
       * \param initial_state initial state of the encoder.
       */
      void encode(char *output, const char *input, int in_len,
        int initial_state = STATE_NOT_SPECIFIED);

      /*!
       * Same as above, but uses data as a vector of char. This is useful for SWIG/Python.
       * \param input pointer to input bits
       * \param initial_state initial state of the encoder.
       * \return vector of encoded bits
       */
      std::vector<char> encode(const std::vector<char> &input,
        int initial_state = STATE_NOT_SPECIFIED);


      /*!
       * Decode the input bits and produced decoded output bits.
       * The caller must ensure the output buffer is enough to
       * contain the result.
       * During decoding, the decoding state is maintained properly,
       * and is SEPARATE from the encoding state.
       *
       * \param output pointer to the output buffer, whose size should be in_len/2
       * \param input pointer to input bits
       * \param in_len size of the input buffer
       * \param initial_state initial state of the encoder.
       * \param end_state end state of the decoder. If not specified, the 
       *                  minimum distance state will be selected.
       * \param puncture_map puncture map.
       */
      void decode(char *output, const char *input, int in_len,
        int initial_state = STATE_NOT_SPECIFIED,
        int end_state = STATE_NOT_SPECIFIED,
        const std::vector<int> &puncture_map = NO_PUNCTURE_MAP);

      /*!
       * Same as above, but uses data as a vector of char. This is useful for SWIG/Python.
       * \param input pointer to input bits
       * \param initial_state initial state of the decoder.
       * \param end_state end state of the decoder. If not specified, the 
       *                  minimum distance state will be selected.
       * \param puncture_map puncture map.
       * \return vector of decoded bits
       */
      std::vector<char> decode(const std::vector<char> &input,
        int initial_state = STATE_NOT_SPECIFIED,
        int end_state = STATE_NOT_SPECIFIED,
        const std::vector<int> &puncture_map = NO_PUNCTURE_MAP);


      /*!
       * Soft-decision Decode the input bits and produced decoded output bits.
       * The caller must ensure the output buffer is enough to
       * contain the result.
       * During decoding, the decoding state is maintained properly,
       * and is SEPARATE from the encoding state.
       *
       * \param output pointer to the output buffer, whose size should be in_len/2
       * \param metrics pointer to input metrics
       * \param in_len number of metric values (a metric value contains 2 floats) on input
       * \param initial_state initial state of the encoder.
       * \param end_state end state of the decoder. If not specified, the 
       *                  minimum distance state will be selected.
       */
      void soft_decode(char *output, const float *metrics, int in_len,
        int initial_state = STATE_NOT_SPECIFIED,
        int end_state = STATE_NOT_SPECIFIED);


    private:
      char *d_outputA;  // d_outputA[s*NUM_STATES+i] is output A of state s and input i
      char *d_outputB;  // d_outputB[s*NUM_STATES+i] is output B of state s and input i
      int *d_nextState; // d_nextState[s*NUM_STATES+i] is next state of state s and input i
      int *d_inByStatePair; // d_inByStatePair[s1*NUM_STATES+s2] is input by which state s1 moves to state s2

      // for encoding
      int d_encode_state;

      // for decoding
      int d_decode_state;
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_WIFI_CONV_1_2_H */

