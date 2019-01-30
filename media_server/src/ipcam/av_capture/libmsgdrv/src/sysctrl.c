/* ===========================================================================
* @file sysctrl.c
*
* @path $(IPNCPATH)/util/
*
* @desc System control interface.
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#include <stdio.h>
#include "sysctrl.h"
#include <sys_env_type.h>
#include "sys_msg_drv.h"
#include "share_mem.h" //Add by Joseph 20131211
#include "ipnc_feature.h"

/**
* @brief Send commands.

* Send commands to system server.
* @param field [I ] command.
* @param *data [I ] pointer to data needed for command.
* @param len [I ] Data size.
* @return Function to excute command.
* @retval -1 Fail to send command.
*/
static int offsetBegin=0,offsetEnd=0;

int ControlSystemData(unsigned int field, void *data, unsigned int len)
{
	int ret = 0;
	if (len == 0 || data == NULL)
		return -1;
	offsetBegin =0;
//Add by Joseph for SetSysSet offset 20131211
//	fprintf(stderr,"111 offsetBegin=%d offsetEnd = %d \n",offsetBegin,offsetEnd);
#if 0
    offsetEnd=offsetBegin + len;
    if(offsetEnd > PROC_MEM_SIZE ){
      offsetBegin=0;
      offsetEnd = len;
    }
#endif
//end
	switch (field)
	{
		case SFIELD_GET_DHCPC_ENABLE:
			ret = GetDhcpEn(data, len);
			break;
		case SFIELD_GET_HTTPPORT:
			ret = GetHttpPort(data, len);
			break;
		case SFIELD_GET_SNTP_FQDN: //RS,MDY
			ret = GetCMD(data, len, SYS_MSG_GET_SNTP_FQDN);
			break;
		case SFIELD_GET_DEVICE_TYPE://RS,MDY
			ret = GetCMD(data, len, SYS_MSG_GET_DEVICE_TYPE);
			break;
		case SFIELD_GET_TITLE:
			ret = GetTitle(data, len);
			break;
		case SFIELD_GET_REGUSR://RS,MDY
			ret = GetCMD(data, len, SYS_MSG_GET_REGUSR);
			break;
		case SFIELD_GET_OSDTEXT://RS,MDY
			ret = GetCMD(data, len, SYS_MSG_GET_OSDTEXT);
			break;
		case SFIELD_GET_MAC:
			ret = GetMAC(data, len);
			break;
		case SFIELD_GET_IP:
			ret = GetIP(data, len);
			break;
		case SFIELD_GET_GATEWAY:
			ret = GetGateway(data, len);
			break;
		case SFIELD_GET_NETMASK:
			ret = GetNetmask(data, len);
			break;
		case SFIELD_GET_FTP_SERVER_IP://RS,MDY
			ret = GetCMD(data, len, SYS_MSG_GET_FTP_SERVER_IP);
			break;
		case SFIELD_GET_FTP_USERNAME:
			ret = GetFtpUsername(data, len);
			break;
		case SFIELD_GET_FTP_PASSWORD:
			ret = GetFtpPassword(data, len);
			break;
		case SFIELD_GET_FTP_FOLDNAME:
			ret = GetFtpFoldername(data, len);
			break;
		case SFIELD_GET_FTP_IMAGEAMOUNT:
			ret = GetFtpImageAmount(data, len);
			break;
		case SFIELD_GET_FTP_PID:
			ret = GetFtpPid(data, len);
			break;
		case SFIELD_GET_FTP_PORT:
			ret = GetSysFtpPort(data, len);
			break;
		case SFIELD_GET_SMTP_SERVER_IP://RS,MDY
			ret = GetCMD(data, len, SYS_MSG_GET_SMTP_SERVER_IP);
			break;
		case SFIELD_GET_SMTP_PORT:
			ret = GetSmtpServerPort(data, len);
			break;
		case SFIELD_GET_SMTP_AUTHENTICATION:
			ret = GetSmtpAuthentication(data, len);
			break;
		case SFIELD_GET_SMTP_USERNAME:
			ret = GetsmtpUsername(data, len);
			break;
		case SFIELD_GET_SMTP_PASSWORD:
			ret = GetSmtpPassword(data, len);
			break;
		case SFIELD_GET_SMTP_SENDER_EMAIL_ADDRESS:
			ret = GetSmtpSenderEmail(data, len);
			break;
		case SFIELD_GET_SMTP_RECEIVER_EMAIL_ADDRESS:
			ret = GetSmtpReceiverEmail(data, len);
			break;
		case SFIELD_GET_SMTP_CC:
			ret = GetSmtpCC(data, len);
			break;
#if 0 // SD_FEATURE	//RS Disable,(NOT Implemented)
		case SFIELD_GET_SD_FILE_NAME:
			ret = SysGetSdFileName(data, len);
			break;
 		case SFIELD_SET_SDAENABLE:
			ret = SetSysSDAlarmEnable(data, len);
			break;
		case SFIELD_SET_SDFILEFORMAT:
			ret = SetSDFileFormat(data, len);
			break;
		case SFIELD_SD_FORMAT:
			ret = SysSdFormat(data, len);
			break;
		case SFIELD_SD_UNMOUNT:
			ret = SysSdUnmount(data, len);
			break;
#endif
		case SFIELD_GET_SMTP_SUBJECT:
			ret = GetSmtpSubject(data, len);
			break;
		case SFIELD_GET_SMTP_TEXT:
			ret = GetSmtpText(data, len);
			break;
		case SFIELD_GET_SMTP_ATTACHMENTS:
			ret = GetSmtpAttachments(data, len);
			break;
		case SFIELD_GET_SMTP_VIEW:
			ret = GetSmtpView(data, len);
			break;
#if UpnpPort_FEATURE	//RS Disable,(NOT Implemented)
		case SFIELD_GET_UPNP_PORT:
			ret = GetSysUpnpPort(data, len);
			break;
#endif
		case SFIELD_SET_NETMASK:
			ret = SetSysSet(data, len, SYS_MSG_SET_NETMASK, offsetBegin);
			break;
		case SFIELD_SET_SNTP_FQDN:
			ret = SetSysSet(data, len, SYS_MSG_SET_SNTP_FQDN, offsetBegin);
			break;
		case SFIELD_SET_RESET_LOG:
			ret = SetSysSet(data, len, SYS_MSG_RESET_LOG, offsetBegin);
			break;
		case SFIELD_SET_TIME:
			ret = SetSysSet(data, len, SYS_MSG_SET_TIME, offsetBegin);
			break;
		case SFIELD_SET_TIMEFORMAT:
			ret = SetSysSet(data, len, SYS_MSG_SET_TIMEFORMAT, offsetBegin);
			break;
/*
		case SFIELD_SET_DAYLIGHT:
			ret = SetSysSet(data, len, SYS_MSG_SET_DAYLIGHT);
			break;
*/
#if WIFI_FEATURE
		//alice_modify_2012_6_29_v1
		case SFIELD_GET_WIFI_NandFlashCN_Data:
			ret=GetWifiNandFlashCNData(data,len);
			break;
		//alice_modify_2012_6_29_v1
		//alice_modify_2012_7_13_v1
		case SFIELD_GET_WIFI_E2P_Data:
			ret=GetWifiE2PData(data,len);
			break;
		//alice_modify_2012_7_13_v1
		//alice_modify_2012_3_27_v1
		case SFIELD_GET_NET_SITESURVEY:
			ret = GetNETSitesurvey(data, len);
			break;

		case SFIELD_GET_NET_SITESURVEYNUM:
			ret = GetNETSitesurveyNUM(data, len);
			break;
		//alice_modify_2012_5_10_v1
		case SFIELD_GET_NET_RSSI:
			ret = GetNETRSSI(data, len);
			break;
	    // alice_modify_2012_3_15_v1
		case SFIELD_GET_NET_CONNECNT:
			ret = GetNETConnectValue(data, len);
			break;
#endif
   	 // alice_modify_2012_3_15_v1
		case SFIELD_SET_TIMEZONE:
			ret = SetSysSet(data, len, SYS_MSG_SET_TIMEZONE, offsetBegin);
			break;
		case SFIELD_SET_IP:
			ret = SetSysSet(data, len, SYS_MSG_SET_IP, offsetBegin);
			break;
		case SFIELD_SET_HTTPPORT:
			ret = SetSysSet(data, len, SYS_MSG_SET_HTTPPORT, offsetBegin);
			break;
		case SFIELD_SET_TITLE:
			ret = SetSysSet(data, len, SYS_MSG_SET_TITLE, offsetBegin);
			break;
		case SFIELD_SET_REGUSR:
			ret = SetSysSet(data, len, SYS_MSG_SET_REGUSR, offsetBegin);
			break;
		case SFIELD_SET_OSDTEXT:
			ret = SetSysSet(data, len, SYS_MSG_SET_OSDTEXT, offsetBegin);
			break;
		case SFIELD_SET_GATEWAY:
			ret = SetSysSet(data, len, SYS_MSG_SET_GATEWAY, offsetBegin);
			break;
		case SFIELD_SET_DHCPC_ENABLE:
			ret = SetSysSet(data, len, SYS_MSG_SET_DHCPC_ENABLE, offsetBegin);
			break;
		case SFIELD_SET_FTP_SERVER_IP:
			ret = SetSysSet(data, len, SYS_MSG_SET_FTP_FQDN, offsetBegin);
			break;
		case SFIELD_SET_FTP_USERNAME:
			ret = SetSysSet(data, len, SYS_MSG_SET_FTP_USERNAME, offsetBegin);
			break;
		case SFIELD_SET_FTP_PASSWORD:
			ret = SetSysSet(data, len, SYS_MSG_SET_FTP_PASSWORD, offsetBegin);
			break;
		case SFIELD_SET_FTP_FOLDNAME:
			ret = SetSysSet(data, len, SYS_MSG_SET_FTP_FOLDNAME, offsetBegin);
			break;
		case SFIELD_SET_FTP_IMAGEAMOUNT:
			ret = SetSysSet(data, len, SYS_MSG_SET_FTP_IMAGEAMOUNT, offsetBegin);
			break;
		case SFIELD_SET_FTP_PID:
			ret = SetSysSet(data, len, SYS_MSG_SET_FTP_PID, offsetBegin);
			break;
		case SFIELD_SET_FTP_PORT:
			ret = SetSysSet(data, len, SYS_MSG_SET_FTP_PORT, offsetBegin);
			break;
		case SFIELD_SET_SMTP_SERVER_IP:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_SERVER_IP, offsetBegin);
			break;
		case SFIELD_SET_SMTP_PORT:
			ret = SetSmtpServerPort(data, len);
			break;
		case SFIELD_SET_SMTP_AUTHENTICATION:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_AUTHENTICATION, offsetBegin);
			break;
		case SFIELD_SET_SMTP_USERNAME:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_USERNAME, offsetBegin);
			break;
		case SFIELD_SET_SMTP_PASSWORD:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_PASSWORD, offsetBegin);
			break;
		case SFIELD_SET_SMTP_SENDER_EMAIL_ADDRESS:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_SENDER_EMAIL_ADDRESS, offsetBegin);
			break;
		case SFIELD_SET_SMTP_RECEIVER_EMAIL_ADDRESS:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_RECEIVER_EMAIL_ADDRESS, offsetBegin);
			break;
		case SFIELD_SET_SMTP_CC:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_CC, offsetBegin);
			break;
		case SFIELD_SET_SMTP_SUBJECT:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_SUBJECT, offsetBegin);
			break;
		case SFIELD_SET_SMTP_TEXT:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_TEXT, offsetBegin);
			break;
		case SFIELD_SET_SMTP_ATTACHMENTS:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_ATTACHMENTS, offsetBegin);
			break;
		case SFIELD_SET_SMTP_VIEW:
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTP_VIEW, offsetBegin);
			break;
		case SFIELD_GET_DNS:
			ret = GetDns(data, len);
			break;
		case SFIELD_SET_HOSTNAME:
			ret = SetHostname(data, len);
			break;
		case SFIELD_SET_DNS:
			ret = SetSysSet(data, len, SYS_MSG_SET_DNS, offsetBegin);
			break;
		/* Airlive IP Finder - Begin - Dean - 20120703 */
		case SFIELD_SET_DNS2:
			ret = SetSysSet(data, len, SYS_MSG_SET_DNS2, offsetBegin);
			break;
		/* Airlive IP Finder - End - Dean - 20120703 */
		case SFIELD_SET_SNTP_SERVER:
			ret = SetSysSet(data, len, SYS_MSG_SET_SNTP_SERVER_IP, offsetBegin);
			break;
		/* Maintainence */
		case SFIELD_SET_MAINTAINENCE_REBOOT:
			ret = SetSysSet(data, len, SYS_MSG_SET_REBOOT, offsetBegin);
			break;
	  	case SFIELD_SET_MAINTAINENCE_FACTORY_RESET:
			fprintf(stderr, "[%s][%s][%d][MIKE_IS_HERE] data=%d\n", __FILE__, __func__, __LINE__, atoi(data));
	  		ret = SetSysSet(data, len, SYS_MSG_SET_FACTORY_RESET, offsetBegin);
			break;
	  	case SFIELD_SET_MAINTAINENCE_BACKUP:
	  		ret = SetSysSet(data, len, SYS_MSG_SET_BACKUP, offsetBegin);
			break;
		case SFIELD_SET_MAINTAINENCE_BACKUP_TO_MMC:
	  		ret = SetSysSet(data, len, SYS_MSG_SET_BACKUP_TO_MMC, offsetBegin);
			break;
		case SFIELD_SET_MAINTAINENCE_RESTORE:
			ret = SetSysSet(data, len, SYS_MSG_SET_RESTORE, offsetBegin);
			break;
		case SFIELD_SET_MAINTAINENCE_RESTORE_FROM_MMC:
			ret = SetSysSet(data, len, SYS_MSG_SET_RESTORE_FROM_MMC, offsetBegin);
			break;
