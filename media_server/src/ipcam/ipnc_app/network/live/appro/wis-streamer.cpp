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
// An application that streams audio/video captured by a WIS GO7007,
// using a built-in RTSP server.
// main program

#include <sys/resource.h>
#include <signal.h>
#include <BasicUsageEnvironment.hh>
#include "GroupsockHelper.hh"
#include "AudioRTPCommon.hh"
#include <getopt.h>
#include <liveMedia.hh>
#include "Err.hh"
#include "WISJPEGVideoServerMediaSubsession.hh"
#include "WISMPEG4VideoServerMediaSubsession.hh"
#include "WISH264VideoServerMediaSubsession.hh"
#include "WISPCMAudioServerMediaSubsession.hh"
#include "WISOnvifServerMediaSubsession.hh"
#include "WISJPEGStreamSource.hh"
#include <stream_msg_drv.h>
#include <avdata_drv.h>
#include <file_msg_drv.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "typedef.h"
#include "amr.h"
#include "rom_enc.h"

#define __WIS_DEBUG
#ifdef __WIS_DEBUG
#define __D(fmt, args...) fprintf(stderr, "Debug: " fmt, ## args)
#else
#define __D(fmt, args...)
#endif

#define __E(fmt, args...) fprintf(stderr, "Error: " fmt, ## args)


int H264_ON = 0;
int mjpg_disable = 0;
//int is_ssm = 0;  /* '0' unicast, '1' multicast */
extern int is_ssm;  /* '0' unicast, '1' multicast */
int rtsp_port = 554;
int rtp_data_port = 6970;
int rtp_port = 6660;
int rtp_port_audio = 6662;
int ttl_num = 255;
int video_priority = 0;
int audio_priority = 0;
int rtsp_srv_socket = 0;
int is_3gpp = 0;  /* '0' enable with audio, '1' disable, '2' enable without audio */
int is_audio = 0;  /* '0' disable audio, '1' enable audio */
char multi_ip[64];
int is_3g_audio = 0;
extern unsigned char mjpeg_type; //65 is Geo, 0 is Xarian
extern unsigned char mjpeg_qFactor; // 255 is Geo, 75 is Xarian. indicates that quantization tables are dynamic or static included.

int MjpegVideoBitrate = 1500000;
int Mpeg4VideoBitrate = 1500000;
int H264VideoBitrate = 1500000;

unsigned audioSamplingFrequency = 16000;//for Geo //8000; 
unsigned audioNumChannels = 2;//for Geo //1;
int audio_enable = 1;
int audioFormat=0;
int qid;
//extern unsigned char mjpeg_quality;
extern unsigned int mjpeg_resolution;
int update_sw = 0;
extern int reset_flag;
pthread_t wait_thread;

RTSPServer* rtspServer = NULL;

portNumBits rtspServerPortNum = 554;
char const* MjpegStreamName = "stream2";
char const* Mpeg4StreamName = "stream1";

//char const* Mpeg43GPPStreamName = "mpeg4_3gpp";
char const* Mpeg43GPPStreamName = "mobile"; 
char const* H264StreamName = "stream4"; 
char const* streamDescription = "RTSP/RTP stream";

static UsageEnvironment* env;
static TaskScheduler* scheduler;
static RTPSink* videoSinkMP4;
static RTPSink* videoSinkMP43GPP;
static RTPSink* videoSinkH264;
static RTPSink* audioSink;
static RTPSink* audioSink3GPP;
static RTPSink* audioSinkH264;
static RTPSink* audioSinkMJ;
static RTPSink* videoSinkMJ;

static Groupsock *rtpGroupsock;
static Groupsock *rtcpGroupsock;
static Groupsock *rtpAudioGroupsock;
static Groupsock *rtcpAudioGroupsock;

static Groupsock *rtpGroupsock3GPP;
static Groupsock *rtcpGroupsock3GPP;
static Groupsock *rtpAudioGroupsock3GPP;
static Groupsock *rtcpAudioGroupsock3GPP;

static Groupsock *rtpGroupsockH264;
static Groupsock *rtcpGroupsockH264;
static Groupsock *rtpAudioGroupsockH264;
static Groupsock *rtcpAudioGroupsockH264;

static Groupsock *rtpMjGroupsock;
static Groupsock *rtcpMjGroupsock;
static Groupsock *rtpMJAudioGroupsock;
static Groupsock *rtcpMJAudioGroupsock;

static APPROInput* MjpegInputDevice;
static APPROInput* Mpeg4InputDevice;
static APPROInput* Mpeg43gppInputDevice;
static APPROInput* H264InputDevice;
static ServerMediaSession* sms;
static ServerMediaSession* sms_3gpp;
static ServerMediaSession* sms_h264;
static ServerMediaSession* sms_mj;

static int last_is_ssm = 0;

