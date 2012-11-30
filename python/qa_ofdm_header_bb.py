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

from gnuradio import gr, gr_unittest
from gruel import pmt
import ofdm_swig as ofdm
import utils
ofdm.utils = utils

class qa_ofdm_header_bb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        packets = ((1, 2, 3, 4), (1, 2), (1, 2, 3, 4))
        tagname = "length"
        data, tags = ofdm.utils.packets_to_vectors(packets, tagname)
        src = gr.vector_source_b(data, tags, False, 1)
        header = ofdm.ofdm_header_bb(16)
        sink = gr.vector_sink_b()
        self.tb.connect(src, header, sink)
        self.tb.run ()
        # check data
        new_packets = ofdm.utils.vectors_to_packets(sink.data(), sink.tags(), tagname)
        expected_packets = ((0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
                            (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0),
                            (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0))
        self.assertEqual(len(new_packets), len(expected_packets))
        for ep, rp in zip(new_packets, expected_packets):
            self.assertEqual(ep, rp)


if __name__ == '__main__':
    gr_unittest.run(qa_ofdm_header_bb, "qa_ofdm_header_bb.xml")
