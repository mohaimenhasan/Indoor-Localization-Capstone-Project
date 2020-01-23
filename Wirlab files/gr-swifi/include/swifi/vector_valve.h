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


#ifndef INCLUDED_SWIFI_VECTOR_VALVE_H
#define INCLUDED_SWIFI_VECTOR_VALVE_H

#include <swifi/api.h>
#include <gnuradio/sync_block.h>
#include <vector>

namespace gr {
  namespace swifi {

    /*!
     * \brief Vector valve.
     * \ingroup swifi
     *
     */
    class SWIFI_API vector_valve : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<vector_valve> sptr;

      /*!
       * \brief Create a new instance of swifi::vector_valve.
       *
       * \param itemsize size of each item.
       * \param valve_map Valve map.
       */
      static sptr make(int itemsize, const std::vector<int> &valve_map);

    };

  } // namespace swifi
} // namespace gr

#endif /* INCLUDED_SWIFI_VECTOR_VALVE_H */