AMR_state amr_ptr;
void sigterm()
{
    printf("caught SIGTERM: shutting down(sigterm)\n");

    unlink("/tmp/wis_arg");
    //if (audioFormat >= 6)
        //AUDIO_AMR_Close(&amr_ptr);
    update_sw = 1;
    rtspServer->removeServerMediaSession(sms);
    rtspServer->removeServerMediaSession(sms_3gpp);
    rtspServer->removeServerMediaSession(sms_h264);
    rtspServer->removeServerMediaSession(sms_mj);
    Medium::close(MjpegInputDevice);
    Medium::close(Mpeg4InputDevice);
    Medium::close(Mpeg43gppInputDevice);
    Medium::close(H264InputDevice);
    Medium::close(rtspServer);
	usleep(1000*500);
#if 0
    ApproInterfaceExit();//sleep(1);
#endif
    exit(1);
}

void sigint()
{
    printf("caught SIGINT: shutting down(sigint)\n");

    unlink("/tmp/wis_arg");
    //if (audioFormat >= 6)
        //AUDIO_AMR_Close(&amr_ptr);
    update_sw = 1;
    rtspServer->removeServerMediaSession(sms);
    rtspServer->removeServerMediaSession(sms_3gpp);
    rtspServer->removeServerMediaSession(sms_h264);
    rtspServer->removeServerMediaSession(sms_mj);
    Medium::close(MjpegInputDevice);
    Medium::close(Mpeg4InputDevice);
    Medium::close(Mpeg43gppInputDevice);
    Medium::close(H264InputDevice);
    Medium::close(rtspServer);
#if 0
    ApproInterfaceExit();//sleep(1);
#endif
    exit(1);
}

void SigChld(void)
{
  //while (waitpid(-1, NULL, WNOHANG) > 0);
}

void sig_audio_enable()
{
    fprintf(stderr, "Wis-Stream Audio enabled!\n");
    audio_enable = 1;
}

void sig_audio_disable()
{
    fprintf(stderr, "Wis-Stream Audio disabled!\n");
    audio_enable = 0;
}

void sig_update()
{
    fprintf(stderr,"sig_update!\n");
    update_sw = 1;
}

UserAuthenticationDatabase *pInitAuthDB(SysInfo *pSysInfo)
{
	UserAuthenticationDatabase *pAuthDB = NULL; 
#if RTSP_AUTHORIZATION_FEATURE
	if (pSysInfo != NULL && pSysInfo->lan_config.rtsp_authorization == 1)
	{
		int i;
		pAuthDB = new UserAuthenticationDatabase();
		for (i=0;i<ACOUNT_NUM;i++)
		{
			if (strlen(pSysInfo->acounts[i].user) > 0)
				pAuthDB->addUserRecord(pSysInfo->acounts[i].user, pSysInfo->acounts[i].password);
		}
	}
#endif
	return pAuthDB;
}

void ChangeRtspAuthorization()
{
	SysInfo *pSysInfo = GetSysInfo();
	UserAuthenticationDatabase *pAuthDB = pInitAuthDB(pSysInfo);
	rtspServer->setAuthenticationDatabase(pAuthDB);
}

void signal_handler(int sig)
{
    fprintf(stderr, "---->Wis-Stream singal handler : %d\n", sig);
    switch(sig)
    {
    case SIGINT:
        fprintf(stderr, "---->Wis-Stream SIGINT\n");
        //sigint();
        break;
    case SIGUSR1://10
        fprintf(stderr, "---->Wis-Stream SIGUSR1\n");
//		ChangeRtspAuthorization();
//        sig_audio_enable();
        break;
    case SIGUSR2://12
        fprintf(stderr, "---->Wis-Stream SIGUSR2\n");
//        sig_audio_disable();
        break;
    case SIGTERM://15
        fprintf(stderr, "---->Wis-Stream SIGTERM\n");
        sigterm();
        break;
    case SIGCHLD: //17
        fprintf(stderr, "---->BOA(Wis-Stream) SIGCHLD\n");
        SigChld();
        break;
    case SIGPIPE:
        fprintf(stderr, "---->Wis-Stream SIGPIPE\n");
        break;
    case SIGXCPU:
        fprintf(stderr, "---->Wis-Stream SIGXCPU\n");
        break;
    case SIGXFSZ:
        fprintf(stderr, "---->Wis-Stream SIGXFSZ\n");
        break;
	case SIGPWR://30
		fprintf(stderr, "---->Wis-Stream SIGPWR\n");
		sig_update();
		break;
//	case 18:
//		ChangeRtspAuthorization();
//		break;
    default:
        fprintf(stderr, "---->Wis-Stream(unknown) signal\n");
		break;
    }
}

void setSignalHandler()
{
    struct sigaction act;

    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_handler = signal_handler;
    act.sa_flags = SA_NOCLDWAIT | SA_RESTART | SA_NOMASK;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGPIPE, &act, NULL);
    sigaction(SIGXCPU, &act, NULL);
    sigaction(SIGXFSZ, &act, NULL);
    sigaction(SIGPWR, &act, NULL);
    sigaction(18, &act, NULL);
}

void share_memory_init(int msg_type)
{
    if (ApproDrvInit(msg_type,1))   {  exit(1);  }
	
    if (func_get_mem(NULL))
    {
        ApproDrvExit();
        exit(1);
    }
}

