/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file file_msg_drv.h
* @brief File message driver
*/
#ifndef _FILE_MSG_DRV_H_
#define _FILE_MSG_DRV_H_
#if defined (__cplusplus)
extern "C" {
#endif

#include <stdio.h>
#include <Msg_Def.h>
#include <sys_env_type.h>
#include <system_default.h>  

#define mtd1_size  	393216
#define env_offset	327680+4
#define env_size	  16380
#define search_size	16*1024
#define PORT_USED_LIST_CMD_GENERATE		"netstat -an > /tmp/UsedPortList"

/**
* @defgroup FILE_MSG_DRV File message driver
* @brief APIs to control file manager.

* This is the communication interface of file manager. Every process needs a
*  message ID (which is defined in @ref File_Msg_Def.h) to communicate with
* file manager. More detail please refer to (@ref FILE_MSG_DRV_HOW).
* @{
*/
/* This function should be called at process innitial !!! */
int InitFileMsgDrv(int key,int iProcId);
void CleanupFileMsgDrv();
/* API */
void SendFileQuitCmd();
SysInfo *GetSysInfo1(); // add by kenny chiu 20110627
SysInfo *GetSysInfo();
int SetSysInfo(int nShmIndex);
int GetSysLog(int nPageNum, int nItemNum, LogEntry_t * pBuf);
int fSetNetMask(struct in_addr netmask);
int fSetIp(struct in_addr ip);
int fSetHttpPort(unsigned short port);
int fSetTitle(void *buf, int len);
int fSetRegUsr(void *buf, int len);
int fSetOSDText(void *buf, int len);
int fSetGateway(struct in_addr gateway);
int fSetDhcpEnable(int enable);
int fSetFtpFqdn(void *buf, int len);
int fSetFtpPort(unsigned short port);
int fSetFtpUsername(void *buf, int len);
int fSetFtpPassword(void *buf, int len);
int fSetFtpFoldname(void *buf, int len);
int fSetFtpImageacount(int imageacount);
int fSetFtpPid(int pid);
int fSetSmtpServerIp(void * buf, int len);
int fSetSmtpServerPort(unsigned short port);
int fSetSmtpAuthentication(unsigned char authentication);
int fSetSmtpUsername(void * buf, int len);
int fSetSmtpPassword(void * buf, int len);
int fSetSmtpSenderEmail(void * buf, int len);
int fSetSmtpReceiverEmail(void * buf, int len);
int fSetSmtpCC(void * buf, int len);
int fSetSmtpSubject(void * buf, int len);
int fSetSmtpText(void * buf, int len);
int fSetSmtpAttachments(unsigned char attachments);
int fSetSmtpView(unsigned char view);
int fSetDns(struct in_addr ip);
int fSetDns2(struct in_addr ip);
int fSetSntpServer(void *buf, int len);
int fSetTimeFormat(unsigned char value);
int fSetDaylightTime(unsigned char value);
int fSetTimeZone(unsigned char value);
int fSetCamDayNight(unsigned char value);
int fSetCamWhiteBalance(unsigned char value);
int fSetCamBacklight(unsigned char value);
int fSetCamBrightness(unsigned char value);
int fSetCamContrast(unsigned char value);
int fSetCamSaturation(unsigned char value);
int fSetCamSharpness(unsigned char value);
int fSetClearSysLog(unsigned char value);
int fSetClearAccessLog(unsigned char value);
int fSetJpegQuality(unsigned char value);
int fSetMirror(unsigned char value);
int fSetAdvMode(unsigned char value);
int fSetM41SFeature(unsigned char value);
int fSetM42SFeature(unsigned char value);
int fSetJPGSFeature(unsigned char value);
//int fSetFaceDetect(unsigned char value);//RS ,DC
int fSetOvDetect(unsigned char value);//RS ,20091120,OV Status
//int fSetDemoCfg(unsigned char value);//RS ,DC
int fSetOSDWin(unsigned char value);
int fSetHistogram(unsigned char value);
int fSetDynRange(unsigned char value);
int fSetDynRangeStr(unsigned char value);
int fSetOSDWinNum(unsigned char value);
int fSetOSDStream(unsigned char value);
int fSetVideoCodec(unsigned char value);
int fSetVideoMode(unsigned char value);
int fSetVideoCodecMode(unsigned char value);
int fSetVideoCodecRes(unsigned char value);
int fSetImageFormat(unsigned char value);
int fSetResolution(unsigned char value);
int fSetMPEG4Res(unsigned char value);
int fSetMPEG42Res(unsigned char value);
int fSetMPEG4Quality(unsigned char value); 
int fSetSchedule(int index,Schedule_t* pSchedule, int day, int year);
//int fSetLostAlarm(unsigned char value);//RS ,DC
//int fSetSDAlarmEnable(unsigned char value);//RS ,DC - SD_FEATURE
int fSetFTPAlarmEnable(unsigned char value);
int fSetSMTPAlarmEnable(unsigned char value);
int fSetAlarmDuration(unsigned char value);
int fSetImageSource(unsigned char value);
int fSetTVcable(unsigned char value);
//int fSetBinning(unsigned char value);//RS ,DC
int fSetBlc(unsigned char value);
int fSetMP41bitrate(int value);
int fSetMP42bitrate(int value);
int fSetEncFramerate(unsigned char stream, unsigned char value);
int fSetRateControl(unsigned char value);
int fSetAVIDuration(unsigned char value);
int fSetAVIFormat(unsigned char value);
int fSetSntpFqdn(unsigned char value);  
int fSetFTPFileFormat(unsigned char value);
//int fSetSDFileFormat(unsigned char value);//RS ,DC
int fSetAttFileFormat(unsigned char value);
int fSetAudioEnable(unsigned char value);
int fSetASmtpAttach(unsigned char value);
int fSetRftpenable(unsigned char value);
//int fSetSdReEnable(unsigned char value);//RS ,DC
int fSetImageAEW(unsigned char value);
int fSetImageAEWType(unsigned char value);
int fSetVideoSize(unsigned char stream, int Xvalue, int Yvalue);
int fSetStreamActive(unsigned char stream1, unsigned char stream2, unsigned char stream3,
                     unsigned char stream4, unsigned char stream5, unsigned char stream6);
int fSetMotionEnable(unsigned char value);
//int fSetMotionCEnable(unsigned char value);//RS ,DC
//int fSetMotionLevel(unsigned char value);//RS ,DC
//int fSetMotionCValue(unsigned char value);//RS ,DC
int fSetMotionBlock(char *pValue,int len);
//int fSetSDInsert(unsigned char value);//RS ,DC
int fSetAcount(int index, Acount_t *acout);
int fDelAcount(int index, Acount_t *acout);
int fSetIpncPtz(unsigned char value);
int fSetGIOInEnable(unsigned char value);
int fSetGIOInType(unsigned char value);
int fSetGIOOutEnable(unsigned char value);
int fSetGIOOutType(unsigned char value);
int fSetTStampEnable(unsigned char value);
int fSetTStampFormat(unsigned char value);
int fSetAudioInVolume(unsigned char value);
int fSetTAudioinVolume(unsigned char value);
int fSetMobileFramerate(unsigned char value);
int fSetMobileRateControl(unsigned char value);
int fSetMobileQuality(unsigned char value);
int fSetMobileBitrate(int value);
int fSetAudioOutVolume(unsigned char value);
int fResetSysInfo();
int fResetLogInfo(); 
int fSetSysLog(LogEntry_t* value);
int fContinueSetSysLog(LogEntry_t* value);//by Larry
int fSetAlarmStatus(unsigned short value);
int fSetPtzSupport(unsigned char value);
int fSetNetMulticast(unsigned char value);
int fSetVideoCodecCombo(unsigned char value);
int fSetVideoMode(unsigned char value);
int fSetStreamConfig(unsigned char stream, int width, int height, char *name, int portnum);
int fSetLocalDisplay(unsigned char value);
int fSetFramerate1(unsigned char value);
int fSetFramerate2(unsigned char value);
int fSetFramerate3(unsigned char value);
int fSetFramerateVal1(unsigned int value);
int fSetFramerateVal2(unsigned int value);
int fSetFramerateVal3(unsigned int value);
int fSetMaxExposureValue(unsigned char value);
int fSetMaxGainValue(unsigned char value);
int fSetVstabValue(unsigned char value);
int fSetLdcValue(unsigned char value);
int fSetVnfValue(unsigned char value);
int fSetVnfMode(unsigned char value);
int fSetVnfStrength(unsigned char value);
int fSetAlarmAudioPlay(unsigned char value);
int fSetAudioON(unsigned char value);
int fSetAudioMode(unsigned char value);
int fSetAudioEncode(unsigned char value);
int fSetAudioSampleRate(unsigned char value);
int fSetAudioBitrate(unsigned char value);
int fSetAudioBitrateValue(int value);
int fSetAudioAlarmLevel(unsigned char value);
int fSetFDX(unsigned int value);
int fSetFDY(unsigned int value);
int fSetFDW(unsigned int value);
int fSetFDH(unsigned int value);
int fSetFDConfLevel(unsigned char value);
int fSetFDDirection(unsigned char value);
int fSetFRecognition(unsigned char value);
int fSetFRConfLevel(unsigned char value);
int fSetFRDatabase(unsigned char value);
int fSetPrivacyMask(unsigned char value);
int fSetMaskOptions(unsigned char value);
int fSetClickSnapFilename(void *buf, int len);
int fSetClickSnapStorage(unsigned char value);
int fSetExpPriority(unsigned char value);
int fSetOSDWinNum(unsigned char value);
int fSetRateControl1(unsigned char value);
int fSetRateControl2(unsigned char value);
int fSetDateFormat(unsigned char value);
int fSetDatePosition(unsigned char value);
int fSetTimePosition(unsigned char value);
int fSetDateStampEnable1(unsigned char value);
int fSetDateStampEnable2(unsigned char value);
int fSetDateStampEnable3(unsigned char value);
int fSetTimeStampEnable1(unsigned char value);
int fSetTimeStampEnable2(unsigned char value);
int fSetTimeStampEnable3(unsigned char value);
int fSetLogoEnable1(unsigned char value);
int fSetLogoEnable2(unsigned char value);
int fSetLogoEnable3(unsigned char value);
int fSetLogoPosition1(unsigned char value);
int fSetLogoPosition2(unsigned char value);
int fSetLogoPosition3(unsigned char value);
int fSetTextPosition1(unsigned char value);
int fSetTextPosition2(unsigned char value);
int fSetTextPosition3(unsigned char value);
int fSetTextEnable1(unsigned char value);
int fSetTextEnable2(unsigned char value);
int fSetTextEnable3(unsigned char value);
int fSetOverlayText1(void *buf, int len);
int fSetOverlayText2(void *buf, int len);
int fSetOverlayText3(void *buf, int len);
int fSetDetailInfo1(unsigned char value);
int fSetDetailInfo2(unsigned char value);
int fSetDetailInfo3(unsigned char value);
int fSetEncryptVideo(unsigned char value);
int fSetROIEnable1(unsigned char value);
int fSetROIEnable2(unsigned char value);
int fSetROIEnable3(unsigned char value);
int fSetStr1X1(unsigned int value);
int fSetStr1Y1(unsigned int value);
int fSetStr1W1(unsigned int value);
int fSetStr1H1(unsigned int value);
int fSetStr1X2(unsigned int value);
int fSetStr1Y2(unsigned int value);
int fSetStr1W2(unsigned int value);
int fSetStr1H2(unsigned int value);
int fSetStr1X3(unsigned int value);
int fSetStr1Y3(unsigned int value);
int fSetStr1W3(unsigned int value);
int fSetStr1H3(unsigned int value);
int fSetStr2X1(unsigned int value);
int fSetStr2Y1(unsigned int value);
int fSetStr2W1(unsigned int value);
int fSetStr2H1(unsigned int value);
int fSetStr2X2(unsigned int value);
int fSetStr2Y2(unsigned int value);
int fSetStr2W2(unsigned int value);
int fSetStr2H2(unsigned int value);
int fSetStr2X3(unsigned int value);
int fSetStr2Y3(unsigned int value);
int fSetStr2W3(unsigned int value);
int fSetStr2H3(unsigned int value);
int fSetStr3X1(unsigned int value);
int fSetStr3Y1(unsigned int value);
int fSetStr3W1(unsigned int value);
int fSetStr3H1(unsigned int value);
int fSetStr3X2(unsigned int value);
int fSetStr3Y2(unsigned int value);
int fSetStr3W2(unsigned int value);
int fSetStr3H2(unsigned int value);
int fSetStr3X3(unsigned int value);
int fSetStr3Y3(unsigned int value);
int fSetStr3W3(unsigned int value);
int fSetStr3H3(unsigned int value);
int fSetQPMax1(unsigned char value);
int fSetQPMax2(unsigned char value);
int fSetQPMax3(unsigned char value);
int fSetMEConfig1(unsigned char value);
int fSetMEConfig2(unsigned char value);
int fSetMEConfig3(unsigned char value);
int fSetPacketSize1(unsigned char value);
int fSetPacketSize2(unsigned char value);
int fSetPacketSize3(unsigned char value);
int fSetIpratio1(unsigned int value);
int fSetIpratio2(unsigned int value);
int fSetIpratio3(unsigned int value);
int fSetForceIframe1(unsigned char value);
int fSetForceIframe2(unsigned char value);
int fSetForceIframe3(unsigned char value);
int fSetQPInit1(unsigned char value);
int fSetQPInit2(unsigned char value);
int fSetQPInit3(unsigned char value);
int fSetQPMin1(unsigned char value);
int fSetQPMin2(unsigned char value);
int fSetQPMin3(unsigned char value);
int fSetAlarmEnable(unsigned char value);
int fSetExtAlarm(unsigned char value);
int fSetAudioAlarm(unsigned char value);
int fSetAlarmAudioFile(unsigned char value);
int fSetScheduleRepeatEnable(unsigned char value);
int fSetScheduleNumWeeks(unsigned char value);
int fSetScheduleInfiniteEnable(unsigned char value);
int fSetAlarmStorage(unsigned char value);
int fSetRecordStorage(unsigned char value);
int fSetHttpsPort(unsigned short value);
int fSetRS485Port(unsigned short value);
int fSetChipConfig(unsigned char value);
void fResetCodecROIParam(int stream, int width, int height);
void fResetFDROIParam(int stream, int width, int height);

/* DMVA specific changes */
int GetDmvaSysLog(int nPageNum, int nItemNum, DmvaLogEntry_t * pBuf);
int fSetDmvaSysLog(DmvaLogEntry_t* value);
int fSetDmvaEnable(unsigned int value);
int fSetDmvaExptAlgoFrameRate(unsigned int value);
int fSetDmvaExptAlgoDetectFreq(unsigned int value);
int fSetDmvaExptEvtRecordEnable(unsigned int value);
int fSetDmvaExptSmetaTrackerEnable(unsigned int value);
/* DMVA TZ specific parameters */
int fSetDmvaTZSensitivity(unsigned int value);
int fSetDmvaTZPersonMinWidth(unsigned int value);
int fSetDmvaTZPersonMinHeight(unsigned int value);
int fSetDmvaTZVehicleMinWidth(unsigned int value);
int fSetDmvaTZVehicleMinHeight(unsigned int value);
int fSetDmvaTZPersonMaxWidth(unsigned int value);
int fSetDmvaTZPersonMaxHeight(unsigned int value);
int fSetDmvaTZVehicleMaxWidth(unsigned int value);
int fSetDmvaTZVehicleMaxHeight(unsigned int value);
int fSetDmvaTZDirection(unsigned int value);
int fSetDmvaTZPresentAdjust(unsigned int value);
int fSetDmvaTZEnable(unsigned int value);
int fSetDmvaTZZone1Numsides(unsigned int value);
int fSetDmvaTZZone2Numsides(unsigned int value);
int fSetDmvaTZZone1_x01(unsigned int value);
int fSetDmvaTZZone1_x02(unsigned int value);
int fSetDmvaTZZone1_x03(unsigned int value);
int fSetDmvaTZZone1_x04(unsigned int value);
int fSetDmvaTZZone1_x05(unsigned int value);
int fSetDmvaTZZone1_x06(unsigned int value);
int fSetDmvaTZZone1_x07(unsigned int value);
int fSetDmvaTZZone1_x08(unsigned int value);
int fSetDmvaTZZone1_x09(unsigned int value);
int fSetDmvaTZZone1_x10(unsigned int value);
int fSetDmvaTZZone1_x11(unsigned int value);
int fSetDmvaTZZone1_x12(unsigned int value);
int fSetDmvaTZZone1_x13(unsigned int value);
int fSetDmvaTZZone1_x14(unsigned int value);
int fSetDmvaTZZone1_x15(unsigned int value);
int fSetDmvaTZZone1_x16(unsigned int value);
int fSetDmvaTZZone1_y01(unsigned int value);
int fSetDmvaTZZone1_y02(unsigned int value);
int fSetDmvaTZZone1_y03(unsigned int value);
int fSetDmvaTZZone1_y04(unsigned int value);
int fSetDmvaTZZone1_y05(unsigned int value);
int fSetDmvaTZZone1_y06(unsigned int value);
int fSetDmvaTZZone1_y07(unsigned int value);
int fSetDmvaTZZone1_y08(unsigned int value);
int fSetDmvaTZZone1_y09(unsigned int value);
int fSetDmvaTZZone1_y10(unsigned int value);
int fSetDmvaTZZone1_y11(unsigned int value);
int fSetDmvaTZZone1_y12(unsigned int value);
int fSetDmvaTZZone1_y13(unsigned int value);
int fSetDmvaTZZone1_y14(unsigned int value);
int fSetDmvaTZZone1_y15(unsigned int value);
int fSetDmvaTZZone1_y16(unsigned int value);
int fSetDmvaTZZone2_x01(unsigned int value);
int fSetDmvaTZZone2_x02(unsigned int value);
int fSetDmvaTZZone2_x03(unsigned int value);
int fSetDmvaTZZone2_x04(unsigned int value);
int fSetDmvaTZZone2_x05(unsigned int value);
int fSetDmvaTZZone2_x06(unsigned int value);
int fSetDmvaTZZone2_x07(unsigned int value);
int fSetDmvaTZZone2_x08(unsigned int value);
int fSetDmvaTZZone2_x09(unsigned int value);
int fSetDmvaTZZone2_x10(unsigned int value);
int fSetDmvaTZZone2_x11(unsigned int value);
int fSetDmvaTZZone2_x12(unsigned int value);
int fSetDmvaTZZone2_x13(unsigned int value);
int fSetDmvaTZZone2_x14(unsigned int value);
int fSetDmvaTZZone2_x15(unsigned int value);
int fSetDmvaTZZone2_x16(unsigned int value);
int fSetDmvaTZZone2_y01(unsigned int value);
int fSetDmvaTZZone2_y02(unsigned int value);
int fSetDmvaTZZone2_y03(unsigned int value);
int fSetDmvaTZZone2_y04(unsigned int value);
int fSetDmvaTZZone2_y05(unsigned int value);
int fSetDmvaTZZone2_y06(unsigned int value);
int fSetDmvaTZZone2_y07(unsigned int value);
int fSetDmvaTZZone2_y08(unsigned int value);
int fSetDmvaTZZone2_y09(unsigned int value);
int fSetDmvaTZZone2_y10(unsigned int value);
int fSetDmvaTZZone2_y11(unsigned int value);
int fSetDmvaTZZone2_y12(unsigned int value);
int fSetDmvaTZZone2_y13(unsigned int value);
int fSetDmvaTZZone2_y14(unsigned int value);
int fSetDmvaTZZone2_y15(unsigned int value);
int fSetDmvaTZZone2_y16(unsigned int value);

int fSetDmvaTZSave(void * buf, int len);
int fSetDmvaTZLoad(unsigned int value);

/* Set DMVA Main Page specific Parameters */
int fSetDmvaCamID(unsigned int value);
int fSetDmvaGetSchedule(unsigned int value);
int fSetDmvaCfgFD(unsigned int value);
int fSetDmvaCfgCTDImdSmetaTzOc(unsigned int value);
int fSetDmvaDisplayOptions(unsigned int value);
int fSetDmvaEventRecordingVAME(unsigned int value);
int fSetDmvaEventListActionPlaySendSearchTrash(unsigned int value);
int fSetDmvaEventListSelectEvent(unsigned int value);
int fSetDmvaEventListArchiveFlag(unsigned int value);
int fSetDmvaEventListEventType(unsigned int value);

int fSetDmvaEventListStartMonth(unsigned int value);
int fSetDmvaEventListStartDay(unsigned int value);
int fSetDmvaEventListStartTime(unsigned int value);
int fSetDmvaEventListEndMonth(unsigned int value);
int fSetDmvaEventListEndDay(unsigned int value);
int fSetDmvaEventListEndTime(unsigned int value);
int fSetDmvaEventListTimeStamp(void * buf, int length);
int fSetDmvaEventListEventDetails(void * buf, int length);

/* Set DMVA Advanced Settings Page specific Parameters */
int fSetDmvaObjectMetaData(unsigned int value);
int fSetDmvaOverlayPropertiesFontType(unsigned int value);
int fSetDmvaOverlayPropertiesFontSize(unsigned int value);
int fSetDmvaOverlayPropertiesFontColor(unsigned int value);
int fSetDmvaOverlayPropertiesBoxColor(unsigned int value);
int fSetDmvaOverlayPropertiesCentroidColor(unsigned int value);
int fSetDmvaOverlayPropertiesActiveZoneColor(unsigned int value);
int fSetDmvaOverlayPropertiesInactiveZoneColor(unsigned int value);
int fSetDmvaOverlayPropertiesVelocityColor(unsigned int value);
int fSetDmvaEnvironmentProfileLoad(unsigned int value);
int fSetDmvaEnvironmentProfileStore(unsigned int value);
int fSetDmvaEnvironmentProfileClear(unsigned int value);
int fSetDmvaEnvironmentProfileState(unsigned int value);
int fSetDmvaEnvironmentProfileDesc(void * buf, int length);

/* Set DMVA event delete specific Parameters */
int fSetDmvaEventDeleteIndex(unsigned int value);
int fSetDmvaEventDeleteStartIndex(unsigned int value);
int fSetDmvaEventDeleteEndIndex(unsigned int value);
int fSetDmvaEventDeleteAll(unsigned int value);

/* Set DMVA CTD Page specific Parameters */
int fSetDmvaCfgTDEnv1(unsigned int value);
int fSetDmvaCfgTDEnv2(unsigned int value);
int fSetDmvaCfgTDSensitivity(unsigned int value);
int fSetDmvaCfgTDResetTime(unsigned int value);
/* Set DMVA OC Page specific Parameters */
int fSetDmvaOCSensitivity(unsigned int value);
int fSetDmvaOCObjectWidth(unsigned int value);
int fSetDmvaOCObjectHeight(unsigned int value);
int fSetDmvaOCDirection(unsigned int value);
int fSetDmvaOCEnable(unsigned int value);
int fSetDmvaOCLeftRight(unsigned int value);
int fSetDmvaOCTopBottom(unsigned int value);
int fSetDmvaOCSave(void * buf, int len);
int fSetDmvaOCLoad(unsigned int value);
/* Set DMVA SMETA Page specific Parameters */
int fSetDmvaSMETASensitivity(unsigned int value);
int fSetDmvaSMETAEnableStreamingData(unsigned int value);
int fSetDmvaSMETAStreamBB(unsigned int value);
int fSetDmvaSMETAStreamVelocity(unsigned int value);
int fSetDmvaSMETAStreamHistogram(unsigned int value);
int fSetDmvaSMETAStreamMoments(unsigned int value);
int fSetDmvaSMETASensitivity(unsigned int value);
int fSetDmvaSMETAPresentAdjust(unsigned int value);
int fSetDmvaSMETAPersonMinWidth(unsigned int value);
int fSetDmvaSMETAPersonMinHeight(unsigned int value);
int fSetDmvaSMETAVehicleMinWidth(unsigned int value);
int fSetDmvaSMETAVehicleMinHeight(unsigned int value);
int fSetDmvaSMETAPersonMaxWidth(unsigned int value);
int fSetDmvaSMETAPersonMaxHeight(unsigned int value);
int fSetDmvaSMETAVehicleMaxWidth(unsigned int value);
int fSetDmvaSMETAVehicleMaxHeight(unsigned int value);
int fSetDmvaSMETANumZones(unsigned int value);
int fSetDmvaSMETAZone1Numsides(unsigned int value);
int fSetDmvaSMETAZone1_x01(unsigned int value);
int fSetDmvaSMETAZone1_x02(unsigned int value);
int fSetDmvaSMETAZone1_x03(unsigned int value);
int fSetDmvaSMETAZone1_x04(unsigned int value);
int fSetDmvaSMETAZone1_x05(unsigned int value);
int fSetDmvaSMETAZone1_x06(unsigned int value);
int fSetDmvaSMETAZone1_x07(unsigned int value);
int fSetDmvaSMETAZone1_x08(unsigned int value);
int fSetDmvaSMETAZone1_x09(unsigned int value);
int fSetDmvaSMETAZone1_x10(unsigned int value);
int fSetDmvaSMETAZone1_x11(unsigned int value);
int fSetDmvaSMETAZone1_x12(unsigned int value);
int fSetDmvaSMETAZone1_x13(unsigned int value);
int fSetDmvaSMETAZone1_x14(unsigned int value);
int fSetDmvaSMETAZone1_x15(unsigned int value);
int fSetDmvaSMETAZone1_x16(unsigned int value);
int fSetDmvaSMETAZone1_y01(unsigned int value);
int fSetDmvaSMETAZone1_y02(unsigned int value);
int fSetDmvaSMETAZone1_y03(unsigned int value);
int fSetDmvaSMETAZone1_y04(unsigned int value);
int fSetDmvaSMETAZone1_y05(unsigned int value);
int fSetDmvaSMETAZone1_y06(unsigned int value);
int fSetDmvaSMETAZone1_y07(unsigned int value);
int fSetDmvaSMETAZone1_y08(unsigned int value);
int fSetDmvaSMETAZone1_y09(unsigned int value);
int fSetDmvaSMETAZone1_y10(unsigned int value);
int fSetDmvaSMETAZone1_y11(unsigned int value);
int fSetDmvaSMETAZone1_y12(unsigned int value);
int fSetDmvaSMETAZone1_y13(unsigned int value);
int fSetDmvaSMETAZone1_y14(unsigned int value);
int fSetDmvaSMETAZone1_y15(unsigned int value);
int fSetDmvaSMETAZone1_y16(unsigned int value);
int fSetDmvaSMETAZone1_Label(void * buf, int length);
int fSetDmvaSMETASave(void * buf, int len);
int fSetDmvaSMETALoad(unsigned int value);
/* Set DMVA IMD Page specific Parameters */
int fSetDmvaIMDSensitivity(unsigned int value);
int fSetDmvaIMDPresentAdjust(unsigned int value);
int fSetDmvaIMDPersonMinWidth(unsigned int value);
int fSetDmvaIMDPersonMinHeight(unsigned int value);
int fSetDmvaIMDVehicleMinWidth(unsigned int value);
int fSetDmvaIMDVehicleMinHeight(unsigned int value);
int fSetDmvaIMDPersonMaxWidth(unsigned int value);
int fSetDmvaIMDPersonMaxHeight(unsigned int value);
int fSetDmvaIMDVehicleMaxWidth(unsigned int value);
int fSetDmvaIMDVehicleMaxHeight(unsigned int value);
int fSetDmvaIMDNumZones(unsigned int value);
int fSetDmvaIMDZoneEnable(unsigned int value);

int fSetDmvaIMDZone1Numsides(unsigned int value);
int fSetDmvaIMDZone2Numsides(unsigned int value);
int fSetDmvaIMDZone3Numsides(unsigned int value);
int fSetDmvaIMDZone4Numsides(unsigned int value);
int fSetDmvaIMDZone1_x01(unsigned int value);
int fSetDmvaIMDZone1_x02(unsigned int value);
int fSetDmvaIMDZone1_x03(unsigned int value);
int fSetDmvaIMDZone1_x04(unsigned int value);
int fSetDmvaIMDZone1_x05(unsigned int value);
int fSetDmvaIMDZone1_x06(unsigned int value);
int fSetDmvaIMDZone1_x07(unsigned int value);
int fSetDmvaIMDZone1_x08(unsigned int value);
int fSetDmvaIMDZone1_x09(unsigned int value);
int fSetDmvaIMDZone1_x10(unsigned int value);
int fSetDmvaIMDZone1_x11(unsigned int value);
int fSetDmvaIMDZone1_x12(unsigned int value);
int fSetDmvaIMDZone1_x13(unsigned int value);
int fSetDmvaIMDZone1_x14(unsigned int value);
int fSetDmvaIMDZone1_x15(unsigned int value);
int fSetDmvaIMDZone1_x16(unsigned int value);
int fSetDmvaIMDZone1_y01(unsigned int value);
int fSetDmvaIMDZone1_y02(unsigned int value);
int fSetDmvaIMDZone1_y03(unsigned int value);
int fSetDmvaIMDZone1_y04(unsigned int value);
int fSetDmvaIMDZone1_y05(unsigned int value);
int fSetDmvaIMDZone1_y06(unsigned int value);
int fSetDmvaIMDZone1_y07(unsigned int value);
int fSetDmvaIMDZone1_y08(unsigned int value);
int fSetDmvaIMDZone1_y09(unsigned int value);
int fSetDmvaIMDZone1_y10(unsigned int value);
int fSetDmvaIMDZone1_y11(unsigned int value);
int fSetDmvaIMDZone1_y12(unsigned int value);
int fSetDmvaIMDZone1_y13(unsigned int value);
int fSetDmvaIMDZone1_y14(unsigned int value);
int fSetDmvaIMDZone1_y15(unsigned int value);
int fSetDmvaIMDZone1_y16(unsigned int value);
int fSetDmvaIMDZone2_x01(unsigned int value);
int fSetDmvaIMDZone2_x02(unsigned int value);
int fSetDmvaIMDZone2_x03(unsigned int value);
int fSetDmvaIMDZone2_x04(unsigned int value);
int fSetDmvaIMDZone2_x05(unsigned int value);
int fSetDmvaIMDZone2_x06(unsigned int value);
int fSetDmvaIMDZone2_x07(unsigned int value);
int fSetDmvaIMDZone2_x08(unsigned int value);
int fSetDmvaIMDZone2_x09(unsigned int value);
int fSetDmvaIMDZone2_x10(unsigned int value);
int fSetDmvaIMDZone2_x11(unsigned int value);
int fSetDmvaIMDZone2_x12(unsigned int value);
int fSetDmvaIMDZone2_x13(unsigned int value);
int fSetDmvaIMDZone2_x14(unsigned int value);
int fSetDmvaIMDZone2_x15(unsigned int value);
int fSetDmvaIMDZone2_x16(unsigned int value);
int fSetDmvaIMDZone2_y01(unsigned int value);
int fSetDmvaIMDZone2_y02(unsigned int value);
int fSetDmvaIMDZone2_y03(unsigned int value);
int fSetDmvaIMDZone2_y04(unsigned int value);
int fSetDmvaIMDZone2_y05(unsigned int value);
int fSetDmvaIMDZone2_y06(unsigned int value);
int fSetDmvaIMDZone2_y07(unsigned int value);
int fSetDmvaIMDZone2_y08(unsigned int value);
int fSetDmvaIMDZone2_y09(unsigned int value);
int fSetDmvaIMDZone2_y10(unsigned int value);
int fSetDmvaIMDZone2_y11(unsigned int value);
int fSetDmvaIMDZone2_y12(unsigned int value);
int fSetDmvaIMDZone2_y13(unsigned int value);
int fSetDmvaIMDZone2_y14(unsigned int value);
int fSetDmvaIMDZone2_y15(unsigned int value);
int fSetDmvaIMDZone2_y16(unsigned int value);

int fSetDmvaIMDZone3_x01(unsigned int value);
int fSetDmvaIMDZone3_x02(unsigned int value);
int fSetDmvaIMDZone3_x03(unsigned int value);
int fSetDmvaIMDZone3_x04(unsigned int value);
int fSetDmvaIMDZone3_x05(unsigned int value);
int fSetDmvaIMDZone3_x06(unsigned int value);
int fSetDmvaIMDZone3_x07(unsigned int value);
int fSetDmvaIMDZone3_x08(unsigned int value);
int fSetDmvaIMDZone3_x09(unsigned int value);
int fSetDmvaIMDZone3_x10(unsigned int value);
int fSetDmvaIMDZone3_x11(unsigned int value);
int fSetDmvaIMDZone3_x12(unsigned int value);
int fSetDmvaIMDZone3_x13(unsigned int value);
int fSetDmvaIMDZone3_x14(unsigned int value);
int fSetDmvaIMDZone3_x15(unsigned int value);
int fSetDmvaIMDZone3_x16(unsigned int value);
int fSetDmvaIMDZone3_y01(unsigned int value);
int fSetDmvaIMDZone3_y02(unsigned int value);
int fSetDmvaIMDZone3_y03(unsigned int value);
int fSetDmvaIMDZone3_y04(unsigned int value);
int fSetDmvaIMDZone3_y05(unsigned int value);
int fSetDmvaIMDZone3_y06(unsigned int value);
int fSetDmvaIMDZone3_y07(unsigned int value);
int fSetDmvaIMDZone3_y08(unsigned int value);
int fSetDmvaIMDZone3_y09(unsigned int value);
int fSetDmvaIMDZone3_y10(unsigned int value);
int fSetDmvaIMDZone3_y11(unsigned int value);
int fSetDmvaIMDZone3_y12(unsigned int value);
int fSetDmvaIMDZone3_y13(unsigned int value);
int fSetDmvaIMDZone3_y14(unsigned int value);
int fSetDmvaIMDZone3_y15(unsigned int value);
int fSetDmvaIMDZone3_y16(unsigned int value);
int fSetDmvaIMDZone4_x01(unsigned int value);
int fSetDmvaIMDZone4_x02(unsigned int value);
int fSetDmvaIMDZone4_x03(unsigned int value);
int fSetDmvaIMDZone4_x04(unsigned int value);
int fSetDmvaIMDZone4_x05(unsigned int value);
int fSetDmvaIMDZone4_x06(unsigned int value);
int fSetDmvaIMDZone4_x07(unsigned int value);
int fSetDmvaIMDZone4_x08(unsigned int value);
int fSetDmvaIMDZone4_x09(unsigned int value);
int fSetDmvaIMDZone4_x10(unsigned int value);
int fSetDmvaIMDZone4_x11(unsigned int value);
int fSetDmvaIMDZone4_x12(unsigned int value);
int fSetDmvaIMDZone4_x13(unsigned int value);
int fSetDmvaIMDZone4_x14(unsigned int value);
int fSetDmvaIMDZone4_x15(unsigned int value);
int fSetDmvaIMDZone4_x16(unsigned int value);
int fSetDmvaIMDZone4_y01(unsigned int value);
int fSetDmvaIMDZone4_y02(unsigned int value);
int fSetDmvaIMDZone4_y03(unsigned int value);
int fSetDmvaIMDZone4_y04(unsigned int value);
int fSetDmvaIMDZone4_y05(unsigned int value);
int fSetDmvaIMDZone4_y06(unsigned int value);
int fSetDmvaIMDZone4_y07(unsigned int value);
int fSetDmvaIMDZone4_y08(unsigned int value);
int fSetDmvaIMDZone4_y09(unsigned int value);
int fSetDmvaIMDZone4_y10(unsigned int value);
int fSetDmvaIMDZone4_y11(unsigned int value);
int fSetDmvaIMDZone4_y12(unsigned int value);
int fSetDmvaIMDZone4_y13(unsigned int value);
int fSetDmvaIMDZone4_y14(unsigned int value);
int fSetDmvaIMDZone4_y15(unsigned int value);
int fSetDmvaIMDZone4_y16(unsigned int value);
int fSetDmvaIMDZone1_Label(void * buf, int length);
int fSetDmvaIMDZone2_Label(void * buf, int length);
int fSetDmvaIMDZone3_Label(void * buf, int length);
int fSetDmvaIMDZone4_Label(void * buf, int length);

int fSetDmvaIMDSave(void * buf, int len);
int fSetDmvaIMDLoad(unsigned int value);

/* DMVA Scheduler */
int fSetDmvaSchEnable(unsigned int value);
int fSetDmvaSchStart(unsigned int value);
int fSetDmvaSchCheckAddRule(DMVA_schedule_t *pDmvaSchedule);
int fSetDmvaSchDelRule(void *buf, int len);

/* DMVA Enc Roi Enable */
int fSetDmvaEncRoiEnable(unsigned int value);

int fSetAudioReceiverEnable(unsigned char value);
int fSetAudioSeverIp(void *buf, int len);
int fSetLinearWdr(unsigned char value);
int fSetFlickerDetect(unsigned int value);
int fSetAcount(int index, Acount_t *acout);
int fsetmediaprofile(int index,media_profile *profile);
int faddvideosourceconfiguration(int index,video_source_conf *Vsource);
int fsetvideosourceconfiguration(video_source_conf *SetVsource);
int faddvideoencoderconfiguration(video_encoder_conf *Vencoder);
int fsetvideoencoderconfiguration(video_encoder_conf *SetVencoder);
int fdelprofile(int index);
int fsetdiscoverymode(unsigned char mode);
int faddscopes(Onvif_Scopes_t onvif_scopes_tmp);
int fdelscopes(Onvif_Scopes_t onvif_scopes_tmp);
int fsetvideoanalytics(video_analytics_conf video_analytics_tmp);
int fsetvideooutput(video_output_conf video_output_tmp);
int fsetrelay(relay_conf relay_tmp);
int fsethostname(hostname_tmp *host);
int fSetSWWatchDogEnable(char flag);

int CheckTestFunctionModeTest(unsigned char value);
int CheckIPv4Format(char *ipv4_addr);
int GetMtdSNandPID(char *sn, char *pid, char *hdVer, int sn_size, int pid_size, int hd_size); //by Larry
#if 0 //by Larry
int Get_IPV6Addr(char *ipv6_addr);
#else
int CheckEthIPV6Addr(char *ipv6_global_addr, char *ipv6_link_addr, int size);
int Get_IPV6Addr(char *ipv6_addr, int size);
#endif
int netsplit(char *pAddress, void *ip);
int ipv4_str_to_num(char *data, struct in_addr *ipaddr);
Motion_Str_Data *GetMDData(char *md_data);

Extra_Info *GetExtraInfo(void);
Extra_Info2 *GetExtraInfo2(void);
int net_get_hwaddr_util(char *ifname, unsigned char *mac);
void get_kernelversion(void);
int extra_info_init(void);
int extra_info2_init(void); // add by kenny chiu 20131120 for Extra_Info2
//alice_modify_2012_3_15_v1
#if WIFI_FEATURE
int SemWifiInit(void);
int ShmWifiLock(int locknum);
int ShmWifiUnLock(int locknum);
int ShareMemsWifiInfoInit(void);
int ShmWifiInfoRead (wifi_wps_data_t * wifi_wps_data);
int ShmWifiInfoWrite (wifi_wps_data_t * wifi_wps_data);
int ShareMemWifiInfoDataInit(void);
#endif // WIFI_FEATURE
//alice_modify_2012_3_15_v1

int fExtraFileReadCheck(void);
int fReadExtraInfo(void);
int fExtraFileWriteCheck(unsigned int field, char *data);
int fWriteExtraInfo(unsigned int field, char *data);
int fResetExtraInfo(void);
void ExtreBufLocalCopy(Extra_Info *buf, int mode); //1:local-->curr   2:curr-->local
// add by kenny chiu 20131120 for Extra_Info2 start
int fExtraFile2ReadCheck(void);
int fReadExtraInfo2(void);
int fExtraFile2WriteCheck(unsigned int field, char *data);
int fWriteExtraInfo2(unsigned int field, char *data);
int fResetExtraInfo2(void);
// add by kenny chiu 20131120 for Extra_Info2 end

ThreeGPPInfo *GetThrGPPInfo(void);
int three_gpp_info_init(void);
int fReadThrGPPInfo(void);
int fWriteThrGPPInfo(void);
int fResetThrGppInfo(void);
int user_info_init(void);

int FileMagMutexInit(void);
int FileMagMutexDestroy(void);
int fSetIrisMode(unsigned char value); // kenny chiu 20110512
int fSetUpnpPortMapping(AUTO_UPNP_MAPPING_INFO *auto_portmapping);
int SetMJPGStatus(unsigned char value);
/* SD Card Mount - Dean - 20121213 - Begin */
long long GetDiskfreeSpace(DISK_MEDIA dMedia);
long long GetDiskTotalSpace(DISK_MEDIA dMedia);			/* Record Media - Dean - 20130530 */
int SDCard_SaveFlag(int iFlag);
int SDCard_CheckPartitionTable(char bSaveFlag);
int SDCard_SearchPartitionTable(int* ppath_id, int* pmajor_id, int* pminor_id, int* pszblocks);
int SDCard_CheckMountStatus(void);
int SDCard_Mount(int path_id, int major_id, int minor_id, int szblocks);
int SDCard_UnMount(int bIsFormat);
int SDCard_CheckAVIFileBusy(char* dirname, char* Filename);
/* SD Card Mount - Dean - 20121213 - End */
void UmountUSB(void);

/* DB - Dean - 20121213 - Begin */
int DB_SetMac(void);															/* DB File - Dean - 20130530 */
void DB_GenerateFullname(DB_MEDIA dbMedia, char* strFullname, char* strFilename);
int DB_GetDatabaseFileName(DB_MEDIA dbMedia, int idbCheckFile, char* strDataBase);	/* DB File - Dean - 20130530 */
int DB_FileCheck(DB_MEDIA dbMedia, int idbCheckFile);			/* DB File Maintain - Dean - 20121226 */
int DB_FileOpen(FILE** ppFile, DB_MEDIA dbMedia, int iAction);
int DB_FileClose(FILE* pfile);
int DB_FileRead(FILE* pfile, DB_MEDIA dbMedia, int iAction, long lStartTime, long lEndTime);
int DB_FileWrite(FILE* pfile, DB_MEDIA dbMedia);
int DB_CheckAvailable(DB_MEDIA dbMedia, DB_COUNT_CHECK dbCheck);
int DB_DoRecycle(DB_MEDIA dbMedia, DB_COUNT_CHECK dbCountCheck, int iCheckSize);
int DB_DoItemAdd(DB_MEDIA dbMedia, DB_COUNT_CHECK dbCountCheck, char* strFilename, long lFiletime, char cRecord);		/* DB - File Time - Dean - 20130702 *//* DB Maintain - Check Duplicate File - Dean - 20130114 */
int DB_DoItemRemove(DB_MEDIA dbMedia, DB_COUNT_CHECK dbCountCheck, char* strFilename);	/* DB File Maintain - Dean - 20121219 */
int DB_InitList(DB_MEDIA dbMedia);
int DB_GetList(DB_MEDIA dbMedia, DB_READ_DATA_ACTION dbReadAct, long lStartTime, long lEndTime);	/* DB Maintain - Dean - 20130110 */
int DB_SaveList(DB_MEDIA dbMedia);
//int DB_RefreshList(DB_MEDIA dbMedia);	/* DB Maintain - Dean - 20130110 - Remove */
//int DB_UpdateList(DB_MEDIA dbMedia);	/* DB Maintain - Dean - 20130110 - Remove */
int DB_FlagSet(DB_MEDIA dbMedia, DB_FLAG dbFlag, DB_FLAG_OP bdFlagOp);
int DB_FlagCheck(DB_MEDIA dbMedia, DB_FLAG dbFlag);
int DB_RecycleCheck(DB_MEDIA dbMedia,DB_RECYCLE_OP dbRecycleOp,int iValue);
int DB_GetListTotal(DB_MEDIA dbMedia);
int DB_SetListTotal(DB_MEDIA dbMedia, int iTotal);
int DB_GetListRecord(DB_MEDIA dbMedia);
int DB_SetListRecord(DB_MEDIA dbMedia, int iIdx);
char* DB_GetInfoListPtr(DB_MEDIA dbMedia);
int DB_GetPath(DB_MEDIA dbMedia, char* strDir);			/* Record Media - Dean - 20130530 */
int DB_SetPath(DB_MEDIA dbMedia, char* strDir);			/* Record Media - Dean - 20130530 */
int DB_SetSambaPath(DB_MEDIA dbMedia, char* strDir);	/* Record Media - Dean - 20130530 */
int DB_GetPathLen(DB_MEDIA dbMedia);
int DB_ItemAdd(DB_MEDIA dbMedia, int iIdx, char* strFilename, long lFiletime);	/* DB - File Time - Dean - 20130702 *//* DB Maintain - Record - Dean - 20130117 */
int DB_ItemRemove(DB_MEDIA dbMedia, int iIdx);
int DB_ItemGetInfo(DB_MEDIA dbMedia, int iIdx, char* pKeep, int* pSize, long* pTime, char* strFilename);
int DB_ItemSetInfo(DB_MEDIA dbMedia, int iIdx, char cKeep, int iSize, long lTime, char* strFilename);
//int DB_ItemCheckFilename(DB_MEDIA dbMedia, char* strFilename, DB_CHECK_FNAME dbCheck, int dbIdx);	/* DB Maintain - Check Duplicate File - Dean - 20130114 */
int DB_AllocateDataBuffer(char** ppBuffer, int iNum, int iLen);
int DB_DestroyDataBuffer(char* pBuffer);
/* DB File Maintain - Dean - 20121219 - Begin */
int DB_DoRead(DB_MEDIA dbMedia, DB_READ_DATA_ACTION dbReadAct, long lStartTime, long lEndTime);					/* DB Maintain - Dean - 20130110 */
int DB_FileMaintain(DB_MEDIA dbMedia, DB_MAINTAIN_DATA_ACTION dbMaintain);
int FileCopy(char* strFileSrc, char* strFileDest);
/* DB File Maintain - Dean - 20121219 - End */

int FileTouch(char* strFilename);
int FileCheck(char* strFilename);
int FileCheckAvailable(char* strFilename);				/* Duplicate DB File for Search - Dean - 20130613 */
int FileUnlink(char* strFilename);
int FileSave(char* strFilename, char* Data, int iSize);		/* AVI Record Msg - Dean - 20130530 */
int FileRead(char* strFilename, char* Data, int iSize);		/* AVI Record Msg - Dean - 20130530 */
int FileDoAccess(FILE_OP FileOp, char* strFilename, char* strFlag, char* Data, int iSize);		/* AVI Record Msg - Dean - 20130530 */
/* DB - Dean - 20121213 - End */

int AviRecord_SaveFlag(int iFlag);					/* AVI Record Msg - Dean - 20130530 */
int AviRecord_AccessParam(FILE_OP FileOp, AviRecordInfo* pAviRecInfo);	/* AVI Record Msg - Dean - 20130530 */

int fSetAudioBroadcastPort(unsigned short port);
int fSetIpMode(unsigned char value) ;//Add by Joseph 20140408

int Cmd_Run_List_Check(void);							/* Dean - 20140317 */
int Port_SettingList(char* PortList);
/** @} */
#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif /* _FILE_MSG_DRV_H_ */

/**
* @page FILE_MSG_DRV_HOW How to use file message driver?

* 1. Add new message ID in \ref File_Msg_Def.h if needed.
* 2. Call the API InitFileMsgDrv(int key,int iProcId) with the ID you added to initialize file
*  message driver.
* 3. Now you can use any file message driver functions as you want.
* 4. Call the API CleanupFileMsgDrv() to cleanup file message driver.
* @section FILE_MSG_DRV_HOW_EX Example
* @code
#include <file_msg_drv.h>
int main()
{
	SysInfo *pSysInfo;
	if( InitFileMsgDrv(FILE_MSG_KEY, FILE_SYS_MSG) != 0)
		return -1;
	pSysInfo = GetSysInfo();
	// more file message driver API
	CleanupFileMsgDrv();
	return 0;
}
* @endcode
* \b See \b also \ref SYS_MSG_DRV_HOW
*/
