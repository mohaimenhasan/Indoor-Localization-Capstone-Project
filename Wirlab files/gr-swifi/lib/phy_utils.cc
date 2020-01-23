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

#include <swifi/phy_utils.h>
#include <swifi/wifi_constellation.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

namespace gr {
  namespace swifi {

    phy_utils::sptr
    phy_utils::make()
    {
      return phy_utils::sptr(new phy_utils());
    }

    phy_utils::phy_utils() {
    }

    phy_utils::~phy_utils() {
    }

    int phy_utils::compute_n_ofdm_syms(int rate, int psdu_octets) {
      int n_dbps = N_DBPS[rate];
      if (n_dbps != 0)
        return (int)ceil(((float)(22 /* 16 bit- SERVICE + 6-bit TAIL */ + 8 * psdu_octets)) / n_dbps);
      return 0;
    }

    int phy_utils::compute_n_syms(int rate, int psdu_octets) {
      return compute_n_ofdm_syms(rate, psdu_octets) * 48;
    }

    int phy_utils::compute_uncoded_frame_size(int rate, int psdu_octets) {
      // return number of bytes of (PLCP Header + payload + payload tail+ padding)
      return (compute_n_ofdm_syms(rate, psdu_octets) * N_DBPS[rate]) / 8 + 3;
    }

    int phy_utils::compute_uncoded_header_no_service_size() {
      return sizeof(phy80211_frame_header) - 2; // result should be 3
    }

    int phy_utils::compute_uncoded_service_payload_tail_pad_size(int rate, int psdu_octets) {
      return (compute_n_ofdm_syms(rate, psdu_octets) * N_DBPS[rate]) / 8;
    }

    int phy_utils::compute_uncoded_tail_pad_size(int rate, int psdu_octets) {
      // return number of bytes of (6-bit payload tail + padding)
      return compute_uncoded_frame_size(rate, psdu_octets)
        - sizeof(phy80211_frame_header) - psdu_octets;
    }

    std::string phy_utils::rate_string(int rate) {
      switch (rate) {
        case BPSK_1_2:
          return "BPSK 1/2 [6 Mbps]";
        case BPSK_3_4:
          return "BPSK 3/4 [9 Mbps]";
        case QPSK_1_2:
          return "QPSK 1/2 [12 Mbps]";
        case QPSK_3_4:
          return "QPSK 3/4 [18 Mbps]";
        case QAM16_1_2:
          return "QAM16 1/2 [24 Mbps]";
        case QAM16_3_4:
          return "QAM16 3/4 [36 Mbps]";
        case QAM64_2_3:
          return "QAM64 2/3 [48 Mbps]";
        case QAM64_3_4:
          return "QAM64 3/4 [54 Mbps]";
        default:
          return "OFDM/UNDEFINED";
      }
    }

    std::string phy_utils::rate_string(int rate, int mode) {
      if (mode == MODE_DSSS) {
        switch (rate) {
          case 0x0A:
            return "Barker/DBPSK [1 Mbps]";
          case 0x14:
            return "Barker/DQPSK [2 Mbps]";
          case 0x37:
            return "CCK 5.5 [5.5 Mbps]";
          case 0x6e:
            return "CCK 11 [11 Mbps]";
          default:
            return "DSSS/UNDEFINED";
        }
      }
      else return rate_string(rate);
    }

    void phy_utils::create_frame(int rate, int len, const char *data,
      std::vector<char> &ppdu) 
    {
      // compute number of bytes required for the uncoded PPDU
      ppdu.resize(compute_uncoded_frame_size(rate, len));
      phy80211_frame_header *hdr = (phy80211_frame_header*)ppdu.data();
      create_header(rate, len, *hdr);
      memcpy(hdr + 1, data, len);
      memset(hdr + sizeof(hdr) + len, 0, ppdu.size() - sizeof(hdr) - len);
    }

    std::vector<char> phy_utils::create_frame(int rate, const std::string &data) {
      std::vector<char> ppdu;
      create_frame(rate, data.size(), data.data(), ppdu);
      return ppdu;
    }

