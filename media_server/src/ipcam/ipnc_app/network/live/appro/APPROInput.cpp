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
// An interface to the WIS GO7007 capture device.
// Implementation

#include "APPROInput.hh"
#include "WISPCMAudioServerMediaSubsession.hh"
//#include "Options.hh"
#include "Err.hh"
#include "Base64.hh"
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <linux/soundcard.h>
#include <sys/types.h>

//#include <system_default.h>

#include "sp_enc.h"
#include "typedef.h"
#include "interf_rom.h"
#include "fixed.h"
#include <string.h>
#include "interf_enc.h"
#include "amr.h"
#include "rom_enc.h"
extern AMR_state amr_ptr;
extern int is_ssm;
extern unsigned audioSamplingFrequency;
extern unsigned audioNumChannels;
extern int audio_enable;
extern int audioFormat;
extern int reset_flag;
extern int audio_reset_flag;
extern int video_priority;
extern int audio_priority;
extern int rtsp_srv_socket;
extern int update_sw;

#define __DEBUG
#ifdef __DEBUG
#define __D(fmt, args...) fprintf(stderr, "Stream Debug: " fmt, ## args)
#else
#define __D(fmt, args...)
#endif

#define __E(fmt, args...) fprintf(stderr, "Stream Error: " fmt, ## args)

#define stream_out_audio_size	128
#if 0
int  amr_size[3]={ 31,26,20};/*RS*/
int AMR_GET_SIZE[3]={155,130,100}; /*RS*/
#else
int  amr_size[3]={ 31,26,20};/*RS*/
int AMR_GET_SIZE[3]={160,135,105}; /*RS*/
#endif

signed short _u2l[256] = {

	-32124,-31100,-30076,-29052,-28028,-27004,-25980,-24956,

	-23932,-22908,-21884,-20860,-19836,-18812,-17788,-16764,

	-15996,-15484,-14972,-14460,-13948,-13436,-12924,-12412,

	-11900,-11388,-10876,-10364, -9852, -9340, -8828, -8316,

	 -7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140,

	 -5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092,

	 -3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004,

	 -2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980,

	 -1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436,

	 -1372, -1308, -1244, -1180, -1116, -1052,  -988,  -924,

	  -876,  -844,  -812,  -780,  -748,  -716,  -684,  -652,

	  -620,  -588,  -556,  -524,  -492,  -460,  -428,  -396,

	  -372,  -356,  -340,  -324,  -308,  -292,  -276,  -260,

	  -244,  -228,  -212,  -196,  -180,  -164,  -148,  -132,

	  -120,  -112,  -104,   -96,   -88,   -80,   -72,   -64,

	   -56,   -48,   -40,   -32,   -24,   -16,    -8,    -2,

	 32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956,

	 23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764,

	 15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412,

	 11900, 11388, 10876, 10364,  9852,  9340,  8828,  8316,

	  7932,  7676,  7420,  7164,  6908,  6652,  6396,  6140,

	  5884,  5628,  5372,  5116,  4860,  4604,  4348,  4092,

	  3900,  3772,  3644,  3516,  3388,  3260,  3132,  3004,

	  2876,  2748,  2620,  2492,  2364,  2236,  2108,  1980,

	  1884,  1820,  1756,  1692,  1628,  1564,  1500,  1436,

	  1372,  1308,  1244,  1180,  1116,  1052,   988,   924,

	   876,   844,   812,   780,   748,   716,   684,   652,

	   620,   588,   556,   524,   492,   460,   428,   396,

	   372,   356,   340,   324,   308,   292,   276,   260,

	   244,   228,   212,   196,   180,   164,   148,   132,

	   120,   112,   104,    96,    88,    80,    72,    64,

	    56,    48,    40,    32,    24,    16,     8,     2,
};

//#define STREAM_DETAIL_OUTPUT
#define TIME_GET_WAY (0)

extern unsigned audioSamplingFrequency;
extern unsigned audioNumChannels;
extern int audio_enable;
extern void sigterm(int dummy);
extern int reset_flag;
////////// OpenFileSource definition //////////

#define EventMsg 	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n\
<SOAP-ENV:Envelope \
  xmlns:SOAP-ENV=\"http://www.w3.org/2003/05/soap-envelope\" \r\n\
  xmlns:SOAP-ENC=\"http://www.w3.org/2003/05/soap-encoding\" \r\n\
  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \r\n\
  xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" \r\n\
  xmlns:c14n=\"http://www.w3.org/2001/10/xml-exc-c14n#\" \r\n\ 
  xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\" \r\n\  
  xmlns:xenc=\"http://www.w3.org/2001/04/xmlenc#\" \r\n\  
  xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#\" \r\n\  
  xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\" \r\n\  
  xmlns:wsa5=\"http://www.w3.org/2005/08/addressing\" \r\n\  
  xmlns:xmime=\"http://tempuri.org/xmime.xsd\" \r\n\  
  xmlns:xop=\"http://www.w3.org/2004/08/xop/include\" \r\n\  
  xmlns:wsbf=\"http://docs.oasis-open.org/wsrf/bf-2\" \r\n\  
  xmlns:wsr=\"http://docs.oasis-open.org/wsrf/r-2\" \r\n\  
  xmlns:wstop=\"http://docs.oasis-open.org/wsn/t-1\" \r\n\  
  xmlns:tns1=\"http://www.onvif.org/ver10/topics\" \r\n\  
  xmlns:tt=\"http://www.onvif.org/ver10/schema\" \r\n\  
  xmlns:tetcp=\"http://www.onvif.org/ver10/events/wsdl/CreatePullPointBinding\" \r\n\  
  xmlns:tete=\"http://www.onvif.org/ver10/events/wsdl/EventBinding\" \r\n\  
  xmlns:tetnc=\"http://www.onvif.org/ver10/events/wsdl/NotificationConsumerBinding\" \r\n\  
  xmlns:tetnp=\"http://www.onvif.org/ver10/events/wsdl/NotificationProducerBinding\" \r\n\  
  xmlns:tetpp=\"http://www.onvif.org/ver10/events/wsdl/PullPointBinding\" \r\n\  
  xmlns:tetpps=\"http://www.onvif.org/ver10/events/wsdl/PullPointSubscriptionBinding\" \r\n\  
  xmlns:tev=\"http://www.onvif.org/ver10/events/wsdl\" \r\n\  
  xmlns:tetps=\"http://www.onvif.org/ver10/events/wsdl/PausableSubscriptionManagerBinding\" \r\n\  
  xmlns:wsnt=\"http://docs.oasis-open.org/wsn/b-2\" \r\n\  
  xmlns:tetsm=\"http://www.onvif.org/ver10/events/wsdl/SubscriptionManagerBinding\"> \r\n\ 
	<SOAP-ENV:Body>\r\n\
		<wsnt:Notify>\r\n\
		<wsnt:NotificationMessage>\r\n\	
		<wsnt:Topic Dialect=\"http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet\">\r\n\
			tns1:VideoSource/tns1:MotionAlarm\r\n\
		</wsnt:Topic>\r\n\
		<wsnt:Message>\r\n\
		<wsnt:Message PropertyOperation=\"Initialized\" UtcTime=\"2012-1-11T13:35:10Z\">\r\n\
		<tt:Source>\r\n\
		<tt:SimpleItem Value=\"1\" Name=\"VideoSourceToken\"></tt:SimpleItem>\r\n\
		</tt:Source>\r\n\
		<tt:Data>\r\n\
		<tt:SimpleItem Value=\"true\" Name=\"State\"></tt:SimpleItem>\r\n\
		</tt:Data>\r\n\
		</wsnt:NotificationMessage>\r\n\
		</wsnt:Notify>\r\n\
	</SOAP-ENV:Body>\r\n\
</SOAP-ENV:Envelope>"

// A common "FramedSource" subclass, used for reading from an open file:

class OpenFileSource: public FramedSource {
public:
  int  uSecsToDelay;
  int  uSecsToDelayMax;
  int  srcType;
protected:
  OpenFileSource(UsageEnvironment& env, APPROInput& input);
  virtual ~OpenFileSource();

  virtual int readFromFile() = 0;

private: // redefined virtual functions:
  virtual void doGetNextFrame();

private:
  static void incomingDataHandler(OpenFileSource* source);
  void incomingDataHandler1();

protected:
  APPROInput& fInput;

//  int fFileNo;
};


////////// VideoOpenFileSource definition //////////

class VideoOpenFileSource: public OpenFileSource {
public:
  VideoOpenFileSource(UsageEnvironment& env, APPROInput& input);
  virtual ~VideoOpenFileSource();

protected: // redefined virtual functions:
  virtual int readFromFile();
  virtual int readFromFile264();
  unsigned int SerialBook;
  unsigned int SerialLock;
  int StreamFlag;
  struct timeval fPresentationTimePre;
  int IsStart;
  int nal_state;
  int Checkduration;
};
//
class OnvifOpenFileSource: public OpenFileSource {
public:
  OnvifOpenFileSource(UsageEnvironment& env, APPROInput& input);
  virtual ~OnvifOpenFileSource();

protected: // redefined virtual functions:
  virtual int readFromFile();
 
};



