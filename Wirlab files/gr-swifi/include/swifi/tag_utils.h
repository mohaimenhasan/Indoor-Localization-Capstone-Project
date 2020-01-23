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


#ifndef INCLUDED_SWIFI_TAG_UTILS_H
#define INCLUDED_SWIFI_TAG_UTILS_H

#include <swifi/api.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/tags.h>
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <pmt/pmt.h>
#include <stdexcept>

#ifdef SWIG
%ignore operator pmt::pmt_t;
#endif

namespace gr {
  namespace swifi {



    /*!
     * \brief A utility for creating item ranges based on specified tags found on
     * an input stream.
     * 
     * \details When constructed with given vector of tags, this class
     * will scan for the tags and split the input stream into
     * separate ranges, where each range has a tag on the first item in the
     * range.
     *
     * For range that does not have a tag, a "PMT_NIL" tag is attached.
     * Therefore, before any tag processing, the caller must check if a tag
     * is not PMT_NIL, otherwise tag processing should not be proceeded.
     */
    class SWIFI_API tag_range {
    public:

      tag_range(long nitems_read, int ninput_items,
        const std::vector<tag_t> &tags);

      /*!
       * Return number of ranges.
       */
      int size();

      /*!
       * Return the absolute index of the first item of a range.
       * The absolute index is based on the index in the input stream.
       * \param range_index which range to query.
       */
      long abs_first_itemidx(int range_index);

      /*!
       * Return a range's last item's absolute index - 1.
       * The absolute index is based on the index in the input stream.
       * \param range_index which range to query.
       */
      long abs_last_itemidx(int range_index);

      /*!
       * Return the relative index of the first item of a range.
       * The relative index is based on how many items read in the input stream
       * at the time tag_range is constructed.
       * \param range_index which range to query.
       */
      int rel_first_itemidx(int range_index);

      /*!
       * Return a range's last item's relative index - 1.
       * The relative index is based on how many items read in the input stream
       * at the time tag_range is constructed.
       * \param range_index which range to query.
       */
      int rel_last_itemidx(int range_index);

      /*!
       * Return number of items in a range.
       * \param range_index which range to query.
       */
      int range_nitems(int range_index);

      /*!
       * Return the tag attached to the first item of a range.
       * \param range_index which range to query.
       */
      tag_t& tag(int range_index);

    private:
      std::vector<tag_t> d_tags;
      std::vector<long> d_abs_last_idx;
      std::vector<int> d_rel_first_idx, d_rel_last_idx, d_range_nitems;
      long d_nitems_read;

    };







    /*!
     * Channel states tag is used for sending channel states information
     * between channel estimator and equalizer blocks.
     * The following information is carried in each channel states tag:
     *   - channel_states: estimated channels per subcarrier
     *   - freq_offset: estimated frequency offset based on pilot subcarriers
     *   - phase_offset: estimated phase offset based on pilot subcarriers
     *
     * To create channel states tag, use the empty constructor and make
     * subsequent calls to setter methods.
     * For example:
     *   channel_states_tag t = channel_states_tag().set_freq_offset(...)
     *              .set_phase_offset(...)
     *              .set_channel_states(...)
     *
     * To query attributes of channel states tag, use getter methods.
     */
    class SWIFI_API channel_states_tag {
    public:

      /*!
       * Empty constructor.
       */
      channel_states_tag() {
      }

      /*!
       * Returns the channel states attribute of the tag.
       */
      std::vector<gr_complex> &channel_states() {
        return d_channel_states;
      }

      /*!
       * Returns the channel states attribute of the tag.
       */
      const std::vector<gr_complex> &channel_states() const {
        return d_channel_states;
      }

      /*!
       * Set the channel states attribute.
       * \param channel_states value of channel states.
       * \return the object itself.
       */
      channel_states_tag &set_channel_states(const std::vector<gr_complex> &channel_states) {
        d_channel_states = channel_states;
        return *this;
      }

      /*!
       * Returns the phase offset attribute of the tag.
       */
      float phase_offset() const {
        return d_phase_offset;
      }

      /*!
       * Set the phase offset attribute.
       * \param phase_offset phase offset value.
       * \return the object itself.
       */
      channel_states_tag &set_phase_offset(float phase_offset) {
        d_phase_offset = phase_offset;
        return *this;
      }

      /*!
       * Returns the frequency offset attribute of the tag.
       */
      float freq_offset() const {
        return d_freq_offset;
      }

      /*!
       * Set the frequency offset attribute.
       * \param freq_offset frequency offset value.
       * \return the object itself.
       */
      channel_states_tag &set_freq_offset(float freq_offset) {
        d_freq_offset = freq_offset;
        return *this;
      }

      /*!
       * Return the PMT object representing the trigger tag to be
       * embedded in GNU Radio tag.
       */
      operator pmt::pmt_t() const;

      /*!
       * Static method for create an instance of channel states tag.
       * It is the same as the empty constructor.
       */
      static channel_states_tag create() {
        return channel_states_tag();
      }