    void phy_utils::create_header(int rate, int len, phy80211_frame_header &hdr) {
      hdr.rate = rate;
      hdr.reserved = 0;
      hdr.length = len;
      hdr.parity = 0;
      for (int i = 0; i < 4; i++)
        hdr.parity ^= ((rate >> i) & 1);
      for (int i = 0; i < 12; i++)
        hdr.parity ^= ((len >> i) & 1);
      hdr.tail = 0;
      hdr.service = 0;
    }

    char phy_utils::scramble(char* data, int len, char seed) {
      return scramble(data, len, seed, data);
    }

    char phy_utils::scramble(const char *data, int len, char seed, char *out) {
      char fb;

      for (int i=0;i<len;i++) {
        fb = ((seed >> 3) & 1) ^ ((seed >> 6) & 1);
        out[i] = data[i] ^ fb;
        seed = (seed << 1) | fb;
      }

      return seed;
    }

    std::vector<int> phy_utils::create_puncture_map(int rate) {
      std::string s;
      switch (rate) {
        case BPSK_3_4:
        case QPSK_3_4:
        case QAM16_3_4:
        case QAM64_3_4:
          s = "111001"; // 1: keep, 0: erase
          break;
        case QAM64_2_3:
          s = "1110"; // 1: keep, 0: erase
          break;
        default:
          s = "1"; // no puncturing
          break;
      }

      std::vector<int> puncture_map;
      for (int i = 0; i < s.size(); i++)
        puncture_map.push_back(s[i] - '0');
      return puncture_map;
    }

    std::vector<int> phy_utils::create_interleaving_map(int rate) { 
      int CBPS = phy_utils::N_CBPS[rate];
      int BPSC = phy_utils::N_BPSC[rate];
      int s = std::max(BPSC/2,1);
      int i,j,k;
      std::vector<int> intl_map(CBPS);

      for (k = 0; k < CBPS; k++) {
        i = (CBPS/16)*(k % 16) + k/16;                    //1st permutation
        j = s*(i/s) + (int)(i + CBPS - 16*i/CBPS) % s;    //2nd permutation
        intl_map[k] = j;
      }
      return intl_map;
    }

    std::vector<int> phy_utils::create_deinterleaving_map(int rate) { 
      int CBPS = phy_utils::N_CBPS[rate];
      int BPSC = phy_utils::N_BPSC[rate];
      int s = std::max(BPSC/2,1);
      int i,j,k;
      std::vector<int> deintl_map(CBPS);

      for (j = 0; j < CBPS; j++) {
        i = s*(j/s) + (int)(j + 16*j/CBPS) % s;       //1st Permutation
        k = 16*i - (CBPS - 1)*(16*i/CBPS);            //2nd Permutation
        deintl_map[j] = k;
      }
      return deintl_map;
    }


    //------------- Non-class methods and variable initialization -----------

    // see Table 18.4
    std::map<int,int> init_n_dbps() {
      std::map<int,int> n_dbps;
      n_dbps[phy_utils::BPSK_1_2] = 24;
      n_dbps[phy_utils::BPSK_3_4] = 36;
      n_dbps[phy_utils::QPSK_1_2] = 48;
      n_dbps[phy_utils::QPSK_3_4] = 72;
      n_dbps[phy_utils::QAM16_1_2] = 96;
      n_dbps[phy_utils::QAM16_3_4] = 144;
      n_dbps[phy_utils::QAM64_2_3] = 192;
      n_dbps[phy_utils::QAM64_3_4] = 216;
      return n_dbps;
    }

    // see Table 18.4
    std::map<int,int> init_n_cbps() {
      std::map<int,int> n_cbps;
      n_cbps[phy_utils::BPSK_1_2] = 48;
      n_cbps[phy_utils::BPSK_3_4] = 48;
      n_cbps[phy_utils::QPSK_1_2] = 96;
      n_cbps[phy_utils::QPSK_3_4] = 96;
      n_cbps[phy_utils::QAM16_1_2] = 192;
      n_cbps[phy_utils::QAM16_3_4] = 192;
      n_cbps[phy_utils::QAM64_2_3] = 288;
      n_cbps[phy_utils::QAM64_3_4] = 288;
      return n_cbps;
    }

