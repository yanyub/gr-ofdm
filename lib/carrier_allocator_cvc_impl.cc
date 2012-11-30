/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <algorithm>
#include <gr_io_signature.h>
#include "carrier_allocator_cvc_impl.h"

#include <iostream>

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
	    set_relative_rate(1.0/d_occupied_carriers[0].size());
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
	long frame_length = 0;

	std::vector<gr_tag_t> tags;
	this->get_tags_in_range(tags, 0, this->nitems_read(0), this->nitems_read(0)+1);
	for (unsigned i = 0; i < tags.size(); i++) {
		if (pmt::pmt_symbol_to_string(tags[i].key) == d_tag_len_key) {
			frame_length = pmt::pmt_to_long(tags[i].value);
		}
	}
	assert(frame_length);

	if (ninput_items[0] < frame_length) { // FIXME check output buffer >= n_ofdm_symbols * sizeof(gr_complex)
		d_input_size = frame_length;
		return 0;
	}

	// 1) Copy data symbols and non-frame-length tags
	long n_ofdm_symbols = 0;
	int curr_set = 0;
	int symbols_to_allocate = d_occupied_carriers[0].size();
	int symbols_allocated = 0;
	for (int i = 0; i < frame_length; i++) {
		if (symbols_allocated == 0) {
			n_ofdm_symbols++;
			// Copy all tags associated with these input symbols onto this OFDM symbol
			this->get_tags_in_range(tags, 0,
					        this->nitems_read(0)+i,
					        this->nitems_read(0)+std::min(i+symbols_to_allocate, (int) frame_length)
						);
			for (int t = 0; t < tags.size(); t++) {
				if (tags[t].offset != this->nitems_read(0)
				    || pmt::pmt_symbol_to_string(tags[t].key) != d_tag_len_key) {
					this->add_item_tag(0, this->nitems_written(0)+n_ofdm_symbols,
							   tags[i].key,
							   tags[i].value);
				}
			}
		}
		out[(n_ofdm_symbols-1) * d_fft_len + d_occupied_carriers[curr_set][symbols_allocated]] = in[i];
		symbols_allocated++;
		if (symbols_allocated == symbols_to_allocate) {
			curr_set = (curr_set + 1) % d_occupied_carriers.size();
			symbols_to_allocate = d_occupied_carriers[curr_set].size();
			symbols_allocated = 0;
		}
	}
	if (symbols_allocated < symbols_to_allocate) { // I.e. the last OFDM symbol wasn't fully filled
		for (int i = 0; i < symbols_to_allocate-symbols_allocated; i++) {
			out[(n_ofdm_symbols-1) * d_fft_len + d_occupied_carriers[curr_set][symbols_allocated + i]] = gr_complex(0, 0);
		}

	}

	// 2) Copy pilot symbols
	curr_set = 0;
	for (int i = 0; i < n_ofdm_symbols; i++) {
		for (int k = 0; k < d_pilot_carriers[curr_set].size(); k++) {
			out[i * d_fft_len + d_pilot_carriers[curr_set][k]] = d_pilot_symbols[curr_set][k];
		}
		curr_set = (curr_set + 1) % d_pilot_carriers.size();
	}

	// 3) Housekeeping
	consume_each(frame_length);
	this->add_item_tag(0, this->nitems_written(0),
			   pmt::pmt_string_to_symbol(d_tag_len_key),
			   pmt::pmt_from_long(n_ofdm_symbols));

        return n_ofdm_symbols;
    }


  } /* namespace ofdm */
} /* namespace gr */

