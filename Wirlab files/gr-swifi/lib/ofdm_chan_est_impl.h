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

#ifndef INCLUDED_SWIFI_OFDM_CHAN_EST_IMPL_H
#define INCLUDED_SWIFI_OFDM_CHAN_EST_IMPL_H

#include <swifi/ofdm_chan_est.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/blocks/control_loop.h>
#include <swifi/wifi_constellation.h>

namespace gr {
  namespace swifi {

    class ofdm_chan_est_impl : public ofdm_chan_est
    {
     private:
      std::vector<std::vector<gr_complex> > d_sync_ofdm_symbols;
      std::vector<std::vector<bool> > d_nonzero_sync_ofdm_symbols;
      std::vector<std::vector<gr_complex> > d_diff_sync;
      float d_alpha, d_one_minus_alpha;
      pmt::pmt_t d_channel_states_tag_key, d_len_tag_key;
      bool d_uses_len_tag_key, d_copy_sync;
      int d_total_ofdm_symbols, d_num_sync_symbols, d_fft_len, d_sym_idx;
      std::vector<tag_t> d_tags_on_sync_symbols;

      std::vector<gr_complex> d_channel_states;
      int d_method;

      gr::fft::fft_complex *d_fft;
      blocks::control_loop d_pll;
      digital::constellation_sptr d_constellation;
      std::vector<gr_complex> d_points;

      // frequency offset correction
      gr_complex d_foc_sum;
      float d_fo;
      blocks::control_loop *d_sc_pll;

      void fo_correction(const gr_complex *in, gr_complex *out);
      void estimate_ls(const gr_complex *in);
      void estimate_ls_foc(const gr_complex *in);
      void estimate_lpi(const gr_complex *in);

     public:

      ofdm_chan_est_impl(int fft_len,
        const std::vector<std::vector<gr_complex> > &sync_ofdm_symbols,
        float alpha,
        const std::string &channel_states_tag_key,
        const std::string &len_tag_key,
        int total_ofdm_symbols,
        bool copy_sync_symbols);
      ~ofdm_chan_est_impl();

      void set_estimation_method(int method);

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      // Where all the action really happens
      int general_work(int noutput_items,
          gr_vector_int& ninput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_OFDM_CHAN_EST_IMPL_H */