    // see Table 18.4
    std::map<int,int> init_n_bpsc() {
      std::map<int,int> n_bpsc;
      n_bpsc[phy_utils::BPSK_1_2] = 1;
      n_bpsc[phy_utils::BPSK_3_4] = 1;
      n_bpsc[phy_utils::QPSK_1_2] = 2;
      n_bpsc[phy_utils::QPSK_3_4] = 2;
      n_bpsc[phy_utils::QAM16_1_2] = 4;
      n_bpsc[phy_utils::QAM16_3_4] = 4;
      n_bpsc[phy_utils::QAM64_2_3] = 6;
      n_bpsc[phy_utils::QAM64_3_4] = 6;
      return n_bpsc;
    }

    std::map<int,digital::constellation_sptr> init_wifi_constellations() {
      std::map<int,digital::constellation_sptr> constellations;

      // set BPSK
      constellations[phy_utils::BPSK_1_2] = constellations[phy_utils::BPSK_3_4] = 
        wifi_constellation_bpsk::make();

      // set QPSK
      constellations[phy_utils::QPSK_1_2] = constellations[phy_utils::QPSK_3_4] =
        wifi_constellation_qpsk::make();

      // set QAM16
      constellations[phy_utils::QAM16_1_2] = constellations[phy_utils::QAM16_3_4] =
        wifi_constellation_16qam::make();

      // set QAM64
      constellations[phy_utils::QAM64_2_3] = constellations[phy_utils::QAM64_3_4] =
        wifi_constellation_64qam::make();

      return constellations;
    }

    std::map<int,std::vector<int> > init_intl_table() {
      std::map<int,std::vector<int> > intl_table;

      // set BPSK
      intl_table[phy_utils::BPSK_1_2] = intl_table[phy_utils::BPSK_3_4] = 
        phy_utils::create_interleaving_map(phy_utils::BPSK_1_2);

      // set QPSK
      intl_table[phy_utils::QPSK_1_2] = intl_table[phy_utils::QPSK_3_4] =
        phy_utils::create_interleaving_map(phy_utils::QPSK_1_2);

      // set QAM16
      intl_table[phy_utils::QAM16_1_2] = intl_table[phy_utils::QAM16_3_4] =
        phy_utils::create_interleaving_map(phy_utils::QAM16_1_2);

      // set QAM64
      intl_table[phy_utils::QAM64_2_3] = intl_table[phy_utils::QAM64_3_4] =
        phy_utils::create_interleaving_map(phy_utils::QAM64_2_3);

      return intl_table;
    }

    std::map<int,std::vector<int> > init_deintl_table() {
      std::map<int,std::vector<int> > deintl_table;

      // set BPSK
      deintl_table[phy_utils::BPSK_1_2] = deintl_table[phy_utils::BPSK_3_4] = 
        phy_utils::create_deinterleaving_map(phy_utils::BPSK_1_2);

      // set QPSK
      deintl_table[phy_utils::QPSK_1_2] = deintl_table[phy_utils::QPSK_3_4] =
        phy_utils::create_deinterleaving_map(phy_utils::QPSK_1_2);

      // set QAM16
      deintl_table[phy_utils::QAM16_1_2] = deintl_table[phy_utils::QAM16_3_4] =
        phy_utils::create_deinterleaving_map(phy_utils::QAM16_1_2);

      // set QAM64
      deintl_table[phy_utils::QAM64_2_3] = deintl_table[phy_utils::QAM64_3_4] =
        phy_utils::create_deinterleaving_map(phy_utils::QAM64_2_3);

      return deintl_table;
    }


    static std::vector<int> init_no_puncture_map() {
      std::vector<int> m;
      m.push_back(1);
      return m;
    }

    // initialize static variables
    std::map<int,int> phy_utils::N_DBPS = init_n_dbps();
    std::map<int,int> phy_utils::N_CBPS = init_n_cbps();
    std::map<int,int> phy_utils::N_BPSC = init_n_bpsc();
    std::map<int,digital::constellation_sptr> phy_utils::WIFI_CONSTELLATIONS = init_wifi_constellations();
    std::map<int,std::vector<int> > phy_utils::INTL_TABLE = init_intl_table();
    std::map<int,std::vector<int> > phy_utils::DEINTL_TABLE = init_deintl_table();
    std::vector<int> phy_utils::NO_PUNCTURE_MAP = init_no_puncture_map();

  } /* namespace swifi */
} /* namespace gr */

