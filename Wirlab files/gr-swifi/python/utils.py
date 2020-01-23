#
# Copyright 2004,2009,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr
import pmt
import sys

#########################################################################
## Helper functions for creating tags ###################################
#########################################################################

def channel_states_tag_create(phase_offset, freq_offset, channel_states):
    return pmt.make_tuple(pmt.from_double(phase_offset),
                            pmt.from_double(freq_offset),
                            pmt.init_c32vector(len(channel_states), channel_states))

def phy_tag_create(nitems = 0, rate = 0, flag = 0, rx_time = 0,
                payload_sample_index = 0):
    return pmt.make_tuple(pmt.from_long(nitems),
                            pmt.from_long(rate),
                            pmt.from_long(flag),
                            pmt.from_long(rx_time),
                            pmt.from_long(payload_sample_index))

def trigger_tag_create(state = True, sample_index = 0):
    return pmt.make_tuple(pmt.from_bool(state),
                            pmt.from_long(sample_index))

def phy_tag(offset = 0, key = 'phy',
            nitems = 0, rate = 0, flag = 0, rx_time = 0,
            payload_sample_index = 0):
    tag = gr.python_to_tag(
            { 'offset' : offset,
              'key' : pmt.intern(key),
              'srcid' : pmt.from_bool(False),
              'value' : phy_tag_create(nitems=nitems, rate=rate,
                                       flag=flag, rx_time=rx_time,
                                       payload_sample_index=payload_sample_index)
            })
    return tag



#########################################################################
## Helper functions for conversion ######################################
#########################################################################

# convert string of letters '0' and '1' to a vector of 0 and 1
def binstr2binv(s):
    return [(ord(x) - ord('0')) for x in s]

# convert a vector of 0 and 1 to a string of letters '0' and '1'
def binv2binstr(charv):
    a = [chr(ord(x) + ord('0')) for x in charv]
    return ''.join(a)

# convert string of 8-bit letter to a vector of 0 and 1 in LSB order:
def str2binv(s):
    v = []
    for x in s:
        x = ord(x)
        for i in range(8):
            v.append(x & 1)
            x = x >> 1
    return v

# convert string of 8-bit letter to a vector of 0 and 1 in MSB order:
def str2binv_msb(s):
    v = []
    for x in s:
        x = ord(x)
        for i in range(8):
            v.append((x & 0x80) >> 7)
            x = (x << 1) & 0xFF
    return v


#########################################################################
## Helper functions for jamming #########################################
#########################################################################

def str_to_jamming_vector(jamming_str, vector_len = 64):
    """
    convert a jamming location specified by a string to a vector
    of 0 and 1, where 0 indicates no-jamming, 1 indicates jamming
    jamming_str: a comma-separated string, where each element
    is either the subcarrier index (e.g., "11, 25, 39, 53" specifies
    jamming on 4 pilot subcarriers), or the subcarrier index range
    (e.g., "8-10" specifies jamming 3 subcarriers at 8,9,10)
    """
    m = [0,] * vector_len   # default: no jamming

    # convert string to array of string
    jamming_str = jamming_str.split(",")

    # convert each string to subcarrier index/range
    for s in jamming_str:
        t = s.split('-')
        if t[0] == "" or t[-1] == "":
            continue

        start = int(t[0])
        end = int(t[-1])
        for jamming in range(start, end + 1):
            if jamming < vector_len:
                m[jamming] = 1

    return m

def subcarrier_jamming_pattern(jamming_str):
    return str_to_jamming_vector(jamming_str, 64)

def ofdmsym_jamming_pattern(jamming_str):
    # create vector up to size 999
    v = str_to_jamming_vector(jamming_str, 999)

    # remove 0 from the right
    k = len(v) - 1
    while k >= 0 and v[k] == 0:
        k -= 1
    v = v[:k+1]

    return v
