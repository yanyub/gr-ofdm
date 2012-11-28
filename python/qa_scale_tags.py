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

from gnuradio import gr, gr_unittest
import ofdm_swig as ofdm

class qa_scale_tags (gr_unittest.TestCase):

    def test_001_t (self):
        tb = gr.top_block()
        data = ('hello', 'there', 'what', 'is', 'happending')
        tx_msgq = gr.msg_queue()
        rx_msgq = gr.msg_queue()
        for d in data:
            tx_msgq.insert_tail(gr.message_from_string(d))
        tagname = "length"
        src  = gr.message_source(gr.sizeof_char, tx_msgq, tagname)
        tag_scaler = ofdm.scale_tags(1, "length", 0.5)
        snk = gr.message_sink(gr.sizeof_char, rx_msgq, False, tagname)
        tb.connect(src, tag_scaler, snk)
        #tb.connect(src, snk)
        tb.start()
        time.sleep(1)
        tb.stop()
        for d in data:
            msg = rx_msgq.delete_head()
            contents = msg.to_string()
            print(d, contents)

if __name__ == '__main__':
    gr_unittest.run(qa_scale_tags, "qa_scale_tags.xml")
