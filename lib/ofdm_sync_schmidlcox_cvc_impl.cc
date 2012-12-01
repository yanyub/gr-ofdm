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
#include "ofdm_sync_schmidlcox_cvc_impl.h"

#include <gr_delay.h>
#include <blocks/conjugate_cc.h>
#include <blocks/multiply_cc.h>
#include <filter/fir_filter_ccf.h>
#include <blocks/complex_to_mag_squared.h>
#include <filter/fir_filter_fff.h>
#include <blocks/multiply_ff.h>
#include <blocks/divide_ff.h>
#include <blocks/complex_to_arg.h>
#include <gr_peak_detector_fb.h>

namespace gr {
  namespace ofdm {

    ofdm_sync_schmidlcox_cvc::sptr
    ofdm_sync_schmidlcox_cvc::make(int fft_len, int cp_len)
    {
      return gnuradio::get_initial_sptr (new ofdm_sync_schmidlcox_cvc_impl(fft_len, cp_len));
    }

    /*
     * The private constructor
     */
    ofdm_sync_schmidlcox_cvc_impl::ofdm_sync_schmidlcox_cvc_impl(int fft_len, int cp_len)
      : gr_hier_block2("ofdm_sync_schmidlcox_cvc",
		      gr_make_io_signature(1, 1, sizeof (gr_complex)),
		      gr_make_io_signature3(3, 3, sizeof (float), sizeof (unsigned char), sizeof (float)))
    {
	std::vector<float> ma_taps(fft_len/2, 1.0);
	gr_delay_sptr                    delay(gr_make_delay(sizeof(gr_complex), fft_len/2));
	gr::blocks::conjugate_cc::sptr   delay_conjugate(gr::blocks::conjugate_cc::make());
	gr::blocks::multiply_cc::sptr    delay_corr(gr::blocks::multiply_cc::make());
	gr::filter::fir_filter_ccf::sptr delay_ma(gr::filter::fir_filter_ccf::make(1, ma_taps));
	gr::blocks::complex_to_mag_squared::sptr delay_magsquare(gr::blocks::complex_to_mag_squared::make());
	gr::blocks::divide_ff::sptr      delay_normalize(gr::blocks::divide_ff::make());

	gr::blocks::complex_to_mag_squared::sptr normalizer_magsquare(gr::blocks::complex_to_mag_squared::make());
	gr::filter::fir_filter_fff::sptr         normalizer_ma(gr::filter::fir_filter_fff::make(1, ma_taps));
	gr::blocks::multiply_ff::sptr            normalizer_square(gr::blocks::multiply_ff::make());

	gr::blocks::complex_to_arg::sptr         peak_to_angle(gr::blocks::complex_to_arg::make());

	// This turns the plateau into a peak
	std::vector<float> mf_taps(cp_len, 1.0/cp_len);
	gr::filter::fir_filter_fff::sptr         matched_filter(gr::filter::fir_filter_fff::make(1, mf_taps));
	gr_peak_detector_fb_sptr                 peak_detector(gr_make_peak_detector_fb(0.20, 0.20, 30, 0.001));

	// Delay Path
        connect(self(),               0, delay, 0);
        connect(delay,                0, delay_conjugate, 0);
        connect(delay_conjugate,      0, delay_corr, 1);
        connect(self(),               0, delay_corr, 0);
        connect(delay_corr,           0, delay_ma, 0);
        connect(delay_ma,             0, delay_magsquare, 0);
        connect(delay_magsquare,      0, delay_normalize, 0);
	// Energy Path
        connect(self(),               0, normalizer_magsquare, 0);
        connect(normalizer_magsquare, 0, normalizer_ma, 0);
        connect(normalizer_ma,        0, normalizer_square, 0);
        connect(normalizer_ma,        0, normalizer_square, 1);
        connect(normalizer_square,    0, delay_normalize, 1);
	// Peak detect
        connect(delay_normalize,      0, matched_filter, 0);
        connect(delay_normalize,      0, self(), 2);
        connect(matched_filter,       0, peak_detector, 0);
        connect(peak_detector,        0, self(), 1);
	// Frequency Output
        connect(delay_ma,             0, peak_to_angle, 0);
        connect(peak_to_angle,        0, self(), 0);
    }

    ofdm_sync_schmidlcox_cvc_impl::~ofdm_sync_schmidlcox_cvc_impl()
    {
    }



  } /* namespace ofdm */
} /* namespace gr */

