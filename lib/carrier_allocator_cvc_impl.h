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

#ifndef INCLUDED_OFDM_CARRIER_ALLOCATOR_CVC_IMPL_H
#define INCLUDED_OFDM_CARRIER_ALLOCATOR_CVC_IMPL_H

#include <string>
#include <ofdm/carrier_allocator_cvc.h>

namespace gr {
  namespace ofdm {

    class carrier_allocator_cvc_impl : public carrier_allocator_cvc
    {
    private:
	    const int d_fft_len;
	   const std::vector<std::vector<int> > d_occupied_carriers;
	   const std::vector<std::vector<int> > d_pilot_carriers;
	   const std::vector<std::vector<gr_complex> > d_pilot_symbols;
	    std::string d_tag_len_key;
	    long d_input_size;
	    int d_mtu;

    public:
      carrier_allocator_cvc_impl(int fft_len,
		                 std::vector<std::vector<int> > occupied_carriers,
				 std::vector<std::vector<int> > pilot_carriers,
				 std::vector<std::vector<gr_complex> > pilot_symbols,
				 std::string tag_len_key);
      ~carrier_allocator_cvc_impl();

	void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      // Where all the action really happens
      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace ofdm
} // namespace gr

#endif /* INCLUDED_OFDM_CARRIER_ALLOCATOR_CVC_IMPL_H */