      /*!
       * Static method for parsing a PMT value of a GNU Radio tag.
       * \param v PMT value of a GNU Radio tag.
       * \param t if the PMT value is a valid channel states tag, this argument
       *          will be instantiate to the parsed channel states tag object.
       *          Otherwise, t is untouched.
       * \return true if parsing is successful, false otherwise.
       */
      static bool parse(const pmt::pmt_t &v, channel_states_tag &t);

    private:
      float d_phase_offset, d_freq_offset;
      std::vector<gr_complex> d_channel_states;
    };







    /*!
     * Define flags used for PHY tag.
     */
    const long PHY_SCRAMBLER_FLAG = 1;
    const long PHY_TAIL_SET_FLAG = 2;
    const long PHY_HEADER_FLAG = 4;


    /*!
     * Phy tag is used for transfer meta information between blocks
     * in SWiFi flow graph.
     * The following information is carried in each PHY tag:
     *   - nitems: size of the PSDU.
     *   - rate: rate specified in the SIGNAL field.
     *   - flag: contains various flags for the processing ()
     *           PHY_SCRAMBLER_FLAG   if set, scrambling/descrambling is required
     *           PHY_TAIL_SET_FLAG    if set, tail bits are at the end (this is used for payload)
     *           PHY_HEADER_FLAG      if set, the current stream is the header stream
     *   - rx_time: received time, measured by the index of the first sample
     *              of the frame (i.e., first sample of preamble) in the received stream
     *   - freq_offset: estimated frequency offset during frame synchronization
     *   - scrambling_seed: detected scrambling seed used by the transmitter
     *
     * To create phy tag, use the empty constructor and make subsequent
     * calls to setter methods.
     * For example:
     *   phy_tag t = phy_tag().set_nitems(...).set_rate(...)
     *
     * To query attributes of phy tag, use getter methods.
     */
    class SWIFI_API phy_tag {
    public:

      /*!
       * Empty constructor.
       */
      phy_tag() {
      }

      /*!
       * Returns the PSDU size attribute of the PHY tag.
       */
      int nitems() const {
        return d_nitems;
      }

      /*!
       * Set the PSDU size attribute of the PHY tag.
       * \param nitems the PSDU size.
       * \return the object itself.
       */
      phy_tag &set_nitems(int nitems) {
        d_nitems = nitems;
        return *this;
      }

      /*!
       * Returns the rate attribute of the PHY tag.
       */
      int rate() const {
        return d_rate;
      }

      /*!
       * Set the rate attribute of the PHY tag.
       * \param rate the rate of the transmission.
       * \return the object itself.
       */
      phy_tag &set_rate(int rate) {
        d_rate = rate;
        return *this;
      }

      /*!
       * Returns the flag attribute of the PHY tag.
       */
      long flag() const {
        return d_flag;
      }

      /*!
       * Returns true if the given flag is present.
       */
      bool flag(long check_flag) const {
        return bool(d_flag & check_flag);
      }

      /*!
       * Set the flag attribute of the PHY tag.
       * \param flag the flag of the PHY tag.
       * \return the object itself.
       */
      phy_tag &set_flag(long flag) {
        d_flag = flag;
        return *this;
      }

      /*!
       * Returns the received time of the frame.
       */
      unsigned long rx_time() const {
        return d_rx_time;
      }

      /*!
       * Set the received time attribute of the PHY tag.
       * \param rx_time the received time of measured as
       *                the index of the preamble's first sample.
       * \return the object itself.
       */
      phy_tag &set_rx_time(unsigned long rx_time) {
        d_rx_time = rx_time;
        return *this;
      }

      /*!
       * Returns the index of the payload's first sample.
       */
      unsigned long payload_sample_index() const {
        return d_payload_sample_index;
      }

      /*!
       * Set the payload sample index attribute of the PHY tag.
       * \param payload_sample_index the index of the payload's first sample.
       * \return the object itself.
       */
      phy_tag &set_payload_sample_index(unsigned long payload_sample_index) {
        d_payload_sample_index = payload_sample_index;
        return *this;
      }


      /*!
       * Return frequency offset attribute of the PHY tag.
       */
      float freq_offset() const {
        return d_freq_offset;
      }

      /*!
       * Set the frequency offset attribute of the PHY tag.
       * \param freq_offset frequency offset estimated based on the preamble.
       * \return the object itself.
       */
      phy_tag &set_freq_offset(float freq_offset) {
        d_freq_offset = freq_offset;
        return *this;
      }

      /*!
       * Return the scrambling seed used by the transmitter.
       */
      int scrambling_seed() const {
        return d_scrambling_seed;
      }

      /*!
       * Set the scrambling seed attribute of the PHY tag.
       * \param scrambling_seed scrambling seed.
       * \return the object itself.
       */
      phy_tag &set_scrambling_seed(int scrambling_seed) {
        d_scrambling_seed = scrambling_seed;
        return *this;
      }

      /*!
       * Return the PMT object representing the PHY tag to be
       * embedded in GNU Radio tag.
       */
      operator pmt::pmt_t() const;

      /*!
       * Static method for create an instance of phy tag.
       * It is the same as the empty constructor.
       */
      static phy_tag create() {
        return phy_tag();
      }

