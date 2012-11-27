/* -*- c++ -*- */

#define OFDM_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "ofdm_swig_doc.i"

%{
#include "ofdm/crc32_bb.h"
#include "ofdm/ofdm_header_bb.h"
%}


%include "ofdm/crc32_bb.h"
GR_SWIG_BLOCK_MAGIC2(ofdm, crc32_bb);
%include "ofdm/ofdm_header_bb.h"
GR_SWIG_BLOCK_MAGIC2(ofdm, ofdm_header_bb);