#if DAY_NIGHT_FEATURE  //RS Disable,(NOT Implemented)
		case SFIELD_SET_DAY_NIGHT:
			ret = SetSysSet(data, len, SYS_MSG_SET_DAY_NIGHT, offsetBegin);
			break;
#endif
		case SFIELD_SET_WHITE_BALANCE:
			ret = SetSysSet(data, len, SYS_MSG_SET_WHITE_BALANCE, offsetBegin);
			break;
#if BACKLIGHT_FEATURE	//RS Disable,(NOT Implemented)
		case SFIELD_SET_BACKLIGHT:
			ret = SetSysBacklight(data, len);
			break;
#endif
		case SFIELD_SET_BRIGHTNESS:
			ret = SetSysSet(data, len, SYS_MSG_SET_BRIGHTNESS, offsetBegin);
			break;
		case SFIELD_SET_CONTRAST:
			ret = SetSysSet(data, len, SYS_MSG_SET_CONTRAST, offsetBegin);
			break;
		case SFIELD_SET_SATURATION:
			ret = SetSysSet(data, len, SYS_MSG_SET_SATURATION, offsetBegin);
			break;

		case SFIELD_SET_SHARPNESS: //RS Adding For VC320 Spec.
			ret = SetSysSet(data, len, SYS_MSG_SET_SHARPNESS, offsetBegin);
			break;
		case SFIELD_SET_IMAGING:
			ret = SetImaging(data, len);
			break;
		case SFIELD_SET_VSTAB:
			ret = SetSysCommon(data, len, SYS_MSG_SET_VSTAB);
			break;
		case SFIELD_SET_COLOR_BAR:
			ret = SetSysSet(data, len, SYS_MSG_SET_COLOR_BAR, offsetBegin);
			break;
		case SFIELD_SET_EXPOSUREMODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_EXPOSUREMODE, offsetBegin);
			break;
		case SFIELD_SET_EXPOSUREVALUE:
			ret = SetSysSet(data, len, SYS_MSG_SET_EXPOSUREVALUE, offsetBegin);
			break;
		case SFIELD_SET_EXPOSUREVALUE1:
			ret = SetSysSet(data, len, SYS_MSG_SET_EXPOSUREVALUE1, offsetBegin);
			break;
		case SFIELD_SET_AGC:
			ret = SetSysSet(data, len, SYS_MSG_SET_AGC, offsetBegin);
			break;
		case SFIELD_SET_AGC_MIN:
			ret = SetSysSet(data, len, SYS_MSG_SET_AGC_MIN, offsetBegin);
			break;
		case SFIELD_SET_AGC1:
			ret = SetSysSet(data, len, SYS_MSG_SET_AGC1, offsetBegin);
			break;
		case SFIELD_SET_AGC_MIN1:
			ret = SetSysSet(data, len, SYS_MSG_SET_AGC_MIN1, offsetBegin);
			break;
		case SFIELD_SET_SHUTTER_TIME:
 		       ret = SetSysSet(data, len, SYS_MSG_SET_SHUTTER_TIME, offsetBegin);
 		      break;
		case SFIELD_SET_SHUTTER_TIME_MIN:
 		       ret = SetSysSet(data, len, SYS_MSG_SET_SHUTTER_TIME_MIN, offsetBegin);
 		      break;
		case SFIELD_SET_SHUTTER_TIME1:
 		       ret = SetSysSet(data, len, SYS_MSG_SET_SHUTTER_TIME1, offsetBegin);
 		      break;
		case SFIELD_SET_SHUTTER_TIME_MIN1:
 		       ret = SetSysSet(data, len, SYS_MSG_SET_SHUTTER_TIME_MIN1, offsetBegin);
 		      break;
		case SFIELD_SET_FLICKERLESS_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_FLICKERLESS_MODE, offsetBegin);
			break;
		case SFIELD_SET_JQUALITY:
			ret = SetSysSet(data, len, SYS_MSG_SET_JPEG_QUALITY, offsetBegin);
			break;