static RTPSink *createAudioRTPSink(Groupsock* rtpGroupsock)
{
    if (audioFormat == 0/*AFMT_PCM_ULAW*/)
    {
        return SimpleRTPSink::createNew(*env, rtpGroupsock, 0,
                    audioSamplingFrequency, "audio", "PCMU", audioNumChannels);
    }
    else if (audioFormat == 1/*AFMT_PCM_ALAW*/)
    {
        return SimpleRTPSink::createNew(*env, rtpGroupsock, 8,
                audioSamplingFrequency, "audio", "PCMA", audioNumChannels);
    }
#if 0	
    else if (audioFormat == 6/*AFMT_AMR MR 122*/)
    {
        return AMRAudioRTPSink::createNew(*env, rtpGroupsock, 97, audioFormat, False, audioNumChannels);
    }
    else if (audioFormat == 7/*AFMT_AMR MR 795*/)
    {
        return AMRAudioRTPSink::createNew(*env, rtpGroupsock, 97, audioFormat, False, audioNumChannels);
    }
    else if (audioFormat == 8/*AFMT_AMR MR 475*/)
    {
        return AMRAudioRTPSink::createNew(*env, rtpGroupsock, 97, audioFormat, False, audioNumChannels);
    }
#endif	
    else if (audioFormat == 2/*AFMT_PCM_G726_16*/)
    {
        return SimpleRTPSink::createNew(*env, rtpGroupsock, 97,
                audioSamplingFrequency, "audio", "G726-16", audioNumChannels);
    }
    else if (audioFormat == 3/*AFMT_PCM_G726_24*/)
    {
        return SimpleRTPSink::createNew(*env, rtpGroupsock, 97,
                audioSamplingFrequency, "audio", "G726-24", audioNumChannels);
    }
    else if (audioFormat == 4/* AFMT_PCM_G726_32*/)
    {
        return SimpleRTPSink::createNew(*env, rtpGroupsock, 97,
                audioSamplingFrequency, "audio", "G726-32", audioNumChannels);
    }
    else if (audioFormat == 5/*AFMT_PCM_G726_40*/)
    {
        return SimpleRTPSink::createNew(*env, rtpGroupsock, 97,
                audioSamplingFrequency, "audio", "G726-40", audioNumChannels);
    }
   else if (audioFormat == 9/*AFMT_AAC*/)//added by frank, 2015 0902
    {
    	char const*enconderConfigStr = "1410";
		audioSamplingFrequency = 16000;//by Geo.
        return MPEG4GenericRTPSink::createNew(*env, rtpGroupsock, 97,
                audioSamplingFrequency, "audio", "AAC-hbr", enconderConfigStr, audioNumChannels);
    }		
    else
    {
        return SimpleRTPSink::createNew(*env, rtpGroupsock, 0,
                    audioSamplingFrequency, "audio", "PCMU", audioNumChannels);
    }
}

void setupUnicastStreaming(APPROInput& inputDevice, APPROInput& inputDevice_3gpp,APPROInput& inputDevice_H264,
        APPROInput& inputDevice_mj, ServerMediaSession* sms, ServerMediaSession* sms_3gpp, ServerMediaSession* sms_h264,
        ServerMediaSession* sms_mj)
{
    if (H264_ON)
    {
        sms->addSubsession(WISH264VideoServerMediaSubsession
            ::createNew(sms->envir(), inputDevice, H264VideoBitrate));
    }
    else
    {
        sms->addSubsession(WISMPEG4VideoServerMediaSubsession
            ::createNew(sms->envir(), inputDevice, Mpeg4VideoBitrate));
    }

    if(is_audio)
    {
        sms->addSubsession(WISPCMAudioServerMediaSubsession::createNew(sms->envir(), inputDevice, audioFormat));
//Modify by Joseph 2013-05-21	
//	 sms->addSubsession(WISOnvifServerMediaSubsession::createNew(sms->envir(), inputDevice, Mpeg4VideoBitrate));	
    }
    if (H264_ON)
    {
        if ((is_3gpp == 1) || (is_3gpp == 2))
            sms_3gpp->addSubsession(WISH264VideoServerMediaSubsession
                ::createNew(sms_3gpp->envir(), inputDevice_3gpp, H264VideoBitrate));
    }
    else
    {
        if ((is_3gpp == 1) || (is_3gpp == 2))
            sms_3gpp->addSubsession(WISMPEG4VideoServerMediaSubsession
                ::createNew(sms_3gpp->envir(), inputDevice_3gpp, Mpeg4VideoBitrate));
    }

    if (H264_ON)
    {
       sms_h264->addSubsession(WISH264VideoServerMediaSubsession
                ::createNew(sms_h264->envir(), inputDevice_H264, H264VideoBitrate));
    }


    fprintf(stderr,"!!!!!!!%d\n",audioFormat);

    
	if (is_3g_audio)//if (1) // modify by kenny chiu 20110920
    {//if(is_audio){
        sms_3gpp->addSubsession(WISPCMAudioServerMediaSubsession::createNew(sms_3gpp->envir(), inputDevice_3gpp, audioFormat));
    }
	
//stream 3
    sms_h264->addSubsession(WISPCMAudioServerMediaSubsession::createNew(sms_h264->envir(), inputDevice_H264, audioFormat));
	
	if (mjpg_disable == 0)
	{
   	 sms_mj->addSubsession(WISJPEGVideoServerMediaSubsession
    	        ::createNew(sms_mj->envir(), inputDevice_mj, MjpegVideoBitrate));
   	 if(is_audio){
   	     sms_mj->addSubsession(WISPCMAudioServerMediaSubsession
   	             ::createNew(sms_mj->envir(), inputDevice_mj, audioFormat));
//Modify by Joseph 2013-05-21 
//		 sms_mj->addSubsession(WISOnvifServerMediaSubsession
//   	             ::createNew(sms_mj->envir(), inputDevice_mj, Mpeg4VideoBitrate));		 
   	 }
	} 
    last_is_ssm = 0;
}

