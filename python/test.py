import unittest

from gnuradio import gr, digital, fft
import ofdm
import utils
ofdm.utils = utils

class test(unittest.TestCase):
    
    def atest_one(self):
        tb = gr.top_block()
        MTU = 4000
        tagname = "_length"
        packets = (
            (0, 0, 0, 0)*32,
            (0, 0, 1, 0)*16,
            (1, 1, 1, 0)*4,
            )
        data, tags = ofdm.utils.packets_to_vectors(packets, tagname)
        constellation = digital.bpsk_constellation()
        src = gr.vector_source_b(data, tags, False, 1)
        tag_scaler1 = ofdm.scale_tags(1, tagname, 0.125)
        packer = gr.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        crc1 = ofdm.crc32_bb(False, MTU, tagname)
        crc2 = ofdm.crc32_bb(True, MTU, tagname)
        tag_scaler2 = ofdm.scale_tags(1, tagname, 8)
        unpacker = gr.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST)
        chunks2symbols = digital.chunks_to_symbols_bc(constellation.points())
        snk = gr.vector_sink_b()
        #tb.connect(src, packer, crc, tag_scaler, unpacker, chunks2symbols, snk)
        tb.connect(src, tag_scaler1, packer, crc1, crc2, tag_scaler2, unpacker, snk)
        tb.run()
        #r_packets = ofdm.utils.vectors_to_packets(snk.data(), snk.tags(), tagname)
        #self.assertEqual(len(r_packets), len(packets))
        #for rp, ep in zip(r_packets, packets):
            #self.assertEqual(len(rp), len(ep))
            #for r, e in zip(rp, ep):
                #self.assertEqual(r, e)

    def test_two(self):
        tb = gr.top_block()
        MTU = 4000
        tagname = "length"
        packets = (
            (0, 0, 0, 0)*32,
            #(0, 0, 1, 0)*16,
            #(1, 1, 1, 0)*4,
            )
        data, tags = ofdm.utils.packets_to_vectors(packets, tagname)
        constellation = digital.bpsk_constellation()
        src = gr.vector_source_b(data, tags, False, 1)
        tag_scaler1 = ofdm.scale_tags(1, tagname, 0.125)
        packer = gr.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        crc = ofdm.crc32_bb(False, MTU, tagname)
        tag_scaler2 = ofdm.scale_tags(1, tagname, 8)
        unpacker = gr.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST)
        mod1 = digital.chunks_to_symbols_bc(constellation.points())
        mod2 = digital.chunks_to_symbols_bc(constellation.points())
        occupied_carriers = ((1, 2, 4, 5),)
        fft_len = 16
        header = ofdm.ofdm_header_bb(len(occupied_carriers[0]))
        mux1 = ofdm.tagged_stream_mux(gr.sizeof_gr_complex, 2, tagname, MTU)
        mux2 = ofdm.tagged_stream_mux(gr.sizeof_gr_complex*fft_len, 2, tagname, MTU)
        ifft = fft.fft_vcc(fft_len, forward=False, window=[1]*fft_len)
        cp_len = 4
        rolloff_len = 2
        cp = digital.ofdm_cyclic_prefixer(fft_len, fft_len+cp_len, rolloff_len, tagname, MTU)
        pilot_symbols = ((1j,),)
        pilot_carriers = ((3,),)
        alloc = ofdm.carrier_allocator_cvc(fft_len,
                       occupied_carriers,
                       pilot_carriers,
                       pilot_symbols,
                       tagname)
        snk = gr.vector_sink_c()
        sync_packet = [0, 1, 0, 1, 0, 1] + [0]*10
        sync_data, sync_tags = ofdm.utils.packets_to_vectors([sync_packet], tagname, vlen=fft_len)
        sync_header = gr.vector_source_c(sync_data, sync_tags, True, fft_len)
        tb.connect(src, tag_scaler1, packer, crc, tag_scaler2, unpacker, mod1, (mux1, 1))
        tb.connect(crc, header, mod2, (mux1, 0))
        tb.connect(mux1, alloc, (mux2, 1))
        tb.connect(sync_header, (mux2, 0))
        tb.connect(mux2, ifft, cp, snk)
        #tb.connect(src, tag_scaler1, packer, crc, tag_scaler2, unpacker, mod1, alloc, snk)
        
        tb.run()
        #r_packets = ofdm.utils.vectors_to_packets(snk.data(), snk.tags(), tagname, vlen=1)

if __name__ == '__main__':
    unittest.main()