#if FDETECT_FEATURE	//RS Disable,(NOT Implemented)
		case SFIELD_SET_FDETECT:
			ret = SetSysFaceDetect(data, len);
			break;
		case SFIELD_SET_DEMOCFG:
			ret = SetSysSet(data, len, SYS_MSG_SET_DEMOCFG, offsetBegin);
			break;
#endif
		case SFIELD_SET_ADVMODE://DM365,Specify,RS,H264
			ret = SetSysSet(data, len, SYS_MSG_SET_ADVMODE, offsetBegin);
			break;
		case SFIELD_SET_M41SFEATURE://DM365,Specify,RS
			ret = SetSysSet(data, len, SYS_MSG_SET_M41SFEATURE, offsetBegin);
			break;
		case SFIELD_SET_M42SFEATURE://DM365,Specify,RS
			ret = SetSysSet(data, len, SYS_MSG_SET_M42SFEATURE, offsetBegin);
			break;
		case SFIELD_SET_JPGSFEATURE://DM365,Specify,RS
			ret = SetSysSet(data, len, SYS_MSG_SET_JPGSFEATURE, offsetBegin);
			break;
#if ROTATION_FEATURE //RS Disable,(NOT Implemented)
		case SFIELD_SET_ROTCTRL:
			ret = SetSysRotation(data, len);
			break;
#endif
		case SFIELD_SET_MIRROR:  //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_MIRROR, offsetBegin);
			break;
		case SFIELD_SET_OSDWIN:  //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_OSDWIN, offsetBegin);
			break;
		case SFIELD_SET_OSDWINNUM:  //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_OSDWINNUM, offsetBegin);
			break;
		case SFIELD_SET_OSDSTREAM:  //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_OSDSTREAM, offsetBegin);
			break;
		case SFIELD_SET_VIDEO_CODEC: //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_VIDEO_CODEC, offsetBegin);
			break;
		case SFIELD_SET_VIDEOCODECMODE: //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_VIDEOCODEC_MODE, offsetBegin);
			break;
		case SFIELD_SET_VIDEOCODECRES: //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_VIDEOCODEC_RES, offsetBegin);
			break;
		case SFIELD_SET_IMAGESOURCE: //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_IMAGE_SOURCE, offsetBegin);
			break;
		case SFIELD_SET_SCHEDULE://RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_SCHEDULE, offsetBegin);
			break;
		case SFIELD_SET_VIDEO_MODE: //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_VIDEO_MODE, offsetBegin);
			break;
		case SFIELD_SET_IMAGEFORMAT:{ //RS Check
	//	    unsigned char* aaaa=(unsigned char*)(data);
	//		fprintf(stderr,"SFIELD_SET_IMAGEFORMAT - data = %d, \n",*aaaa);
			ret = SetSysSet(data, len, SYS_MSG_SET_IMAGE_FORMAT, offsetBegin);
			break;
			}
		case SFIELD_SET_RESOLUTION://RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_RESOLUTION, offsetBegin);
			break;
		case SFIELD_SET_MAIN_DEWARPMODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_MAIN_DEWARPMODE, offsetBegin);
			break;			
		case SFIELD_SET_3rd_DEWARPMODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_3rd_DEWARPMODE, offsetBegin);
			break;			
		case SFIELD_SET_MPEG4_RES://RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG4_RES, offsetBegin);
			break;
		case SFIELD_SET_MPEG42_RES://RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG42_RES, offsetBegin);
			break;
		case SFIELD_SET_MPEG43_RES://RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG43_RES, offsetBegin);
			break;
		case SFIELD_SET_MPEG4_QUALITY://RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG4_QUALITY, offsetBegin);
			break;
		case SFIELD_SET_AFTPENABLE: //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_FTPAENABLE, offsetBegin);
			break;
		case SFIELD_SET_ASMTPENABLE://RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_SMTPAENABLE, offsetBegin);
			break;
		case SFIELD_SET_ALARMDURATION: //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_ALARMDURATION, offsetBegin);
			break;
		case SFIELD_SET_AVIDURATION: //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_AVIDURATION, offsetBegin);
			break;
		case SFIELD_SET_AVIFORMAT: //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_AVIFORMAT, offsetBegin);
			break;
		case SFIELD_SET_FTPFILEFORMAT: //RS Check
			ret = SetSysSet(data, len, SYS_MSG_SET_FTPFILEFORMAT, offsetBegin);
			break;
		case SFIELD_SET_ATTFILEFORMAT:
			ret = SetSysSet(data, len, SYS_MSG_SET_ATTFILEFORMAT, offsetBegin);
			break;
		case SFIELD_SET_AUDIOENABLE:
			ret = SetSysSet(data, len, SYS_MSG_SET_AUDIOENABLE, offsetBegin);
			break;
		case SFIELD_SET_ASMTPATTACH:
			ret = SetSysSet(data, len, SYS_MSG_SET_ASMTPATTACH, offsetBegin);
			break;
		case SFIELD_SET_FTP_RFTPENABLE:
			ret = SetSysSet(data, len, SYS_MSG_SET_FTP_RFTPENABLE, offsetBegin);
			break;
		case SFIELD_GET_FTP_RFTPENABLE:
			ret = GetRftpEnable(data, len);
			break;
		case SFIELD_SET_MOTIONENABLE:
			ret = SetSysSet(data, len, SYS_MSG_SET_MOTIONENABLE, offsetBegin);
			break;
		case SFIELD_SET_MOTIONCENABLE:
			ret = SetSysSet(data, len, SYS_MSG_SET_MOTIONCENABLE, offsetBegin);
			break;
		case SFIELD_SET_MOTIONLEVEL:
			ret = SetSysSet(data, len, SYS_MSG_SET_MOTIONLEVEL, offsetBegin);
			break;
		case SFIELD_SET_MOTIONCVALUE:
			ret = SetSysSet(data, len, SYS_MSG_SET_MOTIONCVALUE, offsetBegin);
			break;
		case SFIELD_SET_MOTIONBLOCK:
			ret = SetSysSet(data, len, SYS_MSG_SET_MOTIONBLOCK, offsetBegin);
			break;
		case SFIELD_SET_PRIVACY_MASK:
			ret = SetSyspmask(data, len);
			break;
		/* OVERLAY_IMAGE_FEATURE - Dean - 20130709 - Begin */
#if 1//OVERLAY_IMAGE_FEATURE
		case SFIELD_SET_OVERLAY_IMAGE_CONFIG:
			ret = SetSysSet(data, len, SYS_MSG_SET_OVERLAY_IMAGE_CONFIG, offsetBegin);
			break;
		case SFIELD_GET_OVERLAY_IMAGE_CONFIG:
			ret = SetSysSet(data, len, SYS_MSG_GET_OVERLAY_IMAGE_CONFIG, offsetBegin);
			break;
#endif	// OVERLAY_IMAGE_FEATURE
		/* OVERLAY_IMAGE_FEATURE - Dean - 20130709 - End */
		/* PRIVACY_MASK_FEATURE - Dean - 20130801 - Begin */
#if PRIVACY_MASK_FEATURE
		case SFIELD_SET_PRIVACY_MASK_CONFIG:
			ret = SetSysSet(data, len, SYS_MSG_SET_PRIVACY_MASK_CONFIG, offsetBegin);
			break;
		case SFIELD_GET_PRIVACY_MASK_CONFIG:
			ret = SetSysSet(data, len, SYS_MSG_GET_PRIVACY_MASK_CONFIG, offsetBegin);
			break;
#endif	// PRIVACY_MASK_FEATURE
		/* PRIVACY_MASK_FEATURE - Dean - 20130801 - End */
		/* TAMPER_FEATURE - Dean - 20131007 - Begin */
#if TAMPER_FEATURE
		case SFIELD_SET_TAMPER_CONFIG:
			ret = SetSysSet(data, len, SYS_MSG_SET_TAMPER_CONFIG, offsetBegin);
			break;
		case SFIELD_GET_TAMPER_CONFIG:
			ret = SetSysSet(data, len, SYS_MSG_GET_TAMPER_CONFIG, offsetBegin);
			break;