#define STREAM_GET_VOL    0x0001
#define STREAM_NEW_GOP    0x0002

////////// AudioOpenFileSource definition //////////

class AudioOpenFileSource: public OpenFileSource {
public:
  AudioOpenFileSource(UsageEnvironment& env, APPROInput& input);
  virtual ~AudioOpenFileSource();

protected: // redefined virtual functions:
  virtual int readFromFile();
  int getAudioData();

  unsigned int AudioBook;
  unsigned int AudioLock;
  struct timeval fPresentationTimePre;
  int IsStart;
  int AudioRemand;
  int Amr_flag;
  unsigned int AudioOrg_s;
  unsigned char tmp_audio[2048];

};

static signed short	ulaw2linear(unsigned char u)
{
	return(_u2l[u]);
}

static int audio_g711_dec(unsigned short *dest, unsigned char *src,
                      int bufsize)
{
    unsigned short  i;

    short nOut;

    unsigned char input;

    for (i = 0; i < bufsize; i++) {
        input = *(src + i);
	    nOut  = ulaw2linear(input) ;
        *(dest + i) = nOut;
    }

	return bufsize;
}

long timevaldiff(struct timeval *starttime, struct timeval *finishtime)
{
  long msec;
  msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
  msec+=(finishtime->tv_usec-starttime->tv_usec)/1000;
  return msec;
}

static void mcpy(unsigned char *dst, unsigned char *src, int size)
{
	int i,n;

	for (i = 0; i < size; i++)
		dst[i] = src[i];

	return;
}

void printErr(UsageEnvironment& env, char const* str = NULL) {
  if (str != NULL) err(env) << str;
  env << ": " << strerror(env.getErrno()) << "\n";
}

////////// APPROInput implementation //////////

APPROInput* APPROInput::createNew(UsageEnvironment& env, int vType) {
  return new APPROInput(env, vType);
}

FramedSource* APPROInput::videoSource() {

  if (fOurVideoSource == NULL) {
    fOurVideoSource = new VideoOpenFileSource(envir(), *this);
  }

  return fOurVideoSource;
}

FramedSource* APPROInput::OnvifSource() {

  if (fOurOnvifSource == NULL) {
    fOurOnvifSource = new OnvifOpenFileSource(envir(), *this);
  }

  return fOurOnvifSource;
}

FramedSource* APPROInput::audioSource() {

  if (fOurAudioSource == NULL) {
    fOurAudioSource = new AudioOpenFileSource(envir(), *this);
  }

  return fOurAudioSource;
}

APPROInput::APPROInput(UsageEnvironment& env, int vType)
  : Medium(env), videoType(vType), fOurVideoSource(NULL), fOurAudioSource(NULL),fOurOnvifSource(NULL) {
}

APPROInput::~APPROInput() {
 if( fOurVideoSource )
 {
 	delete (VideoOpenFileSource *)fOurVideoSource;
 	fOurVideoSource = NULL;
 }

 if( fOurAudioSource )
 {
 	delete (AudioOpenFileSource *)fOurAudioSource;
 	fOurAudioSource = NULL;
 }
 
  if( fOurOnvifSource )
 {
 	delete (OnvifOpenFileSource *)fOurOnvifSource;
 	fOurOnvifSource = NULL;
 }


}

#include <stdio.h>
#include <stdlib.h>
FILE *pFile = NULL;
void OpenFileHdl(void)
{
	if( pFile == NULL )
	{
		pFile = fopen("test.264", "rb");
		if( pFile == NULL )
		{
			fprintf(stderr,"h264 open file fail!!\n");
		}
	}
}

void CloseFileHdl(void)
{
	if( pFile != NULL )
	{
		fclose(pFile);
		pFile = NULL;
	}
}


char FrameBuff[1024*1024];

int NAL_Search(int offsetNow)
{
	unsigned long testflg = 0;
	int IsFail = 0;

	for(;;)
	{
		fseek(pFile, offsetNow, SEEK_SET);
		if( fread(&testflg, sizeof(testflg), 1, pFile) <=  0 )
		{
			IsFail = -1;
			break;
		}

		//printf("testflg=0x%x \n",(int)testflg );

		if( testflg == 0x01000000 )
		{
			break;
		}


		offsetNow++;

	}
	if( IsFail != 0 )
		return IsFail;

	return offsetNow;

}

void *GetFileFrame(int *pSize,int IsReset)
{
	static int offset = 0;
	int offset1 = 0;
	int offset2 = 0;
	int framesize = 0;

	*pSize = 0;

	if( pFile == NULL )
		return NULL;

	if( IsReset == 1 )
	{
		offset = 0;
		fseek(pFile, 0, SEEK_SET);
	}

	if( pFile )
	{
		fseek(pFile, offset, SEEK_SET);

		offset1 = NAL_Search(offset);
		offset2 = NAL_Search(offset1+4);


		framesize = offset2 - offset1;

		/*reset position*/
		fseek(pFile, offset1, SEEK_SET);
		fread(FrameBuff, framesize, 1, pFile);

		offset = offset2;

		*pSize = framesize;
	}

	return FrameBuff;
}



////////// OpenFileSource implementation //////////

OpenFileSource
::OpenFileSource(UsageEnvironment& env, APPROInput& input)
  : FramedSource(env),
    fInput(input) {
}

OpenFileSource::~OpenFileSource() {
	CloseFileHdl();
}

void OpenFileSource::doGetNextFrame() {
	incomingDataHandler(this);
}

void OpenFileSource
::incomingDataHandler(OpenFileSource* source) {
  source->incomingDataHandler1();
}

void OpenFileSource::incomingDataHandler1() {
	int ret;

	if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

	ret = readFromFile();
	if (ret < 0) {
		handleClosure(this);
		fprintf(stderr,"In Grab Image, the source stops being readable!!!!\n");
	}
	else if (ret == 0)
	{

		if( uSecsToDelay >= uSecsToDelayMax )
		{
			uSecsToDelay = uSecsToDelayMax;
		}else{
			uSecsToDelay *= 2;
		}
		nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
			      (TaskFunc*)incomingDataHandler, this);
	}
	else {

		nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)afterGetting, this);
	}
}

