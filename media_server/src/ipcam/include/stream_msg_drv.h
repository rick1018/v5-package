/* ===========================================================================
* @file ApproDrvMsg.h
*
* @path $(IPNCPATH)/include/
*
* @desc Message driver for Appro_interface
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#if defined (__cplusplus)
extern "C" {
#endif

#ifndef _APPRODRVMSG_
#define _APPRODRVMSG_

/**
  \file ApproDrvMsg.h

  \brief Message driver for Appro_interface
*/

#include <Msg_Def.h>

/**
  \defgroup AV_MSG_DRV Appro interface message driver datatypes, data structures, functions

  The section defines some common datatypes, data structures, callbacks which are used by all parts of the Appro interface framework.
  System init APIs are also defined in this section.

  \section AV_MSG_DRV_COMMON_DIR_STRUCT Directory Structure

  \subsection AV_MSG_DRV_COMMON_INTERFACE Interface

  \code
  ApproDrvMsg.h
  \endcode

  \subsection AV_MSG_DRV_COMMON_SOURCE Source

  \code
  ApproDrvMsg.c
  \endcode

  @{
*/
/* This function should be called at process innitial !!! */
void ApproDrv_SetProcId(int proc_id);
void ApproDrv_SaveQid(int iqid);

/* Only one ApproDrvInit is allowed in each process */
int ApproDrvInit(int proc_id,int chn);
int ApproDrvExit();

/* API */
MSG_MEM GetPhyMem();
FrameInfo_t GetCurrentFrame(FrameFormat_t fmt);
FrameInfo_t WaitNewFrame(FrameFormat_t fmt);
void SendQuitCmd();
int LockFrame(FrameInfo_t *pFrame);
int LockMpeg4IFrame(FrameInfo_t *pFrame,int fmt_type);
void UnlockFrame(FrameInfo_t *pFrame);
int GetVolInfo(FrameInfo_t *pFrame,int fmt_type);
int GetMediaInfo(FrameInfo_t *pFrame);
int SetDayNight(unsigned char nDayNight);
int SetWhiteBalance(unsigned char nValue);
int SetAvIrisMode(unsigned char nIrisMode);
#if BACKLIGHT_FEATURE
int SetBacklight(unsigned char nValue);
#endif
int SetBrightness(unsigned char nValue);
int SetContrast(unsigned char nValue);
int SetSaturation(unsigned char nValue);
int SetSharpness(unsigned char nValue);
int SetAACBitrate(int nValue);
int SetAudio_InVolume(unsigned char nValue);//Add by Joseph 20140516
int SetEncBitrate(unsigned char stream, int nValue);
int SetEncFramerate(unsigned char stream, int nValue);
#if GOP_FEATURE	//Add by Joseph 20131119
int SetM41Gop(int nValue);//Add by Joseph 20131118
#endif
int SetJpgQuality(int nValue);
#if ROTATION_FEATURE
int SetRot(int nValue);
#endif
#if FDETECT_FEATURE
int SetFDetect(int nValue);
#else
int SetOvDetect(int nValue);//RS,20091120,OV STATUS
#endif
int SetMirr(int nValue);
int SetOSDWindow(int nValue);
int SetImage2A(unsigned char nValue);
int SetImage2AType(unsigned char nValue);
int SetTvSystem(unsigned char nValue);
#if BINNING_FEATURE
int SetBinningSkip(unsigned char nValue);
#endif
int SetTStamp(unsigned char nValue);
#if IPNCPTZ_FEATURE
int SetPtz(PTZPrm *ptz);
#endif
int ApproSetMotion(ApproMotionPrm* pMotionPrm);
int SetStreamAdvFeature(unsigned char stream, unsigned char nValue);//RS,ADD
#if FDETECT_FEATURE
int SetAvFaceDetect(unsigned char nValue);
#endif
int SetVNFStatus(unsigned char stream, unsigned char nValue);
int SetROIConfig(unsigned char nValue);
int SetAvOsdTextEnable(int enable);
int SetHistEnable(int enable);
//int SetAvOsdLogoEnable(int enable);
int SetAvOsdLogoEnable(OSDPrm* pOSDPrm);//RS,20091231,MDY

int SetAvOsdText(char* strText, int nLength);
int Set2APriority(unsigned char nValue);
int SetDisplayValue(unsigned char nValue);
int SetOSDEnable(unsigned char nValue);
int SetStreamFramerate(unsigned char stream, unsigned int nValue);
int SendFaceDetectMsg(FaceDetectParam* faceParam);
int SetDateTimeFormat(DateTimePrm* datetimeParam);
int SetOSDPrmMsg(int id, OSDPrm* osdprm);
int SetCodecROIMsg(int id, CodecROIPrm* codecROIPrm);
int SetCodecAdvPrmMsg(int id, CodecAdvPrm* codecAdvPrm);
int SetClipSnapName(char* strText, int nLength);
int SetClipSnapLocation(unsigned char nValue);
int SendForceIFrameMsg(unsigned char nValue);
int SetAudioAlarmValue(unsigned char value);
int SetAudioAlarmFlag(unsigned char nValue);
int SetDynRangeParam(int enable);
int SetDynRangePrmMsg(DynRangePrm* dynRangePrm);
int ApproPollingMsg(void);
int SetLinWdr(unsigned char nValue);
int SetFlickDec(int nValue);
int SetWDTResetMsg(unsigned char nValue);
int SetAvIRCutStatus(unsigned char nValue); // kenny chiu 20110516

int SetAudioBroadcastPort(unsigned short port);
int SetEWdrMode(unsigned char value);

/* DMVA Message declarations */
int SendDmvaEnableMsg(DmvaParam* dmvaParam);
int SendDmvaExptAlgoFrameRateMsg(DmvaParam* dmvaParam);
int SendDmvaExptAlgoDetectFreqMsg(DmvaParam* dmvaParam);
int SendDmvaExptEvtRecordEnableMsg(DmvaParam* dmvaParam);
int SendDmvaExptSmetaTrackerEnableMsg(DmvaParam* dmvaParam);

int SendDmvaTZSetupMsg(DmvaTZParam* dmvaTZParam);
int SendDmvaMainSetupMsg(DmvaMainParam* dmvaMainParam);
int SendDmvaCTDSetupMsg(DmvaCTDParam* dmvaCTDParam);
int SendDmvaOCSetupMsg(DmvaOCParam* dmvaOCParam);
int SendDmvaSMETASetupMsg(DmvaSMETAParam* dmvaSMETAParam);
int SendDmvaIMDSetupMsg(DmvaIMDParam* dmvaIMDParam);

/* DMVA Enc Roi Enable */
int SendDmvaEncRoiEnableMsg(int *pEnable);
/* @} */

#endif

#if defined (__cplusplus)
}
#endif