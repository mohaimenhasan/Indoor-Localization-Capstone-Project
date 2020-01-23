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


#ifndef INCLUDED_SWIFI_FRAME_SAMPLES_EXTRACTOR_H
#define INCLUDED_SWIFI_FRAME_SAMPLES_EXTRACTOR_H

#include <swifi/api.h>
#include <gnuradio/block.h>
#include <uhd/types/time_spec.hpp>

namespace gr {
  namespace swifi {

    /*!
     * \brief WiFi Samples Extractor extracts samples per frame
     * based on the XML description file.
     * \details The specific range of samples to be extracted is completely
     * determined by the user, who needs to specify the start (inclusive)
     * and end position (exclusive) of the range by using XML expression.
     * \ingroup swifi
     *
     */
    class SWIFI_API frame_samples_extractor : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<frame_samples_extractor> sptr;

      /*!
       * \brief Create a new instance of swifi::frame_samples_extractor
       *
       * \param xml_dump_file File name of XML dump.
       * \param filter_string Frame filter string.
       * \param start_expr Name of tag/attribute in XML file that
       *             indicates the index of the first sample to get from input stream.
       * \param end_expr Name of tag/attribute in XML file that
       *             indicates the index of the last sample to get from input stream.
       */
      static sptr make(const std::string &xml_dump_file,
        const std::string &filter_string,
        const std::string &start_expr,
        const std::string &end_expr);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_FRAME_SAMPLES_EXTRACTOR_H */

