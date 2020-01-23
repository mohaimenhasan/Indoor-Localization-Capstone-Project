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

#ifndef INCLUDED_SWIFI_SIMPLE_FILTER_H
#define INCLUDED_SWIFI_SIMPLE_FILTER_H

#include "xml_node.h"
#include "query_parser.h"
#include <swifi/api.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

namespace gr {
  namespace swifi {

    /*!
     * \brief Simple frame filter.
     * \details The simple frame filter supports a simple expression
     * syntax for filtering the frames.
     *
     * Example: "frame_control.subtype==ACK" approves only ACK frames.
     * 
     * Logical and arithmetic expressions are also supported.
     * Example: "frame_control.subtype==DATA and mpdu_size < 1000" returns
     *    frames that are data packets and shorter than 1000 bytes
     *
     * Currently the following operations are supported:
     *
     *   Arithmetic: +  -  *  /
     *
     *   Comparison: ==  !=  >  <  >=  <=
     *       Both numeric and string comparison are handled.
     *
     *   Logical: and or not
     *       Logical aliases: && || !
     */
    class SWIFI_API simple_filter
    {
     public:

      /*!
       * Create an instance of simple frame filter with a filter string.
       * \param filter_string filter string.
       */
      simple_filter(const std::string &filter_string);

      /*!
       * Return filter result based on the filter string (initialized
       * at constructor) and the given frame as an XML tree.
       * \param frame frame as an XML tree.
       * \return true if the frame meets the filtering condition, false otherwise.
       */
      bool filter(const XMLNode &frame);

      /*!
       * Evaluate the given frame by the expression represented by the AST.
       * \param frame root node of frame to evaluate.
       */
      std::string evaluate(const XMLNode &frame);

     private:
      ASTNode query;

      std::string evaluate(ASTNode &root, const XMLNode &frame);

      bool cmp(int op, const std::string &left,
        const std::string &right);

      std::string arith(int op, const std::string &left,
        const std::string &right);

      bool is_numeric(const std::string &s);
      bool is_double(const std::string &s);
      double str2double(const std::string &s);
      long str2long(const std::string &s);
      bool str2bool(const std::string &s);

      std::string get_value(const std::string &identifier,
        const XMLNode &root);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_SIMPLE_FILTER_H */

