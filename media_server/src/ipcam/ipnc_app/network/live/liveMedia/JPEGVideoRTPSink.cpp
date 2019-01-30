/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2012 Live Networks, Inc.  All rights reserved.
// RTP sink for JPEG video (RFC 2435)
// Implementation

#ifdef _ORG_TI
#include "JPEGVideoRTPSink.hh"
#include "JPEGVideoSource.hh"

static int pk_cnt = 0;

JPEGVideoRTPSink
::JPEGVideoRTPSink(UsageEnvironment& env, Groupsock* RTPgs)
  : VideoRTPSink(env, RTPgs, 26, 90000, "JPEG") {
}

JPEGVideoRTPSink::~JPEGVideoRTPSink() {
}

JPEGVideoRTPSink*
JPEGVideoRTPSink::createNew(UsageEnvironment& env, Groupsock* RTPgs) {
  return new JPEGVideoRTPSink(env, RTPgs);
}

Boolean JPEGVideoRTPSink::sourceIsCompatibleWithUs(MediaSource& source) {
  return source.isJPEGVideoSource();
}

Boolean JPEGVideoRTPSink
::frameCanAppearAfterPacketStart(unsigned char const* /*frameStart*/,
				 unsigned /*numBytesInFrame*/) const {
  // A packet can contain only one frame
  return False;
}

void JPEGVideoRTPSink
::doSpecialFrameHandling(unsigned fragmentationOffset,
			 unsigned char* /*frameStart*/,
			 unsigned /*numBytesInFrame*/,
			 struct timeval framePresentationTime,
			 unsigned numRemainingBytes) {
  // Our source is known to be a JPEGVideoSource
  JPEGVideoSource* source = (JPEGVideoSource*)fSource;
  if (source == NULL) return; // sanity check

  //u_int8_t mainJPEGHeader[8]; // the special header
  u_int8_t mainJPEGHeader[12]; // the special header


  mainJPEGHeader[0] = 0; // Type-specific
  mainJPEGHeader[1] = fragmentationOffset >> 16;
  mainJPEGHeader[2] = fragmentationOffset >> 8;
  mainJPEGHeader[3] = fragmentationOffset;
  mainJPEGHeader[4] = source->type();
  mainJPEGHeader[5] = source->qFactor();
  mainJPEGHeader[6] = source->width();
  mainJPEGHeader[7] = source->height();
  mainJPEGHeader[8] = 0x00;
  mainJPEGHeader[9] = 0x00;
  mainJPEGHeader[10] = 0xFF;
  mainJPEGHeader[11] = 0xFF;
  setSpecialHeaderBytes(mainJPEGHeader, sizeof mainJPEGHeader);

  if (fragmentationOffset == 0 && source->qFactor() >= 128) {
    // There is also a Quantization Header:
    u_int8_t precision;
    u_int16_t length;
    u_int8_t const* quantizationTables
      = source->quantizationTables(precision, length);

    unsigned const quantizationHeaderSize = 4 + length;
    u_int8_t* quantizationHeader = new u_int8_t[quantizationHeaderSize];

    quantizationHeader[0] = 0; // MBZ
    quantizationHeader[1] = precision;
    quantizationHeader[2] = length >> 8;
    quantizationHeader[3] = length&0xFF;
    if (quantizationTables != NULL) { // sanity check
      for (u_int16_t i = 0; i < length; ++i) {
	quantizationHeader[4+i] = quantizationTables[i];
      }
    }

    setSpecialHeaderBytes(quantizationHeader, quantizationHeaderSize,
			  sizeof mainJPEGHeader /* start position */);
    delete[] quantizationHeader;
  }

  if (numRemainingBytes == 0) {
    // This packet contains the last (or only) fragment of the frame.
    // Set the RTP 'M' ('marker') bit:
    setMarkerBit();
  }

  // Also set the RTP timestamp:
  setTimestamp(framePresentationTime);
}


unsigned JPEGVideoRTPSink::specialHeaderSize() const {
  // Our source is known to be a JPEGVideoSource
  JPEGVideoSource* source = (JPEGVideoSource*)fSource; 

  //unsigned headerSize = 8; // by default
  unsigned headerSize = 12; // by default

  if (curFragmentationOffset() == 0 && source->qFactor() >= 128) {
    // There is also a Quantization Header:
    u_int8_t dummy;
    u_int16_t quantizationTablesSize;
    (void)(source->quantizationTables(dummy, quantizationTablesSize));

    headerSize += 4 + quantizationTablesSize;
  }

  return headerSize;
}
#else
/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2011 Live Networks, Inc.  All rights reserved.
// RTP sink for JPEG video (RFC 2435)
// Implementation

#include "JPEGVideoRTPSink.hh"
#include "JPEGVideoSource.hh"

JPEGVideoRTPSink
::JPEGVideoRTPSink(UsageEnvironment& env, Groupsock* RTPgs)
  : VideoRTPSink(env, RTPgs, 26, 90000, "JPEG") {
}

JPEGVideoRTPSink::~JPEGVideoRTPSink() {
}

