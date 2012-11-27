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

class qa_carrier_allocator_cvc (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        fft_len = 16
        occupied_carriers = ((1, 3, 4, 11, 12, 14),)
        pilot_carriers = ((2, 13),)
        pilot_symbols = ((1, 1),)
        src = gr.vector_source_c(FIXME)
        alloc = ofdm.carrier_allocator_cvc(fft_len,
                       occupied_carriers,
                       pilot_carriers,
                       pilot_symbols,
                       "length")
        sink = gr.vector_sink_c()
        # set up fg
        self.tb.run ()
        # check data


if __name__ == '__main__':
    gr_unittest.run(qa_carrier_allocator_cvc, "qa_carrier_allocator_cvc.xml")
