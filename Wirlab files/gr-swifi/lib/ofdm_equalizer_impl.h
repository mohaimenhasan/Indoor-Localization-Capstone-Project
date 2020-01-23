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

#ifndef INCLUDED_SWIFI_OFDM_EQUALIZER_IMPL_H
#define INCLUDED_SWIFI_OFDM_EQUALIZER_IMPL_H

#include <swifi/ofdm_equalizer.h>
#include <gnuradio/blocks/control_loop.h>
#include <gnuradio/fft/fft.h>

namespace gr {
  namespace swifi {

    class ofdm_equalizer_impl : public ofdm_equalizer
    {
     private:
      // Indicates polarity of pilot symbols according to the OFDM symbol index.
      // The polarity set is specified in IEEE 802.11.
      static const int POLARITY[127];

      // For header equalizer, this should be initialized to 1 in order to fix the polarity index.
      // For payload equalizer, this should be initialized to 127 (the size of polarity set).
      int d_polarity_period;

      // For header equalizer, this should be initialized to 0 to correspond to the first OFDM symbol for header part.
      // For payload equalizer, this should be initialized to 1 to correspond to the first OFDM symbol for payload part.
      int d_polarity_index;

      // Number of OFDM symbols in the DATA field
      int d_n_ofdm_syms;

      pmt::pmt_t d_phy_tag_key, d_channel_states_tag_key, d_info_tag_key;
      bool d_propagate_channel_states;
      float d_alpha, d_one_minus_alpha;
      float d_beta;

      // Indicate that a carrier is either a pilot or data carrier.
      // Size: FFT length.
      std::vector<bool> d_occupied_carriers;

      // Indicate that a carrier is a pilot carrier
      // Size: FFT length.
      std::vector<bool> d_pilot_carriers;

      // Indicate where the pilots are found.
      // Size: number of pilot carriers
      std::vector<int> d_pilot_location;

      // Contains known pilot symbols, each for a pilot carrier.
      // Size: number of pilot carriers
      std::vector<std::vector<gr_complex> > d_pilot_symbols;

      // Stores channel state information for each carrier
      // Size: FFT length.
      std::vector<gr_complex> d_channel_states;

      // equalization method
      int d_method; 

      float d_phase_offset, d_freq_offset;
      bool d_foc_enabled;

      bool d_compute_sfo;
      float d_sfo;
      gr_complex d_sum_sfo, d_last_a;

      // The current constellation may change via PHY tag
      digital::constellation_sptr d_constellation;

      // Stores points of the current constellation
      // Size: number of points on the constellation.
      std::vector<gr_complex> d_points;

      fft::fft_complex *d_fft, *d_ifft;
      blocks::control_loop d_pll;
      blocks::control_loop *d_sc_pll;

      std::vector<gr_complex> G;

      FILE* d_df; // debug file descriptor

      void init();
      void equalize_static(const gr_complex *in, gr_complex *out);
      void equalize_ppt(const gr_complex *in, gr_complex *out);
      void equalize_ppt_dd(const gr_complex *in, gr_complex *out);
      void equalize_sta(const gr_complex *in, gr_complex *out);
      void equalize_dd(const gr_complex *in, gr_complex *out);
      void equalize_li(const gr_complex *in, gr_complex *out);
      void linear_interpolation(int ref1_idx, int ref2_idx,
        int from_idx, int to_idx);  // all indices are inclusive
      void equalize_lpi(const gr_complex *in, gr_complex *out);
      void equalize_lpi2(const gr_complex *in, gr_complex *out);
      void equalize_ppt_lpi(const gr_complex *in, gr_complex *out);
      void equalize_test(const gr_complex *in, gr_complex *out);
      float compute_mse(const gr_complex *out);

      void fo_correction(const gr_complex *in, gr_complex *out);

     public:

      ofdm_equalizer_impl(const std::string& phy_tag_key,
        bool propagate_channel_states,
        const std::string &channel_states_tag_key,
        digital::constellation_sptr header_constellation);
      ~ofdm_equalizer_impl();

      void enable_foc(bool enabled);
      void set_equalization_method(int method);
      void set_equalization_method(const std::string &method);
      void set_alpha(float alpha);
      void set_beta(float beta);
      void set_debug_file(const std::string &filename);

      // Where all the action really happens
      int work(int noutput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_OFDM_EQUALIZER_IMPL_H */

