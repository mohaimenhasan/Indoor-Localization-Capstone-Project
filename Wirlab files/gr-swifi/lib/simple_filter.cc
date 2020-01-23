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

#include "simple_filter.h"
#include "query_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdexcept>

namespace gr {
  namespace swifi {

    enum ArithmeticOp {
      ARITH_ADD, ARITH_SUB, ARITH_MUL, ARITH_DIV
    };

    enum ComparisonOp {
      CMP_EQ, CMP_NE, CMP_LT, CMP_GT, CMP_LE, CMP_GE
    };

    simple_filter::simple_filter(const std::string &filter_string) 
    {
      query = query_parser::parse(filter_string);
    }

    bool simple_filter::filter(const XMLNode &frame) {
      // query.print();  // debug only
      std::string result = evaluate(query, frame);
      return str2bool(result);
    }

    std::string simple_filter::evaluate(const XMLNode &frame) {
      return evaluate(query, frame);
    }

    /*!
     * Compute the result of an expression represented by
     * the syntax tree given by ASTNode.
     *
     * Supported operations are those specified in query_parser.
     *
     * The result is always a string containing either the
     * arithmetic operations result (as number) or logical
     * operations result (as boolean).
     */
    std::string simple_filter::evaluate(ASTNode &root, const XMLNode &frame) {
      if (!root.left && !root.right) {
        return get_value(root.value, frame);
      }

      std::string left, right;
      if (root.left) left = evaluate(*root.left, frame);
      if (root.right) right = evaluate(*root.right, frame);

      bool bool_res;

      // logical
      if (root.value == "!" || root.value == "not") 
        bool_res = !str2bool(right);
      else if (root.value == "&&" || root.value == "and")
        bool_res = str2bool(left) && str2bool(right);
      else if (root.value == "||" || root.value == "or")
        bool_res = str2bool(left) || str2bool(right);

      // comparison
      else if (root.value == "==") bool_res = cmp(CMP_EQ, left, right);
      else if (root.value == "!=") bool_res = cmp(CMP_NE, left, right);
      else if (root.value == "<") bool_res = cmp(CMP_LT, left, right);
      else if (root.value == ">") bool_res = cmp(CMP_GT, left, right);
      else if (root.value == "<=") bool_res = cmp(CMP_LE, left, right);
      else if (root.value == ">=") bool_res = cmp(CMP_GE, left, right);

      // arithmetic
      else if (root.value == "+") return arith(ARITH_ADD, left, right);
      else if (root.value == "-") return arith(ARITH_SUB, left, right);
      else if (root.value == "*") return arith(ARITH_MUL, left, right);
      else if (root.value == "/") return arith(ARITH_DIV, left, right);

      // not implemented
      else {
        bool_res = false;
        printf("simple_filter: operator not implemented ");
        root.print();
        printf("\n");
        fflush(stdout);
      }

      return bool_res ? "true" : "false";
    }

    /*!
     * Compare left and right hand sides of the given operand.
     *
     * Two cases are considered: number comparison and string comparison.
     * Number comparison is handled only if both sides are numeric.
     * Otherwise string comparison is performed.
     */
    bool simple_filter::cmp(int op, const std::string &left,
      const std::string &right)
    {
      if (is_numeric(left) && is_numeric(right)) {
        switch (op) {
          case CMP_EQ:
            return (str2double(left) == str2double(right));
          case CMP_NE:
            return (str2double(left) != str2double(right));
          case CMP_LT:
            return (str2double(left) < str2double(right));
          case CMP_GT:
            return (str2double(left) > str2double(right));
          case CMP_LE:
            return (str2double(left) <= str2double(right));
          case CMP_GE:
            return (str2double(left) >= str2double(right));
        }
      }
      else {
        switch (op) {
          case CMP_EQ:
            return (left == right);
          case CMP_NE:
            return (left != right);
          case CMP_LT:
            return (left < right);
          case CMP_GT:
            return (left > right);
          case CMP_LE:
            return (left <= right);
          case CMP_GE:
            return (left >= right);
        }
      }

      return false;
    }

    /*!
     * Compute arithmetic operations (addition, subtraction,
     * multiplicatoin, division) between 2 numbers on left and right
     * hand sides of the operand.
     *
     * Two cases are considered: real and integer numbers.
     * Only if both numbers are integer, an integer operation is performed.
     * Otherwise a real operation is carried out.
     */
    std::string simple_filter::arith(int op, const std::string &left,
      const std::string &right) 
    {
      std::stringstream ss;
      if (is_double(left) || is_double(right)) {
        switch (op) {
          case ARITH_ADD:
            ss << (str2double(left) + str2double(right));
            break;
          case ARITH_SUB:
            ss << (str2double(left) - str2double(right));
            break;
          case ARITH_MUL:
            ss << (str2double(left) * str2double(right));
            break;
          case ARITH_DIV:
            {
              double r = str2double(right);
              if (r == 0)
                ss << "0";
              else
                ss << (str2double(left) / r);
            }
            break;
        }
      }
      else {
        switch (op) {
          case ARITH_ADD:
            ss << (str2long(left) + str2long(right));
            break;
          case ARITH_SUB:
            ss << (str2long(left) - str2long(right));
            break;
          case ARITH_MUL:
            ss << (str2long(left) * str2long(right));
            break;
          case ARITH_DIV:
            {
              long r = str2long(right);
              if (r == 0)
                ss << "0";
              else
                ss << (str2long(left) / r);
            }
            break;
        }
      }

      return ss.str();
    }

    /*!
     * A string is considered numeric if it only contains dots or
     * decimal digits and possibly starts with either plus/minus sign.
     */
    bool simple_filter::is_numeric(const std::string &s) {
      for (int i = 0; i < s.size(); i++) {
        if (!( (s[i] >= '0' && s[i] <= '9') || 
               (s[i] == '.') || 
               (i == 0 && (s[i] == '-' || s[i] == '+'))))
          return false;
      }
      return true;
    }

    /*!
     * Simply check: if string contains a dot, it is of double type.
     */
    bool simple_filter::is_double(const std::string &s) {
      return s.find('.') != -1;
    }

    /*!
     * Convert string to double.
     */
    double simple_filter::str2double(const std::string &s) {
      return atof(s.c_str());
    }

    /*!
     * Convert string to long.
     */
    long simple_filter::str2long(const std::string &s) {
      return atol(s.c_str());
    }

    /*!
     * A string is equal to false if it is non-empty
     * and contains either "false" or "0".
     * Other cases return true.
     */
    bool simple_filter::str2bool(const std::string &s) {
      return (!s.empty()) && (s != "false") && (s != "0");
    }

    /*!
     * Verify that if the given element is a string by checking
     * the double quotes at the start and end of the element.
     *
     * If it is not a string, look up the identifier in the given
     * XML tree, if found, return the value of the found node.
     *
     * Otherwise, return the given argument itself.
     */
    std::string simple_filter::get_value(const std::string &identifier,
      const XMLNode &xmlroot) 
    {
      // identifier is a string?
      if (identifier.size() >= 2 && identifier[0] == '"' 
        && identifier[identifier.size() - 1] == '"')
        return identifier.substr(1, identifier.size() - 2);

      // find child node based on hiararchical path
      std::string value;
      bool found = xmlroot.retrieve_grandchild_or_attr(value, identifier);
      if (found)
        return value;

      // otherwise return itself
      return identifier;
    }

  } /* namespace swifi */
} /* namespace gr */

