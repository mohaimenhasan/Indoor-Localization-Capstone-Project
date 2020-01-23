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

#ifndef INCLUDED_SWIFI_SCRAMBLING_H
#define INCLUDED_SWIFI_SCRAMBLING_H

#include <stdexcept>

namespace gr {
  namespace swifi {

    class scrambling_base {
      public:

        /*!
         * \brief Create an instance given the initial content of the shift register.
         *
         * \param shiftreg initial content of the shift register
         */
        scrambling_base(char shiftreg = 0) : d_shiftreg(shiftreg) { }

        /*!
         * \brief Virtual destructor.
         */
        virtual ~scrambling_base() { }

        /*!
         * \brief Return the current content of the shift register.
         */
        virtual char shiftreg() const { 
          return d_shiftreg; 
        }

        /*!
         * \brief Set the current content of the shift register.
         *
         * \param shiftreg new content of the shift register
         */
        virtual void set_shiftreg(char shiftreg) { 
          d_shiftreg = shiftreg; 
        }

        /*!
         * \brief Execute the circuit given the single input bit.
         *
         * \param bit input bit
         */
        virtual char execute(char bit) = 0;

      protected:
        char d_shiftreg;  //!< current content of the shift register
    };

    class ieee80211b_scrambler : public scrambling_base
    {
      public:

        /*!
         * \brief Create an instance of IEEE 802.11b scrambler.
         *
         * \param shiftreg initial content of the shift register
         */
        ieee80211b_scrambler(char shiftreg = 0)
          : scrambling_base(shiftreg) 
        {
        }

        /*!
         * \brief Scramble the given input bit.
         *
         * \param bit input bit
         */
        virtual char execute(char bit) {
          throw std::runtime_error("ieee80211b_scrambler not yet implemented");
        }

    };

    class ieee80211b_descrambler : public scrambling_base
    {
      public:

        /*!
         * \brief Create an instance of IEEE 802.11b descrambler.
         *
         * \param shiftreg initial content of the shift register
         */
        ieee80211b_descrambler(char shiftreg = 0)
          : scrambling_base(shiftreg)
        {
        }

        /*!
         * \brief Descramble the given input bit.
         *
         * \param bit input bit
         */
        virtual char execute(char bit) {
          d_shiftreg = (d_shiftreg << 1) | bit;
          return (bit ^ (d_shiftreg >> 4) ^ (d_shiftreg >> 7)) & 1;
        }
    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_SCRAMBLING_H */

