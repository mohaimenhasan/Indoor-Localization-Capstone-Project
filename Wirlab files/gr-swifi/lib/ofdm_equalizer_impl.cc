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

#include "ofdm_equalizer_impl.h"
#include <swifi/phy_utils.h>
#include <swifi/tag_utils.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/expj.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    std::map<int, std::string> init_equalizer_names() 
    {
      std::map<int, std::string> names;
      names[0] = "static";
      names[1] = "ppt";
      names[2] = "sta";
      names[3] = "dd";
      names[4] = "li";
      names[5] = "lpi";
      names[6] = "ppt_lpi";
      names[7] = "ppt_dd";
      names[99] = "test";
      return names;
    }

    std::map<int, std::string> ofdm_equalizer::EQUALIZER_NAME = init_equalizer_names();

    ofdm_equalizer::sptr
    ofdm_equalizer::make(const std::string& phy_tag_key,
      bool propagate_channel_states,
      const std::string &channel_states_tag_key,
      digital::constellation_sptr constellation)
    {
      return gnuradio::get_initial_sptr(
        new ofdm_equalizer_impl(phy_tag_key,
          propagate_channel_states,
          channel_states_tag_key,
          constellation));
    }

    ofdm_equalizer_impl::ofdm_equalizer_impl(
      const std::string& phy_tag_key,
      bool propagate_channel_states,
      const std::string &channel_states_tag_key,
      digital::constellation_sptr constellation)
    : sync_block("ofdm_equalizer",
            gr::io_signature::make(1, 1, 64 * sizeof(gr_complex)),
            gr::io_signature::make(1, 1, 48 * sizeof(gr_complex))),
      d_polarity_period(phy_tag_key.empty() ? 1 : 127),
      d_polarity_index(phy_tag_key.empty() ? 0 : 1),
      d_phy_tag_key(pmt::intern(phy_tag_key)),
      d_constellation(constellation),
      d_alpha(1), d_one_minus_alpha(0), d_beta(1),
      d_propagate_channel_states(propagate_channel_states),
      d_channel_states_tag_key(pmt::intern(channel_states_tag_key)),
      d_foc_enabled(false),
      d_compute_sfo(true),
      d_sfo(0),
      d_info_tag_key(pmt::intern("info"))
    {
      init();
    }

    ofdm_equalizer_impl::~ofdm_equalizer_impl() {
      delete d_fft;
      delete d_ifft;
      if (d_df) delete d_df;

      delete[] d_sc_pll;
    }

    void ofdm_equalizer_impl::init() {
      set_relative_rate(1);

      // true indicates a data or pilot carrier, false indicates a null carrier
      d_occupied_carriers.resize(64, false);
      for (int i = 6; i < 59; i++) {
        if (i != 32) // avoid DC carrier
          d_occupied_carriers[i] = true;
      }

      // true indicates a pilot carrier
      d_pilot_carriers.resize(64, false);
      d_pilot_carriers[11] = d_pilot_carriers[25] = d_pilot_carriers[39]
        = d_pilot_carriers[53] = true;

      // initialize pilot locations
      d_pilot_location.resize(4, 0);
      d_pilot_location[0] = 11;
      d_pilot_location[1] = 25;
      d_pilot_location[2] = 39;
      d_pilot_location[3] = 53;

      // IEEE 802.11 OFDM pilot symbols ("P" in the standard, 18.3.5.10)
      d_pilot_symbols.resize(127);
      for (int i = 0; i < 127; i++) {
        d_pilot_symbols[i].resize(4, gr_complex(float(POLARITY[i]), 0.0f));
        d_pilot_symbols[i][3] = gr_complex(float(-POLARITY[i]), 0.0f);
      }

      // initial channel states
      d_channel_states.resize(64, gr_complex(1, 0));

      // default constellation
      if (!d_constellation)
        d_constellation = phy_utils::WIFI_CONSTELLATIONS[phy_utils::BPSK_1_2];
      d_points = d_constellation->points();

      d_pll = blocks::control_loop(2*3.14159 / 100, 0.25, -0.25);

      // default equalization method
      set_equalization_method(EQUALIZE_STATIC);

      // for LS_LPI2
      d_fft = new fft::fft_complex(64, true, 1);
      d_ifft = new fft::fft_complex(64, false, 1);

      // PLL per carrier
      d_sc_pll = new blocks::control_loop[64];
      for (int i = 0; i < 64; i++)
        d_sc_pll[i] = blocks::control_loop(2*3.14159 / 100, 0.25, -0.25);

      // debug file descriptor
      d_df = NULL;
    }

    void ofdm_equalizer_impl::enable_foc(bool enabled) {
      d_foc_enabled = enabled;
    }

    void ofdm_equalizer_impl::set_equalization_method(int method) {
      d_method = method;
    }

    void ofdm_equalizer_impl::set_equalization_method(const std::string &method) {
      std::map<int, std::string>::iterator it = EQUALIZER_NAME.begin();
      for (; it != EQUALIZER_NAME.end(); ++it) {
        if (it->second.compare(method) == 0) {
          d_method = it->first;
          break;
        }
      }
    }

    void ofdm_equalizer_impl::set_alpha(float alpha) {
      d_alpha = alpha;
      d_one_minus_alpha = 1 - alpha;
    }

    void ofdm_equalizer_impl::set_beta(float beta) {
      d_beta = beta;
    }

    void ofdm_equalizer_impl::set_debug_file(const std::string &filename) {
      if (!filename.empty())
        d_df = fopen(filename.c_str(), "w");
    }

    void ofdm_equalizer_impl::equalize_static(const gr_complex *in, gr_complex *out) {
      for (int i = 0, o = 0; i < 64; i++) {
        if (d_occupied_carriers[i] && !d_pilot_carriers[i])
          out[o++] = in[i] / d_channel_states[i];
      }
    }

    void ofdm_equalizer_impl::equalize_ppt(const gr_complex *in, gr_complex *out) {
      int o = 0;
      gr_complex a(0, 0);
      gr_complex h;

      for (int i = 0; i < d_pilot_location.size(); i++) {
        // channel state for current symbol
        h = in[d_pilot_location[i]] / d_pilot_symbols[d_polarity_index][i];

        // compare in terms of phase to the channel state for the initial symbol (preamble)
        a += h * conj(d_channel_states[d_pilot_location[i]]);
      }

      // get the phase diff compared to the initial channel states
      a = gr_expj(arg(a));

      // equalize
      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i] && !d_pilot_carriers[i]) {
          out[o] = in[i] / (d_channel_states[i] * a);
          o++;
        }
      }
    }

    // void ofdm_equalizer_impl::equalize_ppt_dd(const gr_complex *in, gr_complex *out) {
    //   int o = 0, p = 0;
    //   gr_complex a(0, 0);
    //   gr_complex h;
    //   float A = 0;
    //   float phase_error;

    //   for (int i = 0; i < d_pilot_location.size(); i++) {
    //     h = in[d_pilot_location[i]] * gr_expj(d_sc_pll[d_pilot_location[i]].get_phase()) / 
    //       d_pilot_symbols[d_polarity_index][i];

    //     a += h * conj(d_channel_states[d_pilot_location[i]]);
    //   }
    //   a = gr_expj(arg(a));

    //   o = 0;
    //   p = 0;
    //   for (int i = 0; i < 64; i++) {
    //     if (!d_occupied_carriers[i])
    //       continue;

    //     if (!d_pilot_carriers[i]) {
    //       out[o] = in[i] * gr_expj(d_sc_pll[i].get_phase()) / (d_channel_states[i] * a);
    //       phase_error = -arg(out[o] * 
    //         conj(d_points[d_constellation->decision_maker(&out[o])]));
    //       if (abs(phase_error) < M_PI * d_beta) {
    //         d_sc_pll[i].advance_loop(phase_error);
    //         d_sc_pll[i].phase_wrap();
    //         d_sc_pll[i].frequency_limit();
    //       }
    //       o++;
    //     }
    //     else {
    //       h = in[i] * gr_expj(d_sc_pll[i].get_phase()) / (d_channel_states[i] * a);
    //       phase_error = -arg(h * conj(d_pilot_symbols[d_polarity_index][p]));
    //       if (abs(phase_error) < M_PI * d_beta) {
    //         d_sc_pll[i].advance_loop(phase_error);
    //         d_sc_pll[i].phase_wrap();
    //         d_sc_pll[i].frequency_limit();
    //       }
    //       p++;
    //     }
    //   }
    // }

