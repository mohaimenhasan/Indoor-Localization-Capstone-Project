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


#ifndef INCLUDED_SWIFI_PHY_UTILS_H
#define INCLUDED_SWIFI_PHY_UTILS_H

#include <swifi/api.h>
#include <swifi/wifi_constellation.h>
#include <map>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

namespace gr {
  namespace swifi {

    typedef boost::shared_array<char> c_sharr;

#pragma pack(push, 1)
    struct phy80211_dsss_frame_header {
      uint8_t signal;
      uint8_t service;
      uint16_t length;
      uint16_t crc;
    };

    struct phy80211_frame_header {
      uint8_t rate : 4;
      uint8_t reserved : 1;
      uint16_t length : 12;
      uint8_t parity : 1;
      uint8_t tail : 6;
      uint16_t service : 16;
    };
#pragma pack(pop)


    class SWIFI_API phy_utils {
    public:
      typedef boost::shared_ptr<phy_utils> sptr;

      enum PhyMode {
        MODE_DSSS, MODE_OFDM
      };

      enum PHY_MCS {
        MCS0, MCS1, MCS2, MCS3, MCS4, MCS5, MCS6, MCS7,
                    // R4-R1
        QAM64_2_3,  // 1000
        QAM16_1_2,  // 1001
        QPSK_1_2,   // 1010
        BPSK_1_2,   // 1011
        QAM64_3_4,  // 1100
        QAM16_3_4,  // 1101
        QPSK_3_4,   // 1110
        BPSK_3_4    // 1111
      };

      // see Table 18.4
      static std::map<int,int> N_DBPS;  // num. data bits per OFDM symbol
      static std::map<int,int> N_CBPS;  // num. coded bits per OFDM symbol
      static std::map<int,int> N_BPSC;  // num. bits per subcarrier

      static std::map<int,digital::constellation_sptr> WIFI_CONSTELLATIONS;
      static std::map<int,std::vector<int> > INTL_TABLE;
      static std::map<int,std::vector<int> > DEINTL_TABLE;

      static std::vector<int> NO_PUNCTURE_MAP;

      static sptr make();

      /*!
       * Computes number of OFDM symbols required to transmit
       * a number of PSDU/MPDU octects at a given rate.
       * \param rate value of RATE field.
       * \param psdu_octets number of PSDU/MPDU octets.
       */
      static int compute_n_ofdm_syms(int rate, int psdu_octets);

      /*!
       * Computes number of constellation symbols required to transmit
       * a number of PSDU/MPDU octets at a given rate.
       * This value is equal to n_ofdm_syms * 48.
       * \param rate value of RATE field.
       * \param psdu_octets number of PSDU/MPDU octets.
       */
      static int compute_n_syms(int rate, int psdu_octets);

      /*!
       * Computes number of bytes required by the uncoded frame.
       * \param rate rate of transmission.
       * \param psdu_octets number of PSDU/MPDU octets.
       */
      static int compute_uncoded_frame_size(int rate, int psdu_octets);

      /*!
       * Compute number of bytes required for the PLCP header without
       * SERVICE field.
       */
      static int compute_uncoded_header_no_service_size();

      /*!
       * Computes number of bytes required by the combined part of
       * \param rate rate of transmission.
       * \param psdu_octets number of PSDU/MPDU octets.
       * (SERVICE, PAYLOAD, TAIL, PAD).
       */
      static int compute_uncoded_service_payload_tail_pad_size(int rate, int psdu_octets);

      /*!
       * Computes number of tail plus padding bytes.
       * \param rate rate of transmission.
       * \param psdu_octets number of PSDU/MPDU octets.
       */
      static int compute_uncoded_tail_pad_size(int rate, int psdu_octets);

      /*!
       * Returns string representation of RATE value.
       * \param rate value of RATE field.
       */
      static std::string rate_string(int rate);

      /*!
       * Returns string representation of RATE/SIGNAL value.
       * \param rate value of RATE/SIGNAL field.
       * \param mode mode can be DSSS or OFDM.
       */
      static std::string rate_string(int rate, int mode);

      /*!
       * Create PHY frame with requested rate and length of data.
       * \param rate rate of transmission.
       * \param len number of bytes in payload.
       * \param data pointer to the payload.
       * \param ppdu contains the created PPDU frame.
       */
      static void create_frame(int rate, int len,
        const char *data, std::vector<char> &ppdu);

      /*!
       * Create PHY frame with requested rate and data.
       * \param rate rate of transmission.
       * \param data data payload.
       * \return uncoded PPDU as a vector of bytes.
       */
      static std::vector<char> create_frame(int rate, const std::string &data);

      /*!
       * Create PHY header with specified rate and length.
       * \param rate rate of transmission.
       * \param len length of data payload.
       * \param hdr created header.
       */
      static void create_header(int rate, int len, phy80211_frame_header &hdr);

      /*!
       * Scramble a given bit array with a predefined seed. The results are
       * stored back to the input pointer.
       * \param data pointer to the bit array.
       * \param len number of bits in the array.
       * \param seed 7-bit initial integer as the seed to the scrambler.
       * \return new seed after scrambling.
       */
      static char scramble(char* data, int len, char seed);

      /*!
       * Scramble a given bit array with a predefined seed. The results are
       * stored in the output pointer.
       * \param data pointer to the bit array.
       * \param len number of bits in the array.
       * \param seed 7-bit initial integer as the seed to the scrambler.
       * \param out pointer to the output.
       * \return new seed after scrambling.
       */
      static char scramble(const char *data, int len, char seed, char *out);

      static std::vector<int> create_puncture_map(int rate);

      static std::vector<int> create_interleaving_map(int rate);
      static std::vector<int> create_deinterleaving_map(int rate);

      phy_utils();
      ~phy_utils();


    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_PHY_UTILS_H */

