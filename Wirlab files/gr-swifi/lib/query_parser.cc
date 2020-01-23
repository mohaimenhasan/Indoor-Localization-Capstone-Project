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

#include "query_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdexcept>

namespace gr {
  namespace swifi {

/**********************************************************************/
/********************* PREDEFINED OPERATORS ***************************/
/**********************************************************************/

    #define NUM_OPS     15

    std::string _prioritized_ops[NUM_OPS] = {
      "!", "*", "/", "+", "-",      // arithmetic
      "==", "!=", "<", ">", "<=", ">=",   // comparison
      "and", "&&", "or", "||"       // logical
    };

    static std::vector<std::string> init_vector_str(const std::string str[], int size) {
      std::vector<std::string> v;
      for (int i = 0; i < size; i++)
        v.push_back(str[i]);
      return v;
    }

    std::vector<std::string> query_parser::OPS =
      init_vector_str(_prioritized_ops, NUM_OPS);



/**********************************************************************/
/********************* PREDEFINED OPERATORS ***************************/
/**********************************************************************/

    void ASTNode::print() {
      if (left || right) printf("(");
      if (left) left->print();
      printf("%s", value.c_str());
      if (right) right->print();
      if (left || right) printf(")");
    }



/**********************************************************************/
/************************** REDUCTION TASKS ***************************/
/**********************************************************************/


    void query_parser::reduce_parentheses(std::vector<ASTNode> &nodes) {
      int level = 0, open_idx = -1, closing_idx = -1;
      for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].value == "(") {
          if (level == 0) {
            open_idx = i;
          }
          level++;
        }
        else if (nodes[i].value == ")") {
          level--;
          if (level == 0) {
            closing_idx = i;

            // reduce inside nodes
            std::vector<ASTNode> inside_nodes(
              nodes.begin() + (open_idx+1), 
              nodes.begin() + closing_idx);
            ASTNode newnode = reduce(inside_nodes);

            // replace parentheses with reduced results
            nodes.erase(nodes.begin() + open_idx,
              nodes.begin() + (closing_idx+1));
            nodes.insert(nodes.begin() + open_idx, newnode);
          }
        }
      }

    }

    ASTNode query_parser::reduce_operators(std::vector<ASTNode> &nodes,
      const std::vector<std::string> operators)
    {
      for (int k = operators.size() - 1; k >= 0; k--) {
        for (int i = 0; i < nodes.size(); i++) {
          if (!nodes[i].visited && nodes[i].value == operators[k]) {
            ASTNode newnode(operators[k], true);

            // reduce nodes on the left
            std::vector<ASTNode> left_nodes(
              nodes.begin(),
              nodes.begin() + i);
            newnode.left.reset(new ASTNode());
            if (left_nodes.size())
              *newnode.left = reduce(left_nodes);

            // reduce nodes on the right
            std::vector<ASTNode> right_nodes(
              nodes.begin() + (i + 1),
              nodes.end());
            newnode.right.reset(new ASTNode());
            if (right_nodes.size())
              *newnode.right = reduce(right_nodes);

            return newnode;
          }
        }
      }

      if (nodes.size() > 0)
        return nodes[0];
      else
        throw std::runtime_error("Syntax error");
    }

    ASTNode query_parser::reduce(std::vector<ASTNode> &nodes) {
      reduce_parentheses(nodes);
      return reduce_operators(nodes, OPS);
    }



/**********************************************************************/
/**************************** TOKENIZING ******************************/
/**********************************************************************/


    enum TokenType {
      WHITE, OPERATOR, IDENTIFIER, PARENTHESES
    };


    void query_parser::tokenize(const std::string &query,
      std::vector<ASTNode> &nodes)
    {
      std::string token;
      int token_type = WHITE;

      // assume 0-terminated query string
      for (int i = 0; i <= query.size(); i++) {
        switch (query[i]) {
          case 0:     // end of query

          case ' ':   // white spaces
          case '\t':
            {
              if (token_type != WHITE) {
                nodes.push_back(ASTNode(token));
                token.clear();
              }
              token_type = WHITE;
            }
            break;

          // special characters: they can sit next to parentheses or identifiers
          // Note: multiple special characters form a single token (e.g., "<=" is a single token)
          case '<': case '>': case '=': 
          case '!': case '&': case '|':
          case '+': case '-': case '*': case '/':
            {
              if (token_type != OPERATOR && token_type != WHITE) {
                nodes.push_back(ASTNode(token));
                token.clear();
              }
              token_type = OPERATOR;
              token.append(1, query[i]);
            }
            break;

          // parentheses are special: no gaps needed between parentheses and other types of token
          case '(': case ')':
            {
              if (token_type != WHITE) {
                nodes.push_back(ASTNode(token));
                token.clear();
              }
              token_type = PARENTHESES;
              token.append(1, query[i]);
            }
            break;

          // all others are identifiers (including operators "and", "or")
          default:
            {
              if (token_type != IDENTIFIER && token_type != WHITE) {
                nodes.push_back(ASTNode(token));
                token.clear();
              }
              token_type = IDENTIFIER;
              token.append(1, query[i]);
            }
            break;
        }
      }
    }


    ASTNode query_parser::parse(const std::string &query) {
      std::vector<ASTNode> nodes;

      tokenize(query, nodes);

      if (nodes.size() == 0)
        return ASTNode("true");

      ASTNode root = reduce(nodes);
      // printf("Reduce: ");
      // root.print();
      // printf("\n");
      // fflush(stdout);

      return root;
    }
  } /* namespace swifi */
} /* namespace gr */

