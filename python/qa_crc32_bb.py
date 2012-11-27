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
import ofdm_swig as ofdm

class qa_crc32_bb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_check_fail (self):
        " Pass a stream without a tag, check it fails "
        pass
        #sink = gr.vector_sink_b()
        #self.tb.connect(gr.vector_source_b((0,)*16), ofdm.crc32_bb(), sink)
        #self.tb.run() # Fails

    #def test_crc_len (self):
        #data = (...)
        #src  = ben's cool new block
        #sink = gr.vector_sink_b()
        #self.tb.connect(src, ofdm.crc32_bb(), sink)
        #self.tb.run()
        #self.assertEqual(len(sink.data()), len(data)+4)

if __name__ == '__main__':
    gr_unittest.run(qa_crc32_bb, "qa_crc32_bb.xml")