void setupMulticastStreaming(APPROInput& inputDevice, APPROInput& inputDevice_3gpp, APPROInput& inputDevice_H264,
        APPROInput& inputDevice_mj, ServerMediaSession* sms, ServerMediaSession* sms_3gpp,ServerMediaSession* sms_h264,
        ServerMediaSession* sms_mj, char *mul_addr)
{
    // Create 'groupsocks' for RTP and RTCP:
    // Create 'groupsocks' for RTP and RTCP:
    struct in_addr destinationAddress;
#if 1
    destinationAddress.s_addr = our_inet_addr(mul_addr);
#else
    destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
#endif
    fprintf(stderr,"setupMulticastStreaming\n");
    const unsigned short rtpPortNum = rtp_port;
    const unsigned short rtcpPortNum = rtpPortNum+1;
    const unsigned short rtpPortNumAudio = rtp_port_audio;
    const unsigned short rtcpPortNumAudio = rtpPortNumAudio+1;
    const unsigned short rtpPortNum_3gpp = rtp_port;
    const unsigned short rtcpPortNum_3gpp = rtpPortNum+1;
    const unsigned short rtpPortNumAudio_3gpp = rtp_port_audio;
    const unsigned short rtcpPortNumAudio_3gpp = rtpPortNumAudio+1;
    const unsigned short rtpPortNumMj = rtp_port;
    const unsigned short rtcpPortNumMj = rtpPortNumAudio+1;
    const unsigned short rtpPortNumAudioMj = rtp_port_audio;
    const unsigned short rtcpPortNumAudioMj = rtpPortNumAudio+1;
    const unsigned char ttl = ttl_num;

    const Port rtpPort(rtpPortNum);
    const Port rtcpPort(rtcpPortNum);
    const Port rtpAudioPort(rtpPortNumAudio);
    const Port rtcpAudioPort(rtcpPortNumAudio);
    const Port rtpPort_3gpp(rtpPortNum_3gpp);
    const Port rtcpPort_3gpp(rtcpPortNum_3gpp);
    const Port rtpAudioPort_3gpp(rtpPortNumAudio_3gpp);
    const Port rtcpAudioPort_3gpp(rtcpPortNumAudio_3gpp);
    const Port rtpMjPort(rtpPortNumMj);
    const Port rtcpMjPort(rtcpPortNumMj);
    const Port rtpMjAudioPort(rtpPortNumAudioMj);
    const Port rtcpMjAudioPort(rtcpPortNumAudioMj);

    rtpGroupsock = new Groupsock(*env, destinationAddress, rtpPort, ttl);
    rtcpGroupsock = new Groupsock(*env, destinationAddress, rtcpPort, ttl);
    rtpAudioGroupsock = new Groupsock(*env, destinationAddress, rtpAudioPort, ttl);
    rtcpAudioGroupsock = new Groupsock(*env, destinationAddress, rtcpAudioPort, ttl);
    rtpMjGroupsock = new Groupsock(*env, destinationAddress, rtpMjPort, ttl);
    rtpGroupsock3GPP = new Groupsock(*env, destinationAddress, rtpPort_3gpp, ttl);
    rtcpGroupsock3GPP = new Groupsock(*env, destinationAddress, rtcpPort_3gpp, ttl);
    rtpAudioGroupsock3GPP = new Groupsock(*env, destinationAddress, rtpAudioPort_3gpp, ttl);
    rtcpAudioGroupsock3GPP = new Groupsock(*env, destinationAddress, rtcpAudioPort_3gpp, ttl);
    rtpMjGroupsock = new Groupsock(*env, destinationAddress, rtpMjPort, ttl);
    rtcpMjGroupsock = new Groupsock(*env, destinationAddress, rtcpMjPort, ttl);
    rtpMJAudioGroupsock = new Groupsock(*env, destinationAddress, rtpMjAudioPort, ttl);
    rtcpMJAudioGroupsock = new Groupsock(*env, destinationAddress, rtcpMjAudioPort, ttl);
    rtpGroupsock->multicastSendOnly(); // we're a SSM source
    rtcpGroupsock->multicastSendOnly(); // we're a SSM source
    rtpAudioGroupsock->multicastSendOnly(); // we're a SSM source
    rtcpAudioGroupsock->multicastSendOnly(); // we're a SSM source
    rtpGroupsock3GPP->multicastSendOnly(); // we're a SSM source
    rtcpGroupsock3GPP->multicastSendOnly(); // we're a SSM source
    rtpAudioGroupsock3GPP->multicastSendOnly(); // we're a SSM source
    rtcpAudioGroupsock3GPP->multicastSendOnly(); // we're a SSM source
    rtpMjGroupsock->multicastSendOnly(); // we're a SSM source
    rtcpMjGroupsock->multicastSendOnly(); // we're a SSM source
    rtpMJAudioGroupsock->multicastSendOnly(); // we're a SSM source
    rtcpMJAudioGroupsock->multicastSendOnly(); // we're a SSM source

    if (H264_ON)
	{
    	//	OutPacketBuffer::maxSize = 800000; 
		videoSinkMP4 = H264VideoRTPSink::createNew(*env, rtpGroupsock, 96);
	}
    else
        videoSinkMP4 = MPEG4ESVideoRTPSink::createNew(*env, rtpGroupsock, 96);
    audioSink = createAudioRTPSink(rtpAudioGroupsock);
	
    if (H264_ON)
    {
    	//	OutPacketBuffer::maxSize = 800000; 
		videoSinkMP43GPP = H264VideoRTPSink::createNew(*env, rtpGroupsock, 96);
    }
    else
        videoSinkMP43GPP = MPEG4ESVideoRTPSink::createNew(*env, rtpGroupsock3GPP, 96);
    audioSink3GPP = createAudioRTPSink(rtpAudioGroupsock3GPP);

    if (H264_ON)
    {
    	//	OutPacketBuffer::maxSize = 800000; 
		videoSinkH264 = H264VideoRTPSink::createNew(*env, rtpGroupsock, 96);
    }
    audioSinkH264 = createAudioRTPSink(rtpAudioGroupsockH264);

	
    videoSinkMJ = JPEGVideoRTPSink::createNew(*env, rtpMjGroupsock);
    audioSinkMJ = createAudioRTPSink(rtpMJAudioGroupsock);

    const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
    const unsigned maxCNAMElen = 100;
    unsigned char CNAME[maxCNAMElen+1];
    gethostname((char*)CNAME, maxCNAMElen);
    CNAME[maxCNAMElen] = '\0'; // just in case
    RTCPInstance* rtcp
        = RTCPInstance::createNew(*env, rtcpGroupsock,
                500, CNAME,
                videoSinkMP4, NULL /* we're a server */,
                True /* we're a SSM source */);
    RTCPInstance* rtcpaudio
        = RTCPInstance::createNew(*env, rtcpAudioGroupsock,
                160, CNAME,
                audioSink, NULL /* we're a server */,
                True /* we're a SSM source */);
    RTCPInstance* rtcp_3gpp
        = RTCPInstance::createNew(*env, rtcpGroupsock3GPP,
                500, CNAME,
                videoSinkMP43GPP, NULL /* we're a server */,
                True /* we're a SSM source */);
    RTCPInstance* rtcpaudio_3gpp
        = RTCPInstance::createNew(*env, rtcpAudioGroupsock3GPP,
                160, CNAME,
                audioSink3GPP, NULL /* we're a server */,
                True /* we're a SSM source */);
    RTCPInstance* rtcp_H264
        = RTCPInstance::createNew(*env, rtcpGroupsockH264,
                500, CNAME,
                videoSinkH264, NULL /* we're a server */,
                True /* we're a SSM source */);
    RTCPInstance* rtcpaudio_H264
        = RTCPInstance::createNew(*env, rtcpAudioGroupsockH264,
                160, CNAME,
                audioSink3GPP, NULL /* we're a server */,
                True /* we're a SSM source */);

    RTCPInstance* rtcpmj
        = RTCPInstance::createNew(*env, rtcpGroupsock,
                500, CNAME,
                videoSinkMJ, NULL /* we're a server */,
                True /* we're a SSM source */);
    RTCPInstance* rtcpaudiomj
        = RTCPInstance::createNew(*env, rtcpMJAudioGroupsock,
                160, CNAME,
                audioSinkMJ, NULL /* we're a server */,
                True /* we're a SSM source */);

    sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSinkMP4, rtcp));
	
    if(audio_enable){
        sms->addSubsession(PassiveServerMediaSubsession::createNew(*audioSink, rtcpaudio));
    }
