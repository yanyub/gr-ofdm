#!/usr/bin/env python
# Copyright 2012 Free Software Foundation, Inc.
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

import time
import itertools

from gnuradio import gr, gr_unittest
from gruel import pmt
import ofdm_swig as ofdm

class qa_scale_tags (gr_unittest.TestCase):

    def test_001_t (self):
        tb = gr.top_block()
        data = ((1, 2, 3, 4), (1, 2, 3), (1, 2, 3, 4, 5))
        tags = []
        pos = 0
        for d in data:
            tag = gr.gr_tag_t()
            tag.offset = pos
            tag.key = pmt.pmt_string_to_symbol("length")
            tag.value = pmt.pmt_from_long(len(d))
            pos += len(d)
            tags.append(tag)
        data = list(itertools.chain(*data))
        print(data)
        print(tags)
        src = gr.vector_source_b(data, tags, False, 1)
        snk = gr.vector_sink_b()
        tag_scaler = ofdm.scale_tags(1, "length", 0.5)
        tb.connect(src, tag_scaler, snk)
        #tb.connect(src, snk)
        tb.start()
        time.sleep(1)
        tb.stop()
        print(snk.data())
        for tag in snk.tags():
            print(tag.offset, pmt.pmt_symbol_to_string(tag.key), pmt.pmt_to_long(tag.value))

if __name__ == '__main__':
    gr_unittest.run(qa_scale_tags, "qa_scale_tags.xml")