JPEGVideoRTPSink*
JPEGVideoRTPSink::createNew(UsageEnvironment& env, Groupsock* RTPgs) {
  return new JPEGVideoRTPSink(env, RTPgs);
}

Boolean JPEGVideoRTPSink::sourceIsCompatibleWithUs(MediaSource& source) {
  return source.isJPEGVideoSource();
}

Boolean JPEGVideoRTPSink
::frameCanAppearAfterPacketStart(unsigned char const* /*frameStart*/,
				 unsigned /*numBytesInFrame*/) const {
  // A packet can contain only one frame
  return False;
}

void JPEGVideoRTPSink
::doSpecialFrameHandling(unsigned fragmentationOffset,
			 unsigned char* /*frameStart*/,
			 unsigned /*numBytesInFrame*/,
			 struct timeval frameTimestamp,
			 unsigned numRemainingBytes) {
  // Our source is known to be a JPEGVideoSource
  JPEGVideoSource* source = (JPEGVideoSource*)fSource; 
  unsigned header_offset = 0;
  if (source == NULL) return; // sanity check

  u_int8_t mainJPEGHeader[8]; // the special header
  u_int8_t const type = source->type();

  mainJPEGHeader[0] = 0; // Type-specific
  mainJPEGHeader[1] = fragmentationOffset >> 16;
  mainJPEGHeader[2] = fragmentationOffset >> 8;
  mainJPEGHeader[3] = fragmentationOffset;
  mainJPEGHeader[4] = type;
  mainJPEGHeader[5] = source->qFactor();
  mainJPEGHeader[6] = source->width();
  mainJPEGHeader[7] = source->height();
  setSpecialHeaderBytes(mainJPEGHeader, sizeof mainJPEGHeader);
#if 0
  unsigned restartMarkerHeaderSize = 0; // by default
  if (type >= 64 && type <= 127) {
    // There is also a Restart Marker Header:
    restartMarkerHeaderSize = 4;
    u_int16_t const restartInterval = source->restartInterval(); // should be non-zero

    u_int8_t restartMarkerHeader[4];
    restartMarkerHeader[0] = restartInterval>>8;
    restartMarkerHeader[1] = restartInterval&0xFF;
    restartMarkerHeader[2] = restartMarkerHeader[3] = 0xFF; // F=L=1; Restart Count = 0x3FFF

    setSpecialHeaderBytes(restartMarkerHeader, restartMarkerHeaderSize,
                          sizeof mainJPEGHeader/* start position */);
  }
#endif

  header_offset += sizeof mainJPEGHeader;
  if( source->type() > 63 )
  {
	u_int8_t precision;
    u_int16_t length;
    u_int8_t const* restartintervalTables
      = source->restartIntervalTables(precision, length);
	setSpecialHeaderBytes(restartintervalTables, length,
			  header_offset /* start position */);
	header_offset += length;
  }

  if (fragmentationOffset == 0 && source->qFactor() >= 128) {
    // There is also a Quantization Header:
    u_int8_t precision;
    u_int16_t length;
    u_int8_t const* quantizationTables
      = source->quantizationTables(precision, length);
    
    unsigned const quantizationHeaderSize = 4 + length;
    u_int8_t* quantizationHeader = new u_int8_t[quantizationHeaderSize];

    quantizationHeader[0] = 0; // MBZ
    quantizationHeader[1] = precision;
    quantizationHeader[2] = length >> 8;
    quantizationHeader[3] = length&0xFF;
    if (quantizationTables != NULL) { // sanity check
      for (u_int16_t i = 0; i < length; ++i) {
	quantizationHeader[4+i] = quantizationTables[i];
      }
    }
#if 0
    setSpecialHeaderBytes(quantizationHeader, quantizationHeaderSize,
			  sizeof mainJPEGHeader + restartMarkerHeaderSize/* start position */);
#endif

    setSpecialHeaderBytes(quantizationHeader, quantizationHeaderSize,
			  header_offset /* start position */);

    delete[] quantizationHeader;
  }

  if (numRemainingBytes == 0) {
    // This packet contains the last (or only) fragment of the frame.
    // Set the RTP 'M' ('marker') bit:
    setMarkerBit();
  }

  // Also set the RTP timestamp:
  setTimestamp(frameTimestamp);
}


unsigned JPEGVideoRTPSink::specialHeaderSize() const {
  // Our source is known to be a JPEGVideoSource
  JPEGVideoSource* source = (JPEGVideoSource*)fSource; 
  if (source == NULL) return 0; // sanity check

  unsigned headerSize = 8; // by default
/*
  u_int8_t const type = source->type();
  if (type >= 64 && type <= 127) {
    // There is also a Restart Marker Header:
    headerSize += 4;
  }
*/


  if (curFragmentationOffset() == 0 && source->qFactor() >= 128) {
    // There is also a Quantization Header:
    u_int8_t dummy;
    u_int16_t quantizationTablesSize;
    (void)(source->quantizationTables(dummy, quantizationTablesSize));

    headerSize += 4 + quantizationTablesSize;
  }

  if(source->type() > 63 )
  {
	headerSize += 4;
  }

  // Note: We assume that there are no 'restart markers'

  return headerSize;
}

#endif