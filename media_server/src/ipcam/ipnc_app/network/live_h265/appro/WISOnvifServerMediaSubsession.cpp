/*
 * Copyright (C) 2005-2006 WIS Technologies International Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and the associated README documentation file (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// A "ServerMediaSubsession" subclass for on-demand unicast streaming
// of motion-JPEG video from a WIS GO7007 capture device.
// Implementation

#include "WISOnvifServerMediaSubsession.hh"
#include "WISOnvifStreamSource.hh"
#include <OnvifVideoRTPSink.hh>

WISOnvifServerMediaSubsession* WISOnvifServerMediaSubsession
::createNew(UsageEnvironment& env, APPROInput& mjpegInput, unsigned estimatedBitrate) {
  return new WISOnvifServerMediaSubsession(env, mjpegInput, estimatedBitrate);
}

WISOnvifServerMediaSubsession
::WISOnvifServerMediaSubsession(UsageEnvironment& env, APPROInput& mjpegInput,
				    unsigned estimatedBitrate)
  : WISServerMediaSubsession(env, mjpegInput, estimatedBitrate) {
}

WISOnvifServerMediaSubsession::~WISOnvifServerMediaSubsession() {
}

FramedSource* WISOnvifServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  estBitrate = fEstimatedKbps;

  return WISOnvifStreamSource::createNew(fWISInput.OnvifSource());
}

RTPSink* WISOnvifServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char /*rtpPayloadTypeIfDynamic*/,
		   FramedSource* /*inputSource*/) {
  	setVideoRTPSinkBufferSize();
  	return OnvifVideoRTPSink::createNew(envir(), rtpGroupsock);
}