#if 0    // modify by kenny chiu 20111201
    sms_3gpp->addSubsession(PassiveServerMediaSubsession::createNew(*videoSinkMP43GPP, rtcp_3gpp));
    //if (1)
    {//if(audio_enable){
        sms_3gpp->addSubsession(PassiveServerMediaSubsession::createNew(*audioSink3GPP, rtcpaudio_3gpp));
    }
#endif
    sms_mj->addSubsession(PassiveServerMediaSubsession::createNew(*videoSinkMJ, rtcpmj));
    if(audio_enable){
        sms_mj->addSubsession(PassiveServerMediaSubsession::createNew(*audioSinkMJ, rtcpaudiomj));
    }
    if (H264_ON)
    {
        videoSinkMP4->startPlaying(*(H264VideoStreamDiscreteFramer::createNew(*env, inputDevice.videoSource())), NULL, videoSinkMP4);
    }
    else
    {
        videoSinkMP4->startPlaying(*(MPEG4VideoStreamDiscreteFramer::createNew(*env, inputDevice.videoSource())), NULL, videoSinkMP4);
    }
//    if (1)
	if(is_audio)
    {//if(is_audio){
        audioSink->startPlaying(*(inputDevice.audioSource()), NULL, audioSink);
    }
#if 0  // update by kenny chiu 20110916
    if (H264_ON)
    {
        if ((is_3gpp == 1) || (is_3gpp == 2))
            videoSinkMP43GPP->startPlaying(*(H264VideoStreamFramer::createNew(*env, inputDevice_3gpp.videoSource())), NULL, videoSinkMP43GPP);
    }
    else
    {
        if ((is_3gpp == 1) || (is_3gpp == 2))
            videoSinkMP43GPP->startPlaying(*(MPEG4VideoStreamDiscreteFramer::createNew(*env, inputDevice_3gpp.videoSource())), NULL, videoSinkMP43GPP);
    }

    if ((is_3gpp == 2))
    {//if((is_3gpp == 0) && (is_audio)){
        audioSink3GPP->startPlaying(*(inputDevice_3gpp.audioSource()), NULL, audioSink3GPP);
    }
