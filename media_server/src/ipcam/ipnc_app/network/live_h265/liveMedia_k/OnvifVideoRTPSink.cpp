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
// Copyright (c) 1996-2007 Live Networks, Inc.  All rights reserved.
// RTP sink for JPEG video (RFC 2435)
// Implementation

#include "OnvifVideoRTPSink.hh"


OnvifVideoRTPSink
::OnvifVideoRTPSink(UsageEnvironment& env, Groupsock* RTPgs)
  : OnvifRTPSink(env, RTPgs, 107, 90000, "vnd.onvif.metadata") {
}

OnvifVideoRTPSink::~OnvifVideoRTPSink() {
}

OnvifVideoRTPSink*
OnvifVideoRTPSink::createNew(UsageEnvironment& env, Groupsock* RTPgs) {
  return new OnvifVideoRTPSink(env, RTPgs);
}

Boolean OnvifVideoRTPSink::sourceIsCompatibleWithUs(MediaSource& source) {
	return True;	
}

Boolean OnvifVideoRTPSink
::frameCanAppearAfterPacketStart(unsigned char const* /*frameStart*/,
				 unsigned /*numBytesInFrame*/) const {
  // A packet can contain only one frame
  return False;
}

void OnvifVideoRTPSink
::doSpecialFrameHandling(unsigned fragmentationOffset,
			 unsigned char* /*frameStart*/,
			 unsigned /*numBytesInFrame*/,
			 struct timeval frameTimestamp,
			 unsigned numRemainingBytes) {
  // Our source is known to be a JPEGVideoSource 
  if (numRemainingBytes == 0) {
    // This packet contains the last (or only) fragment of the frame.
    // Set the RTP 'M' ('marker') bit:
    setMarkerBit();
  }

  // Also set the RTP timestamp:
  setTimestamp(frameTimestamp);
}


unsigned OnvifVideoRTPSink::specialHeaderSize() const {
  
  unsigned headerSize =0; // by default
 
  return headerSize;
}