/*static int av_field[8][7] = {
	{AV_OP_LOCK_MP4_VOL,
	AV_OP_UNLOCK_MP4_VOL,
	AV_OP_LOCK_MP4,
	AV_OP_LOCK_MP4_IFRAME,
	AV_OP_UNLOCK_MP4,
	AV_OP_GET_MPEG4_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_SERIAL},

	{AV_OP_LOCK_MP4_CIF_VOL,
	AV_OP_UNLOCK_MP4_CIF_VOL,
	AV_OP_LOCK_MP4_CIF,
	AV_OP_LOCK_MP4_CIF_IFRAME,
	AV_OP_UNLOCK_MP4_CIF,
	AV_OP_GET_MPEG4_CIF_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_CIF_SERIAL},

	{AV_OP_LOCK_MP4_VOL,
	AV_OP_UNLOCK_MP4_VOL,
	AV_OP_LOCK_MP4,
	AV_OP_LOCK_MP4_IFRAME,
	AV_OP_UNLOCK_MP4,
	AV_OP_GET_MPEG4_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_SERIAL},

	{AV_OP_LOCK_MP4_VOL,
	AV_OP_UNLOCK_MP4_VOL,
	AV_OP_LOCK_MP4,
	AV_OP_LOCK_MP4_IFRAME,
	AV_OP_UNLOCK_MP4,
	AV_OP_GET_MPEG4_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_SERIAL},

	{AV_OP_LOCK_MP4_VOL,
	AV_OP_UNLOCK_MP4_VOL,
	AV_OP_LOCK_MP4,
	AV_OP_LOCK_MP4_IFRAME,
	AV_OP_UNLOCK_MP4,
	AV_OP_GET_MPEG4_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_SERIAL},

	{AV_OP_LOCK_MP4_VOL,
	AV_OP_UNLOCK_MP4_VOL,
	AV_OP_LOCK_MP4,
	AV_OP_LOCK_MP4_IFRAME,
	AV_OP_UNLOCK_MP4,
	AV_OP_GET_MPEG4_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_SERIAL},

	{AV_OP_LOCK_MP4_VOL,
	AV_OP_UNLOCK_MP4_VOL,
	AV_OP_LOCK_MP4,
	AV_OP_LOCK_MP4_IFRAME,
	AV_OP_UNLOCK_MP4,
	AV_OP_GET_MPEG4_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_SERIAL},

	{AV_OP_LOCK_MP4_CIF_VOL,
	AV_OP_UNLOCK_MP4_CIF_VOL,
	AV_OP_LOCK_MP4_CIF,
	AV_OP_LOCK_MP4_CIF_IFRAME,
	AV_OP_UNLOCK_MP4_CIF,
	AV_OP_GET_MPEG4_CIF_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_CIF_SERIAL},
};
#define AV_LOCK_MP4_VOL	0
#define AV_UNLOCK_MP4_VOL 1
#define AV_LOCK_MP4		2
#define AV_LOCK_MP4_IFRAME	3
#define AV_UNLOCK_MP4	4
#define AV_GET_MPEG4_SERIAL 5
#define AV_WAIT_NEW_MPEG4_SERIAL 6
*/
static int av_field[5][7] = {
	{AV_OP_LOCK_MP4_VOL,
	AV_OP_UNLOCK_MP4_VOL,
	AV_OP_LOCK_MP4,
	AV_OP_LOCK_MP4_IFRAME,
	AV_OP_UNLOCK_MP4,
	AV_OP_GET_MPEG4_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_SERIAL},

	{AV_OP_LOCK_MP4_CIF_VOL,
	AV_OP_UNLOCK_MP4_CIF_VOL,
	AV_OP_LOCK_MP4_CIF,
	AV_OP_LOCK_MP4_CIF_IFRAME,
	AV_OP_UNLOCK_MP4_CIF,
	AV_OP_GET_MPEG4_CIF_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_CIF_SERIAL},

	{AV_OP_LOCK_MP4_VOL,
	AV_OP_UNLOCK_MP4_VOL,
	AV_OP_LOCK_MP4,
	AV_OP_LOCK_MP4_IFRAME,
	AV_OP_UNLOCK_MP4,
	AV_OP_GET_MPEG4_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_SERIAL},

	{AV_OP_LOCK_H264_VOL,
	AV_OP_UNLOCK_H264_VOL,
	AV_OP_LOCK_H264,
	AV_OP_LOCK_H264_IFRAME,
	AV_OP_UNLOCK_H264,
	AV_OP_GET_H264_SERIAL,
	AV_OP_WAIT_NEW_H264_SERIAL},

	{AV_OP_LOCK_MP4_CIF_VOL,
	AV_OP_UNLOCK_MP4_CIF_VOL,
	AV_OP_LOCK_MP4_CIF,
	AV_OP_LOCK_MP4_CIF_IFRAME,
	AV_OP_UNLOCK_MP4_CIF,
	AV_OP_GET_MPEG4_CIF_SERIAL,
	AV_OP_WAIT_NEW_MPEG4_CIF_SERIAL},
};
#define AV_LOCK_MP4_VOL	0
#define AV_UNLOCK_MP4_VOL 1
#define AV_LOCK_MP4		2
#define AV_LOCK_MP4_IFRAME	3
#define AV_UNLOCK_MP4	4
#define AV_GET_MPEG4_SERIAL 5
#define AV_WAIT_NEW_MPEG4_SERIAL 6
////////// VideoOpenFileSource implementation //////////

VideoOpenFileSource
::VideoOpenFileSource(UsageEnvironment& env, APPROInput& input)
  : OpenFileSource(env, input), SerialBook(0), SerialLock(0), StreamFlag(STREAM_GET_VOL),IsStart(1),nal_state(0),Checkduration(0) {

 uSecsToDelay = 1000; //5000;
 uSecsToDelayMax = 1666;//33000/4;
 srcType = 0;


}

VideoOpenFileSource::~VideoOpenFileSource() {

  fInput.fOurVideoSource = NULL;

  if ((fInput.videoType <= VIDEO_TYPE_H264_CIF) && SerialLock > 0)
  {
    GetAVData(av_field[fInput.videoType][AV_UNLOCK_MP4], SerialLock, NULL);
  }
  SerialLock = 0;
}

int VideoOpenFileSource::readFromFile264()
{
#if 1
	void *pframe = NULL;
	int framesize = 0;
	int ret = 0;
	int offset = 0;
	OpenFileHdl();

	if (StreamFlag & STREAM_GET_VOL)
	{
		pframe = GetFileFrame(&framesize,1);
		memcpy(fTo+offset, pframe, framesize);
		offset += framesize;

		StreamFlag &= ~(STREAM_GET_VOL|STREAM_NEW_GOP);
		StreamFlag |= STREAM_NEW_GOP;

	}
	else if (StreamFlag & STREAM_NEW_GOP) {
		pframe = GetFileFrame(&framesize,0);
		memcpy(fTo+offset, pframe, framesize);
		offset += framesize;
		pframe = GetFileFrame(&framesize,0);
		memcpy(fTo+offset, pframe, framesize);
		offset += framesize;
		pframe = GetFileFrame(&framesize,0);
		memcpy(fTo+offset, pframe, framesize);
		offset += framesize;
		pframe = GetFileFrame(&framesize,0);
		memcpy(fTo+offset, pframe, framesize);
		offset += framesize;
		pframe = GetFileFrame(&framesize,0);
		memcpy(fTo+offset, pframe, framesize);
		offset += framesize;
		pframe = GetFileFrame(&framesize,0);
		memcpy(fTo+offset, pframe, framesize);
		offset += framesize;
	}

	if (1) {
		fFrameSize = offset;
		if (fFrameSize > fMaxSize) {
			printf("Frame Truncated\n");
			printf("fFrameSize = %d\n",fFrameSize);
			printf("fMaxSize = %d\n",fMaxSize);
			fNumTruncatedBytes = fFrameSize - fMaxSize;
			fFrameSize = fMaxSize;
		}
		else {
			fNumTruncatedBytes = 0;
		}
		//memcpy(fTo+offset, av_data.ptr, fFrameSize-offset);

		// Note the timestamp and size:
		gettimeofday(&fPresentationTime, NULL);

		return 1;
	}
	else if (ret == RET_NO_VALID_DATA) {
		return 0;
	}
	else {
		StreamFlag |= STREAM_NEW_GOP;
		return 0;
	}
#else
	return 0;
#endif

}

static unsigned char mpeg4_header[] = {0x00, 0x00, 0x01, 0xb0, 0x01, 0x00, 0x00, 0x01, 0xb5, 0x09};

void printheader(char *pData, int size)
{
	int cnt = 0;

	fprintf(stderr,"printheader = %d\n",size);
	for(cnt = 0 ;cnt < size; cnt++ )
	{

		fprintf(stderr," 0x%X ",*pData++);
		if( cnt!=0 && cnt%4 == 0 )
		fprintf(stderr,"\n");
	}
}

#define AV_LOCK_MP4_VOL	0
#define AV_UNLOCK_MP4_VOL 1
#define AV_LOCK_MP4		2
#define AV_LOCK_MP4_IFRAME	3
#define AV_UNLOCK_MP4	4
#define AV_GET_MPEG4_SERIAL 5
#define AV_WAIT_NEW_MPEG4_SERIAL 6
char *sps = NULL;
char *pps = NULL;
int WaitVideoStart( int vType , int sleep_unit=300)
{
	AV_DATA av_data;
	int cnt = 0;
	int serialnum = -1;
	int *cmd = av_field[vType];

	while(1)
	{
		GetAVData(cmd[AV_GET_MPEG4_SERIAL], -1, &av_data);

		if (av_data.flags != AV_FLAGS_MP4_I_FRAME)
		{
			usleep(3);
		}
		else
		{
			serialnum = av_data.serial ;
			break;
		}
		cnt++;
		if (cnt > 1000)
			break;
	}

	return serialnum;
}
static int get_pps_pos(unsigned char *src, unsigned int size)
{
	int offset = 0;
	while(1) {
		if((src[offset] == 0x00) && (src[offset + 1] == 0x00) && (src[offset + 2] == 0x00)
			&& (src[offset + 3] == 0x01) && ((src[offset + 4] & 0xF) == 8)) {
			//printf("get_pps_pos %d\n",offset+4);
			break;
		}
		offset++;
		if((offset > size) || ((offset + 1) > size) || ((offset + 2) > size) ||
			((offset + 3) > size) || ((offset + 4) > size)) {
			offset = 0;
			break;
		}
	}

	return offset + 4;
}
int nsps = 0;
static int get_sps_pos(unsigned char *src, unsigned int size)
{

	int i = 0, sps =0, sps_len=0;
	for (i=0; i<size; i++) {
		if (src[i]==0 && src[i+1]==0 && src[i+2]==0 && src[i+3]==1 && (src[i+4]&0x1f)==7)
			sps = i+4;
		if (sps) {
			nsps = sps;
			if (src[i]==0 && src[i+1]==0 && src[i+2]==0 && src[i+3]==1 && (src[i+4]&0x1f)==8)
			{
				sps_len = i-sps;
				break;
			}
		}
	}
	return sps_len;
}
static int get_idr_pos(unsigned char *src, unsigned int size)
{
	int offset = 0;
	while(1)
	{
		if ((src[offset] == 0x00) && (src[offset + 1] == 0x00) && (src[offset + 2] == 0x00)
			&& (src[offset + 3] == 0x01) && ((src[offset + 4] & 0xF) == 5)) {
			//printf("get_idr_pos %d\n",offset+4);
			break;
		}
		offset++;
		if ((offset > size) || ((offset + 1) > size) || ((offset + 2) > size) ||
			((offset + 3) > size) || ((offset + 4) > size)) {
			offset = 0;
			break;
		}
	}

	return offset + 4;
}

