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


#ifndef INCLUDED_OFDM_CRC32_BB_H
#define INCLUDED_OFDM_CRC32_BB_H

#include <ofdm/api.h>
#include <gr_block.h>
#include <string>

namespace gr {
  namespace ofdm {

    /*!
     * \brief Byte-stream CRC block
     * \ingroup ofdm
     *
     * Input: stream of bytes, which form a packet. The first byte of the packet
     * has a tag with key "length" and the value being the number of bytes in the
     * packet.
     *
     * Output: The same bytes as incoming, but trailing a CRC32 of the packet.
     * The tag is re-set to the new length.
     */
    class OFDM_API crc32_bb : virtual public gr_block
    {
    public:
       typedef boost::shared_ptr<crc32_bb> sptr;
       static sptr make(bool check=false, int mtu=4096, const std::string& lengthtagname="length");
    };

  } // namespace ofdm
} // namespace gr

#endif /* INCLUDED_OFDM_CRC32_BB_H */