#endif
	if (mjpg_disable == 0)
	{
	    videoSinkMJ->startPlaying(*(WISJPEGStreamSource::createNew(inputDevice_mj.videoSource())), NULL, videoSinkMJ);
    	//if (1)
	    if (is_audio)
	    {//if(is_audio){
	        audioSinkMJ->startPlaying(*(inputDevice_mj.audioSource()), NULL, audioSinkMJ);
	    }
	}
    last_is_ssm = 1;
}

static int wait_av_server(void)//static int wait_av_server(void)
{
    int cnt = 0;
    struct stat st;

    while (1)
    {
        if (cnt >= 20)
            break;

        if (stat("/tmp/stream_start", &st) >= 0)
        {
            fprintf(stderr, "wait_av_server OK %d!!!!!!!\n", cnt);
            //usleep(5000000);
            return 0;
        }
        //fprintf(stderr, "wait_av_server %d!!!!!!!\n", cnt);
        cnt++;
        sleep(1);
    }
    fprintf(stderr, "wait_av_server Fail %d!!!!!!!\n", cnt);

    return -1;
}

//extern unsigned char mjpeg_quality;
int main(int argc, char** argv)
{  
    setSignalHandler();
#if 0
  //fprintf(stderr, "1111<---------------------------------------\n");
    if (wait_av_server() < 0)
    {
        fprintf(stderr, "Wait AV server time out!\n");
        exit(-1);
    }
  //fprintf(stderr, "2222<---------------------------------------\n");
#endif
    // Begin by setting up our usage environment:
    
    TaskScheduler* scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);
    int msg_type, video_type;
    FrameInfo_t curframe ; 
	SysInfo *pSysInfo = NULL;
//	Extra_Info *get_extra_info_ptr = NULL;
	UserAuthenticationDatabase *pAuthDB = NULL; 

	/* Init FileMsg/Shared Mem */
	if(InitFileMsgDrv(FILE_MSG_KEY, FILE_SYS_MSG) < 0)
	{
		//ApproDrvExit();
		//SysDrvExit();
		//exit(1);
		return 0;
	}

	pSysInfo = GetSysInfo();
	if (pSysInfo == NULL)
	{
		fprintf(stderr,"%s[%d] Shared Memory Fail !\n",__func__,__LINE__);
		return 0;
	}
//	get_extra_info_ptr = &pSysInfo->extrainfo;

    is_ssm =  pSysInfo->extrainfo.general_net_mode.multicastenable; //atoi(argv[1]);
   // rtsp_port = pSysInfo->extrainfo.general_net_mode.rtspport; //atoi(argv[2]);
    rtp_port = pSysInfo->extrainfo.general_net_mode.videoport; //atoi(argv[3]);
    rtp_port_audio = pSysInfo->extrainfo.general_net_mode.audioport; //atoi(argv[4]);
    ttl_num = pSysInfo->extrainfo.general_net_mode.ttl; //atoi(argv[5]);
    video_priority = 0;//atoi(argv[6]);
    audio_priority = 0;//atoi(argv[7]);
    //mjpeg_quality = 60;// atoi(argv[8]);
    mjpeg_type = 0;//65;//WIS_MJPEG_TYPE;
	mjpeg_qFactor = 75;//255;//WIS_MJPEG_QFACTOR;
	
    rtp_data_port = pSysInfo->extrainfo.general_net_mode.rtpdataport; //atoi(argv[9]);
    is_3gpp = 2; //atoi(argv[10]);
    is_audio = pSysInfo->audio_config.audioenable;//atoi(argv[11]);
    H264_ON = 1; //atoi(argv[12]);  /*1:H264 3GPP; 0:MPEG4 3GPP*/