#define NO_FINGERPRINT 0

#if NO_FINGERPRINT

    void ofdm_equalizer_impl::equalize_ppt_dd(const gr_complex *in, gr_complex *out) {
      int o = 0;
      gr_complex a(0, 0);
      gr_complex h;
      gr_complex remap;

      for (int i = 0; i < d_pilot_location.size(); i++) {
        h = in[d_pilot_location[i]] / d_pilot_symbols[d_polarity_index][i];

        // should we normalize before adding up?
        // a += gr_expj(arg(h * conj(d_channel_states[d_pilot_location[i]])));

        // or just simply adding up?
        a += h * conj(d_channel_states[d_pilot_location[i]]);
      }
      a = gr_expj(arg(a));

      o = 0;
      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i] && !d_pilot_carriers[i]) {
          out[o] = in[i] / (d_channel_states[i] * a);
          remap = d_points[d_constellation->decision_maker(&out[o])];
          if (norm(out[o] - remap) < d_beta) {
            d_channel_states[i] = d_channel_states[i] * d_one_minus_alpha +
              in[i] / remap / a * d_alpha;
          }
          o++;
        }
      }
    }

#elif FINGERPRINT_OLD

    void ofdm_equalizer_impl::equalize_ppt_dd(const gr_complex *in, gr_complex *out) {
      int o = 0;
      gr_complex a(0, 0);
      gr_complex h;
      gr_complex remap;
      gr_complex b;

      if (d_compute_sfo) {
        if (d_polarity_index == 1) {
          d_sum_sfo = gr_complex(0, 0);
          d_last_a = gr_complex(1, 0);
          // printf("start\n");
        }
      }

      for (int i = 0; i < d_pilot_location.size(); i++) {
        h = in[d_pilot_location[i]] / d_pilot_symbols[d_polarity_index][i];
        a += h * conj(d_channel_states[d_pilot_location[i]]);
      }
      a = gr_expj(arg(a));

      if (d_compute_sfo && d_polarity_index >= 500) {
        b = gr_expj(arg(d_sum_sfo) * d_polarity_index);
      }
      else
        b = a;

      if (d_compute_sfo) {
        if (d_polarity_period != 1) {
          // printf("diff %f mean %f deviate %f\n", arg(a * conj(d_last_a)),
          //   arg(d_sum_sfo), 
          //   arg(a * conj(gr_expj(arg(d_sum_sfo) * d_polarity_index))));
        }

        d_sum_sfo += a * conj(d_last_a);
        d_last_a = a;
      }



      o = 0;
      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i] && !d_pilot_carriers[i]) {
          out[o] = in[i] / (d_channel_states[i] * b);
          remap = d_points[d_constellation->decision_maker(&out[o])];
          if (norm(out[o] - remap) < d_beta) {
            d_channel_states[i] = d_channel_states[i] * d_one_minus_alpha +
              in[i] / remap / b * d_alpha;
          }
          o++;
        }
      }

      // for fingerprinting: get sampling frequency offset
      if (d_compute_sfo) {
        if (d_polarity_index == d_n_ofdm_syms) {
          d_sfo = arg(d_sum_sfo);
          // printf("end\n");
          // printf("d_n_ofdm_syms %d d_sfo %f %f\n", d_n_ofdm_syms, d_sfo, arg(d_last_a));
        }
      }
    }

