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


#ifndef INCLUDED_OFDM_OFDM_SYNC_SCHMIDLCOX_CVC_H
#define INCLUDED_OFDM_OFDM_SYNC_SCHMIDLCOX_CVC_H

#include <ofdm/api.h>
#include <gr_hier_block2.h>

namespace gr {
  namespace ofdm {

    /*!
     * \brief <+description of block+>
     * \ingroup block
     *
     */
    class OFDM_API ofdm_sync_schmidlcox_cvc : virtual public gr_hier_block2
    {
    public:
       typedef boost::shared_ptr<ofdm_sync_schmidlcox_cvc> sptr;

       /*!
        * \brief Return a shared_ptr to a new instance of ofdm::ofdm_sync_schmidlcox_cvc.
        *
        * To avoid accidental use of raw pointers, ofdm::ofdm_sync_schmidlcox_cvc's
        * constructor is in a private implementation
        * class. ofdm::ofdm_sync_schmidlcox_cvc::make is the public interface for
        * creating new instances.
        */
       static sptr make(int fft_len, int cp_len);
    };

  } // namespace ofdm
} // namespace gr

#endif /* INCLUDED_OFDM_OFDM_SYNC_SCHMIDLCOX_CVC_H */