//    jpeg_resolution = atoi(argv[13]);
    
    strcpy(multi_ip, pSysInfo->extrainfo.general_net_mode.group_addr /*argv[13]*/);
    mjpg_disable = 0;//atoi(argv[14]);
    
    is_3g_audio = (is_audio ? 1 : (is_3gpp == 2) ? 1: 0);  
    rtspServerPortNum = rtsp_port;
   // printf( "LIVE 3GPP H264_ON = %d \n", H264_ON);

//fprintf(stderr, "3333<---------------------------------------\n");
    Mpeg4VideoBitrate = 500000;

#if 0
    switch (fork())
    {
    case -1:
        /* error */
        perror("fork");
        exit(1);
        break;
    case 0:
        /* child, success */
        msg_type = LIVE_MSG_TYPE2;
        video_type = VIDEO_TYPE_MPEG4_CIF;
        rtspServerPortNum = rtsp_port + 1;
        Mpeg4VideoBitrate = 500000;
        break;
    default:
        /* parent, success */
        msg_type = LIVE_MSG_TYPE;
        video_type = VIDEO_TYPE_MPEG4;
        rtspServerPortNum = rtsp_port;
        Mpeg4VideoBitrate = 1500000;
        break;
    }
#endif
    //RS
    //msg_type = LIVE_MSG_TYPE2;
    //video_type = VIDEO_TYPE_MPEG4_CIF;
    //rtspServerPortNum = 554;
    //Mpeg4VideoBitrate = 500000;
    //fprintf(stderr,"kenneth(CS8200) streamer\n");
    //sleep(5);
    share_memory_init(LIVE_MSG_TYPE);
    __D("Wis-streamer 1 !\n");
    //share_memory_init(LIVE_MSG_TYPE2);
//    while(1) {  
//		curframe = GetCurrentFrame(FMT_MJPEG); __D("Wis-streamer curframe = %d %d %d\n",curframe.format,curframe.size,curframe.serial_no);	
//		curframe = GetCurrentFrame(FMT_MPEG4); __D("Wis-streamer curframe = %d %d %d\n",curframe.format,curframe.size,curframe.serial_no);	
//		curframe = GetCurrentFrame(FMT_AUDIO); __D("Wis-streamer curframe = %d %d %d\n",curframe.format,curframe.size,curframe.serial_no);
//		}
//    __D("Wis-streamer 2 !\n");	
  //    fprintf(stderr, "Wis-streamwer 3 !\n");
	audioFormat = 9; //Geo AAC_LC
//    *env << "Initializing...\n";
    __D("LIVE Audio Codec = %d \n", audioFormat);
    MjpegInputDevice = NULL;
    // Initialize the WIS input device:
    MjpegInputDevice = APPROInput::createNew(*env, VIDEO_TYPE_MJPEG);
    if (MjpegInputDevice == NULL)
    {
        err(*env) << "Failed to create MJPEG input device\n";
        exit(1);
    }

    Mpeg4InputDevice = Mpeg43gppInputDevice = H264InputDevice = NULL;

    if (H264_ON)
    { //VIDEO_TYPE_H264_CIF,VIDEO_TYPE_H264
        Mpeg4InputDevice = APPROInput::createNew(*env, VIDEO_TYPE_H264);
        if (Mpeg4InputDevice == NULL)
        {
            err(*env) << "Failed to create H264 input device\n";
            exit(1);
        }

        Mpeg43gppInputDevice = APPROInput::createNew(*env, VIDEO_TYPE_H264_CIF);
        if (Mpeg43gppInputDevice == NULL)
        {
            err(*env) << "Failed to create H264 3GPP input device\n";
            exit(1);
        }

        H264InputDevice = APPROInput::createNew(*env, VIDEO_TYPE_H264_VGA);
        if (H264InputDevice == NULL)
        {
            err(*env) << "Failed to create H264 stream3 input device\n";
            exit(1);
        }
    }
    else
    {
        Mpeg4InputDevice = APPROInput::createNew(*env, VIDEO_TYPE_MPEG4);
        if (Mpeg4InputDevice == NULL)
        {
            err(*env) << "Failed to create MPEG4 input device\n";
            exit(1);
        }

        Mpeg43gppInputDevice = APPROInput::createNew(*env, VIDEO_TYPE_MPEG4_CIF);
        if (Mpeg43gppInputDevice == NULL)
        {
            err(*env) << "Failed to create MPEG4 3GPP input device\n";
            exit(1);
        }
    }

    // Create the RTSP server:
    //RTSPServer* rtspServer = NULL;
    // Normal case: Streaming from a built-in RTSP server:
#if RTSP_AUTHORIZATION_FEATURE
	pAuthDB = pInitAuthDB(pSysInfo);
