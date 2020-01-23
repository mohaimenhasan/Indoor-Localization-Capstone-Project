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

#ifndef INCLUDED_SWIFI_QUERY_PARSER_H
#define INCLUDED_SWIFI_QUERY_PARSER_H

#include <swifi/api.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace gr {
  namespace swifi {

    struct ASTNode {
      friend class query_parser;

      std::string value;
      boost::shared_ptr<ASTNode> left, right;
      ASTNode() : visited(false) {}
      ASTNode(const std::string &value, bool visited = false)
        : value(value), visited(visited)
      {
      }

      void print();

      private:
        bool visited;
    };

    /*!
     * \brief Simple frame filter.
     */
    class SWIFI_API query_parser
    {
     public:

      static ASTNode parse(const std::string &query);

     private:
      static std::vector<std::string> OPS;

      static void tokenize(const std::string &query,
        std::vector<ASTNode> &nodes);

      static ASTNode reduce(std::vector<ASTNode> &nodes);
      static void reduce_parentheses(std::vector<ASTNode> &nodes);
      static ASTNode reduce_operators(std::vector<ASTNode> &nodes,
        const std::vector<std::string> operators);
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_QUERY_PARSER_H */

