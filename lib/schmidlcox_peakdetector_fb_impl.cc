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
#include "schmidlcox_peakdetector_fb_impl.h"

namespace gr {
  namespace ofdm {

    schmidlcox_peakdetector_fb::sptr
    schmidlcox_peakdetector_fb::make(int cp_len)
    {
      return gnuradio::get_initial_sptr (new schmidlcox_peakdetector_fb_impl(cp_len));
    }

    /*
     * The private constructor
     */
    schmidlcox_peakdetector_fb_impl::schmidlcox_peakdetector_fb_impl(int cp_len)
      : gr_sync_block("schmidlcox_peakdetector_fb",
		      gr_make_io_signature(1, 1, sizeof (float)),
		      gr_make_io_signature(1, 1, sizeof (unsigned char)))
    {}

    schmidlcox_peakdetector_fb_impl::~schmidlcox_peakdetector_fb_impl()
    {
    }

    int
    schmidlcox_peakdetector_fb_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const float *in = (const float *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
	bool in_plateau = false;
	int flank_start;
	int flank_end;

	memset((void *) out, 0x00, noutput_items);
	int i;
	for (i = 0; i < noutput_items - 2*d_cp_len; i++) {
		if (in_plateau) {
			if (in[i] < 0.9) {
				in_plateau = false;
				flank_end = i;
				out[flank_start + (flank_end-flank_start)/2] = 1;
			}
		} else {
			if (in[i] >= 0.9) {
				in_plateau = true;
				flank_start = i;
			}
		}
	}
	// Make sure we find the end of a peak if we found the beginning
	if (in_plateau) {
		while (i < noutput_items && in[i] >= 0.9)
			i++;
		flank_end = i;
		out[flank_start + (flank_end-flank_start)/2] = 1;
	}

        return i;
    }

  } /* namespace ofdm */
} /* namespace gr */