#endif
		/* TAMPER_FEATURE - Dean - 20131007 - End */
		/* Port Used List CGI - Dean - 20131021 - Begin */
		case SFIELD_GET_PORT_USED_LIST:
			ret = GetSysSet(data, len, SYS_MSG_GET_PORT_USED_LIST);
			break;
		/* Port Used List CGI - Dean - 20131021 - End */
		case SFIELD_SET_MASK_OPTIONS:
			ret = SetSyspmaskopt(data, len);
			break;
		case SFIELD_GET_DHCP_CONFIG:
			ret = GetSysDhcpConfig(data, len);
			break;
		case SFIELD_DEL_SCHEDULE:
			ret = SetSysSet(data, len, SYS_MSG_DEL_SCHEDULE, offsetBegin);
			break;
		case SFIELD_SET_IMAGE2A:
			ret = SetSysSet(data, len, SYS_MSG_SET_2A_SWITCH, offsetBegin);
			break;
		case SFIELD_SET_TVCABLE:
			ret = SetSysSet(data, len, SYS_MSG_SET_TVCABLE, offsetBegin);
			break;
#if BINNING_FEATURE	//RS Disable,(NOT Implemented)
		case SFIELD_SET_BINNING:
			ret = SetSysSet(data, len, SYS_MSG_SET_BINNING, offsetBegin);
			break;
#endif
		case SFIELD_SET_BLC:
			ret = SetSysSet(data, len, SYS_MSG_SET_BLC, offsetBegin);
			break;
		case SFIELD_SET_MPEG41_BITRATE:
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG41_BITRATE, offsetBegin);
			break;
		case SFIELD_SET_MPEG42_BITRATE:
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG42_BITRATE, offsetBegin);
			break;
		case SFIELD_SET_MPEG43_BITRATE:
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG43_BITRATE, offsetBegin);
			break;
		case SFIELD_SET_IMAGEDEFAULT:
			ret = SetSysSet(data, len, SYS_MSG_SET_IMAGE_DEFAULT, offsetBegin);
			break;
		case SFIELD_DO_LOGIN:
			ret = DoSysLogin(data, len);
			break;
		case SFIELD_GET_USER_AUTHORITY:
			ret = GetSysUserAuthority(data, len);
			break;
		case SFIELD_ADD_USER:
			ret = SetSysSet(data, len, SYS_MSG_ADD_USER, offsetBegin);
			break;
		case SFIELD_DEL_USER:
			ret = SetSysSet(data, len, SYS_MSG_DEL_USER, offsetBegin);
			break;
		case SFIELD_SET_GIOINENABLE://
			ret = SetSysSet(data, len, SYS_MSG_SET_GIOINENABLE, offsetBegin);
			break;
		case SFIELD_SET_GIOINTYPE://
			ret = SetSysSet(data, len, SYS_MSG_SET_GIOINTYPE, offsetBegin);
			break;
		case SFIELD_SET_GIOOUTENABLE://
			ret = SetSysSet(data, len, SYS_MSG_SET_GIOOUTENABLE, offsetBegin);
			break;
		case SFIELD_SET_GIOOUTTYPE://
			ret = SetSysSet(data, len, SYS_MSG_SET_GIOOUTTYPE, offsetBegin);
			break;
		case SFIELD_SET_TSTAMPENABLE: //
			ret = SetSysSet(data, len, SYS_MSG_SET_TSTAMPENABLE, offsetBegin);
			break;
		case SFIELD_SET_TSTAMPFORMAT: //
		    ret = SetSysSet(data, len, SYS_MSG_SET_TSTAMPFORMAT, offsetBegin);
			break;
		case SFIELD_SET_AUDIOINVOLUME://
			ret = SetSysSet(data, len, SYS_MSG_SET_AUDIOINVOLUME, offsetBegin);
			break;
		case SFIELD_SET_RATE_CONTROL://
			ret = SetSysSet(data, len, SYS_MSG_SET_RATE_CONTROL, offsetBegin);
			break;
		case SFIELD_SET_SPEAKERENABLE://
			ret = SetSysSet(data, len, SYS_MSG_SET_SPEAKER_ENABLE, offsetBegin);
			break;			
		case SFIELD_SET_JPEG_FRAMERATE://
			ret = SetSysSet(data, len, SYS_MSG_SET_JPEG_FRAMERATE, offsetBegin);
			break;
		case SFIELD_SET_MPEG41_FRAMERATE://
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG41_FRAMERATE, offsetBegin);
			break;
		case SFIELD_SET_MPEG42_FRAMERATE://
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG42_FRAMERATE, offsetBegin);
			break;
		case SFIELD_SET_MPEG43_FRAMERATE://
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG43_FRAMERATE, offsetBegin);
			break;
#if GOP_FEATURE	//Add by Joseph 20131119
		case SFIELD_SET_MPEG41_GOP://Add by Joseph 20131118
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG41_GOP, offsetBegin);
			break;
#endif
		case SFIELD_SCHEDULE_STOP://
			ret = SetSysSet(data, len, SYS_MSG_SCHEDULE_STOP, offsetBegin);
			break;
		case SFIELD_SCHEDULE_RESUME://
			ret = SetSysSet(data, len, SYS_MSG_SCHEDULE_RESUME, offsetBegin);
			break;
		case SFIELD_SET_BOA_PROTOCOL:  //Larry Check
			ret = SetSysSet(data, len, SYS_MSG_SET_BOA_PROTOCOL, offsetBegin);
			break;
		case SFIELD_SET_RESET_DEFAULT: //Larry Check Kenny : never used now
			ret = SetSysSet(data, len, SYS_MSG_RESET_TO_DEFAULT, offsetBegin);
			break;
		case SFIELD_SET_RESET_DEFAULT_PREPARAT: // Kenny : never used now
			ret = SetSysSet(data, len, SYS_MSG_RESET_TO_DEFAULT_PREPARAT, offsetBegin);
			break;
		case SFIELD_SET_IP_MODE:    //Larry Check
			ret = SetSysSet(data, len, SYS_MSG_CHECK_IP_MODE, offsetBegin);
			break;
		case SFIELD_SET_PPPOE_MODE: //
			ret = SetSysSet(data, len, SYS_MSG_CHECK_PPPOE_MODE, offsetBegin);
			break;
		case SFIELD_SET_RTSP_MODE: //Larry Check
			ret = SetSysSet(data, len, SYS_MSG_CHECK_RTSP_MODE, offsetBegin);
			break;
		case SFIELD_SET_FTP_EVEN_MODE: //
			ret = SetSysSet(data, len, SYS_MSG_CHECK_FTP_EVEN_MODE, offsetBegin);
			break;
		case SFIELD_SET_TEST_FUNCTION_MODE: //Larry Check
			ret = SetSysSet(data, len, SYS_MSG_CHECK_TESTFUNCTION_MODE, offsetBegin);
			break;
		case SFIELD_SET_SEC8021X_MODE: //
			ret = SetSysSet(data, len, SYS_MSG_CHECK_SEC8021X_MODE, offsetBegin);
			break;
		case SFIELD_SET_SIP_MODE: //
			ret = SetSysSet(data, len, SYS_MSG_CHECK_SIP_MODE, offsetBegin);
			break;
		case SFIELD_SET_SAMBA_REC_MODE: //Larry Check
			ret = SetSysSet(data, len, SYS_MSG_SET_SAMBA_REC_MODE, offsetBegin);		/* Record Media - Dean - 20130530 */
			break;
		case SFIELD_CHECK_SAMBA_REC_MODE: 								/* Record Media - Dean - 20130530 */
			ret = SetSysSet(data, len, SYS_MSG_CHECK_SAMBA_REC_MODE, offsetBegin);	/* Record Media - Dean - 20130530 */
			break;														/* Record Media - Dean - 20130530 */
		case SFIELD_SET_SAMBA_MODE: //Larry Check
			ret = SetSysSet(data, len, SYS_MSG_CHECK_SAMBA_MODE, offsetBegin);
			break;
		case SFIELD_SET_MMC_STATUS: //
			ret = SetSysSet(data, len, SYS_MSG_CHECK_MMC_STATUS, offsetBegin);
			break;
		case SFIELD_SET_MMC_MODE: //
			ret = SetSysSet(data, len, SYS_MSG_CHECK_MMC_MODE, offsetBegin);
			break;
		case SFIELD_SET_LED_MODE: //Larry Check
			ret = SetSysSet(data, len, SYS_MSG_CHECK_LED_MODE, offsetBegin);
			break;
		case SFIELD_SET_DDNS_MODE: //Larry Check
			ret = SetSysSet(data, len, SYS_MSG_CHECK_DDNS_MODE, offsetBegin);
			break;
		/* DIPS_FEATURE - Begin - Dean - 20120530 */
