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
// A class that encapsulates a 'FramedSource' object that
// returns JPEG video frames.
// Implementation
#include "WISOnvifStreamSource.hh"
#include <fcntl.h>
#include <sys/stat.h>
 #include <sys/types.h>

WISOnvifStreamSource*
WISOnvifStreamSource::createNew(FramedSource* inputSource) {
  return new WISOnvifStreamSource(inputSource);
}

WISOnvifStreamSource::WISOnvifStreamSource(FramedSource* inputSource)
  : OnvifVideoSource(inputSource->envir())
    {
  fSource = inputSource;
 
}

WISOnvifStreamSource::~WISOnvifStreamSource() {
  Medium::close(fSource);
}

void WISOnvifStreamSource::doGetNextFrame() {
	
  fSource->getNextFrame(fBuffer, sizeof fBuffer,
			WISOnvifStreamSource::afterGettingFrame, this,
			FramedSource::handleClosure, this);
}

void WISOnvifStreamSource
::afterGettingFrame(void* clientData, unsigned frameSize,
		    unsigned numTruncatedBytes,
		    struct timeval presentationTime,
		    unsigned durationInMicroseconds) {
  WISOnvifStreamSource* source = (WISOnvifStreamSource*)clientData;
  source->afterGettingFrame1(frameSize, numTruncatedBytes,
			     presentationTime, durationInMicroseconds);
}


void WISOnvifStreamSource
::afterGettingFrame1(unsigned frameSize, unsigned numTruncatedBytes,
		     struct timeval presentationTime,
		     unsigned durationInMicroseconds) {
  fFrameSize = frameSize < fMaxSize ? frameSize : fMaxSize;



  fFrameSize = frameSize;	
  memmove(fTo, &fBuffer, fFrameSize);	
  fNumTruncatedBytes = numTruncatedBytes;
  fPresentationTime = presentationTime;
  fDurationInMicroseconds = fDurationInMicroseconds;

  FramedSource::afterGetting(this);
}