#else

    void ofdm_equalizer_impl::equalize_ppt_dd(const gr_complex *in, gr_complex *out) {
      int o = 0;
      gr_complex a(0, 0);
      gr_complex h;
      gr_complex remap;
      gr_complex b(1, 0);
      gr_complex c(1, 0);

      if (d_compute_sfo) {
        if (d_polarity_index == 1) {
          d_sum_sfo = gr_complex(0, 0);
          d_last_a = gr_complex(1, 0);
          // printf("start\n");
        }
      }

      for (int i = 0; i < d_pilot_location.size(); i++) {
        h = in[d_pilot_location[i]] / d_pilot_symbols[d_polarity_index][i];
        b = h * conj(d_channel_states[d_pilot_location[i]]);
        a += b;
        c = c * b;
      }
      a = gr_expj(arg(a));

      if (d_compute_sfo) {
        d_sum_sfo += gr_expj(arg(c * conj(d_last_a)) / (11.0 + 25 + 39 + 53));
        d_last_a = c;

        // try this, but didn't work, why?
        // a = gr_expj(arg(d_sum_sfo) * d_polarity_index * 64);
      }



      o = 0;
      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i] && !d_pilot_carriers[i]) {
          out[o] = in[i] / (d_channel_states[i] * a);
          remap = d_points[d_constellation->decision_maker(&out[o])];
          if (norm(out[o] - remap) < d_beta) {
            d_channel_states[i] = d_channel_states[i] * d_one_minus_alpha +
              in[i] / remap / a * d_alpha;
          }
          o++;
        }
      }

      // for fingerprinting: get sampling frequency offset
      if (d_compute_sfo) {
        if (d_polarity_index == d_n_ofdm_syms) {
          d_sfo = arg(d_sum_sfo);
          // printf("end\n");
          // printf("d_n_ofdm_syms %d d_sfo %f %f\n", d_n_ofdm_syms, d_sfo, arg(d_last_a));
        }
      }
    }