#if DIPS_FEATURE
		case SFIELD_SET_DIPS_MODE:
			ret = SetSysSet(data, len, SYS_MSG_CHECK_DIPS_MODE, offsetBegin);
			break;
#endif
		/* DIPS_FEATURE - End - Dean - 20120530 */
		case SFIELD_GET_UPDATE_MODE: //
			ret = GetUpdateMode(data, len);
			break;
		case SFIELD_SET_UPDATE_MODE: //
			ret = SetSysSet(data, len, SYS_MSG_SET_UPDATE_MODE, offsetBegin);
			break;
		case SFIELD_SET_UPNP_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_UPNP_MODE, offsetBegin);
			break;
		case SFIELD_SET_BONJOUR_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_BONJOUR_MODE, offsetBegin);
			break;
		case SFIELD_SET_SNMP_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_SNMP_MODE, offsetBegin);
			break;
		case SFIELD_SET_QOS_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_QOS_MODE, offsetBegin);
			break;
		case SFIELD_SET_COS_MODE: //NOT Implemented
			break;
#if WIFI_FEATURE
		// alice_modify_2012_3_15_v1
		//alice_modify_2012_5_10_v1
		case SFIELD_SET_WIFI_TEST_BEGIN:
			ret = SetSysSet(data, len, SYS_MSG_SET_WIFI_TEST_BEGIN, offsetBegin);
			break;
		//alice_modify_2012_5_10_v1
		case SFIELD_SET_WIFI_WPS:
			ret = SetSysSet(data, len, SYS_MSG_SET_WIFI_WPS, offsetBegin);
			break;
		// alice_modify_2012_3_15_v1
		// alice_modify_2012_3_19_v1
		case SFIELD_SET_WIFI_Conf_Data:
			ret = SetSysSet(data, len, SYS_MSG_SET_WIFI_Conf_Data, offsetBegin);
			break;
		// alice_modify_2012_3_19_v1
		//alice_modify_2012_6_29_v1
		case SFIELD_SET_WIFI_NandFlashCN_Data:
			ret = SetSysSet(data, len, SYS_MSG_SET_WIFI_NandFlashCN_Data, offsetBegin);
			break;
		//alice_modify_2012_6_29_v1
		//alice_modify_2012_7_13_v1
		case SFIELD_SET_WIFI_E2P_Data:
			ret = SetSysSet(data, len, SYS_MSG_SET_WIFI_E2P_Data, offsetBegin);
			break;
		//alice_modify_2012_7_13_v1
#endif
#if IPNCPTZ_FEATURE	 //RS Disable,(NOT Implemented)
		case SFIELD_IPNCPTZ:
			;//ret = SetIpncPtz(data, len);
			break;
		case SFIELD_INIT_IPNCPTZ:
			;//ret = InitIpncPtz(data, len);
			break;
		case SFIELD_PTZ_GOTO_PRESET:
			ret = SetSysSet(data, len, SYS_MSG_PTZ_GOTO_PRESET, offsetBegin);
			break;
		case SFIELD_SET_PTZPATROL:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_PATROL, offsetBegin);
			break;
		case SFIELD_SET_PTZSPEED:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_SPEED, offsetBegin);
			break;
		case SFIELD_SET_PTZMOVE:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_MOVE, offsetBegin);
			break;
			//Add by Joseph for ONVIF 2013-03
		case SFIELD_SET_PTZMOVEORIGINAL:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_MOVE_ORG, offsetBegin);
			break;
			//Add by Joseph for ONVIF 2013-08
		case SFIELD_SET_PTZSTOP:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_STOP, offsetBegin);
			break;
		case SFIELD_SET_PTZDEFAULTHOME:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_DEFAULT_HOME, offsetBegin);
			break;
		case SFIELD_PTZ_SET_HOME:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_AS_HOME, offsetBegin);
			break;
		//case SFIELD_PTZ_GET_HOME:
		//	ret = GetPtzHome(data, len);
		//	break;
		case SFIELD_PTZ_GO_HOME:
			ret = SetSysSet(data, len, SYS_MSG_GO_PTZ_HOME, offsetBegin);
			break;
		case SFIELD_SET_PTZMOVEVIDEO:
			;//ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_MOVEVIDEO, offsetBegin);
			break;
		case SFIELD_SET_PTZMOVETOVIDEO:
			;//ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_MOVETOVIDEO, offsetBegin);
			break;
		case SFIELD_SET_PTZMOVETO:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_MOVETO, offsetBegin);
			break;
		case SFIELD_SET_PTZUPDATE:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_UPDATE, offsetBegin);
			break;
		case SFIELD_SET_PTZ_PRESET_UPDATE: // add by kenny chiu 20110722
			ret = SetSysSet(data, len, SYS_MSG_SET_UPDATE_PRESET, offsetBegin);
			break;
		case SFIELD_SET_PTZ_PATROL_UPDATE: // add by kenny chiu 20110722
			ret = SetSysSet(data, len, SYS_MSG_SET_UPDATE_PATROL, offsetBegin);
			break;
		case SFIELD_SET_PTZGLOBALPATROL:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_GLOBAL_PATROL, offsetBegin);
			break;
		case SFIELD_SET_PTZPRESET:
			ret = SetSysSet(data, len, SYS_MSG_PTZ_SET_PRESET, offsetBegin);
			break;
		case SFIELD_SET_PTZFOCUS:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_FOCUS, offsetBegin);
			break;
		case SFIELD_SET_PTZZOOM:
//			fprintf(stderr, "SFIELD_SET_PTZZOOM:len:%d\n", len);
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_ZOOM, offsetBegin);
			break;
		case SFIELD_SET_PTZTRACKING:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_TRACKING, 0);
			break;
		case SFIELD_SET_PTZROTATE:
			ret = SetSysSet(data, len, SYS_MSG_SET_PTZ_ROTATE, 0);
			break;			
		case SFIELD_GET_PTZ_SPEED:
			ret = GetPtzSpeed(data, len);
			break;
		case SFIELD_GET_PTZ_INFO:
			ret = GetPtzInfo(data, len);
			break;
		//alice_modify_onvif_code_2011
		case SFIELD_GET_PTZ_POS:{
			ret = GetPtzPos(data, len);
			PTData* ptz_data1 =(PTData*) data;
			fprintf(stderr,"SFIELD_GET_PTZ_POS -  ret = %d  (PTData)data.x=%d (PTData)data.y=%d\n",  ret,ptz_data1->x,ptz_data1->y);
			break;
			}
		case SFIELD_GET_PTZ_MVStatus:
			ret = GetPtzMVStatus(data, len);
			break;
		case SFIELD_PTZ_GET_PRESET_LIST:
			ret = GetPtzPresetList(data, len);
			break;
		case SFIELD_PTZ_GET_PRESET_LIST2:
			ret = GetPtzPresetList2(data, len);
			break;
		case SFIELD_PTZ_GET_PATROL_LIST:
			ret = GetPatrolList(data, len);
			break;
		case SFIELD_SET_PTZ_UPDATESTATUS:
			ret = SetSysSet(data, len, SYS_MSG_SET_UPDATE_STATUS, offsetBegin);
			break;
		case SFIELD_INIT_ZF_CALIBRATE:
			ret = SetSysSet(data, len, SYS_MSG_INIT_ZF_CALIBRATE, offsetBegin);
			break;
		case SFIELD_INIT_PTZ_DAEMON:
			ret = SetSysSet(data, len, SYS_MSG_INIT_PTZ_DAEMON, offsetBegin);
			break;
		case SFIELD_SET_ZOOMIN_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_ZOOMIN_MODE, offsetBegin);
			break;
		case SFIELD_SET_ZOOMOUT_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_ZOOMOUT_MODE, offsetBegin);
			break;
		case SFIELD_SET_FOCUSUP_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_FOCUSUP_MODE, offsetBegin);
			break;
		case SFIELD_SET_FOCUSDOWN_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_FOCUSDOWN_MODE, offsetBegin);
			break;
		case SFIELD_SET_FOCUSAUTO_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_FOCUSAUTO_MODE, offsetBegin);
			break;
		case SFIELD_GET_ZOOM_VALUE:
			ret = GetZoomValue(data, len);
			break;
		case SFIELD_PTZ_CALIBRATE:	   // Kenny Chiu
			ret = SetSysSet(data, len, SYS_MSG_PTZ_CALIBRATE, offsetBegin);
			break;
		case SFIELD_PTZ_SET_DURATION:
			ret = SetSysSet(data, len, SYS_MSG_PTZ_SET_DURATION, offsetBegin);
			break;
