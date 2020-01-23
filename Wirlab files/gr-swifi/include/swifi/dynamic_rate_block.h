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


#ifndef INCLUDED_SWIFI_DYNAMIC_RATE_BLOCK_H
#define INCLUDED_SWIFI_DYNAMIC_RATE_BLOCK_H

#include <swifi/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/block.h>
#include <map>

namespace gr {
  namespace swifi {

    /*!
     * This block facilitates a tag processing mode in which the special tags
     * divide the stream into multiple blocks (possibly of different size),
     * and each block is processed at different rate determined based on the
     * tag on the first item of the corresponding block.
     *
     * An example is processing received packets of different rates.
     * After parsing the header, an upstream block can now put a special tag
     * to the first item carrying the rate information. When the downstream block
     * receives this stream, based on the tag it adaptively adjusts the
     * output/input item rate.
     *
     * In comparison to GNU Radio's built-in tagged_stream_block, we don't need
     * the length information and don't need to buffer the whole chunk before
     * the actual processing can be performed. Instead, as long as no new tag is
     * found, the same rate remains and the block can consistently produce
     * items at the output.
     *
     * This block simplifies the cumbersome tag handling and propagation.
     *
     * Note: We currently only support SINGLE input SINGLE output block.
     */
    class SWIFI_API dynamic_rate_block
      : public block
    {
    public:
      // gr::swifi::dynamic_rate_block::sptr
      typedef boost::shared_ptr<dynamic_rate_block> sptr;

      /*!
       * Similar to regular block's forecast method.
       * In general, no need to override this method.
       * But for custom behavior, it can be overriden.
       */
      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      /*!
       * DO NOT override this method.
       * Instead override dynamic_rate_work()
       */
      int general_work(int noutput_items,
          gr_vector_int& ninput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);

    protected:

      dynamic_rate_block(void) {} // allows pure virtual interface sub-classes

      /*!
       * \brief Creates the dynamic_rate_block.
       * \details The dynamic_rate_block provides two modes: tag-based and regular.
       * To work in tag-based mode, specify tag_key to be anything other than empty.
       * To work in regular mode, specify tag_key to be an empty string.
       *
       * Note: We currently only support single input single output block.
       *
       * \param name name of the block
       * \param input_signature input signature of the block
       * \param output_signature output signature of the block
       * \param tag_key key of the tag
       */
      dynamic_rate_block(const std::string &name,
            gr::io_signature::sptr input_signature,
            gr::io_signature::sptr output_signature,
            const std::string &tag_key="");


      /*!
       * Override this method to handle custom tags for stream rate updating.
       * The overriding method should call set_relative_rate() to set the
       * new stream rate for this block. Note that updating rate is important for
       * tag propagation, as the rate is used for computing propagated tags' offsets.
       *
       * This method is called whenever a proper tag is found on the first item
       * of the current range on the input stream. A tag is proper when both
       * its key and value are not PMT_NIL. It is left to the overriding method
       * to validate the tag according to a custom format.
       *
       * \param tag the received tag on the first item in the current range
       *            on the input stream.
       */
      virtual void update_rate(const tag_t &tag) {} // custom update should override this

      /*!
       * \brief Propagate tags to the downstream block.
       * \details This method should always be called after consume() and
       * produce() are called, because it uses nitems_read(0) and nitems_written(0)
       * for computing the new offset of propagated tags.
       */
      virtual void propagate_tags();

      /*!
       * Actual item processing should override this method.
       * \param noutput_items maximum number of items to be produced
       * \param ninput_items number of items available on the input
       * \param input const pointer to the input stream that contains the received input values
       * \param output pointer to the output stream that contains the processed output values
       */
      virtual int dynamic_rate_work(int noutput_items,
          int ninput_items, const void* input, void* output) = 0;


      pmt::pmt_t d_tag_key;
      int d_input_itemsize, d_output_itemsize;
      uint64_t d_start_read_idx, d_start_write_idx; // used for tag propagation


    };

  } /* namespace swifi */
} /* namespace gr */

#endif /* INCLUDED_SWIFI_DYNAMIC_RATE_BLOCK_H */
