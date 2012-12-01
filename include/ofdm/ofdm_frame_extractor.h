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


#ifndef INCLUDED_OFDM_OFDM_FRAME_EXTRACTOR_H
#define INCLUDED_OFDM_OFDM_FRAME_EXTRACTOR_H

#include <ofdm/api.h>
#include <gr_block.h>

namespace gr {
  namespace ofdm {

    /*!
     * \brief <+description of block+>
     * \ingroup block
     *
     */
    class OFDM_API ofdm_frame_extractor : virtual public gr_block
    {
    public:
       typedef boost::shared_ptr<ofdm_frame_extractor> sptr;

       /*!
        * \brief Return a shared_ptr to a new instance of ofdm::ofdm_frame_extractor.
        *
        * To avoid accidental use of raw pointers, ofdm::ofdm_frame_extractor's
        * constructor is in a private implementation
        * class. ofdm::ofdm_frame_extractor::make is the public interface for
        * creating new instances.
        */
       static sptr make(int fftlen, int cplen);
    };

  } // namespace ofdm
} // namespace gr

#endif /* INCLUDED_OFDM_OFDM_FRAME_EXTRACTOR_H */