#endif
#if FOCUS_ZOOM_FEATURE
		case SFIELD_SET_MOVEZOOMMOTOR:
			ret = SetSysSet(data, len, SYS_MSG_MOVE_ZOOM_MOTOR, offsetBegin);
			break;
		case SFIELD_SET_MOVEZOOM:
			ret = SetSysSet(data, len, SYS_MSG_MOVE_ZOOM, offsetBegin);
			break;
		case SFIELD_SET_MOVEZOOMIN:
			ret = SetSysSet(data, len, SYS_MSG_MOVE_ZOOMIN, offsetBegin);
			break;
		case SFIELD_SET_MOVEZOOMOUT:
			ret = SetSysSet(data, len, SYS_MSG_MOVE_ZOOMOUT, offsetBegin);
			break;
		case SFIELD_SET_FINETUNE_ZOOMIN:
			ret = SetSysSet(data, len, SYS_MSG_FINETUNE_ZOOMIN, offsetBegin);
			break;
		case SFIELD_SET_FINETUNE_ZOOMOUT:
			ret = SetSysSet(data, len, SYS_MSG_FINETUNE_ZOOMOUT, offsetBegin);
			break;
		case SFIELD_SET_MOVEFOCUSMOTOR:
			ret = SetSysSet(data, len, SYS_MSG_MOVE_FOCUS_MOTOR, offsetBegin);
			break;
		case SFIELD_SET_FOCUSMINUS:
			ret = SetSysSet(data, len, SYS_MSG_FINETUNE_FOCUSMINUS, offsetBegin);
			break;
		case SFIELD_SET_FOCUSPLUS:
			ret = SetSysSet(data, len, SYS_MSG_FINETUNE_FOCUSPLUS, offsetBegin);
			break;
		case SFIELD_SET_FOCUSMINUS_1X:
			ret = SetSysSet(data, len, SYS_MSG_FINETUNE_FOCUSMINUS_1X, offsetBegin);
			break;
		case SFIELD_SET_FOCUSPLUS_1X:
			ret = SetSysSet(data, len, SYS_MSG_FINETUNE_FOCUSPLUS_1X, offsetBegin);
			break;
		case SFIELD_SET_FOCUSMINUS_2X:
			ret = SetSysSet(data, len, SYS_MSG_FINETUNE_FOCUSMINUS_2X, offsetBegin);
			break;
		case SFIELD_SET_FOCUSPLUS_2X:
			ret = SetSysSet(data, len, SYS_MSG_FINETUNE_FOCUSPLUS_2X, offsetBegin);
			break;
		case SFIELD_SET_FOCUSMINUS_3X:
			ret = SetSysSet(data, len, SYS_MSG_FINETUNE_FOCUSMINUS_3X, offsetBegin);
			break;
		case SFIELD_SET_FOCUSPLUS_3X:
			ret = SetSysSet(data, len, SYS_MSG_FINETUNE_FOCUSPLUS_3X, offsetBegin);
			break;
      case SFIELD_SET_RESETZOOM:
			ret = SetSysSet(data, len, SYS_MSG_RESET_ZOOM, offsetBegin);
         break;
      case SFIELD_SET_RESETFOCUS:
			ret = SetSysSet(data, len, SYS_MSG_RESET_FOCUS, offsetBegin);
         break;
		case SFIELD_SET_FULLYFOCUSSCAN:
			ret = SetSysSet(data, len, SYS_MSG_FULLY_FOCUS_SCAN, offsetBegin);
			break;
      case SFIELD_SET_SMARTFOCUSSCAN:
			ret = SetSysSet(data, len, SYS_MSG_SMART_FOCUS_SCAN, offsetBegin);
			break;
		case SFIELD_SET_GET_MOTOR_CUR_POS:
			ret = SetSysSet(data, len, SYS_MSG_GET_MOTOR_CUR_POS, offsetBegin);
			break;
#endif
#if DISCARD_FEATURE
		case SFIELD_CHECK_OVERLAY_MODE:
			ret = SetSysSet(data, len, SYS_MSG_CHECK_OVERLAY_MODE, offsetBegin);
			break;
#endif
		case SFIELD_SET_GPI1_VALUE:
			ret = SetSysSet(data, len, SYS_MSG_UPDATE_GPI1, offsetBegin);
			break;
		case SFIELD_SET_GPI2_VALUE:
			ret = SetSysSet(data, len, SYS_MSG_UPDATE_GPI2, offsetBegin);
			break;
		case SFIELD_SET_GPO_VALUE:
			ret = SetSysSet(data, len, SYS_MSG_UPDATE_GPO, offsetBegin);
			break;
		case SFIELD_GET_GPI1_STATUS:
			ret = GetGPI1Status(data, len);
			break;
		case SFIELD_GET_GPI2_STATUS:
			ret = GetGPI2Status(data, len);
			break;
		case SFIELD_GET_GPO_STATUS:
			ret = GetGPOStatus(data, len);
			break;
		case SFIELD_SET_AV_PROCESS:
			ret = SetSysSet(data, len, SYS_MSG_SET_AV_PROCESS, offsetBegin);
			break;
		case SFIELD_SET_GO_ALARM_DURATION:
			ret = SetSysSet(data, len, SYS_MSG_SET_GO_ALARM_DUTATION, offsetBegin);
			break;
		case SFIELD_SET_EXTRA_RTSP_MULTI:
			ret = SetSysSet(data, len, SYS_MSG_SET_EXTRA_RTSP_MULTI, offsetBegin);
			break;
		case SFIELD_SYS_QUIT:
			ret = SetSysSet(data, len, SYS_MSG_QUIT, offsetBegin);
			break;
		case SFIELD_SET_RESETBUTTON_FACTORY_VALUE:
			ret = SetSysSet(data, len, SYS_MSG_SET_RESET_BUTTON_TEST, offsetBegin);
			break;
		case SFIELD_SET_ECHO_CANCEL_INIT:
			ret = SetSysSet(data, len, SYS_MSG_SET_ECHO_CANCEL_INIT, offsetBegin);
			break;
#if IPNC_PELCOD_FEATURE
		case SFIELD_SET_RS485:
			ret = SetSysSet(data, len, SYS_MSG_SET_RS485, offsetBegin);
			break;
		case SFIELD_PELCOD_MOVING:
			ret = SetSysSet(data, len, SYS_MSG_PELCOD_MOVING, offsetBegin);
			break;
		case SFIELD_PELCOD_ADD_PRESET:
			ret = SetSysSet(data, len, SYS_MSG_PELCOD_ADD_PRESET, offsetBegin);
			break;
		case SFIELD_PELCOD_DEL_PRESET:
			ret = SetSysSet(data, len, SYS_MSG_PELCOD_DEL_PRESET, offsetBegin);
			break;
		case SFIELD_PELCOD_GOTO_PRESET:
			ret = SetSysSet(data, len, SYS_MSG_PELCOD_GOTO_PRESET, offsetBegin);
			break;
		case SFIELD_PELCOD_SETHOME:
			ret = SetSysSet(data, len, SYS_MSG_PELCOD_SET_HOME, offsetBegin);
			break;
		case SFIELD_PELCOD_GOHOME:
			ret = SetSysSet(data, len, SYS_MSG_PELCOD_GO_HOME, offsetBegin);
			break;
		case SFIELD_PELCOD_PATROLLING:
			ret = SetSysSet(data, len, SYS_MSG_PELCOD_PATROLLING, offsetBegin);
			break;
		case SFIELD_RS485_TESTCMD:
			ret = SetSysSet(data, len, SYS_MSG_RS485_TESTCMD, offsetBegin);
			break;
#endif	//IPNC_PELCOD_FEATURE
#if IPNC_IRCUT_FEATURE
		case SFIELD_IRCUT_MODE:
			ret = SetSysSet(data, len, SYS_MSG_IRCUT_MODE, offsetBegin);
			break;
#endif //IPNC_IRCUT_FEATURE
#if	Intelligence_FEATURE
		case SFIELD_OV_ENABLE:
			ret = SetSysSet(data, len, SYS_MSG_OV_ENABLE, offsetBegin);
			break;