static int pps_pos = 0;
static int idr_pos = 0;

static void update_h264_info(unsigned char *src, unsigned int size)
{
	int sps_len = 0;
	pps_pos = get_pps_pos(src, size);
	idr_pos = get_idr_pos(src, size);
	sps_len = get_sps_pos(src, size);
	sps = base64Encode((const char *)(src + nsps), sps_len);
	pps = base64Encode((const char *)(src + pps_pos), 4);
	fprintf(stderr,"update_h264_info:nsps:%d, sps_len:%d, pps_pos:%d\n",nsps, sps_len, pps_pos );
}

int GetVideoSerial(int vType)
{
	AV_DATA av_data;
	int *cmd = av_field[vType];

	GetAVData(cmd[AV_GET_MPEG4_SERIAL], -1, &av_data);

	return av_data.serial;
}
extern int agent_qt;

#ifdef STREAM_DETAIL_OUTPUT
#define	RUN_FRAME_NUM	(1000)
void PrintStreamDetails(void)
{
	static int strmfrmCnt = 0;
	static int strmfrmSkip = 0;
	static struct timeval startTime, endTime, passTime;
	double calcTime;

	if (strmfrmSkip < 150)
	{
		strmfrmSkip++;
	}
	else
	{
		if (strmfrmCnt == 0)
			gettimeofday(&startTime, NULL);
		if (strmfrmCnt == RUN_FRAME_NUM)
		{
			gettimeofday(&endTime, NULL);
			printf("\n==================== STREAMING DETAILS ====================\n");
			printf("Start Time : %ldsec %06ldusec\n", (long)startTime.tv_sec, (long)startTime.tv_usec);
			printf("End Time   : %ldsec %06ldusec\n", (long)endTime.tv_sec, (long)endTime.tv_usec);
			timersub(&endTime, &startTime, &passTime);
			calcTime = (double)passTime.tv_sec*1000.0 + (double)passTime.tv_usec/1000.0;
			printf("Total Time to stream %d frames: %.3f msec\n", RUN_FRAME_NUM , calcTime);
			printf("Time per frame: %3.4f msec\n", calcTime/RUN_FRAME_NUM);
			printf("Streaming Performance in FPS: %3.4f\n", RUN_FRAME_NUM/(calcTime/1000));
			printf("===========================================================\n");
			strmfrmCnt 	= 0;
			strmfrmSkip = 0;
		}
		else
		{
			strmfrmCnt++;
		}
	}
}
#endif

