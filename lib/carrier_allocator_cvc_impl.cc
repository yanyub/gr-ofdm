/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include "carrier_allocator_cvc_impl.h"

namespace gr {
  namespace ofdm {

    carrier_allocator_cvc::sptr
    carrier_allocator_cvc::make(int fft_len,
		                const std::vector<std::vector<int> > occupied_carriers,
				const std::vector<std::vector<int> > pilot_carriers,
				const std::vector<std::vector<gr_complex> > pilot_symbols,
				const std::string tag_len_key,
				int mtu)
    {
      return gnuradio::get_initial_sptr (new carrier_allocator_cvc_impl(
			      fft_len,
			      occupied_carriers,
			      pilot_carriers,
			      pilot_symbols,
			      tag_len_key,
			      mtu));
    }

    carrier_allocator_cvc_impl::carrier_allocator_cvc_impl(int fft_len,
		                                           const std::vector<std::vector<int> > occupied_carriers,
							   const std::vector<std::vector<int> > pilot_carriers,
							   const std::vector<std::vector<gr_complex> > pilot_symbols,
							   const std::string tag_len_key,
							   int mtu)
      : gr_block("carrier_allocator_cvc",
		      gr_make_io_signature(1, 1, sizeof (gr_complex)),
		      gr_make_io_signature(1, 1, sizeof (gr_complex) * fft_len)),
	d_fft_len(fft_len),
	d_occupied_carriers(occupied_carriers),
	d_pilot_carriers(pilot_carriers),
	d_pilot_symbols(pilot_symbols),
	d_tag_len_key(tag_len_key),
	d_input_size(1),
	d_mtu(mtu)
    {
	    // TODO make sure the occupied_carriers and pilot_carriers < fft_len
	    // and occupied_carriers.size() == pilot_carriers.size()
	    // and pilot_carriers[i].size() == pilot_symbols[i].size() \-/ i
	    set_output_multiple(d_mtu);
	    set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    carrier_allocator_cvc_impl::~carrier_allocator_cvc_impl()
    {
    }

    void
    carrier_allocator_cvc_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
	    ninput_items_required[0] = d_input_size;
    }

    int
    carrier_allocator_cvc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];
	long packet_length;
	long n_symbols_read;
	gr_complex next_symbol;

	std::vector<gr_tag_t> tags;
	this->get_tags_in_range(tags, 0, 0, 1);
	for (int i = 0; i < tags.size(); i++) {
		if (pmt::pmt_symbol_to_string(tags[i].key) == d_tag_len_key) {
			packet_length = pmt::pmt_to_long(tags[i].value);
		}
	}

	if (ninput_items[0] < packet_length) { // FIXME check output buffer >= n_ofdm_symbols * sizeof(gr_complex)
		d_input_size = packet_length;
		return 0;
	}

	long n_ofdm_symbols = packet_length / d_fft_len;
	if (packet_length % d_fft_len)
		n_ofdm_symbols++;

	// FIXME add flush functionality (send an empty OFDM symbol to flush the cyclic prefixer)
	// TODO run this multiple times if input_items >= N * packet_length
	for (int i = 0; i < n_ofdm_symbols; i++) {
		int curr_set = 0;
		for (int k = 0; k < d_occupied_carriers[curr_set].size(); k++) {
			if (n_symbols_read < packet_length) {
				next_symbol = *out++;
			} else { // Padding
				next_symbol = gr_complex(0, 0); // TODO perhaps putting sthg here is better than nothing
			}
			out[d_occupied_carriers[curr_set][k]] = next_symbol;
		}
		for (int k = 0; k < d_pilot_carriers[curr_set].size(); k++) {
			out[d_pilot_carriers[curr_set][k]] = d_pilot_symbols[curr_set][k];
		}
		curr_set = (curr_set + 1) % d_pilot_carriers.size();
	}
	consume_each(packet_length); // FIXME if looped, packet_length * n_packets
	this->add_item_tag(0, this->nitems_written(0), // FIXME same as above
			   pmt::pmt_string_to_symbol("mux_length"),
			   pmt::pmt_from_long(n_ofdm_symbols));

        // Tell runtime system how many output items we produced.
        return n_ofdm_symbols; // FIXME n_ofdm_symbols * n_packets
    }


  } /* namespace ofdm */
} /* namespace gr */

