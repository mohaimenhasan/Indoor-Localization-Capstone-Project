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

#include "xml_reader.h"
#include "rapidxml.hpp"
#include <fstream>
#include <sstream>
#include <stdio.h>

namespace gr {
  namespace swifi {

    XMLNode XMLReader::read(const std::string &filename, const std::string &rootname) {
      std::ifstream f(filename.c_str());
      std::stringstream ss;
      ss << f.rdbuf();
      return parse(ss.str());
    }

    static std::string trim(const std::string &s) {
      int start = 0, end;
      while (s[start] == ' ' || s[start] == '\n' || s[start] == '\t') start++;
      for (end = s.size() - 1; end > start; end--)
        if (s[end] != ' ' && s[end] != '\n' && s[end] != '\t')
          break;
      return s.substr(start, end - start + 1);
    }

    static void retrieve(XMLNode &root, rapidxml::xml_node<> &rapidnode) {
      rapidxml::xml_node<> *node = rapidnode.first_node();
      while (node) {
        if (strlen(node->name()) == 0) {
          root.set_value(trim(node->value()));  // rapidxml treats the text value of a node as the first node
        }
        else {
          root.add(node->name());
          XMLNode &child = root.get_last_child();
          for (rapidxml::xml_attribute<> *attr = node->first_attribute();
            attr; attr = attr->next_attribute())
          {
            child.set_attr(attr->name(), attr->value());
          }
          retrieve(child, *node);
        }
        node = node->next_sibling();
      }
    }

    XMLNode XMLReader::parse(const std::string &data, const std::string &rootname) {
      XMLNode root(rootname);

      rapidxml::xml_document<> doc;
      doc.parse<0>(const_cast<char*>(data.data()));

      retrieve(root, doc);

      return root;
    }

  } /* namespace swifi */
} /* namespace gr */