int VideoOpenFileSource::readFromFile()
{
	AV_DATA av_data;
	int ret;
	int opt = video_priority << 5;
	//int iSerial; // 20120417
	//
	//setsockopt(rtsp_srv_socket, SOL_IP, IP_TOS, &opt, sizeof(opt));
//	setsockopt(rtsp_srv_socket, SOL_SOCKET, SO_PRIORITY, &video_priority, sizeof(video_priority));

	if (fInput.videoType == VIDEO_TYPE_MJPEG)
	{
		if (GetAVData(AV_OP_GET_MJPEG_SERIAL, -1, &av_data) != RET_SUCCESS)
        {
			fprintf(stderr,"Get MJ Serial Fail\n");
			//usleep(33333);
			//usleep(3333);
			return 0;
		}

		if (av_data.serial == SerialLock)
        {
			//usleep(30000);
			//usleep(16666);
			//usleep(3333);
			//if (GetAVData(AV_OP_GET_MJPEG_SERIAL, -1, &av_data) != RET_SUCCESS)
			//fprintf(stderr,"Get MJ av_data.serial == SerialLockl\n");
			return 0;
		}

		if (GetAVData(AV_OP_LOCK_MJPEG, av_data.serial, &av_data) != RET_SUCCESS) // 20120417
		{
			//if (SerialBook == 0)
			//	SerialBook = av_data.serial;
			//usleep(3333);
			return 0;
		}

		SerialLock = av_data.serial;
		fFrameSize = av_data.size;
		if (fFrameSize > fMaxSize)
        {
			printf("Frame Truncated\n");
			fNumTruncatedBytes = fFrameSize - fMaxSize;
			fFrameSize = fMaxSize;
		}
		else
			fNumTruncatedBytes = 0;

//		fNumTruncatedBytes = 0;
		memcpy(fTo, av_data.ptr, fFrameSize);
//		printf("FB:%x, %x, ", av_data.ptr[0], av_data.ptr[1]);
//		printf("FB:%x, %x\n", av_data.ptr[fFrameSize-2], av_data.ptr[fFrameSize-1]);

		//mcpy(fTo, av_data.ptr, fFrameSize);

#if TIME_GET_WAY
		gettimeofday(&fPresentationTime, NULL);
#else
		fPresentationTime.tv_sec = av_data.timestamp/1000;
		fPresentationTime.tv_usec = (av_data.timestamp%1000)*1000;
#endif
		//usleep(1000); //modify by kenny
		GetAVData(AV_OP_UNLOCK_MJPEG, SerialLock, &av_data);
		return 1;
	}
	else if (fInput.videoType == VIDEO_TYPE_MPEG4 ||
			 fInput.videoType == VIDEO_TYPE_MPEG4_CIF)
#if 1
	{
		int offset = 0;
		int *cmd = av_field[fInput.videoType];
#if 1   // add by kenny chiu 20121002
		if (reset_flag)
        {
			if (is_ssm)
				reset_flag = 0;
            else
            {
				fprintf(stderr,"Video reset\n");
				//if(SerialBook == 0)
					StreamFlag = STREAM_GET_VOL;
				//else
				//	StreamFlag &= ~(STREAM_GET_VOL|STREAM_NEW_GOP);
				//usleep(33333);
				reset_flag = 0;
			}
		}
#endif
		if (StreamFlag & STREAM_GET_VOL)
		{
			AV_DATA vol_data;
			memcpy(fTo, mpeg4_header, sizeof(mpeg4_header));
			offset = sizeof(mpeg4_header);
			if (GetAVData(cmd[AV_LOCK_MP4_VOL], -1, &vol_data) != RET_SUCCESS)
			{
				printf("Error on Get Vol data\n");
				return -1;
			}

			memcpy(fTo+offset, vol_data.ptr, vol_data.size);
			offset += vol_data.size;
		
			GetAVData(cmd[AV_UNLOCK_MP4_VOL], -1, &vol_data);
#if 0			
			int i;
			printf("Stream %d VOL size = %d\n",fInput.videoType, vol_data.size);
			printf("VOL data = \n");
			for (i=0;i<vol_data.size;i++)
			{
			    printf("0x%02X ", vol_data.ptr[i]);
			    if ((++i % 32) == 0) printf("\n");
			}
			printf("\n");
#endif			

			if (vol_data.size != 18)
			{
				printf("Stream %d is not avaliable~~~~~~~~ size = %d\n",fInput.videoType, vol_data.size);
				//sigterm(0);
			}

			WaitVideoStart( fInput.videoType ,300);

			ret = GetAVData(cmd[AV_LOCK_MP4_IFRAME], -1, &av_data);
			SerialBook = av_data.serial;
			StreamFlag &= ~(STREAM_GET_VOL|STREAM_NEW_GOP);
		}
		else if (StreamFlag & STREAM_NEW_GOP)
		{
			WaitVideoStart(fInput.videoType ,5000);

			ret = GetAVData(cmd[AV_LOCK_MP4_IFRAME], -1, &av_data);
			SerialBook = av_data.serial;
			StreamFlag &= ~STREAM_NEW_GOP;
		}
		else
		{
			ret = GetAVData(cmd[AV_LOCK_MP4], SerialBook, &av_data);
		}

		if (ret == RET_SUCCESS)
		{
			static int IscheckKey = 1;
			if (av_data.flags == AV_FLAGS_MP4_I_FRAME && IscheckKey == 1)
			{
				int serial_now;
				serial_now = GetVideoSerial(fInput.videoType);
				IscheckKey = 0;
				//printf("serial_now = %d SerialBook = %d \n",serial_now,SerialBook);
				if ((serial_now - SerialBook) > 30)
				{
					GetAVData(cmd[AV_UNLOCK_MP4], SerialBook, &av_data);
					StreamFlag |= STREAM_NEW_GOP;
					return 0;
				}
			}
			else
			{
				IscheckKey = 1;
			}
		}

		if (ret == RET_SUCCESS)
		{
			fFrameSize = av_data.size + offset;
			if (fFrameSize > fMaxSize)
			{
				printf("Frame Truncated\n");
				fNumTruncatedBytes = fFrameSize - fMaxSize;
				fFrameSize = fMaxSize;
			}
			else
			{
				fNumTruncatedBytes = 0;
			}
			memcpy(fTo+offset, av_data.ptr, fFrameSize-offset);

#ifdef STREAM_DETAIL_OUTPUT
			PrintStreamDetails();
#endif
			if (SerialLock > 0)
			{
				GetAVData(cmd[AV_UNLOCK_MP4], SerialLock, &av_data);
			}
			SerialLock = SerialBook;

			// Note the timestamp and size:
#if TIME_GET_WAY
			gettimeofday(&fPresentationTime, NULL);
#else
			fPresentationTime.tv_sec = av_data.timestamp/1000;
			fPresentationTime.tv_usec = (av_data.timestamp%1000)*1000;
#endif
			if (IsStart)
			{
				AV_DATA av_data_media;
				IsStart = 0;

				//GetAVData(AV_OP_GET_MEDIA_INFO, -1, &av_data_media );
				if(0)// av_data_media.flags == 1 )
				{
					Checkduration = 16666;
				}
				else{
					Checkduration = 33333;
				}
			}
			fDurationInMicroseconds = Checkduration;
			SerialBook++;
			return 1;
		}
		else if (ret == RET_NO_VALID_DATA) {
			return 0;
		}
		else {
			StreamFlag |= STREAM_NEW_GOP;
			return 0;
		}
		
	}
#else
	{
		int offset = 0;
		int *cmd = av_field[fInput.videoType];
#if 1
		if (reset_flag)
        {
			if (is_ssm)
				reset_flag = 0;
            else
            {
				fprintf(stderr,"Video reset\n");
				//if(SerialBook == 0)
					StreamFlag = STREAM_GET_VOL;
				//else
				//	StreamFlag &= ~(STREAM_GET_VOL|STREAM_NEW_GOP);
				//usleep(33333);
				reset_flag = 0;
			}
		}
#endif

		if (StreamFlag & STREAM_GET_VOL)
        {
			//fprintf(stderr,"STREAM_GET_VOL\n");
			AV_DATA vol_data;
			//memcpy(fTo, mpeg4_header, sizeof(mpeg4_header));
			mcpy(fTo, mpeg4_header, sizeof(mpeg4_header));
			offset = sizeof(mpeg4_header);
			if (GetAVData(cmd[AV_LOCK_MP4_VOL], -1, &vol_data) != RET_SUCCESS)
            {
				printf("Error on Get Vol data\n");
				return -1;
			}
			//memcpy(fTo+offset, vol_data.ptr, vol_data.size);
			memcpy(fTo+offset, vol_data.ptr, vol_data.size);
			offset += vol_data.size;

			GetAVData(cmd[AV_UNLOCK_MP4_VOL], -1, &vol_data);
        	WaitVideoStart( fInput.videoType );
			ret = GetAVData(cmd[AV_LOCK_MP4_IFRAME], -1, &av_data);
			SerialBook = av_data.serial;
			StreamFlag &= ~(STREAM_GET_VOL|STREAM_NEW_GOP);
		}
		else if (StreamFlag & STREAM_NEW_GOP)
        {
			//fprintf(stderr,"STREAM_NEW_GOP\n");
			ret = GetAVData(cmd[AV_LOCK_MP4_IFRAME], -1, &av_data );
			SerialBook = av_data.serial;
			StreamFlag &= ~STREAM_NEW_GOP;
		}
		else
        {
			//fprintf(stderr,"STREAM_NORMAL\n");
			ret = GetAVData(cmd[AV_LOCK_MP4], SerialBook, &av_data);
#if 0
			if (reset_flag)
            {
				if (av_data.frameType == AV_FRAME_TYPE_I_FRAME)
					reset_flag = 0;
				else
					ret = RET_NO_VALID_DATA;
			}
#endif
		}

		if (ret == RET_SUCCESS)
        {
			fFrameSize = av_data.size + offset;
			if (fFrameSize > fMaxSize)
            {
				printf("Frame Truncated\n");
				fNumTruncatedBytes = fFrameSize - fMaxSize;
				fFrameSize = fMaxSize;
			}
			else
				fNumTruncatedBytes = 0;

			//memcpy(fTo+offset, av_data.ptr, fFrameSize-offset);
			memcpy(fTo+offset, av_data.ptr, fFrameSize-offset);

			//if (SerialLock > 0) {
            GetAVData(cmd[AV_UNLOCK_MP4], SerialLock, &av_data);
			//}
			//SerialLock = SerialBook;

			SerialLock = av_data.serial;
            SerialBook = av_data.serial+1;
			// Note the timestamp and size:
			gettimeofday(&fPresentationTime, NULL);
			//SerialBook++;

			//fprintf(stderr, "VideoOpenFileSource !!!!!!!!\n");
			return 1;
		}
		else if (ret == RET_NO_VALID_DATA)
        {
			//fprintf(stderr,"RET_NO_VALID_DATA\n");
			//usleep(1000);
			return 0;
		}
		else
        {
			fprintf(stderr,"Get video error\n");
			//StreamFlag = STREAM_GET_VOL;
			//usleep(1000);
			//fprintf(stderr,"!!!!!!!!!!!!!!!!!!\n");
			GetAVData(AV_OP_GET_MPEG4_SERIAL, -1, &av_data );
            SerialBook = av_data.serial;
			//StreamFlag |= STREAM_NEW_GOP;
			return 0;
		}
	}
#endif
 	else if (fInput.videoType == VIDEO_TYPE_H264 || fInput.videoType == VIDEO_TYPE_H264_VGA || fInput.videoType == VIDEO_TYPE_H264_CIF)
#if 1
 	{
#if 1
		int offset = 0;
		int start = 0;
		int *cmd = av_field[fInput.videoType];

/*		if (reset_flag)
		{	
			fprintf(stderr,"Video reset\n");
			StreamFlag = STREAM_GET_VOL;
			reset_flag = 0;
		}*/

		if (StreamFlag & STREAM_GET_VOL)
		{
			AV_DATA vol_data;
			if (GetAVData(cmd[AV_LOCK_MP4_VOL], -1, &vol_data) != RET_SUCCESS)
			{
				printf("Error on Get Vol data\n");
				return -1;
			}
			if ((vol_data.size == 18) || (vol_data.size >= 256))
			{
				GetAVData(cmd[AV_UNLOCK_MP4_VOL], -1, &vol_data);
				printf("Stream %d is not avaliable~~~~~~~~\n",fInput.videoType);
//				sigterm(0);
			}
			if(0)
			{
				memcpy(fTo+offset, vol_data.ptr, vol_data.size);
				offset += vol_data.size;
				fFrameSize = 0;
				nal_state = 5;
			}
			else
			{
				unsigned int i, sps=0, sps_len=0, pps=0;

				char *ptr =(char *)vol_data.ptr;
				if (nal_state == 0)
				{
					for (i=0; i<vol_data.size; i++)
					{
						if (ptr[i]==0 && ptr[i+1]==0 && ptr[i+2]==0 && ptr[i+3]==1 && (ptr[i+4]&0x1f)==7)
						{
							sps = i + 4;
						}
						if (sps)
						{
							if (ptr[i]==0 && ptr[i+1]==0 && ptr[i+2]==0 && ptr[i+3]==1 && (ptr[i+4]&0x1f)==8)
							{
								sps_len = i - sps;
								break;
							}
						}
						//sps_len = vol_data.size;
					}

					if (sps_len)
					{
						fFrameSize = sps_len;
						if (fFrameSize > fMaxSize)
						{
							printf("Frame Truncated\n");
							fNumTruncatedBytes = fFrameSize - fMaxSize;
							fFrameSize = fMaxSize;
						}
						else
						{
							fNumTruncatedBytes = 0;
						}
						memcpy(fTo, ptr+sps, fFrameSize);
					}
					else
					{
						printf("SPS not found\n");
						return -1;
					}
				}
				else if (nal_state == 1)
				{
					for (i=0; i<vol_data.size; i++)
					{
						if (ptr[i]==0 && ptr[i+1]==0 && ptr[i+2]==0 && ptr[i+3]==1 && (ptr[i+4]&0x1f)==8)
						{
							pps = i+4;
							break;
						}
					}
					if (pps)
					{
						//fFrameSize = 4;
						fFrameSize = vol_data.size - pps;
						fFrameSize -= 1;
						if (fFrameSize > fMaxSize)
						{
							printf("Frame Truncated\n");
							fNumTruncatedBytes = fFrameSize - fMaxSize;
							fFrameSize = fMaxSize;
						}
						else
						{
							fNumTruncatedBytes = 0;
						}
						memcpy(fTo, ptr+pps, fFrameSize);
					}
				}
			}
			GetAVData(cmd[AV_UNLOCK_MP4_VOL], -1, &vol_data);

			if ((vol_data.size == 18) || (vol_data.size >= 256))
			{
				printf("Stream %d is not avaliable~~~~~~~~\n",fInput.videoType);
//				sigterm(0);
			}

			if (nal_state < 2)
			{
#if TIME_GET_WAY
				gettimeofday(&fPresentationTime, NULL);
#else
				int cnt = 0;
				while(1)
				{
					GetAVData(cmd[AV_GET_MPEG4_SERIAL], -1, &av_data);
					if (av_data.flags != AV_FLAGS_MP4_I_FRAME)
					{
						usleep(3);
						cnt++;
						if (cnt > 1000)
						{
							gettimeofday(&fPresentationTime, NULL);
							nal_state++;
							return 1;
						}
					}
					else
						break;
				}
				fPresentationTime.tv_sec = av_data.timestamp/1000;
				fPresentationTime.tv_usec = (av_data.timestamp%1000)*1000;
#endif
				nal_state++;
				return 1;
			}


			WaitVideoStart(fInput.videoType, 300);

			ret = GetAVData(cmd[AV_LOCK_MP4_IFRAME], -1, &av_data);
			SerialBook = av_data.serial;
			StreamFlag &= ~(STREAM_GET_VOL|STREAM_NEW_GOP);

		}
		else if (StreamFlag & STREAM_NEW_GOP)
		{
			WaitVideoStart(fInput.videoType, 5000);

			ret = GetAVData(cmd[AV_LOCK_MP4_IFRAME], -1, &av_data);
			SerialBook = av_data.serial;
			StreamFlag &= ~STREAM_NEW_GOP;
		}
		else
		{
			ret = GetAVData(cmd[AV_LOCK_MP4], SerialBook, &av_data);
		}

 		if (ret == RET_SUCCESS)
 		{
 			static int IscheckKey = 1;
 			if (av_data.flags == AV_FLAGS_MP4_I_FRAME && IscheckKey == 1)
 			{
 				int serial_now;
 				serial_now = GetVideoSerial(fInput.videoType);
 				IscheckKey = 0;
 				//printf("serial_now = %d SerialBook = %d \n",serial_now,SerialBook);
 				if ((serial_now - SerialBook) > 30)
 				{
 					GetAVData(cmd[AV_UNLOCK_MP4], SerialBook, &av_data);
 					StreamFlag |= STREAM_NEW_GOP;
 					return 0;
 				}
 			}
			else
			{
 				IscheckKey = 1;
 			}
 		}

 		start = 4;
 		if ((ret == RET_SUCCESS)/*&&(av_data.flags == AV_FLAGS_MP4_I_FRAME)*/)
 		{
 			unsigned int i;
			char *ptr = (char *)av_data.ptr;

			for (i=0; i<av_data.size; i++)
			{
				if (ptr[i]==0 && ptr[i+1]==0 && ptr[i+2]==0 && ptr[i+3]==1 && (ptr[i+4]&0x1f)<=5)
				{
					start = i+4;
					break;
				}
			}
			//printheader((char *)av_data.ptr, 300);
 		}

		if (ret == RET_SUCCESS)
		{
			fFrameSize = av_data.size + offset - start;
			if (fFrameSize > fMaxSize)
			{
				printf("Frame Truncated\n");
				fNumTruncatedBytes = fFrameSize - fMaxSize;
				fFrameSize = fMaxSize;
			}
			else
			{
				fNumTruncatedBytes = 0;
			}
			memcpy(fTo+offset, av_data.ptr+start, fFrameSize);

#ifdef STREAM_DETAIL_OUTPUT
			PrintStreamDetails();
#endif
			if (SerialLock > 0)
			{
				GetAVData(cmd[AV_UNLOCK_MP4], SerialLock, &av_data);
			}
			SerialLock = SerialBook;
			
			// Note the timestamp and size:
#if TIME_GET_WAY
			gettimeofday(&fPresentationTime, NULL);
#else
			fPresentationTime.tv_sec = av_data.timestamp/1000;
			fPresentationTime.tv_usec = (av_data.timestamp%1000)*1000;
#endif
#if 0
			{
				static int iVCount = 0;
				if (iVCount++ < 200)
					fprintf(stderr, "H264 frame time = %d.%06d\n", fPresentationTime.tv_sec, fPresentationTime.tv_usec);
			}
#endif
			if (IsStart)
			{
				AV_DATA av_data_media;
				IsStart = 0;

				//GetAVData(AV_OP_GET_MEDIA_INFO, -1, &av_data_media);  // remarked by kenny chiu 20120828 per frank.lin
				//if (av_data_media.flags == 1)
				//	Checkduration = 16666;
				//else
					Checkduration = 33333;
			}
			fDurationInMicroseconds = Checkduration;

			SerialBook++;
			return 1;
		}
		else if (ret == RET_NO_VALID_DATA)
		{
			;//usleep(Checkduration);
			return 0;
		}
		else
		{
			StreamFlag |= STREAM_NEW_GOP;
			return 0;
		}
#else
		return readFromFile264();
#endif

 	}
#endif 

#if 0
 	else if (0)//((fInput.videoType == VIDEO_TYPE_H264_SVC_30FPS)|| (fInput.videoType == VIDEO_TYPE_H264_SVC_15FPS)
				//|| (fInput.videoType == VIDEO_TYPE_H264_SVC_7FPS) || (fInput.videoType == VIDEO_TYPE_H264_SVC_3FPS))
 	{
#if 1
		int offset = 0;
		int start = 0;
		int *cmd = av_field[fInput.videoType];

		if (StreamFlag & STREAM_GET_VOL) {
			AV_DATA vol_data;

			if(GetAVData(cmd[AV_LOCK_MP4_VOL], -1, &vol_data) != RET_SUCCESS) {
				printf("Error on Get Vol data\n");
				return -1;
			}
			if( (vol_data.size == 18) || (vol_data.size  >= 256)  )
			{
				GetAVData(cmd[AV_UNLOCK_MP4_VOL], -1, &vol_data);
				printf("Stream %d is not avaliable~~~~~~~~\n",fInput.videoType);
//				sigterm(0);
			}
			if(0)
			{
				memcpy(fTo+offset, vol_data.ptr, vol_data.size);
				offset += vol_data.size;
				fFrameSize = 0;
				nal_state = 5;
			}else{
				unsigned int i, sps=0, sps_len=0,pps=0;
				char *ptr =(char *)vol_data.ptr;
				if( nal_state == 0 )
				{

					for (i=0; i<vol_data.size; i++) {
						if (ptr[i]==0 && ptr[i+1]==0 && ptr[i+2]==0 && ptr[i+3]==1 && (ptr[i+4]&0x1f)==7)
							sps = i+4;
						if (sps) {
							if (ptr[i]==0 && ptr[i+1]==0 && ptr[i+2]==0 && ptr[i+3]==1 && (ptr[i+4]&0x1f)==8)
							{
								sps_len = i-sps;
								break;
							}
						}
					}
					if (sps_len) {

						fFrameSize = sps_len;
						if (fFrameSize > fMaxSize) {
							printf("Frame Truncated\n");
							fNumTruncatedBytes = fFrameSize - fMaxSize;
							fFrameSize = fMaxSize;
						}
						else {
							fNumTruncatedBytes = 0;
						}
						memcpy(fTo, ptr+sps, fFrameSize);
					}
					else {
						printf("SPS not found\n");
						return -1;
					}

				}else if(nal_state == 1 ){
					for (i=0; i<vol_data.size; i++) {
						if (ptr[i]==0 && ptr[i+1]==0 && ptr[i+2]==0 && ptr[i+3]==1 && (ptr[i+4]&0x1f)==8)
						{
							pps = i+4;
							break;
						}
					}
					if (pps) {

						fFrameSize = 4;
						if (fFrameSize > fMaxSize) {
							printf("Frame Truncated\n");
							fNumTruncatedBytes = fFrameSize - fMaxSize;
							fFrameSize = fMaxSize;
						}
						else {
							fNumTruncatedBytes = 0;
						}
						memcpy(fTo, ptr+pps, fFrameSize);
					}

				}
			}
			GetAVData(cmd[AV_UNLOCK_MP4_VOL], -1, &vol_data);

			if( (vol_data.size == 18) || (vol_data.size  >= 256)  )
			{
				printf("Stream %d is not avaliable~~~~~~~~\n",fInput.videoType);
//				sigterm(0);
			}


			if( nal_state < 2 )
			{
				#if TIME_GET_WAY
				gettimeofday(&fPresentationTime, NULL);
				#else
				fPresentationTime.tv_sec = av_data.timestamp/1000;
				fPresentationTime.tv_usec = (av_data.timestamp%1000)*1000;
				#endif
				nal_state++;
				return 1;
			}

			WaitVideoStart( fInput.videoType,300 );

			ret = GetAVData(cmd[AV_LOCK_MP4_IFRAME], -1, &av_data);
			SerialBook = av_data.serial;
			StreamFlag &= ~(STREAM_GET_VOL|STREAM_NEW_GOP);

		}
		else if (StreamFlag & STREAM_NEW_GOP) {

			WaitVideoStart( fInput.videoType,5000);

			ret = GetAVData(cmd[AV_LOCK_MP4_IFRAME], -1, &av_data );
			SerialBook = av_data.serial;
			StreamFlag &= ~STREAM_NEW_GOP;
		}
		else {
			ret = GetAVData(cmd[AV_LOCK_MP4], SerialBook, &av_data );
		}

 		if (ret == RET_SUCCESS)
 		{
 			static int IscheckKey = 1;
 			if( av_data.flags == AV_FLAGS_MP4_I_FRAME && IscheckKey == 1 )
 			{
 				int serial_now;
 				serial_now = GetVideoSerial(fInput.videoType);
 				IscheckKey = 0;
 				//printf("serial_now = %d SerialBook = %d \n",serial_now,SerialBook);
 				if( (serial_now - SerialBook) > 30 )
 				{
 					GetAVData(cmd[AV_UNLOCK_MP4], SerialBook, &av_data);
 					StreamFlag |= STREAM_NEW_GOP;
 					return 0;
 				}
 			}else{
 				IscheckKey = 1;
 			}
 		}


 		start = 4;
 		if ((ret == RET_SUCCESS)/*&&(av_data.flags == AV_FLAGS_MP4_I_FRAME)*/)
 		{
 			unsigned int i;
			char *ptr = (char *)av_data.ptr;

			for (i=0; i<av_data.size; i++)
			{
				if (ptr[i]==0 && ptr[i+1]==0 && ptr[i+2]==0 && ptr[i+3]==1 && (ptr[i+4]&0x1f)<=5)
				{
					start = i+4;
					break;
				}
			}
			//printheader((char *)av_data.ptr, 300);
 		}
		if (ret == RET_SUCCESS) {
			//printf("SVC DEBUG MSG: %d\n", av_data.temporalId);
			if (0){//((fInput.videoType == VIDEO_TYPE_H264_SVC_30FPS) ||
				//((fInput.videoType == VIDEO_TYPE_H264_SVC_15FPS) && (av_data.temporalId <=2 )) ||
					//((fInput.videoType == VIDEO_TYPE_H264_SVC_7FPS) && (av_data.temporalId <= 1)) ||
						//((fInput.videoType == VIDEO_TYPE_H264_SVC_3FPS) && (av_data.temporalId == 0))) {
				fFrameSize = av_data.size + offset - start;
				if (fFrameSize > fMaxSize) {
					printf("Frame Truncated\n");
					fNumTruncatedBytes = fFrameSize - fMaxSize;
					fFrameSize = fMaxSize;
				}
				else {
					fNumTruncatedBytes = 0;
				}
				memcpy(fTo+offset, av_data.ptr+start, fFrameSize);
			}
			else {
				fFrameSize = 0;
				fNumTruncatedBytes = 0;
			}

#ifdef STREAM_DETAIL_OUTPUT
			PrintStreamDetails();
#endif
			if (SerialLock > 0) {
				GetAVData(cmd[AV_UNLOCK_MP4], SerialLock, &av_data);
			}
			SerialLock = SerialBook;

			// Note the timestamp and size:
#if TIME_GET_WAY
			gettimeofday(&fPresentationTime, NULL);
#else
			fPresentationTime.tv_sec = av_data.timestamp/1000;
			fPresentationTime.tv_usec = (av_data.timestamp%1000)*1000;
#endif
			if( IsStart )
			{
				AV_DATA av_data_media;
				IsStart = 0;

				//GetAVData(AV_OP_GET_MEDIA_INFO, -1, &av_data_media );
				if(0)// av_data_media.flags == 1 )
				{
					Checkduration = 16666;
				}else{
					Checkduration = 33333;
				}

			}
			fDurationInMicroseconds = Checkduration;

			SerialBook++;
			return 1;
		}
		else if (ret == RET_NO_VALID_DATA) {
			return 0;
		}
		else {
			StreamFlag |= STREAM_NEW_GOP;
			return 0;
		}
#else
		return readFromFile264();
#endif

 	}
#endif
	else
	{
		return 0;
	}
}