      /*!
       * Static method for parsing a PMT value of a GNU Radio tag.
       * \param v PMT value of a GNU Radio tag.
       * \param t if the PMT value is a valid phy tag, this argument
       *          will be instantiate to the parsed phy tag object.
       *          Otherwise, t is untouched.
       * \return true if parsing is successful, false otherwise.
       */
      static bool parse(const pmt::pmt_t &v, phy_tag &t);

    private:
      int d_nitems, d_rate;
      long d_flag;
      unsigned long d_rx_time, d_payload_sample_index;
      float d_freq_offset;
      int d_scrambling_seed;
    };






    /*!
     * Trigger tag is used for frame synchronization block to
     * pass trigger state to the header/payload demux block.
     * In addition, other information can also be passed such as
     *   - state, which indicates the frame beginning or the frame end
     *   - sample index, where the preamble starts
     *   - frequency offset, which was estimated during preamble detection
     *
     * To create trigger tag, use the empty constructor and make
     * subsequent calls to setter methods.
     * For example:
     *   trigger_tag t = trigger_tag()
     *                    .set_state(...)
     *                    .set_sample_index(...)
     *
     * To query attributes of trigger tag, use getter methods.
     */
    class SWIFI_API trigger_tag {
    public:

      /*!
       * Empty constructor.
       */
      trigger_tag() {
      }

      /*!
       * Return state attribute of the trigger tag.
       */
      bool state() const {
        return d_state;
      }

      /*!
       * Set the state attribute of the trigger tag.
       * \param state true indicates a frame is found,
       *              false indicates a frame end.
       * \return the object itself.
       */
      trigger_tag &set_state(bool state) {
        d_state = state;
        return *this;
      }

      /*!
       * Return sample index attribute of the trigger tag.
       */
      unsigned long sample_index() const {
        return d_sample_index;
      }

      /*!
       * Set the sample index attribute of the trigger tag.
       * \param sample_index indicates the absolute position of the first
       *                     sample of the preamble.
       * \return the object itself.
       */
      trigger_tag &set_sample_index(unsigned long sample_index) {
        d_sample_index = sample_index;
        return *this;
      }

      /*!
       * Return frequency offset attribute of the trigger tag.
       */
      float freq_offset() const {
        return d_freq_offset;
      }

      /*!
       * Set the frequency offset attribute of the trigger tag.
       * \param freq_offset frequency offset estimated based on the preamble.
       * \return the object itself.
       */
      trigger_tag &set_freq_offset(float freq_offset) {
        d_freq_offset = freq_offset;
        return *this;
      }

      /*!
       * Return the PMT object representing the trigger tag to be
       * embedded in GNU Radio tag.
       */
      operator pmt::pmt_t() const;

      /*!
       * Static method for create an instance of trigger tag.
       * It is the same as the empty constructor.
       */
      static trigger_tag create() {
        return trigger_tag();
      }

      /*!
       * Static method for parsing a PMT value of a GNU Radio tag.
       * \param v PMT value of a GNU Radio tag.
       * \param t if the PMT value is a valid trigger tag, this argument
       *          will be instantiate to the parsed trigger tag object.
       *          Otherwise, t is untouched.
       * \return true if parsing is successful, false otherwise.
       */
      static bool parse(const pmt::pmt_t &v, trigger_tag &t);


    private:
      int d_state;
      unsigned long d_sample_index;
      float d_freq_offset;
    };



    /*!
     * Info tag is used for add information to downstream blocks.
     * The following information is carried in each info tag:
     *   - sfo: sampling frequency offset.
     *
     * To create info tag, use the empty constructor and make subsequent
     * calls to setter methods.
     * For example:
     *   info_tag t = info_tag().set_sfo(...)
     *
     * To query attributes of info tag, use getter methods.
     */
    class SWIFI_API info_tag {
    public:

      /*!
       * Empty constructor.
       */
      info_tag() {
      }

      /*!
       * Return sampling frequency offset attribute of the info tag.
       */
      float sfo() const {
        return d_sfo;
      }

      /*!
       * Set the frequency offset attribute of the info tag.
       * \param sfo frequency offset estimated based on the preamble.
       * \return the object itself.
       */
      info_tag &set_sfo(float sfo) {
        d_sfo = sfo;
        return *this;
      }

      /*!
       * Return the PMT object representing the info tag to be
       * embedded in GNU Radio tag.
       */
      operator pmt::pmt_t() const;

      /*!
       * Static method for create an instance of info tag.
       * It is the same as the empty constructor.
       */
      static info_tag create() {
        return info_tag();
      }

      /*!
       * Static method for parsing a PMT value of a GNU Radio tag.
       * \param v PMT value of a GNU Radio tag.
       * \param t if the PMT value is a valid info tag, this argument
       *          will be instantiate to the parsed info tag object.
       *          Otherwise, t is untouched.
       * \return true if parsing is successful, false otherwise.
       */
      static bool parse(const pmt::pmt_t &v, info_tag &t);

    private:
      float d_sfo;
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_TAG_UTILS_H */

