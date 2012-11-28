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
#include "crc32_bb_impl.h"
#include <digital_crc32.h>

namespace gr {
  namespace ofdm {

    crc32_bb::sptr
    crc32_bb::make(int mtu, const std::string& lengthtagname)
    {
      return gnuradio::get_initial_sptr (new crc32_bb_impl(mtu, lengthtagname));
    }

    /*
     * The private constructor
     */
    crc32_bb_impl::crc32_bb_impl(int mtu, const std::string& lengthtagname)
      : gr_block("crc32_bb",
		      gr_make_io_signature(1, 1, sizeof (char)),
		      gr_make_io_signature(1, 1, sizeof (char))),
	d_input_size(1),
    d_mtu(mtu),
    d_lengthtagname(lengthtagname)
    {
	    set_output_multiple(d_mtu);
	    set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    crc32_bb_impl::~crc32_bb_impl()
    {
    }


    void
    crc32_bb_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
	    ninput_items_required[0] = d_input_size;
    }


    int
    crc32_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
	long packet_length = 0;
	unsigned int crc;

	std::vector<gr_tag_t> tags;
	this->get_tags_in_range(tags, 0, this->nitems_read(0), this->nitems_read(0)+1);
	for (int i = 0; i < tags.size(); i++) {
		if (pmt::pmt_symbol_to_string(tags[i].key) == d_lengthtagname) {
			packet_length = pmt::pmt_to_long(tags[i].value);
		}
	}
	assert(packet_length != 0);
	assert(packet_length <= d_mtu-4);

	assert(noutput_items >= d_mtu);
	// TODO run this multiple times if input_items >= N * packet_length
	// FIXME copy existing tags to output
	if (ninput_items[0] >= packet_length ) {
		memcpy((void *) out, (const void *) in, packet_length);
		crc = digital_crc32(in, packet_length);
		memcpy((void *) (out + packet_length), &crc, 4); // FIXME big-endian/little-endian, this might be wrong

		pmt::pmt_t key = pmt::pmt_string_to_symbol(d_lengthtagname);
		pmt::pmt_t value = pmt::pmt_from_long(packet_length+4);
		//write at tag to output port 0 with given absolute item offset
		this->add_item_tag(0, this->nitems_written(0), key, value);

		consume_each(packet_length);
		d_input_size = 1;
		return packet_length + 4;
	}

	// If this didn't work, increase the input size
	d_input_size = packet_length;
    return 0;
    }

  } /* namespace ofdm */
} /* namespace gr */

