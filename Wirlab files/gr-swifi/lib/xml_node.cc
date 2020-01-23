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

#include "xml_node.h"
#include <string.h>
#include <stdio.h>

namespace gr {
  namespace swifi {

    int XMLNode::indentation = 2;

    std::string XMLNode::escape_tag(const std::string &s) const {
      std::string r;
      for (int i = 0; i < s.size(); i++) {
        if (s[i] == '<')
          r.append("&lt;");
        else if (s[i] == '>')
          r.append("&gt;");
        else
          r.append(1, s[i]);
      }
      return r;
    }

    std::string XMLNode::escape_quote(const std::string &s) const {
      std::string r;
      for (int i = 0; i < s.size(); i++) {
        // we omit '"' because rapidxml does not handle attribute value
        // with double quote inside (even escaped double quote)
        if (s[i] != '"')
          r.append(1, s[i]);
      }
      return r;
    }

    std::string XMLNode::str(int level) const {
      // set identation
      std::string s(level * indentation, ' ');

      // begin tag
      s.append("<").append(name);

      // attributes
      if (attr.size()) {
        for (std::map<std::string,std::string>::const_iterator it = attr.begin();
          it != attr.end(); ++it) 
        {
          s.append(" ").append(it->first).append("=\"")
            .append(escape_quote(it->second)).append("\"");
        }
      }

      s.append(">").append(escape_tag(value));

      // children tags
      if (children.size()) {
        s.append("\n");
        for (int i = 0; i < children.size(); i++)
          s.append(children[i].str(level + 1));
        s.append(std::string(level * indentation, ' '));
      }

      // end tag
      s.append("</").append(name).append(">\n");

      return s;
    }
  } /* namespace swifi */
} /* namespace gr */

