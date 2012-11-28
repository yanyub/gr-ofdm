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

class qa_tagged_stream_mux (gr_unittest.TestCase):

    def test_1(self):
        datas = (
            ('hello', 'you', 'there'),
            ('one', 'two', 'three')
            )
        tx_msgqs = []
        srcs = []
        tagnames = []
        for i, data in enumerate(datas):
            tx_msgq = gr.msg_queue () 
            for d in data:
                tx_msgq.insert_tail(gr.message_from_string(d))
                tx_msgqs.append(tx_msgq)
            tagname = "packet_length" + str(i)
            src = gr.message_source(gr.sizeof_char, tx_msgq, tagname)
            srcs.append(src)
            tagnames.append(tagname)
        rx_msgq = gr.msg_queue ()

        tb = gr.top_block()
        MTU = 4000
        print(tagnames)
        tagged_stream_mux = ofdm.tagged_stream_mux(gr.sizeof_char, tagnames, MTU)
        snk = gr.message_sink(gr.sizeof_char, rx_msgq, False, "packet_length")
        for i, src in enumerate(srcs):
            tb.connect(src, (tagged_stream_mux, i))
        tb.connect(tagged_stream_mux, snk)
        tb.start()
        time.sleep(1)
        tb.stop()
        for d in data:
            msg = rx_msgq.delete_head()
            contents = msg.to_string()
            self.assertEqual(d, contents)

if __name__ == '__main__':
    gr_unittest.run(qa_tagged_stream_mux, "qa_mux.xml")
