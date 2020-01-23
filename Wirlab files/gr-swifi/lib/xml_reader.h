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

#ifndef INCLUDED_SWIFI_XML_READER_H
#define INCLUDED_SWIFI_XML_READER_H

#include "xml_node.h"
#include <swifi/api.h>
#include <string>

namespace gr {
  namespace swifi {

    /*!
     * \brief XML Node.
     */
    class SWIFI_API XMLReader
    {
     public:

      static XMLNode read(const std::string &filename, const std::string &rootname = "document");

      static XMLNode parse(const std::string &data, const std::string &rootname = "document");

    };
  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_XML_READER_H */

