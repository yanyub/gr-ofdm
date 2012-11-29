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
#include "tagged_stream_mux_impl.h"

namespace gr {
  namespace ofdm {

    tagged_stream_mux::sptr
    tagged_stream_mux::make(size_t itemsize, unsigned int nstreams, std::string lengthtagname, long MTU)
    {
      return gnuradio::get_initial_sptr (new tagged_stream_mux_impl(itemsize, nstreams, lengthtagname, MTU));
    }

    /*
     * The private constructor
     */
    tagged_stream_mux_impl::tagged_stream_mux_impl(size_t itemsize, unsigned int nstreams, std::string lengthtagname, long MTU)
      : gr_block("tagged_stream_mux",
                 gr_make_io_signature(nstreams, nstreams, itemsize),
                 gr_make_io_signature(1, 1, itemsize)),
        d_lengthtagname(lengthtagname), d_MTU(MTU), d_itemsize(itemsize), d_nstreams(nstreams)
    {
      for (unsigned int i=0; i<d_nstreams; i++) {
        d_forecast.push_back(1);
      }
      set_output_multiple(d_MTU);
      set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    tagged_stream_mux_impl::~tagged_stream_mux_impl()
    {
    }

    void
    tagged_stream_mux_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      for (unsigned int i=0; i<d_nstreams; i++) {
	    ninput_items_required[i] = d_forecast[i];
      }
    }

    int
    tagged_stream_mux_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        // Loop through all the inputs and check if we have an entire packet waiting for each
        // one.
        std::vector<long> packet_lengths;
        bool ready = true;
        long packet_length_sum = 0;
        
        for (unsigned int i=0; i<d_nstreams; i++) {
          long packet_length = 0;
          std::vector<gr_tag_t> tags;
          const uint64_t offset = this->nitems_written(0);
          this->get_tags_in_range(tags, i, nitems_read(i), nitems_read(i)+1);
          //const size_t ninput_items = noutput_items; //assumption for sync block, this can change
          for (unsigned int j = 0; j < tags.size(); j++) {
            if (pmt::pmt_symbol_to_string(tags[j].key) == d_lengthtagname) {
              packet_length = pmt::pmt_to_long(tags[j].value);
            }
          }
          assert(packet_length != 0);
          packet_lengths.push_back(packet_length);
          if (ninput_items[i] < packet_length ) {
            ready = false;
          }
          d_forecast[i] = packet_length - ninput_items[i];
        }
        char *out = (char *) output_items[0];
        if (ready) {
          for (unsigned int i=0; i<d_nstreams; i++) {
            const char *in = (const char *) input_items[i];

            std::vector<gr_tag_t> tags;
            this->get_tags_in_range(tags, i, nitems_read(i), nitems_read(i)+packet_lengths[i]);
            for (unsigned int j = 0; j < tags.size(); j++) {
              if (pmt::pmt_symbol_to_string(tags[j].key) != d_lengthtagname) {
                const uint64_t offset = packet_length_sum + tags[j].offset - nitems_read(i) + nitems_written(0);
                this->add_item_tag(0, offset, tags[j].key, tags[j].value);
              }
            }
            memcpy((void *) (out+packet_length_sum*d_itemsize), (const void *) in, packet_lengths[i]*d_itemsize);
            packet_length_sum += packet_lengths[i];
            consume(i, packet_lengths[i]);
            d_forecast[i] = 1;
          }
          this->add_item_tag(0, nitems_written(0), pmt::pmt_string_to_symbol(d_lengthtagname), pmt::pmt_from_long(packet_length_sum));
          return packet_length_sum;
        }
        return 0;
    }


  } /* namespace ofdm */
} /* namespace gr */

