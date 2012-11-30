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
		      gr_make_io_signature(<+MIN_IN+>, <+MAX_IN+>, sizeof (<+float+>)),
		      gr_make_io_signature(<+MIN_IN+>, <+MAX_IN+>, sizeof (<+float+>)))
    {}

    /*
     * Our virtual destructor.
     */
    ofdm_frame_extractor_impl::~ofdm_frame_extractor_impl()
    {
    }

    int
    ofdm_frame_extractor_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const float *in = (const float *) input_items[0];
        float *out = (float *) output_items[0];

        // Do <+signal processing+>
        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (noutput_items);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }


  } /* namespace ofdm */
} /* namespace gr */