OnvifOpenFileSource
::OnvifOpenFileSource(UsageEnvironment& env, APPROInput& input)
  : OpenFileSource(env, input)
{
 uSecsToDelay = 5000;
 uSecsToDelayMax = 33000/4;
 srcType = 0;
}

OnvifOpenFileSource::~OnvifOpenFileSource()
{
  fInput.fOurOnvifSource = NULL;
}


int OnvifOpenFileSource::readFromFile()
{
	fFrameSize = sizeof(EventMsg);
	memcpy(fTo, EventMsg, fFrameSize);
	if (fFrameSize > fMaxSize) {
		
		fNumTruncatedBytes = fFrameSize - fMaxSize;
		fFrameSize = fMaxSize;
	}
	else {
		fNumTruncatedBytes = 0;
	}
	
	gettimeofday(&fPresentationTime, NULL);	
	fDurationInMicroseconds = 33333;
	return 1;
}

////////// AudioOpenFileSource implementation //////////

AudioOpenFileSource
::AudioOpenFileSource(UsageEnvironment& env, APPROInput& input)
  : OpenFileSource(env, input), AudioBook(0), AudioLock(0), IsStart(1) {
  uSecsToDelay = 5000;
  uSecsToDelayMax = 125000;
  srcType = 1;
  AudioRemand = 0;
  AudioOrg_s = 0;
  memset(tmp_audio, 0, 256);
}

