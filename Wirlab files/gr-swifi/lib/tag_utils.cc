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

#include <swifi/tag_utils.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

namespace gr {
  namespace swifi {

    /***************************************************************************/
    /********** Tag range related methods and classes implementation ***********/
    /***************************************************************************/

    tag_range::tag_range(long nitems_read, int ninput_items,
      const std::vector<tag_t> &tags)
      : d_tags(tags), d_nitems_read(nitems_read)
    {
      tag_t none_tag;
      if (d_tags.size() == 0 || d_tags[0].offset != nitems_read) {
        none_tag.offset = nitems_read;
        none_tag.key = pmt::PMT_NIL;
        none_tag.value = pmt::PMT_NIL;
        d_tags.insert(d_tags.begin(), none_tag);
      }
      if (d_tags.back().offset != nitems_read + ninput_items) {
        none_tag.offset = nitems_read + ninput_items;
        d_tags.push_back(none_tag);
      }

      for (int i = 0; i < size(); i++) {
        d_abs_last_idx.push_back(d_tags[i + 1].offset);
        d_rel_last_idx.push_back(d_abs_last_idx[i] - d_nitems_read);
        d_rel_first_idx.push_back(d_tags[i].offset - d_nitems_read);
        d_range_nitems.push_back(d_abs_last_idx[i] - d_tags[i].offset);
      }
    }

    int tag_range::size() {
      return d_tags.size() - 1;
    }

    long tag_range::abs_first_itemidx(int range_index) {
      return d_tags[range_index].offset;
    }

    long tag_range::abs_last_itemidx(int range_index) {
      return d_abs_last_idx[range_index];
    }

    int tag_range::rel_first_itemidx(int range_index) {
      return d_rel_first_idx[range_index];
    }

    int tag_range::rel_last_itemidx(int range_index) {
      return d_rel_last_idx[range_index];
    }

    int tag_range::range_nitems(int range_index) {
      return d_range_nitems[range_index];
    }

    tag_t& tag_range::tag(int range_index) {
      return d_tags[range_index];
    }



    /**********************************************************************/
    /********************* Channel states tag  ****************************/
    /**********************************************************************/

    channel_states_tag::operator pmt::pmt_t() const {
      return pmt::make_tuple(
          pmt::from_double(d_phase_offset),
          pmt::from_double(d_freq_offset),
          pmt::init_c32vector(d_channel_states.size(), d_channel_states));
    }

    bool channel_states_tag::parse(const pmt::pmt_t &v, channel_states_tag &t) {
      if (pmt::is_tuple(v)) {
        t.set_phase_offset((float)pmt::to_double(pmt::tuple_ref(v, 0)));
        t.set_freq_offset((float)pmt::to_double(pmt::tuple_ref(v, 1)));
        t.set_channel_states(pmt::c32vector_elements(pmt::tuple_ref(v, 2)));
        return true;
      }

      return false;
    }



    /**********************************************************************/
    /***************************** PHY  tag  ******************************/
    /**********************************************************************/

    phy_tag::operator pmt::pmt_t() const {
      return pmt::make_tuple(
          pmt::from_long(d_nitems),
          pmt::from_long(d_rate),
          pmt::from_long(d_flag),
          pmt::from_long(d_rx_time),
          pmt::from_long(d_payload_sample_index),
          pmt::from_double(d_freq_offset),
          pmt::from_long(d_scrambling_seed));
    }

    bool phy_tag::parse(const pmt::pmt_t &v, phy_tag &t) {
      if (pmt::is_tuple(v)) {
        if (pmt::length(v) >= 1) t.set_nitems(pmt::to_long(pmt::tuple_ref(v, 0)));
        if (pmt::length(v) >= 2) t.set_rate(pmt::to_long(pmt::tuple_ref(v, 1)));
        if (pmt::length(v) >= 3) t.set_flag(pmt::to_long(pmt::tuple_ref(v, 2)));
        if (pmt::length(v) >= 4) t.set_rx_time(pmt::to_long(pmt::tuple_ref(v, 3)));
        if (pmt::length(v) >= 5) t.set_payload_sample_index(pmt::to_long(pmt::tuple_ref(v, 4)));
        if (pmt::length(v) >= 6) t.set_freq_offset((float)pmt::to_double(pmt::tuple_ref(v, 5)));
        if (pmt::length(v) >= 7) t.set_scrambling_seed((int)pmt::to_long(pmt::tuple_ref(v, 6)));
        return true;
      }

      return false;
    }



    /**********************************************************************/
    /**************************** Trigger tag  ****************************/
    /**********************************************************************/

    trigger_tag::operator pmt::pmt_t() const {
      return pmt::make_tuple(
          pmt::from_bool(d_state),
          pmt::from_long(d_sample_index),
          pmt::from_double(d_freq_offset));
    }

    bool trigger_tag::parse(const pmt::pmt_t &v, trigger_tag &t) {
      if (pmt::is_tuple(v)) {
        if (pmt::length(v) >= 1) t.set_state(pmt::to_bool(pmt::tuple_ref(v, 0)));
        if (pmt::length(v) >= 2) t.set_sample_index(pmt::to_long(pmt::tuple_ref(v, 1)));
        if (pmt::length(v) >= 3) t.set_freq_offset((float)pmt::to_double(pmt::tuple_ref(v, 2)));
        return true;
      }

      return false;
    }


    /**********************************************************************/
    /***************************** Info tag  ******************************/
    /**********************************************************************/

    info_tag::operator pmt::pmt_t() const {
      return pmt::make_tuple(
          pmt::from_double(d_sfo));
    }

    bool info_tag::parse(const pmt::pmt_t &v, info_tag &t) {
      if (pmt::is_tuple(v)) {
        if (pmt::length(v) >= 1) t.set_sfo((float)pmt::to_double(pmt::tuple_ref(v, 0)));
        return true;
      }

      return false;
    }


  } /* namespace swifi */
} /* namespace gr */

