#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2015 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import swifi_swig as swifi

class qa_stream_pack (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        sp = swifi.stream_pack([1, 1])
        source0 = blocks.vector_source_b([0, 1, 0, 1])
        source1 = blocks.vector_source_b([0, 0, 1, 1])
        sink = blocks.vector_sink_b()
        self.tb.connect(source0, (sp, 0))
        self.tb.connect(source1, (sp, 1))
        self.tb.connect(sp, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), tuple([0, 1, 2, 3]))

    def test_002_t (self):
        sp = swifi.stream_pack([2, 1])
        source0 = blocks.vector_source_b([0, 1, 0, 1])
        source1 = blocks.vector_source_b([0, 0, 1, 1])
        sink = blocks.vector_sink_b()
        self.tb.connect(source0, (sp, 0))
        self.tb.connect(source1, (sp, 1))
        self.tb.connect(sp, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), tuple([0, 1, 4, 5]))

if __name__ == '__main__':
    gr_unittest.run(qa_stream_pack, "qa_stream_pack.xml")