#endif


    void ofdm_equalizer_impl::equalize_sta(const gr_complex *in, gr_complex *out) {
      static std::vector<std::vector<float> > avg_coeff;
      int pilot_symbol_idx = 0;
      int o = 0;
      std::vector<gr_complex> h(64);

      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i]) {
          if (d_pilot_carriers[i]) {
            h[i] = in[i] / d_pilot_symbols[d_polarity_index][pilot_symbol_idx];
            pilot_symbol_idx++;
          }
          else {
            out[o] = in[i] / d_channel_states[i];
            h[i] = in[i] / d_points[d_constellation->decision_maker(&out[o])];
            o++;
          }
        }
      }

      // initialize avg_coeff
      if (avg_coeff.empty()) {
        avg_coeff.resize(64);
        for (int i = 0; i < 64; i++) {
          avg_coeff[i].resize(2*int(d_beta)+1, 0);

          // count num. neighbors
          int neighbor = 0;
          for (int k = -int(d_beta); k <= int(d_beta); k++) {
            if (i + k >= 0 && i + k < 64 && d_occupied_carriers[i + k])
              neighbor++;
          }

          // Set equal coefficients for neighborhood occupied carrier for averaging.
          // The coefficients should sum up to 1.0
          for (int k = -int(d_beta); k <= int(d_beta); k++) {
            if (i + k >= 0 && i + k < 64 && d_occupied_carriers[i + k])
              avg_coeff[i][k+int(d_beta)] = 0.5 / neighbor;
          }
          avg_coeff[i][0] = 0.5;  // center subcarrier has higher weight
        }
      }

      // averaging channel states
      std::vector<gr_complex> update(64, gr_complex(0, 0));
      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i]) {
          for (int k = -int(d_beta); k <= int(d_beta); k++) {
            update[i] += h[i] * avg_coeff[i][k+int(d_beta)];
          }

          d_channel_states[i] = d_channel_states[i] * d_one_minus_alpha +
            update[i] * d_alpha;
        }
      }
    }

    void ofdm_equalizer_impl::equalize_dd(const gr_complex *in, gr_complex *out) {
      int pilot_symbol_idx = 0;
      int o = 0;
      gr_complex h;

      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i]) {
          if (d_pilot_carriers[i]) {
            h = in[i] / d_pilot_symbols[d_polarity_index][pilot_symbol_idx];
            pilot_symbol_idx++;
          }
          else {
            out[o] = in[i] / d_channel_states[i];
            h = in[i] / d_points[d_constellation->decision_maker(&out[o])];
            o++;
          }

          // averaging channel states
          d_channel_states[i] = d_channel_states[i] * d_one_minus_alpha +
            h * d_alpha;
        }
      }
    }

    void ofdm_equalizer_impl::linear_interpolation(int ref1_idx, int ref2_idx,
      int from_idx, int to_idx)
    {
      gr_complex h = (d_channel_states[ref2_idx] - d_channel_states[ref1_idx])
        / float(ref2_idx - ref1_idx);
      for (int i = from_idx; i <= to_idx; i++) {
        d_channel_states[i] = d_channel_states[ref1_idx] + h * float(i - ref1_idx);
      }
    }

    void ofdm_equalizer_impl::equalize_li(const gr_complex *in, gr_complex *out) {
      static std::vector<int> ref_carrier_idx;
      int pilot_symbol_idx = 0;
      int o = 0;
      gr_complex h;

      // least-square estimation for pilot subcarriers
      for (int i = 0; i < 64; i++) {
        if (d_pilot_carriers[i]) {
          h = in[i] / d_pilot_symbols[d_polarity_index][pilot_symbol_idx];
          pilot_symbol_idx++;
        }

        // averaging pilot channel states
        d_channel_states[i] = d_channel_states[i] * d_one_minus_alpha +
          h * d_alpha;
      }

      // find pilot subcarrier indices
      if (ref_carrier_idx.empty()) {
        for (int i = 0; i < 64; i++) {
          if (d_pilot_carriers[i]) {
            ref_carrier_idx.push_back(i);
          }
        }
        if (ref_carrier_idx.front() != 0)
          ref_carrier_idx.insert(ref_carrier_idx.begin(), 0);
        if (ref_carrier_idx.back() != 63)
          ref_carrier_idx.push_back(63);
      }

      // linear interpolation for data subcarriers
      for (int k = 0; k < ref_carrier_idx.size() - 1; k++) {
        // data carriers on the left of the first pilot carrier
        // use the first two pilots to compute linear interpolation
        if (!d_pilot_carriers[ref_carrier_idx[k]]) {
          linear_interpolation(ref_carrier_idx[k+1], ref_carrier_idx[k+2], 
            ref_carrier_idx[k], ref_carrier_idx[k+1]-1);
        }
        // data carriers on the right of the last pilot carrier
        // use the last two pilots to compute linear interpolation
        else if (!d_pilot_carriers[ref_carrier_idx[k+1]]) {
          linear_interpolation(ref_carrier_idx[k-1], ref_carrier_idx[k],
            ref_carrier_idx[k]+1, ref_carrier_idx[k+1]);
        }
        // data carriers within a pair of two pilot carriers
        // use those two pilots to compute linear interpolation
        else {
          linear_interpolation(ref_carrier_idx[k], ref_carrier_idx[k+1],
            ref_carrier_idx[k]+1, ref_carrier_idx[k+1]-1);
        }
      }

      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i] && !d_pilot_carriers[i]) {
          out[o] = in[i] / d_channel_states[i];
          o++;
        }
      }
    }

    void ofdm_equalizer_impl::equalize_lpi(const gr_complex *in, gr_complex *out) {
      // low-pass interpolation filter taps generated by Matlab:
      // [~,taps]=interp([0,1,3,4,1,0],14,2);
      // Note: [0,1,3,4,1,0] is not important, it is just some random data vector
      // with the requirement of having length 6
      static float taps[65] = {0.000000,0.000000,0.000000,0.000000,0.000000,-0.017553,-0.034691,-0.050798,-0.065255,-0.077456,-0.086812,-0.092757,-0.094761,-0.092331,-0.085020,-0.072435,-0.054241,-0.030164,0.000000,0.075572,0.156232,0.240604,0.327261,0.414736,0.501543,0.586188,0.667195,0.743116,0.812551,0.874164,0.926699,0.968995,1.000000,0.968995,0.926699,0.874164,0.812551,0.743116,0.667195,0.586188,0.501543,0.414736,0.327261,0.240604,0.156232,0.075572,0.000000,-0.030164,-0.054241,-0.072435,-0.085020,-0.092331,-0.094761,-0.092757,-0.086812,-0.077456,-0.065255,-0.050798,-0.034691,-0.017553,0.000000,0.000000,0.000000,0.000000,0.000000};
      gr_complex pilot_channel_states[4];
      gr_complex h;
      int pilot_symbol_idx = 0;
      int o = 0, k;

      // least-square estimation for pilot subcarriers
      for (int i = 0; i < 4; i++) {
        pilot_channel_states[i] = in[d_pilot_location[i]] /
          d_pilot_symbols[d_polarity_index][i];
      }

      // low-pass interpolation for channel states
      std::vector<gr_complex> new_channel_states(64);
      for (int m = -32; m <= 32; m++) {
        h = gr_complex(0, 0);
        for (int i = 0; i < 4; i++) {
          k = m + d_pilot_location[i];
          if (k >= 0 && k <= 64) {
            h += pilot_channel_states[i] * taps[k];
          }
        }

        // get channel states
        if (m > -32) {
          new_channel_states[32 - m] = h;
        }
      }

      // equalizing
      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i] && !d_pilot_carriers[i]) {
          out[o] = in[i] / new_channel_states[i];
          o++;
        }
      }
    }


    void ofdm_equalizer_impl::equalize_lpi2(const gr_complex *in, gr_complex *out) {
      int o = 0;
      gr_complex pilot_channel_states[64];
      memset(pilot_channel_states, 0, sizeof(gr_complex) * 64);

      // least-square estimation for pilot subcarriers
      for (int i = 0; i < 4; i++) {
        pilot_channel_states[i] = in[d_pilot_location[i]] /
          d_pilot_symbols[d_polarity_index][i];
      }

      gr_complex fpilot_channel_states[64];
      gr_complex fchannel_states[64];

      // get FFT of pilot channel states
      memcpy(d_fft->get_inbuf(), pilot_channel_states, sizeof(gr_complex) * 64);
      d_fft->execute();
      memcpy(fpilot_channel_states, d_fft->get_outbuf(), sizeof(gr_complex) * 64);

      // get FFT of full channel states
      for (int i = 0; i < 64; i++) {
        fchannel_states[i] = G[i] * fpilot_channel_states[i];
      }

      // get full channel states
      memcpy(d_ifft->get_inbuf(), fchannel_states, sizeof(gr_complex) * 64);
      d_ifft->execute();
      memcpy(d_channel_states.data(), d_ifft->get_outbuf(), sizeof(gr_complex) * 64);

      // equalizing
      printf("equalizing ");
      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i] && !d_pilot_carriers[i]) {
          out[o] = in[i] / (d_channel_states[i] / 64.0f);
          printf("(%f,%f) (%f,%f) (%f,%f) --> (%f,%f), in (%f,%f)\n", 
            fpilot_channel_states[i].real(), fpilot_channel_states[i].imag(),
            G[i].real(), G[i].imag(),
            fchannel_states[i].real(), fchannel_states[i].imag(),
            (d_channel_states[i] / 64.0f).real(), (d_channel_states[i] / 64.0f).imag(),
            in[i].real(), in[i].imag());
          o++;
        }
      }
      printf("\n");
    }

    float ofdm_equalizer_impl::compute_mse(const gr_complex *samples) {
      int o = 0;
      float se = 0;
      gr_complex e;

      for (int i = 0; i < 64; i++) {
        if (d_occupied_carriers[i] && !d_pilot_carriers[i]) {
          e = samples[o] - d_points[d_constellation->decision_maker(&samples[o])];
          se += e.real() * e.real() + e.imag() * e.imag();
          o++;
        }
      }

      return se / 48; // mean square error
    }

    void ofdm_equalizer_impl::equalize_ppt_lpi(const gr_complex *in, gr_complex *out) {
      gr_complex out1[48], out2[48];
      float mse1 = 999999, mse2 = mse1;

      equalize_ppt(in, out1);
      mse1 = compute_mse(out1);
      if (mse1 > d_beta) {
        equalize_lpi(in, out2);
        mse2 = compute_mse(out2);
      }

      if (mse1 < mse2)
        memcpy(out, out1, sizeof(gr_complex) * 48);
      else
        memcpy(out, out2, sizeof(gr_complex) * 48);
    }

    void ofdm_equalizer_impl::equalize_test(const gr_complex *in, gr_complex *out) {
      int o = 0, p = 0;
      gr_complex a(0, 0);
      gr_complex h;
      float A = 0;
      float phase_error;

      for (int i = 0; i < d_pilot_location.size(); i++) {
        h = in[d_pilot_location[i]] * gr_expj(d_sc_pll[d_pilot_location[i]].get_phase()) / 
          d_pilot_symbols[d_polarity_index][i];

        a += h * conj(d_channel_states[d_pilot_location[i]]);
      }
      a = gr_expj(arg(a));

      o = 0;
      for (int i = 0; i < 64; i++) {
        if (!d_occupied_carriers[i])
          continue;

        if (!d_pilot_carriers[i]) {
          out[o] = in[i] * gr_expj(d_sc_pll[i].get_phase()) / (d_channel_states[i] * a);
          o++;
        }
      }


      // if (d_df) fprintf(d_df,"%d ", d_polarity_index);
      // for (int i = 0; i < d_pilot_location.size(); i++) {
      //   // channel state for current symbol
      //   h = in[d_pilot_location[i]] / d_pilot_symbols[d_polarity_index][i];

      //   // compare in terms of phase to the channel state for the initial symbol (preamble)
      //   a += h * conj(d_channel_states[d_pilot_location[i]]);
      //   A += norm(in[d_pilot_location[i]]);

      //   if (d_df) fprintf(d_df, "%f %f ", 
      //     abs(in[d_pilot_location[i]]),
      //     arg(h * conj(d_channel_states[d_pilot_location[i]])));
      // }

      // A = sqrt(A/4);

      // // get the phase diff compared to the initial channel states
      // a = gr_expj(arg(a));

      // // equalize
      // float phase_error;
      // for (int i = 0; i < 64; i++) {
      //   if (d_occupied_carriers[i] && !d_pilot_carriers[i]) {
      //     out[o] = in[i] * gr_expj(d_sc_pll[i].get_phase()) / (d_channel_states[i] * a);
      //     phase_error = -arg(out[o] * 
      //       conj(d_points[d_constellation->decision_maker(&out[o])]));
      //     if (abs(phase_error) < M_PI * d_beta) {
      //       d_sc_pll[i].advance_loop(phase_error);
      //       d_sc_pll[i].phase_wrap();
      //       d_sc_pll[i].frequency_limit();
      //     }
      //     o++;
      //   }
      // }

      float mse = compute_mse(out);
      o = 0;
      p = 0;
      if (mse < d_beta) {
        for (int i = 0; i < 64; i++) {
          if (d_occupied_carriers[i]) {
            gr_complex remap;
            if (d_pilot_carriers[i]) {
              remap = d_pilot_symbols[d_polarity_index][p++];
              // if (norm(in[i] / d_channel_states[i] - remap) < d_beta) {
              //   d_channel_states[i] = gr_expj(arg(d_channel_states[i])) *
              //     (abs(d_channel_states[i]) * d_one_minus_alpha + abs(in[i] / remap) * d_alpha);
              //   // d_channel_states[i] = d_channel_states[i] * d_one_minus_alpha +
              //   //   in[i] / remap / a * d_alpha;
              //   }
            }
            else {
              remap = d_points[d_constellation->decision_maker(&out[o])];
              if (norm(out[o] - remap) < d_beta) {
                d_channel_states[i] = d_channel_states[i] * d_one_minus_alpha +
                  in[i] / remap / a * d_alpha;
              }
              o++;
            }

          }
        }
      }
    }

    void ofdm_equalizer_impl::fo_correction(const gr_complex *in,
      gr_complex *out) 
    {
      // frequency offset correction
      for (int n = 0; n < 64; n++) {
        if (d_occupied_carriers[n]) {
          out[n] = in[n] * gr_expj(-d_freq_offset * n);
        }
      }
    }

    int ofdm_equalizer_impl::work (int noutput_items, 
        gr_vector_const_void_star& input_items,
        gr_vector_void_star& output_items) 
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];
      std::vector<tag_t> phy_tags, cs_tags;
      tag_t *cs_tag = NULL;
      int n;
      gr_complex corrected[64];
      const gr_complex *in_corrected;

      get_tags_in_window(phy_tags, 0, 0, noutput_items, d_phy_tag_key);
      std::sort(phy_tags.begin(), phy_tags.end(), tag_t::offset_compare);
      tag_range tr(nitems_read(0), noutput_items, phy_tags);

      get_tags_in_window(cs_tags, 0, 0, noutput_items, d_channel_states_tag_key);
      std::sort(cs_tags.begin(), cs_tags.end(), tag_t::offset_compare);
      if (cs_tags.size()) {
        cs_tag = cs_tags.data();  // get first channel state tag
      }

      for (int k = 0; k < tr.size(); k++) {
        phy_tag t;
        if (phy_tag::parse(tr.tag(k).value, t)) {
          d_n_ofdm_syms = phy_utils::compute_n_ofdm_syms(t.rate(), t.nitems());
          d_constellation = phy_utils::WIFI_CONSTELLATIONS[t.rate()];
          d_points = d_constellation->points();
          d_polarity_index = 1; // payload OFDM symbols start from polarity index 1
        }

        for (n = 0; n < tr.range_nitems(k); n++) {
          in_corrected = in;

          if (cs_tag && cs_tag->offset == tr.abs_first_itemidx(k) + n) {
            channel_states_tag cst;
            if (channel_states_tag::parse(cs_tag->value, cst)) {
              d_phase_offset = cst.phase_offset();
              d_freq_offset = cst.freq_offset();
              d_channel_states = cst.channel_states();
              d_pll.set_phase(d_phase_offset);
              d_pll.set_frequency(d_freq_offset);

              // reset PLL per carrier
              for (int i = 0; i < 64; i++) {
                d_sc_pll[i].set_phase(0);
                d_sc_pll[i].set_frequency(0);
              }

              if (d_method == EQUALIZE_LPI2) {
                G = cst.channel_states();
              }
            }

            remove_item_tag(0, *cs_tag);
            cs_tag++;
            if (cs_tag == cs_tags.data() + cs_tags.size())
              cs_tag = NULL;  // no more channel states tag
          }

          if (d_foc_enabled) {
            fo_correction(in, corrected);
            in_corrected = corrected;
          }

          switch (d_method) {
            case EQUALIZE_STATIC:
              equalize_static(in_corrected, out);
              break;
            case EQUALIZE_PPT:
              equalize_ppt(in_corrected, out);
              break;
            case EQUALIZE_STA:
              equalize_sta(in_corrected, out);
              break;
            case EQUALIZE_DD:
              equalize_dd(in_corrected, out);
              break;
            case EQUALIZE_LI:
              equalize_li(in_corrected, out);
              break;
            case EQUALIZE_LPI:
              equalize_lpi(in_corrected, out);
              break;
            case EQUALIZE_PPT_LPI:
              equalize_ppt_lpi(in_corrected, out);
              break;
            case EQUALIZE_PPT_DD:
              equalize_ppt_dd(in_corrected, out);
              break;
            case EQUALIZE_LPI2:
              equalize_lpi2(in_corrected, out);
              break;
            case EQUALIZE_TEST:
              equalize_test(in_corrected, out);
              break;
          }

          in += 64;
          out += 48;

          // add info tag at the end of payload
          if (d_compute_sfo) {
            // only add for payload chain (polarity index > 0)
            if (d_polarity_index > 0 && d_polarity_index == d_n_ofdm_syms) {
              add_item_tag(0, tr.abs_first_itemidx(k) + n,
                d_info_tag_key, info_tag::create().set_sfo(d_sfo));
            }
          }

          if (d_propagate_channel_states) {
            if (d_method == EQUALIZE_LPI2) {
              add_item_tag(0, tr.abs_first_itemidx(k) + n,
                d_channel_states_tag_key,
                channel_states_tag::create()
                  .set_phase_offset(d_phase_offset)
                  .set_freq_offset(d_freq_offset)
                  .set_channel_states(G));
            }
            else {
              add_item_tag(0, tr.abs_first_itemidx(k) + n,
                d_channel_states_tag_key,
                channel_states_tag::create()
                  .set_phase_offset(d_phase_offset)
                  .set_freq_offset(d_freq_offset)
                  .set_channel_states(d_channel_states));
            }
          }

          d_polarity_index = (d_polarity_index+1) % d_polarity_period;
        }
      }

      return noutput_items;
    }

    const int ofdm_equalizer_impl::POLARITY[127] = {
         1, 1, 1, 1,-1,-1,-1, 1,-1,-1,-1,-1, 1, 1,-1, 1,
        -1,-1, 1, 1,-1, 1, 1,-1, 1, 1, 1, 1, 1, 1,-1, 1,
         1, 1,-1, 1, 1,-1,-1, 1, 1, 1,-1, 1,-1,-1,-1, 1,
        -1, 1,-1,-1, 1,-1,-1, 1, 1, 1, 1, 1,-1,-1, 1, 1,
        -1,-1, 1,-1, 1,-1, 1, 1,-1,-1,-1, 1, 1,-1,-1,-1,
        -1, 1,-1,-1, 1,-1, 1, 1, 1, 1,-1, 1,-1, 1,-1, 1,
        -1,-1,-1,-1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1, 1,-1,
        -1, 1,-1,-1,-1, 1, 1, 1,-1,-1,-1,-1,-1,-1,-1 };

  }
}