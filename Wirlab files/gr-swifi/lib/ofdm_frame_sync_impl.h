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

#ifndef INCLUDED_SWIFI_OFDM_FRAME_SYNC_IMPL_H
#define INCLUDED_SWIFI_OFDM_FRAME_SYNC_IMPL_H

#include <swifi/ofdm_frame_sync.h>
#include <swifi/mac_utils.h>

namespace gr {
  namespace swifi {

    class ofdm_frame_sync_impl : public ofdm_frame_sync
    {
     public:
      ofdm_frame_sync_impl(float short_threshold, int min_short_plateau_len,
        float long_threshold, float avg_alpha, float drop_ratio_threshold,
        const std::string &trigger_tag_key);
      virtual ~ofdm_frame_sync_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      // Where all the action really happens
      int work (int noutput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);

     private:
      enum State { SHORT_SEARCH, LONG_SEARCH, SYNCED };
      static const gr_complex TD_LONG_SYM[64];
      static const float TD_LONG_SYM_PWR = 0.8125f;

      float d_short_threshold, d_long_threshold, d_drop_ratio_threshold;
      float d_avg_alpha, d_avg_one_minus_alpha;
      int d_min_short_plateau_len;

      pmt::pmt_t d_trigger_tag_key;
      bool d_uses_trigger_tag_key;

      int d_state;
      int d_plateau_len;
      float d_coarse_fo, d_fine_fo, d_current_phase;
      float d_signal_pwr, d_preamble_pwr;
      unsigned long d_expected_max_long_location;

      int short_search(const gr_complex *in, int nitems);
      int long_search(const gr_complex *in, int nitems, unsigned long nread);
      bool find_peak(const gr_complex *in, const gr_complex *ref, int len, float threshold);
      void fo_correction(const gr_complex *in, int nitems, float fo,
        gr_complex *out);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_OFDM_FRAME_SYNC_IMPL_H */

