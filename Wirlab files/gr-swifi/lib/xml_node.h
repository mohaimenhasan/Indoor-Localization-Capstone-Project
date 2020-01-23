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

#ifndef INCLUDED_SWIFI_XML_NODE_H
#define INCLUDED_SWIFI_XML_NODE_H

#include "utils.h"
#include <swifi/api.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stdint.h>

namespace gr {
  namespace swifi {

    /*!
     * \brief XML Node.
     */
    class SWIFI_API XMLNode
    {
     public:
      static int indentation;


      //------------------- Constructors --------------------------

      XMLNode(const std::string &name) {
        set_name(name);
      }

      XMLNode(const std::string &name, const XMLNode &child) {
        set_name(name);
        children.push_back(child);
      }

      XMLNode(const std::string &name, const std::vector<XMLNode> &children) {
        set_name(name);
        this->children = children;
      }

      template<class T>
      XMLNode(const std::string &name, const T &value) {
        set_name(name);
        set_value(value);
      }

      ~XMLNode() {}



      //------------------- Setters/Getters --------------------------

      std::string &get_name() {
        return name;
      }

      const std::string &get_name() const {
        return name;
      }

      void set_name(const std::string &name) {
        this->name = name;
      }

      std::string &get_value() {
        return value;
      }

      const std::string &get_value() const {
        return value;
      }

      template<class T>
      void set_value(const T &value) {
        std::stringstream ss;
        ss << value;
        this->value = ss.str();
      }



      int num_children() const {
        return children.size();
      }

      XMLNode &get_child(int i) {
        return children[i];
      }

      const XMLNode &get_child(int i) const {
        return children[i];
      }

      XMLNode &get_first_child() {
        return children.front();
      }

      const XMLNode &get_first_child() const {
        return children.front();
      }

      XMLNode &get_last_child() {
        return children.back();
      }

      const XMLNode &get_last_child() const {
        return children.back();
      }

      int num_attrs() const {
        return attr.size();
      }

      bool has_attr(const std::string &name) const {
        return attr.count(name) > 0;
      }

      std::string &get_attr(const std::string &name) {
        return attr[name];
      }

      const std::string &get_attr(const std::string &name) const {
        return attr.at(name);
      }

      std::map<std::string,std::string> &get_attrs() {
        return attr;
      }

      const XMLNode* find_child(const std::string &name) const
      {
        for (int i = 0; i < children.size(); i++) {
          if (name == children[i].name) {
            return &(children[i]);
          }
        }
        return NULL;
      }

      const XMLNode* find_grandchild(const std::string &path, 
        char delim = '.') const
      {
        // hierarchical split
        std::vector<std::string> p = strsplit(path, delim);

        // the name does not start with XML root node name
        const XMLNode *node = this;

        // match each hierarchy
        for (int i = 0; i < p.size(); i++) {
          node = node->find_child(p[i]);
          if (!node) {
            break;
          }
        }

        return node;
      }

      bool retrieve_grandchild_or_attr(std::string &value,
        const std::string &path, 
        char delim = '.', char attr_delim = '@') const
      {
        int n = path.find(attr_delim);
        if (n == -1) {
          const XMLNode *node = find_grandchild(path, delim);
          if (node) {
            value = node->get_value();
            return true;
          }
          return false;
        }

        const XMLNode *node = find_grandchild(path.substr(0, n), delim);
        if (node) {
          std::string attr_name = path.substr(n + 1);
          if (node->has_attr(attr_name)) {
            value = node->get_attr(attr_name);
            return true;
          }
        }

        return false;
      }

      //------------------- Add children --------------------------

      XMLNode &add(const std::string &name) {
        return add(XMLNode(name));
      }

      template<class T>
      XMLNode &add(const std::string &name, const T &value) {
        XMLNode child(name);
        child.set_value(value);
        children.push_back(child);
        return *this;
      }

      XMLNode &add(const XMLNode &child) {
        children.push_back(child);
        return *this;
      }

      XMLNode &add(const std::vector<XMLNode> &children) {
        this->children.insert(this->children.end(),
          children.begin(), children.end());
        return *this;
      }



      //------------------- Set attributes --------------------------

      XMLNode &set_attr(const std::string &name) {
        attr[name] = std::string();
        return *this;
      }

      template<class T>
      XMLNode &set_attr(const std::string &name, const T &value) {
        std::stringstream ss;
        ss << value;
        attr[name] = ss.str();
        return *this;
      }

      XMLNode &unset_attr(const std::string &name) {
        attr.erase(name);
      }



      //------------------- Get string representation -----------------

      std::string str(int level = 0) const;

     protected:
      std::string name;
      std::string value;
      std::map<std::string,std::string> attr;
      std::vector<XMLNode> children;

      std::string escape_tag(const std::string &s) const;
      std::string escape_quote(const std::string &s) const;
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_XML_NODE_H */

