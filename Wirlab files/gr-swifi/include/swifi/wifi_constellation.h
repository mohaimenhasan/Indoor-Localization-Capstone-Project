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

#ifndef INCLUDED_SWIFI_WIFI_CONSTELLATION_H
#define	INCLUDED_SWIFI_WIFI_CONSTELLATION_H

#include <swifi/api.h>
#include <swifi/constellation_metrics.h>
#include <gnuradio/digital/constellation.h>


namespace gr {
  namespace swifi {

    class SWIFI_API wifi_constellation_bpsk : public gr::digital::constellation,
      public constellation_metrics
    {
    public:
      typedef boost::shared_ptr<wifi_constellation_bpsk> sptr;

      /*!
       * Create BPSK constellation specified by IEEE 802.11a/Figure 18-10.
       
         \verbatim

                           |
                           |
                           |
                           |
                0 ------------------- 1
                           |
                           |
                           |
                           |
         \endverbatim
       */
      static sptr make();

      ~wifi_constellation_bpsk();

      unsigned int decision_maker(const gr_complex *sample);

      void bitmetrics(const gr_complex *sample, float *metrics);

    protected:
      wifi_constellation_bpsk();
    };


    class SWIFI_API wifi_constellation_qpsk : public gr::digital::constellation,
      public constellation_metrics
    {
    public:
      typedef boost::shared_ptr<wifi_constellation_qpsk> sptr;

      /*!
       * Create QPSK constellation specified by IEEE 802.11a/Figure 18-10.

         Bit order: b0b1
         \verbatim
                  01       |        11
                           |
                           |
                           |
                  --------------------
                           |
                           |
                           |
                  00       |        10
         \endverbatim
         Decimal representation:
         \verbatim
                   2       |        3
                           |
                           |
                           |
                  --------------------
                           |
                           |
                           |
                   0       |        1
         \endverbatim
       */
      static sptr make();

      ~wifi_constellation_qpsk();

      unsigned int decision_maker(const gr_complex *sample);

      void bitmetrics(const gr_complex *sample, float *metrics);

    protected:
      wifi_constellation_qpsk();
    };



    class SWIFI_API wifi_constellation_16qam : public gr::digital::constellation,
      public constellation_metrics
    {
    public:
      typedef boost::shared_ptr<wifi_constellation_16qam> sptr;

      /*!
       * Create 16-QAM constellation specified by IEEE 802.11a (Figure 18-10).
         Coordinates are normalized with scaling factor 1/sqrt(10).

         Bit order: b0b1b2b3
         \verbatim
             0010    0110  3  1110     1010
                           |
                           |
                           | 
             0011    0111  1  1111     1011
                           |
           -- -3 ---- -1 ----- 1 ------- 3 -- 
                           |
             0001    0101 -1  1101     1001
                           |
                           |
                           |
             0000    0100 -3  1100     1000
         \endverbatim
         Decimal representation:
         \verbatim
               4       6   3   7         5 
                           |
                           |
                           | 
              12      14   1   15       13 
                           |
           -- -3 ---- -1 ----- 1 ------- 3 -- 
                           |
               8      10   |   11        9
                           |
                           |
                           |
               0       2  -3   3         1
         \endverbatim
       */
      static sptr make();

      ~wifi_constellation_16qam();

      unsigned int decision_maker(const gr_complex *sample);

      void bitmetrics(const gr_complex *sample, float *metrics);

    protected:
      wifi_constellation_16qam();

      float d_midvalue;
    };

    class SWIFI_API wifi_constellation_64qam : public gr::digital::constellation,
      public constellation_metrics
    {
    public:
      typedef boost::shared_ptr<wifi_constellation_64qam> sptr;

      // public constructor
      static sptr make();

      ~wifi_constellation_64qam();

      unsigned int decision_maker(const gr_complex *sample);

      void bitmetrics(const gr_complex *sample, float *metrics);

    protected:
      wifi_constellation_64qam();

      float d_midvalue;
      char *d_regular_to_gray;
    };

  } /* namespace swifi */
} /* namespace gr */

#endif /* INCLUDED_SWIFI_WIFI_CONSTELLATION_H */
