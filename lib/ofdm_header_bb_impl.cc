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
#include "ofdm_header_bb_impl.h"
#include <iostream>

namespace gr {
  namespace ofdm {

    ofdm_header_bb::sptr
    ofdm_header_bb::make(int header_len, void (*formatter_cb)(long, long, unsigned char*))
    {
      return gnuradio::get_initial_sptr (new ofdm_header_bb_impl(header_len, formatter_cb));
    }

    ofdm_header_bb::sptr
    ofdm_header_bb::make(int header_len)
    {
      return gnuradio::get_initial_sptr (new ofdm_header_bb_impl(header_len, NULL));
    }

    /*
     * The private constructor
     */
    ofdm_header_bb_impl::ofdm_header_bb_impl(int header_len, void (*formatter_cb)(long, long, unsigned char*))
      : gr_block("ofdm_header_bb",
		 gr_make_io_signature(1, 1, sizeof (char)),
		 gr_make_io_signature(1, 1, sizeof (char))),
	d_header_len(header_len), d_formatter_cb(formatter_cb), d_input_size(1)
    {
      set_output_multiple(header_len);
      set_tag_propagation_policy(TPP_DONT);
      if (d_formatter_cb == NULL)
	d_formatter_cb = &default_formatter;
    }

    void
    ofdm_header_bb_impl::default_formatter(long packet_size, long header_len, unsigned char* buf)
    {
      // Default header formatter if the user doesn't specify one
      // BPSK, MSB first, 16 bit packet length, pad with zeros
      int i;
      for (i=0;i<16;i++) // FIXME header_len might be < 16
	buf[i] = (unsigned char) (packet_size >> (15-i)) & 1;
      while (i<header_len)
	buf[i] = 0;
    }

    /*
     * Our virtual destructor.
     */
    ofdm_header_bb_impl::~ofdm_header_bb_impl()
    {
    }

    void
    ofdm_header_bb_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
	    ninput_items_required[0] = d_input_size;
    }

    int
    ofdm_header_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];

	long packet_length = 0;

	std::vector<gr_tag_t> tags;
	this->get_tags_in_range(tags, 0, this->nitems_read(0), this->nitems_read(0)+1);  // get tags from first element only

	for (int i = 0; i < tags.size(); i++) {
		if (pmt::pmt_symbol_to_string(tags[i].key) == "length") {
			packet_length = pmt::pmt_to_long(tags[i].value);
		}
	}
	std::cout << "packet len " << packet_length << std::endl;

	if (ninput_items[0] >= packet_length) {
	  d_formatter_cb(packet_length, d_header_len, out);  // tell formatter where to put output, how long packet is

	  pmt::pmt_t key = pmt::pmt_string_to_symbol("length");
	  pmt::pmt_t value = pmt::pmt_from_long(d_header_len);
	  //write at tag to output port 0 with given absolute item offset
	  this->add_item_tag(0, this->nitems_written(0), key, value);

	  consume_each(packet_length);
	  d_input_size = 1;
	  return d_header_len;
	}

	// fall through to here if we aren't given a whole input packet
	d_input_size = packet_length;
	return 0;
    }


  } /* namespace ofdm */
} /* namespace gr */

