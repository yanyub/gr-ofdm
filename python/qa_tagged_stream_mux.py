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
import utils
ofdm.utils = utils

class qa_tagged_stream_mux (gr_unittest.TestCase):

    def test_1(self):
        datas = (
            ('h', 'ee', 'lll', 'qqqq'),
            ('oooo', 'yyyyy', 'oooooo', 'pppppppppp')
            )
        expected = [a+b for a,b in zip(datas[0], datas[1])]
        srcs = []
        tagname = "packet_length"
        for i, data in enumerate(datas):
            dat, tags = ofdm.utils.strings_to_vectors(data, tagname)
            src = gr.vector_source_b(dat, tags, False, 1)
            srcs.append(src)
        MTU = 4000
        tagged_stream_mux = ofdm.tagged_stream_mux(gr.sizeof_char, len(datas), tagname, MTU)
        snk = gr.vector_sink_b()
        tb = gr.top_block()
        for i, src in enumerate(srcs):
            tb.connect(src, (tagged_stream_mux, i))
        tb.connect(tagged_stream_mux, snk)
        tb.run()
        dataout = ofdm.utils.vectors_to_strings(snk.data(), snk.tags(), tagname)
        for r, e in zip(dataout, expected):
            self.assertEqual(r, e)

if __name__ == '__main__':
    gr_unittest.run(qa_tagged_stream_mux, "qa_mux.xml")
