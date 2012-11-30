/* -*- c++ -*- */

#define OFDM_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "ofdm_swig_doc.i"

%{
#include "ofdm/crc32_bb.h"
#include "ofdm/tagged_stream_mux.h"
#include "ofdm/carrier_allocator_cvc.h"
#include "ofdm/ofdm_header_bb.h"
#include "ofdm/scale_tags.h"
#include "ofdm/ofdm_frame_extractor.h"
%}


%include "ofdm/crc32_bb.h"
GR_SWIG_BLOCK_MAGIC2(ofdm, crc32_bb);
%include "ofdm/tagged_stream_mux.h"
GR_SWIG_BLOCK_MAGIC2(ofdm, tagged_stream_mux);
%include "ofdm/carrier_allocator_cvc.h"
GR_SWIG_BLOCK_MAGIC2(ofdm, carrier_allocator_cvc);
%include "ofdm/ofdm_header_bb.h"
GR_SWIG_BLOCK_MAGIC2(ofdm, ofdm_header_bb);

%include "ofdm/scale_tags.h"
GR_SWIG_BLOCK_MAGIC2(ofdm, scale_tags);
%include "ofdm/ofdm_frame_extractor.h"
GR_SWIG_BLOCK_MAGIC2(ofdm, ofdm_frame_extractor);
