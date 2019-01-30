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
// of PCM audio from a WIS GO7007 capture device.
// Implementation

#include "WISPCMAudioServerMediaSubsession.hh"
//#include "Options.hh"
//#include "AudioRTPCommon.hh"
#include <liveMedia.hh>

extern unsigned audioSamplingFrequency;
extern unsigned audioNumChannels;

WISPCMAudioServerMediaSubsession* WISPCMAudioServerMediaSubsession
::createNew(UsageEnvironment& env, APPROInput& Input, int  af) {
  return new WISPCMAudioServerMediaSubsession(env, Input, af);
}

WISPCMAudioServerMediaSubsession
::WISPCMAudioServerMediaSubsession(UsageEnvironment& env, APPROInput& Input, int  af)
  : WISServerMediaSubsession(env, Input,
			     audioSamplingFrequency*8*audioNumChannels) {
/*	if(Input.videoType == VIDEO_TYPE_MPEG4_CIF) {
	aFormat = af;
	} else {
		if(af >= 6)
			aFormat = 0;
		else
			aFormat = af;
	} */
	aFormat = af;
}

WISPCMAudioServerMediaSubsession::~WISPCMAudioServerMediaSubsession() {
}

FramedSource* WISPCMAudioServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  estBitrate = fEstimatedKbps;
  return fWISInput.audioSource();
}

RTPSink* WISPCMAudioServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/) {
  	setVideoRTPSinkBufferSize();
	if (aFormat == 0/*AFMT_PCM_ULAW*/) { 
		return SimpleRTPSink::createNew(envir(), rtpGroupsock, 0,
					audioSamplingFrequency, "audio", "PCMU", audioNumChannels);
	} else if (aFormat == 1/*AFMT_PCM_ALAW*/) {
		return SimpleRTPSink::createNew(envir(), rtpGroupsock, 8,
				audioSamplingFrequency, "audio", "PCMA", audioNumChannels);
	#if 0	
	} else if (aFormat == 6/*AFMT_AMR*/) {
		return AMRAudioRTPSink::createNew(envir(), rtpGroupsock,
					   97, aFormat, False, audioNumChannels);
	} else if (aFormat == 7/*AFMT_AMR*/) {
		return AMRAudioRTPSink::createNew(envir(), rtpGroupsock,
					   97, aFormat, False, audioNumChannels);
	} else if (aFormat == 8/*AFMT_AMR*/) {
		return AMRAudioRTPSink::createNew(envir(), rtpGroupsock,
					   97, aFormat, False, audioNumChannels);
	#endif 	
	}else if (aFormat == 2/*AFMT_PCM_G726_16*/) {
		return SimpleRTPSink::createNew(envir(), rtpGroupsock, 97,
				audioSamplingFrequency, "audio", "G726-16", audioNumChannels);
	}else if (aFormat == 3/*AFMT_PCM_G726_24*/) {
		return SimpleRTPSink::createNew(envir(), rtpGroupsock, 97,
				audioSamplingFrequency, "audio", "G726-24", audioNumChannels);
	}else if (aFormat == 4/*AFMT_PCM_G726_32*/) {
		return SimpleRTPSink::createNew(envir(), rtpGroupsock, 97,
				audioSamplingFrequency, "audio", "G726-32", audioNumChannels);
	}else if (aFormat == 5/*AFMT_PCM_G726_40*/) {
		return SimpleRTPSink::createNew(envir(), rtpGroupsock, 97,
				audioSamplingFrequency, "audio", "G726-40", audioNumChannels);
	}				
	else if (aFormat == 9/*AFMT_AAC_LC*/) {//by frank 2015, for Geo's AAC_LC
    	char const*enconderConfigStr = "1410";
		audioSamplingFrequency = 16000;//for Geo.
        return MPEG4GenericRTPSink::createNew(envir(), rtpGroupsock, 97,
                audioSamplingFrequency, "audio", "AAC-hbr", enconderConfigStr, audioNumChannels);		
	} else {
		return SimpleRTPSink::createNew(envir(), rtpGroupsock, 0,
					audioSamplingFrequency, "audio", "PCMU", audioNumChannels);
	}
}

