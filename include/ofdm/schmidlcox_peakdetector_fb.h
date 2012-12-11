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


#ifndef INCLUDED_OFDM_SCHMIDLCOX_PEAKDETECTOR_FB_H
#define INCLUDED_OFDM_SCHMIDLCOX_PEAKDETECTOR_FB_H

#include <ofdm/api.h>
#include <gr_sync_block.h>

namespace gr {
  namespace ofdm {

    /*!
     * \brief <+description of block+>
     * \ingroup block
     *
     */
    class OFDM_API schmidlcox_peakdetector_fb : virtual public gr_sync_block
    {
    public:
       typedef boost::shared_ptr<schmidlcox_peakdetector_fb> sptr;

       /*!
        * \brief Return a shared_ptr to a new instance of ofdm::schmidlcox_peakdetector_fb.
        *
        * To avoid accidental use of raw pointers, ofdm::schmidlcox_peakdetector_fb's
        * constructor is in a private implementation
        * class. ofdm::schmidlcox_peakdetector_fb::make is the public interface for
        * creating new instances.
        */
       static sptr make(int cp_len);
    };

  } // namespace ofdm
} // namespace gr

#endif /* INCLUDED_OFDM_SCHMIDLCOX_PEAKDETECTOR_FB_H */