#endif	//Intelligence_FEATURE
#if IVS_FEATURE
		case SFIELD_GET_IVS_VIDEO_SETTING:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_VIDEO_SETTING);
			//fprintf(stderr,"SFIELD_GET_IVS_VIDEO_SETTING - data = %s, len = %d\n", data, len);
			break;
		case SFIELD_UPDATE_IVS_VIDEO_SETTING:
			//fprintf(stderr,"SFIELD_UPDATE_IVS_VIDEO_SETTING - data = %s, len = %d\n", data, len);
			ret = GetSysSet(data, len, SYS_MSG_UPDATE_IVS_VIDEO_SETTING);
			break;
		case SFIELD_UPDATE_IVS_CONFIG:
			ret = SetSysSet(data, len, SYS_MSG_UPDATE_IVS_CONFIG, offsetBegin);
			break;
		case SFIELD_SET_IVS_ENABLE:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ENABLE, offsetBegin);
			break;
		case SFIELD_GET_IVS_ENABLE:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ENABLE);
			break;
		case SFIELD_SET_IVS_DEMO:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_DEMO, offsetBegin);
			break;
		case SFIELD_SET_IVS_PARAMETER:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_PARAMETER, offsetBegin);
			break;
		case SFIELD_GET_IVS_PARAMETER:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_PARAMETER);
			break;
		case SFIELD_LOAD_IVS_PARAMETER_FROM_FILE:
			ret = SetSysSet(data, len, SYS_MSG_LOAD_IVS_PARAMETER_FROM_FILE, offsetBegin);
			break;
		case SFIELD_SAVE_IVS_PARAMETER_TO_FILE:
			ret = SetSysSet(data, len, SYS_MSG_SAVE_IVS_PARAMETER_TO_FILE, offsetBegin);
			break;
		case SFIELD_SET_IVS_FUNCTION_MOTION_DETECT:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_FUNCTION_MOTION_DETECT, offsetBegin);
			break;
		case SFIELD_GET_IVS_FUNCTION_MOTION_DETECT:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_FUNCTION_MOTION_DETECT);
			break;
		case SFIELD_SET_IVS_FUNCTION_OBJECT_SIZE_FILTER:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_FUNCTION_OBJECT_SIZE_FILTER, offsetBegin);
			break;
		case SFIELD_GET_IVS_FUNCTION_OBJECT_SIZE_FILTER:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_FUNCTION_OBJECT_SIZE_FILTER);
			break;
		case SFIELD_SET_IVS_FUNCTION_FORBIDDEN_ZONE:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_FUNCTION_FORBIDDEN_ZONE, offsetBegin);
			break;
		case SFIELD_GET_IVS_FUNCTION_FORBIDDEN_ZONE:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_FUNCTION_FORBIDDEN_ZONE);
			break;
		case SFIELD_SET_IVS_FUNCTION_UNINTERESTED_ZONE:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_FUNCTION_UNINTERESTED_ZONE, offsetBegin);
			break;
		case SFIELD_GET_IVS_FUNCTION_UNINTERESTED_ZONE:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_FUNCTION_UNINTERESTED_ZONE);
			break;
		case SFIELD_SET_IVS_FUNCTION_SHADOW_REJECTION:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_FUNCTION_SHADOW_REJECTION, offsetBegin);
			break;
		case SFIELD_GET_IVS_FUNCTION_SHADOW_REJECTION:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_FUNCTION_SHADOW_REJECTION);
			break;
		case SFIELD_SET_IVS_FUNCTION_SMALL_VIBRATION_REJECTION:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_FUNCTION_SMALL_VIBRATION_REJECTION, offsetBegin);
			break;
		case SFIELD_GET_IVS_FUNCTION_SMALL_VIBRATION_REJECTION:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_FUNCTION_SMALL_VIBRATION_REJECTION);
			break;
		case SFIELD_SET_IVS_FUNCTION_SCENE_CHANGE_DETECTION:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_FUNCTION_SCENE_CHANGE_DETECTION, offsetBegin);
			break;
		case SFIELD_GET_IVS_FUNCTION_SCENE_CHANGE_DETECTION:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_FUNCTION_SCENE_CHANGE_DETECTION);
			break;
		case SFIELD_SET_IVS_FUNCTION_CROWD_DETECTION:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_FUNCTION_CROWD_DETECTION, offsetBegin);
			break;
		case SFIELD_GET_IVS_FUNCTION_CROWD_DETECTION:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_FUNCTION_CROWD_DETECTION);
			break;
		case SFIELD_SET_IVS_OBJECT_DISPLAY_TITLE:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_OBJECT_DISPLAY_TITLE, offsetBegin);
			break;
		case SFIELD_GET_IVS_OBJECT_DISPLAY_TITLE:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_OBJECT_DISPLAY_TITLE);
			break;
		case SFIELD_SET_IVS_OBJECT_DISPLAY_CROWD:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_OBJECT_DISPLAY_CROWD, offsetBegin);
			break;
		case SFIELD_GET_IVS_OBJECT_DISPLAY_CROWD:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_OBJECT_DISPLAY_CROWD);
			break;
		case SFIELD_SET_IVS_OBJECT_SIZE_FILTER:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_OBJECT_SIZE_FILTER, offsetBegin);
			break;
		case SFIELD_GET_IVS_OBJECT_SIZE_FILTER:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_OBJECT_SIZE_FILTER);
			break;
		case SFIELD_SET_IVS_DETECTED_OBJECT:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_DETECTED_OBJECT, offsetBegin);
			break;
		case SFIELD_GET_IVS_DETECTED_OBJECT:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_DETECTED_OBJECT);
			break;
		case SFIELD_SET_IVS_MERGE_RECT:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_MERGE_RECT, offsetBegin);
			break;
		case SFIELD_GET_IVS_MERGE_RECT:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_MERGE_RECT);
			break;
		case SFIELD_SET_IVS_SENSITIVITY:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_SENSITIVITY, offsetBegin);
			break;
		case SFIELD_GET_IVS_SENSITIVITY:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_SENSITIVITY);
			break;
		case SFIELD_SET_IVS_SHADOW_REJECTION:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_SHADOW_REJECTION, offsetBegin);
			break;
		case SFIELD_GET_IVS_SHADOW_REJECTION:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_SHADOW_REJECTION);
			break;
		case SFIELD_SET_IVS_SMALL_VIBRATION_REJECTION:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_SMALL_VIBRATION_REJECTION, offsetBegin);
			break;
		case SFIELD_GET_IVS_SMALL_VIBRATION_REJECTION:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_SMALL_VIBRATION_REJECTION);
			break;
		case SFIELD_SET_IVS_SCENE_CHANGE:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_SCENE_CHANGE, offsetBegin);
			break;
		case SFIELD_GET_IVS_SCENE_CHANGE:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_SCENE_CHANGE);
			break;
		case SFIELD_SET_IVS_ZONE_FORBIDDEN_CONFIG:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_FORBIDDEN_CONFIG, offsetBegin);
			break;
		case SFIELD_GET_IVS_ZONE_FORBIDDEN_CONFIG:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_FORBIDDEN_CONFIG);
			break;
		case SFIELD_SET_IVS_ZONE_FORBIDDEN_LOCATION:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_FORBIDDEN_LOCATION, offsetBegin);
			break;
		case SFIELD_SET_IVS_ZONE_FORBIDDEN_LOCATION_X:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_FORBIDDEN_LOCATION_X, offsetBegin);
			break;
		case SFIELD_SET_IVS_ZONE_FORBIDDEN_LOCATION_Y:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_FORBIDDEN_LOCATION_Y, offsetBegin);
			break;
		case SFIELD_GET_IVS_ZONE_FORBIDDEN_LOCATION:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_FORBIDDEN_LOCATION);
			break;
		case SFIELD_GET_IVS_ZONE_FORBIDDEN_LOCATION_X:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_FORBIDDEN_LOCATION_X);
			break;
		case SFIELD_GET_IVS_ZONE_FORBIDDEN_LOCATION_Y:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_FORBIDDEN_LOCATION_Y);
			break;
		case SFIELD_SET_IVS_ZONE_UNINTERESTED_CONFIG:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_UNITERESTED_CONFIG, offsetBegin);
			break;
		case SFIELD_GET_IVS_ZONE_UNINTERESTED_CONFIG:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_UNITERESTED_CONFIG);
			break;
		case SFIELD_SET_IVS_ZONE_UNINTERESTED_LOCATION:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_UNITERESTED_LOCATION, offsetBegin);
			break;
		case SFIELD_SET_IVS_ZONE_UNINTERESTED_LOCATION_X:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_UNITERESTED_LOCATION_X, offsetBegin);
			break;
		case SFIELD_SET_IVS_ZONE_UNINTERESTED_LOCATION_Y:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_UNITERESTED_LOCATION_Y, offsetBegin);
			break;
		case SFIELD_GET_IVS_ZONE_UNINTERESTED_LOCATION:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_UNITERESTED_LOCATION);
			break;
		case SFIELD_GET_IVS_ZONE_UNINTERESTED_LOCATION_X:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_UNITERESTED_LOCATION_X);
			break;
		case SFIELD_GET_IVS_ZONE_UNINTERESTED_LOCATION_Y:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_UNITERESTED_LOCATION_Y);
			break;
		case SFIELD_SET_IVS_ZONE_CROWD_CONFIG:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_CROWD_CONFIG, offsetBegin);
			break;
		case SFIELD_GET_IVS_ZONE_CROWD_CONFIG:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_CROWD_CONFIG);
			break;
		case SFIELD_SET_IVS_ZONE_CROWD_LOCATION:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_CROWD_LOCATION, offsetBegin);
			break;
		case SFIELD_SET_IVS_ZONE_CROWD_LOCATION_X:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_CROWD_LOCATION_X, offsetBegin);
			break;
		case SFIELD_SET_IVS_ZONE_CROWD_LOCATION_Y:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_ZONE_CROWD_LOCATION_Y, offsetBegin);
			break;
		case SFIELD_GET_IVS_ZONE_CROWD_LOCATION:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_CROWD_LOCATION);
			break;
		case SFIELD_GET_IVS_ZONE_CROWD_LOCATION_X:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_CROWD_LOCATION_X);
			break;
		case SFIELD_GET_IVS_ZONE_CROWD_LOCATION_Y:
			ret = GetSysSet(data, len, SYS_MSG_GET_IVS_ZONE_CROWD_LOCATION_Y);
			break;
		case SFIELD_SET_IVS_RESET:
			ret = SetSysSet(data, len, SYS_MSG_SET_IVS_RESET, offsetBegin);
			break;
		case SFIELD_RUN_IVS_TEST:
			ret = SetSysSet(data, len, SYS_MSG_RUN_IVS_TEST, offsetBegin);
			break;
