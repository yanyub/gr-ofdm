/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#include <gr_io_signature.h>
#include "ofdm_frame_extractor_impl.h"

#define msg_port_id     pmt::mp("header_data")

namespace gr {
  namespace ofdm {

    ofdm_frame_extractor::sptr
    ofdm_frame_extractor::make(int fftlen, int cplen)
    {
      return gnuradio::get_initial_sptr (new ofdm_frame_extractor_impl(fftlen, cplen));
    }

    /*
     * The private constructor
     */
    ofdm_frame_extractor_impl::ofdm_frame_extractor_impl(int fftlen, int cplen)
      : gr_block("ofdm_frame_extractor",
		 gr_make_io_signature3(3, 3, sizeof (gr_complex), sizeof (unsigned char), sizeof (float)),
		 gr_make_io_signature(2, 2, sizeof (gr_complex) * fftlen)),
	d_fftlen(fftlen), d_cplen(cplen), d_remaining_symbols(0)
    {
      set_tag_propagation_policy(TPP_DONT);

      // FIXME need to get Tim's message stuff in gnuradio-core
      //message_port_register_in(msg_port_id);
    }

    /*
     * Our virtual destructor.
     */
    ofdm_frame_extractor_impl::~ofdm_frame_extractor_impl()
    {
    }

    void
    ofdm_frame_extractor_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items * d_symbolsize;
      ninput_items_required[1] = noutput_items * d_symbolsize;
      ninput_items_required[2] = noutput_items * d_symbolsize;
    }
    
    int
    ofdm_frame_extractor_impl::general_work (int noutput_items,
					     gr_vector_int &ninput_items,
					     gr_vector_const_void_star &input_items,
					     gr_vector_void_star &output_items)
    {
      const gr_complex *sample_in = (const gr_complex *) input_items[0];
      const unsigned char *peak = (const unsigned char *) input_items[1];
      const float *angle = (const float *) input_items[2];

      gr_complex *header_out = (gr_complex *) output_items[0];
      gr_complex *packet_out = (gr_complex *) output_items[1];

      int num_inputs = std::min(std::min(ninput_items[0],ninput_items[1]),ninput_items[2]);
      int i = 0;

      if(d_remaining_symbols == 0) {
      // Searching
	while(i <= (num_inputs-d_symbolsize)) {
	  if(peak[i]) {
	    // Found it, attach fine frequency offset to SYNC output symbol
	    pmt::pmt_t key = pmt::pmt_string_to_symbol("fine_offset");
	    pmt::pmt_t value = pmt::pmt_from_double((double)angle[i]);
	    this->add_item_tag(0, this->nitems_written(0), key, value);

	    if((noutput_items>=2) && (i <= (num_inputs-2*d_symbolsize))) {
	      // Send both SYNC and Header
	      d_remaining_symbols = -2;
	      memcpy(header_out,&sample_in[i],d_fftlen);
	      memcpy(&header_out[d_fftlen],&sample_in[i+d_symbolsize],d_fftlen);
	      consume_each(i+2*d_symbolsize);
	      produce(0,2);
	      produce(1,0);
	      return WORK_CALLED_PRODUCE;
	    } else {
	      // Just send SYNC
	      d_remaining_symbols = -1;
	      memcpy(header_out,&sample_in[i],d_fftlen);
	      consume_each(i+d_symbolsize);
	      produce(0,1);
	      produce(1,0);
	      return WORK_CALLED_PRODUCE;
	    }
	  } else {
	    i++;
	  }
	}
	// Didn't find anything, throw away samples
	consume_each(i);
	return 0;
      }

      if(d_remaining_symbols == -1) {
	// Sent SYNC last time, send Header this time
	d_remaining_symbols = -2;
	memcpy(header_out,sample_in,d_fftlen);
	consume_each(d_symbolsize);
	produce(0,1);
	produce(1,0);
	return WORK_CALLED_PRODUCE;
      }

      if(d_remaining_symbols == -2) {
	// Wait for return message
	// FIXME need to get Tim's message stuff in gnuradio-core
	pmt::pmt_t msg(/* delete_head_blocking( msg_port_id )*/0  );
	pmt::pmt_t length(pmt::pmt_car(msg));
	pmt::pmt_t metadata(pmt::pmt_cdr(msg));
	d_remaining_symbols == 	pmt::pmt_to_long(length);

	pmt::pmt_t key = pmt::pmt_string_to_symbol("length");
	this->add_item_tag(1, this->nitems_written(1), key, length);

	pmt::pmt_t keys = pmt::pmt_dict_keys(metadata);
	pmt::pmt_t values = pmt::pmt_dict_values(metadata);
	for (int i = 0; i < pmt::pmt_length(keys); i++) {
	  this->add_item_tag(1, this->nitems_written(1),pmt::pmt_nth(i,keys),pmt::pmt_nth(i,values));
	}
	// FIXME should we propagate any tags?  What about UHD tags from receiver so we know the time?
      }
      
      // In packet here, send out as much as possible
      int produced = 0;
      while( (produced < noutput_items) && (d_remaining_symbols > 0) && (i < (num_inputs-d_symbolsize)) ) {
	memcpy(&packet_out[produced*d_fftlen],&sample_in[i],d_fftlen);
	produced++;
	i+=d_symbolsize;
	d_remaining_symbols--;
      }

      consume_each(i);
      produce(0,0);
      produce(1,produced);
      return WORK_CALLED_PRODUCE;
    }
    
  } /* namespace ofdm */
} /* namespace gr */
