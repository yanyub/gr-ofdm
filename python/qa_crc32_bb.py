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

from gnuradio import gr, gr_unittest
import ofdm_swig as ofdm

class qa_crc32_bb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    #def test_check_fail (self):
    #    " Pass a stream without a tag, check it fails "
    #    sink = gr.vector_sink_b()
    #    self.tb.connect(gr.vector_source_b((0,)*16), ofdm.crc32_bb(), sink)
    #    self.tb.run()

    def test_crc_len (self):
        data = ('hello', 'you', 'there')
        tx_msgq = gr.msg_queue()
        rx_msgq = gr.msg_queue()
        for d in data:
            tx_msgq.insert_tail(gr.message_from_string(d))
        tagname = "packet_length"
        src  = gr.message_source(gr.sizeof_char, tx_msgq, tagname)
        snk = gr.message_sink(gr.sizeof_char, rx_msgq, False, tagname)
        mtu = 4096
        crc = ofdm.crc32_bb(mtu, tagname)
        self.tb.connect(src, crc, snk)
        #self.tb.connect(src, snk)
        self.tb.start()
        time.sleep(1)
        self.tb.stop()
        for d in data:
            msg = rx_msgq.delete_head()
            contents = msg.to_string()
            print(d, contents)
            self.assertEqual(len(d)+4, len(contents))
        
        

if __name__ == '__main__':
    gr_unittest.run(qa_crc32_bb, "qa_crc32_bb.xml")

