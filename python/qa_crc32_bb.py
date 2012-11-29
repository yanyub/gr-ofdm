#!/usr/bin/env python
# 
# Copyright 2012 <+YOU OR YOUR COMPANY+>.
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

import time
from gruel import pmt

from gnuradio import gr, gr_unittest
import ofdm_swig as ofdm

class qa_crc32_bb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    #def test_crc_len (self):
        #data = range(16)
        #tag_name = "len"
        #tag = gr.gr_tag_t()
        #tag.offset = 0
        #tag.key = pmt.pmt_string_to_symbol(tag_name)
        #tag.value = pmt.pmt_from_long(len(data))
        #src = gr.vector_source_b(data, (tag,), False, 1)
        #mtu = 64
        #crc = ofdm.crc32_bb(False, mtu, tag_name)
        #sink = gr.vector_sink_b()
        #self.tb.connect(src, crc, sink)
        #self.tb.run()
        # Check that the packets before crc_check are 4 bytes longer that the input.
        #self.assertEqual(len(data)+4, len(sink.data()))

    def test_crc_equal (self):
        data = (0, 1, 2, 3, 4, 5, 6, 7, 8)
        tag_name = "len"
        tag = gr.gr_tag_t()
        tag.offset = 0
        tag.key = pmt.pmt_string_to_symbol(tag_name)
        tag.value = pmt.pmt_from_long(len(data))
        src = gr.vector_source_b(data, (tag,), False, 1)
        mtu = 64
        crc = ofdm.crc32_bb(False, mtu, tag_name)
        crc_check = ofdm.crc32_bb(True, mtu, tag_name)
        sink = gr.vector_sink_b()
        sink2 = gr.vector_sink_b()
        self.tb.connect(src, crc, crc_check, sink)
        self.tb.run()
        # Check that the packets after crc_check are the same as input.
        self.assertEqual(data, sink.data())

if __name__ == '__main__':
    gr_unittest.run(qa_crc32_bb, "qa_crc32_bb.xml")