AudioOpenFileSource::~AudioOpenFileSource() {
  fInput.fOurAudioSource = NULL;
  if (AudioLock > 0) {
    GetAVData(AV_OP_UNLOCK_ULAW, AudioLock, NULL);
    AudioLock = 0;
  }
}

int AudioOpenFileSource::getAudioData()
{
#if 0
	AV_DATA av_data;
	int ret;
	int info_mode = -1;
	int opt = audio_priority << 5;

    if (!audio_enable)
    {
        usleep(1111);//3333
        return 0;
    }

	if (audio_reset_flag > 0)
    {
		if (!is_ssm)
			audio_reset_flag = 0;
		else
        {
			//fprintf(stderr,"reset audio(%d)\n",audio_reset_flag);
			//audio_reset_flag--;
			audio_reset_flag = 0;
			return 0;
		}
	}
    else
    {
		if (is_ssm)
			audio_reset_flag = 1;
	}

	setsockopt(rtsp_srv_socket, SOL_IP, IP_TOS, &opt, sizeof(opt));
	setsockopt(rtsp_srv_socket, SOL_SOCKET, SO_PRIORITY, &audio_priority, sizeof(audio_priority));

	if (AudioBook == 0)
    {
		GetAVData(AV_OP_GET_ULAW_SERIAL, -1, &av_data );
		if (av_data.serial <= AudioLock)
        {
			printf("av_data.serial <= audio_lock!!!\n");
			//sleep(10);
			return 0;
		}
		AudioBook = av_data.serial;
		//fprintf(stderr,"Init Get Audio Data book:%d lock:%d\n",AudioBook,AudioLock);
	}
    else
    {
		//fprintf(stderr,"Init Get Audio Data book!!!!!!!!:%d lock:%d\n",AudioBook,AudioLock);
	}

	if (AudioRemand > 0)
    {  /* AMR Case */
		//if (audioFormat >= 6) {
		if ((audioFormat >= 6) && (fInput.videoType == VIDEO_TYPE_MPEG4_CIF))
        {
			//memcpy(fTo, tmp_audio+(AMR_GET_SIZE[audioFormat-6] - AudioRemand), amr_size[audioFormat-6]);
			mcpy(fTo, tmp_audio+(AMR_GET_SIZE[audioFormat - 6] - AudioRemand) + 1, amr_size[audioFormat - 6]);
			//fprintf(stderr,"1.amr %d %x %x %x\n",amr_size[0] - 1, fTo[0], fTo[1], fTo[2]);
			AudioRemand -= (amr_size[audioFormat - 6] + 1);
			return amr_size[audioFormat - 6];
		}
        else
        {
			if (AudioRemand >= stream_out_audio_size)
            {
				mcpy(fTo, tmp_audio+(AudioOrg_s - AudioRemand), stream_out_audio_size);
				AudioRemand -= stream_out_audio_size;
				return stream_out_audio_size;
			}
            else
            {
				int tmp_size = AudioRemand;
				mcpy(fTo, tmp_audio+(AudioOrg_s - AudioRemand), AudioRemand);
				AudioRemand = 0;
				return tmp_size;
			}
		}
	}

	ret = GetAVData(AV_OP_LOCK_ULAW, AudioBook, &av_data );

	if (ret == RET_SUCCESS)
    {
		int pk_size = 0;
		if (AudioLock > 0)
        {
			GetAVData(AV_OP_UNLOCK_ULAW, AudioLock, NULL);
			AudioLock = 0;
		}

		if (av_data.size > fMaxSize)
			av_data.size = fMaxSize;

		if ((audioFormat >= 6) && (fInput.videoType == VIDEO_TYPE_MPEG4_CIF))
        {
			unsigned char t_buf[1600];
			int raw_size = audio_g711_dec((unsigned short *)t_buf, av_data.ptr, av_data.size);
			int en_size = AUDIO_AMR_Encode((short *)tmp_audio, (short *)t_buf, raw_size*2, &amr_ptr);
			AudioRemand = en_size - (amr_size[audioFormat - 6] + 1);
			pk_size = amr_size[audioFormat-6];
			mcpy(fTo, tmp_audio+1, pk_size);
		}
        else
        {
			pk_size = av_data.size;
			memcpy(fTo, av_data.ptr, pk_size);
			AudioRemand = 0;
		}

		AudioLock = av_data.serial;
		AudioBook = av_data.serial + 1;

		//fprintf(stderr,"Get Audio Data : seral:%d book:%d lock:%d size:%d\n",av_data.serial,AudioBook,AudioLock,av_data.size);
 		return pk_size;
	}
	else if (ret == RET_NO_VALID_DATA)
    {
		//fprintf(stderr,"NO_VALID_DATA live book:%d lock:%d\n",AudioBook,AudioLock);
		//usleep(33333);
		return 0;
	}
	else
    {
		fprintf(stderr,"Get Audio Error : %d\n",ret);
		AudioBook = 0;
		dbg("ERROR, ret=%d\n", ret);
		//usleep(33333);
		return -1;
	}
#else
	AV_DATA av_data;
	int ret;
/*
    if (!audio_enable)
    {
        usleep(1111);//3333
        return 0;
    }
*/
	if (AudioBook == 0) {
		GetAVData(AV_OP_GET_ULAW_SERIAL, -1, &av_data );
		if (av_data.serial <= AudioLock) {
			printf("av_data.serial <= audio_lock!!!\n");
			return 0;
		}
		AudioBook = av_data.serial;
	}

	ret = GetAVData(AV_OP_LOCK_ULAW, AudioBook, &av_data );
	if (ret == RET_SUCCESS) {
		if (AudioLock > 0) {
			GetAVData(AV_OP_UNLOCK_ULAW, AudioLock, NULL);
			AudioLock = 0;
		}
		if (av_data.size > fMaxSize)
			av_data.size = fMaxSize;
		memcpy(fTo, av_data.ptr, av_data.size);

		AudioLock = av_data.serial;
		AudioBook = av_data.serial + 1;
#if TIME_GET_WAY
		gettimeofday(&fPresentationTime, NULL);
#else
		fPresentationTime.tv_sec = av_data.timestamp/1000;
		fPresentationTime.tv_usec = (av_data.timestamp%1000)*1000;
#endif
 		return av_data.size;
	}
	else if (ret == RET_NO_VALID_DATA) {
		return 0;
	}
	else {
		AudioBook = 0;
		dbg("ERROR, ret=%d\n", ret);
		return -1;
	}
#endif
}

