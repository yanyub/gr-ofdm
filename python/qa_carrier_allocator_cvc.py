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

class qa_carrier_allocator_cvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        """
        pretty simple
        """
        fft_len = 6
        tx_symbols = (1, 2, 3)
        pilot_symbols = ((1j,),)
        occupied_carriers = ((0, 1, 2),)
        pilot_carriers = ((3,),)
        expected_result = (1, 2, 3, 1j, 0, 0)
        mtu = 128
        tag_name = "len"
        tag = gr.gr_tag_t()
        tag.offset = 0
        tag.key = pmt.pmt_string_to_symbol(tag_name)
        tag.value = pmt.pmt_from_long(len(tx_symbols))
        src = gr.vector_source_c(tx_symbols, (tag,), False, 1)
        alloc = ofdm.carrier_allocator_cvc(fft_len,
                       occupied_carriers,
                       pilot_carriers,
                       pilot_symbols,
                       tag_name)
        sink = gr.vector_sink_c(fft_len)

        self.tb.connect(src, alloc, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), expected_result)

    def test_001_t (self):
        """
        more advanced:
        - 6 symbols per carrier
        - 2 pilots per carrier
        - have enough data for nearly 3 OFDM symbols
        """
        tx_symbols = range(1, 16);
        pilot_symbols = ((1j, 2j), (3j, 4j))
        occupied_carriers = ((1, 3, 4, 11, 12, 14), (1, 2, 4, 11, 13, 14),)
        pilot_carriers = ((2, 13), (3, 12))
        expected_result = (0, 1,  1j,  2,  3, 0, 0, 0, 0, 0, 0, 4,  5,  2j, 6,  0,
                           0, 7,  8,  3j,  9, 0, 0, 0, 0, 0, 0, 10, 4j, 11, 12, 0,
                           0, 13, 1j, 14, 15, 0, 0, 0, 0, 0, 0, 0,  0,  2j, 0,  0)
        fft_len = 16
        mtu = 4096
        tag_name = "len"
        tag = gr.gr_tag_t()
        tag.offset = 0
        tag.key = pmt.pmt_string_to_symbol(tag_name)
        tag.value = pmt.pmt_from_long(len(tx_symbols))
        src = gr.vector_source_c(tx_symbols, (tag,), False, 1)
        alloc = ofdm.carrier_allocator_cvc(fft_len,
                       occupied_carriers,
                       pilot_carriers,
                       pilot_symbols,
                       tag_name)
        sink = gr.vector_sink_c(fft_len)

        self.tb.connect(src, alloc, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), expected_result)

if __name__ == '__main__':
    gr_unittest.run(qa_carrier_allocator_cvc, "qa_carrier_allocator_cvc.xml")
