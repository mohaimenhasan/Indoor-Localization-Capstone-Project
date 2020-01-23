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


#ifndef INCLUDED_SWIFI_BITMETRICS_COMPUTER_IMPL_H
#define INCLUDED_SWIFI_BITMETRICS_COMPUTER_IMPL_H

#include <swifi/bitmetrics_computer.h>
#include <swifi/constellation_metrics.h>

namespace gr {
  namespace swifi {

    class bitmetrics_computer_impl : public bitmetrics_computer
    {
    private:
      digital::constellation_sptr d_constellation;
      constellation_metrics::sptr d_metrics;
      int d_bpsc;

      void set_constellation(int rate);
      void set_constellation(digital::constellation_sptr c);
      void update_settings();

      void update_rate(const tag_t &tag);

      int dynamic_rate_work(int noutput_items, int ninput_items,
        const void* input, void* output);

    public:
      bitmetrics_computer_impl(const std::string& phy_tag_key);
      bitmetrics_computer_impl(digital::constellation_sptr constellation);

    };

  } /* namespace swifi */
} /* namespace gr */

#endif /* INCLUDED_SWIFI_BITMETRICS_COMPUTER_IMPL_H */