int AudioOpenFileSource::readFromFile() {
  int timeinc;

  if (!audio_enable)  return 0;

  // Read available audio data:
  int ret = getAudioData();

  if (ret <= 0) return 0;
  if (ret < 0) ret = 0;
  fFrameSize = (unsigned)ret;
  fNumTruncatedBytes = 0;

#if (TIME_GET_WAY)
/* PR#2665 fix from Robin
   * Assuming audio format = AFMT_S16_LE
   * Get the current time
   * Substract the time increment of the audio oss buffer, which is equal to
   * buffer_size / channel_number / sample_rate / sample_size ==> 400+ millisec
   */
 timeinc = fFrameSize * 1000 / audioNumChannels / (audioSamplingFrequency/1000) ;/// 2;
 while (fPresentationTime.tv_usec < timeinc)
 {
   fPresentationTime.tv_sec -= 1;
   timeinc -= 1000000;
 }

 fPresentationTime.tv_usec -= timeinc;

#else
#if 0 // test by Kenny Chiu 20130830
  timeinc = fFrameSize*1000 / audioNumChannels / (audioSamplingFrequency/1000);
  if( IsStart )
  {
  	IsStart = 0;
  	fPresentationTimePre = fPresentationTime;
  	fDurationInMicroseconds = timeinc;
  }else{
	fDurationInMicroseconds = timevaldiff(&fPresentationTimePre, &fPresentationTime )*1000;
	fPresentationTimePre = fPresentationTime;
  }

  if( fDurationInMicroseconds < timeinc)
  {
  	unsigned long msec;
  	//printf("1.fPresentationTime.tv_sec = %d fPresentationTime.tv_usec = %d \n",fPresentationTime.tv_sec,fPresentationTime.tv_usec);
	msec = fPresentationTime.tv_usec;
  	msec += (timeinc - fDurationInMicroseconds);
	fPresentationTime.tv_sec += msec/1000000;
	fPresentationTime.tv_usec = msec%1000000;
	//printf("2.fPresentationTime.tv_sec = %d fPresentationTime.tv_usec = %d \n",fPresentationTime.tv_sec,fPresentationTime.tv_usec);
	fDurationInMicroseconds = timeinc;

	fPresentationTimePre = fPresentationTime;
  }
  #endif
#endif
#if 0
	{
		static int iCount = 0;
		if (iCount++ < 200)
			fprintf(stderr, "Audio frame time = %d.%06d\n", fPresentationTime.tv_sec, fPresentationTime.tv_usec);
	}
#endif
  return 1;
}

int GetVolInfo(int vType,void *pBuff,int bufflen)
{
	int *cmd = av_field[vType];
	AV_DATA vol_data;

	if(GetAVData(cmd[AV_LOCK_MP4_VOL], -1, &vol_data) != RET_SUCCESS)
	{
		printf("Error on Get Vol data\n");
		return 0;
	}

	memcpy(pBuff, vol_data.ptr, vol_data.size);
	GetAVData(cmd[AV_UNLOCK_MP4_VOL], -1, &vol_data);

	return vol_data.size;
}

int GetSprop(void *pBuff, char vType)
{
	static char tempBuff[512];
	int ret = 0;
	int cnt = 0;
	int IsSPS = 0;
	int IsPPS = 0;
	int SPS_LEN = 0;
	int PPS_LEN = 4;
	char *pSPS=tempBuff;//0x7
	char *pPPS=tempBuff;//0x8
	char *pSPSEncode=NULL;
	char *pPPSEncode=NULL;

	ret = GetVolInfo(vType,tempBuff,sizeof(tempBuff));

	for(;;)
	{
		if(pSPS[0]==0&&pSPS[1]==0&&pSPS[2]==0&&pSPS[3]==1)
		{
			if( (pSPS[4]& 0x1F) == 7 )
			{
				IsSPS = 1;
				break;
			}
		}
		pSPS++;
		cnt++;
		if( (cnt+4)>ret )
			break;
	}
	if(IsSPS)
		pSPS += 4;

	cnt = 0;
	for(;;)
	{
		if(pPPS[0]==0&&pPPS[1]==0&&pPPS[2]==0&&pPPS[3]==1)
		{
			if( (pPPS[4]& 0x1F) == 8 )
			{
				IsPPS = 1;
				break;
			}
		}
		pPPS++;
		cnt++;
		if( (cnt+4)>ret )
			break;
	}

	if(IsPPS)
		pPPS += 4;

//	SPS_LEN = (unsigned int)pPPS - (unsigned int)pSPS;
	SPS_LEN = pPPS - pSPS;

	pSPSEncode = base64Encode(pSPS,SPS_LEN);
	pPPSEncode = base64Encode(pPPS,PPS_LEN);

	sprintf((char *)pBuff,"%s,%s",(char *)pSPSEncode,(char *)pPPSEncode);
	//printf("vType = %d pBuff = %s \n",vType,(char *)pBuff);

	delete[] pSPSEncode;
	delete[] pPPSEncode;

	return 1;
}