#endif
	rtspServer = RTSPServer::createNew(*env, rtspServerPortNum, pAuthDB);
    if (rtspServer == NULL)
    {
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
        exit(1);
    }

 //  rtsp_srv_socket = rtspServer->fServerSocket;

    *env << "...done initializing\n";

    /* Select Unicast or Multicast */
    Boolean sSM = (is_ssm) ? True : False;

    ServerMediaSession* sms  = ServerMediaSession::createNew(*env, Mpeg4StreamName, Mpeg4StreamName, streamDescription, sSM);	
    ServerMediaSession* sms_3gpp = ServerMediaSession::createNew(*env, Mpeg43GPPStreamName, Mpeg43GPPStreamName, streamDescription, sSM);	
    ServerMediaSession* sms_h264 = ServerMediaSession::createNew(*env, H264StreamName, H264StreamName, streamDescription, sSM);	
    ServerMediaSession* sms_mj = ServerMediaSession::createNew(*env, MjpegStreamName, MjpegStreamName, streamDescription, sSM);

    rtspServer->addServerMediaSession(sms);
    rtspServer->addServerMediaSession(sms_3gpp);
    rtspServer->addServerMediaSession(sms_h264);
    rtspServer->addServerMediaSession(sms_mj);

    if (is_ssm)
    {
        //*env << "before setupMulticastStreaming \n";
        setupMulticastStreaming(*Mpeg4InputDevice, *Mpeg43gppInputDevice,*H264InputDevice,
                                *MjpegInputDevice, sms, sms_3gpp, sms_h264, sms_mj, multi_ip);
        //*env << "after setupMulticastStreaming \n";
    }
    else
    {
        //*env << "before setupUnicastStreaming \n";
        setupUnicastStreaming(*Mpeg4InputDevice, *Mpeg43gppInputDevice,*H264InputDevice,
                                *MjpegInputDevice, sms, sms_3gpp,sms_h264,sms_mj);
        //*env << "after setupUnicastStreaming \n";
    }
#if 1
	if (  rtspServer->setUpTunnelingOverHTTP(8000) ) {
	*env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
	} else {
	*env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
	}
#endif 

#if 0 // RTSP-over-HTTP tunneling is not yet working   
  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.   
  // Try first with the default HTTP port (80), and then with the alternative HTTP   
  // port number (8000).   
  RTSPOverHTTPServer* rtspOverHTTPServer;  
  portNumBits httpServerPortNum = 8000;  
  rtspOverHTTPServer = RTSPOverHTTPServer::createNew(*env, httpServerPortNum, rtsp_port);  
  if (rtspOverHTTPServer == NULL) {  
    httpServerPortNum = 8000;  
    rtspOverHTTPServer = RTSPOverHTTPServer::createNew(*env, httpServerPortNum, rtsp_port);  
  }  
  if (rtspOverHTTPServer == NULL) {  
    *env << "(No server for RTSP-over-HTTP tunneling was created.)\n";  
  } else {  
    *env << "(We use port " << httpServerPortNum << " for RTSP-over-HTTP tunneling.)\n";  
  }  
#endif

#if 1
	int pid = getpid();
	
	if (setpriority(PRIO_PROCESS, pid, -2) < 0)
		fprintf(stderr, "wis-streamer setpriority failed !!\n");
    int prio_process = getpriority(PRIO_PROCESS, pid);
    fprintf(stderr ,"wis-streamer Process (%d) Priority is %d.\n", pid, prio_process);
#endif


    char *url = rtspServer->rtspURL(sms);
    *env << "Play this Main stream using the URL:\n\t" << url << "\n";
    delete[] url;
	if (!is_ssm)  
	{
	    if (is_3gpp == 1 || is_3gpp == 2)
	    {
	        char *url_3gpp = rtspServer->rtspURL(sms_3gpp);
	        *env << "Play this 3GPP stream using the URL:\n\t" << url_3gpp << "\n";
	        delete[] url_3gpp;
	    }
	}
	
	if (mjpg_disable == 0)
	{
	    char *url_mj = rtspServer->rtspURL(sms_mj);
	    *env << "Play this MJPEG stream using the URL:\n\t" << url_mj << "\n";
	    delete[] url_mj;
	}

    char *url_h264 = rtspServer->rtspURL(sms_h264);
    *env << "Play this third stream using the URL:\n\t" << url_h264 << "\n";
    delete[] url_h264;

    // Begin the LIVE555 event loop:
    //*env << "before doEventLoop\n";
    env->taskScheduler().doEventLoop(); // does not return
    //*env << "after doEventLoop\n";

    //if (audioFormat >= 6)
        //AUDIO_AMR_Close(&amr_ptr);

    update_sw = 1;
    rtspServer->removeServerMediaSession(sms);
    rtspServer->removeServerMediaSession(sms_3gpp);
    rtspServer->removeServerMediaSession(sms_h264);
    rtspServer->removeServerMediaSession(sms_mj);
    Medium::close(MjpegInputDevice);
    Medium::close(Mpeg4InputDevice);
    Medium::close(Mpeg43gppInputDevice);
    Medium::close(rtspServer);
    //sleep(1);
    //ApproInterfaceExit();
    exit(1);
    delete scheduler;

    return 0; // only to prevent compiler warning
}