#endif	// IVS_FEATURE
		case SFIELD_SET_PORT_MAPPING://
			ret = SetSysSet(data, len, SYS_MSG_SET_PORT_MAPPING, offsetBegin);
			break;
		case SFIELD_DEL_PORT_MAPPING://
			ret = SetSysSet(data, len, SYS_MSG_DEL_PORT_MAPPING, offsetBegin);
			break;
		case SFIELD_START_RESTART_ALL:
			ret = SetSysSet(data, len, SYS_MSG_START_RESTART_ALL, offsetBegin);
			break;
		case SFIELD_STOP_REC:
			ret = SetSysSet(data, len, SYS_MSG_STOP_REC, offsetBegin);
			break;
		case SFIELD_RESTART_SCHEDULER: // add by kenny chiu 20110428
			break;
		case SFIELD_SET_IRIS_MODE:	// kenny chiu 20110512
			ret = SetSysSet(data, len, SYS_MSG_SET_IRIS_MODE, offsetBegin);
			break;
		case SFIELD_SET_WDR_MODE:	// Cato@20120508
			ret = SetSysSet(data, len, SYS_MSG_SET_WDR_MODE, offsetBegin);
			break;
		case SFIELD_SET_TV_OUT:	// kenny chiu 20110512
			ret = SetSysSet(data, len, SYS_MSG_SET_TV_OUT, offsetBegin);
			break;
		case SFIELD_SET_MJPG_STATUS:
			ret = SetSysSet(data, len, SYS_MSG_SET_MJPG_STATUS, offsetBegin);
			break;
		case SFIELD_SET_RTSP_RESTART:
			ret = SetSysSet(data, len, SYS_MSG_SET_RTSP_RESTART, offsetBegin);
			break;
		case SFIELD_SET_GPIO_PIN:
			ret = SetSysSet(data, len, SYS_MSG_SET_GPIO_PIN, offsetBegin);
			break;
		case SFIELD_SET_GPIO_STATUS:
			ret = SetSysSet(data, len, SYS_MSG_SET_GPIO_STATUS, offsetBegin);
			break;
		case SFIELD_SET_MPEG41_FRAMERATE_DYNAMIC://
			ret = SetSysSet(data, len, SYS_MSG_SET_MPEG41_FRAMERATE_DYNAMIC, offsetBegin);
			break;
		case SFIELD_SET_AEWB_RESET:
			ret = SetSysSet(data, len, SYS_MSG_SET_AEWB_RESET, offsetBegin);
			break;
		case SFILED_SET_MOBILE_FRAMERATE:
			ret = SetSysSet(data, len, SYS_MSG_SET_MOBILE_FRAMERATE, offsetBegin);
			break;
		case SFILED_SET_MOBILE_RATECONTROL:
			ret = SetSysSet(data, len, SYS_MSG_SET_MOBILE_RATECONTROL, offsetBegin);
			break;
		case SFILED_SET_MOBILE_QUALITY:
			ret = SetSysSet(data, len, SYS_MSG_SET_MOBILE_QUALITY, offsetBegin);
			break;
		case SFILED_SET_MOBILE_BITRATE:
			ret = SetSysSet(data, len, SYS_MSG_SET_MOBILE_BITRATE, offsetBegin);
			break;
		case SFILED_SET_AUTOIRIS_ACTION:
			ret = SetSysSet(data, len, SYS_MSG_SET_AUTOIRIS_ACTION, offsetBegin);
			break;
		case SFIELD_SET_AUDIOBROADCASTPORT:
			ret = SetSysSet(data, len, SYS_MSG_SET_AUDIOBROADCASTPORT, offsetBegin);
			break;
		case SFIELD_SET_EWDR_MODE:
			ret = SetSysSet(data, len, SYS_MSG_SET_EWDR_MODE, offsetBegin);
			break;
		case SFIELD_SET_HTTPSPORT:
			ret = SetSysCommon(data, len, SYS_MSG_SET_HTTPSPORT);
			break;
/* Onvif */
        case SFIELD_ADD_PROFILE:
                        ret = CreateMedProfile(data, len);
                        break;
        case SFIELD_ADD_VIDEOSOURCE_CONF:
                        ret = AddVideoSourceConfiguration(data, len);
                        break;
        case SFIELD_SET_VIDEOSOURCE_CONF:
                        ret = SetVideoSourceConfiguration(data, len);
                      break;
        case SFIELD_ADD_VIDEOENCODER_CONF:
                        ret = AddVideoEncoderConfiguration(data, len);
                        break;
        case SFIELD_SET_VIDEOENCODER_CONF:
                        ret = SetVideoEncoderConfiguration(data, len);
                        break;
		case SFIELD_DEL_VIDEOENCODER_CONF:
                        ret = RemoveVideoEncoderConfiguration(data, len);
                        break;
		case SFIELD_SET_AUDIO_SOURCE:
                        ret = SetAudioSourceConfiguration(data, len);
                        break;
	 	case SFIELD_ADD_AUDIOSOURCE_CONF:
	     		        ret = AddAudioSourceConfiguration(data, len);
			            break;
		case SFIELD_ADD_AUDIOENCODER_CONF:
			            ret = AddAudioEncoderConfiguration(data, len);
			            break;
		case SFIELD_SET_AUDIOENCODER_CONF:
			            ret = SetAudioEncoderConfiguration(data, len);
			            break;
		case SFIELD_DEL_AUDIO_SOURCE:
			            ret = RemoveAudioSourceConfiguration(data, len);
			            break;
		case SFIELD_DEL_AUDIO_ENCODER:
			            ret = RemoveAudioEncoderConfiguration(data, len);
			            break;
		case SFIELD_DEL_PROFILE:
			            ret = DelProfile(data, len);
                        break;
        case SFIELD_DIS_MODE:
                        ret = SetDiscoveryMode(data,len);
                        break;
        case SFIELD_ADD_SCOPE:
                        ret = AddScopes(data,len);
                        break;
        case SFIELD_DEL_SCOPE:
                        ret = DeleteScopes(data,len);
                        break;
        case SFIELD_SET_VIDEOANALYTICS_CONF:
                        ret = SetVideoAnalytics(data, len);
                        break;
		case SFIELD_SET_VIDEOOUTPUT_CONF:
                        ret = SetVideoOutput(data, len);
                        break;
		case SFIELD_SET_RELAYS_CONF:
                        ret = SetRelays(data, len);
                        break;
		case  SFIELD_SET_RELAY_LOGICALSTATE:
			            ret = SetRelayLogicalState(data, len);
			            break;
		case SFIELD_SET_METADATA:
                        ret = SetMetadata(data, len);
                        break;
		case SFIELD_ADD_METADATA:
                        ret = AddMetadata(data, len);
                       break;
/* End-Onvif */

		case SFIELD_SET_SW_WATCH_DOG_EANBLE:
			ret = SetSysSet(data, len, SYS_MSG_SET_SW_WATCH_DOG_ENABLE, offsetBegin);
			break;
		case SFIELD_SET_EDIMAX_P2P_VALUE:
			ret = SetSysSet(data, len, SYS_MSG_SET_EDIMAX_P2P_VALUE, offsetBegin);
			break;
		case SFIELD_SET_WDTRESET:
//			fprintf(stderr,"[%s:%d] data = %d, %d, %d\n",__func__,__LINE__,data,len,offsetBegin);
			ret = SetSysSet(data, len, SYS_MSG_SET_WDTRST, offsetBegin);
			break;
		default:
			ret = -1;
			break;
	}
	//offsetBegin = offsetEnd + 1; //Add by Joseph for offset
	return ret;
}
