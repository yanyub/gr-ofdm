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

#ifndef INCLUDED_OFDM_MUX_IMPL_H
#define INCLUDED_OFDM_MUX_IMPL_H

#include <ofdm/tagged_stream_mux.h>
#include <vector>

namespace gr {
  namespace ofdm {

    class tagged_stream_mux_impl : public tagged_stream_mux
    {
    private:
      std::string d_lengthtagname;
      long d_MTU;
      size_t d_itemsize;
      std::vector<long> d_forecast;
      size_t d_nstreams;

    public:
      tagged_stream_mux_impl(size_t itemsize, unsigned int nstreams, std::string lengthtagname, long MTU);
      ~tagged_stream_mux_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      // Where all the action really happens
      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace ofdm
} // namespace gr

#endif /* INCLUDED_OFDM_MUX_IMPL_H */

