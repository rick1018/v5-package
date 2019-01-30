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
#if defined (__cplusplus)
extern "C" {
#endif

#ifndef __SYSTEM_DEFAULT_H__
#define __SYSTEM_DEFAULT_H__

#include "ipnc_feature.h"
#include "ipnc_ver.h" // add by kenny chiu 20110520

/* Macro */
#define IPV4(a,b,c,d)		((d<<24)|(c<<16)|(b<<8)|(a))
#define TITLE_DEFAULT				MODEL_NAME //"MT7620"
#define HOST_DEFAULT                MODEL_NAME //"IPNC-RDK"
#define MODELNAME_DEFAULT			MAIN_MODEL //"IPNC_RDK_DM36x"

#define SYS_BUF_FILE	"/mnt/firmware/sysenv.cfg"

#define EXTRA_INFO_PATH     "/mnt/nand/extra.info"
#define EXTRA_INFO2_PATH    "/mnt/nand/extra.info2"         
#define LIMIT_USER_NUM_PATH "/mnt/nand/limit_user_num.info"
#define THREE_GPP_PATH      "/mnt/nand/three_gpp.info"      
#define SNMP_CONF_PATH      "/mnt/nand/snmp.conf"
#define SEC8021X_CONF_PATH  "/mnt/nand/wpa_supplicant.conf"
#define VENDER_CONF         "/mnt/nand/vender.conf"

#define TEMP_FOLDER		"/tmp"

#define DEF_BACKUP	"/mnt/nand/factory_reset_backup.dat" // add by kenny chiu 20130422
#define SYS_FILE	  "/mnt/nand/sysenv.cfg"
#define SYS_DMVATZ_FILE     "/mnt/nand/sysenv_tz.cfg"
#define SYS_DMVAIMD_FILE    "/mnt/nand/sysenv_imd.cfg"
#define SYS_DMVAOC_FILE     "/mnt/nand/sysenv_oc.cfg"
#define SYS_DMVASMETA_FILE  "/mnt/nand/sysenv_smeta.cfg"
#define LOG_FILE	  "/mnt/nand/logfile.log"
//#define LOG_DB_FILE	"/mnt/nand/logdbfile.log"
#define DMVA_LOG_FILE		"/mnt/nand/dmvalogfile.log"
#define REBOOT_FLAG         "/tmp/sys_reboot_flag"
#define AUDIO_CODEC_TYPE  "AAC" // "G711" // add by Kenny Chiu 20151013

#define SYS_LOG_FILE 		"/mnt/nand/systemlog.log"
#define ACCESS_LOG_FILE 	"/mnt/nand/accesslog.log"
#define SUPPORT_REPORT 	    "/var/www/SupportReport.zip"

#define SMTP_TEST_FLAG           "/tmp/smtp_test_flag"
#define SMTP_PPPOE_FLAG          "/tmp/smtp_pppoe_flag"
#define SMTP_IPNOTIFICATION_FLAG "/tmp/smtp_ipnotification_flag"
#define SMTP_TEST_FAIL       "/tmp/smtp_test_fail"
#define SMTP_TEST_SUCCESS    "/tmp/smtp_test_success"

#define FTP_TEST_FAIL        "/tmp/ftp_test_fail"
#define FTP_TEST_SUCCESS     "/tmp/ftp_test_success"

#define PATROL_CONF         "/mnt/nand/PatrolConf.conf"

#define EVENT_MEDIA_RUNNING  "/tmp/event_media_running"	

//#define RECORD_MEDIA_NONE		0x00	
//#define RECORD_MEDIA_CLEAR		0x00	
//#define RECORD_MEDIA_ENABLE		0x01	
//#define RECORD_MEDIA_SDCARD		0x02	
//#define RECORD_MEDIA_SAMBA		0x04	
//#define RECORD_MEDIA_USB		0x08	
//#define RECORD_MEDIA_TMP		0x10	
//#define RECORD_MEDIA_DISABLE	0x1E	
//#define RECORD_MEDIA_ALL		0x1F	
//#define RECORD_MEDIA_DEFAULT	RECORD_MEDIA_SDCARD		

#define SAMBA_DIR           "/tmp/smb_dir"
#define SAMBA_TEST_DIR      "/tmp/smb_test_dir"
#define SAMBA_REC_DIR       "/tmp/smb_dir"
#define SAMBA_REC_FOLDER    "/tmp/smb_dir/Record"       

#define SAMBA_TO_START       "/tmp/samba_to_start"
#define SAMBA_CAN_START      "/tmp/samba_can_start"
#define SAMBA_REC_TO_START   "/tmp/samba_rec_to_start"
#define SAMBA_REC_CAN_START  "/tmp/samba_rec_can_start"

#define SAMBA_MOUNT_SH      "/tmp/samba_mount.sh"
#define SAMBA_STATUS_SH     "/tmp/samba_status.sh"
#define SAMBA_REC_MOUNT_SH  "/tmp/samba_rec_mount.sh"
#define SAMBA_REC_STATUS_SH "/tmp/samba_rec_status.sh"

#define SAMBE_ERROR_FLAG    "/tmp/samba_error_flag"
#define SAMBE_SUCCESS_FLAG  "/tmp/samba_success_flag"
#define SAMBA_SUCCESS       "/tmp/samba_success"
#define SAMBA_FAIL          "/tmp/samba_fail"
#define SAMBA_TEST_FAIL      "/tmp/samba_test_fail"
#define SAMBA_TEST_SUCCESS   "/tmp/samba_test_success"

#define SAMBE_REC_ERROR_FLAG    "/tmp/samba_rec_error_flag"
#define SAMBE_REC_SUCCESS_FLAG  "/tmp/samba_rec_success_flag"
#define SAMBA_REC_SUCCESS       "/tmp/samba_rec_success"
#define SAMBA_REC_FAIL          "/tmp/samba_rec_fail"

#define SAMBA_REC_SUCCESS        "/tmp/samba_rec_success"
#define SAMBA_REC_FAIL           "/tmp/samba_rec_fail"
#define SAMBA_REC_SUCCESS_STATUS "/tmp/samba_rec_success_status"
#define SAMBA_REC_FAIL_STATUS    "/tmp/samba_rec_fail_status"
#define SAMBA_STOP_MOUNT         "/tmp/samba_stop_mount"
#define SAMBA_REC_STOP_MOUNT     "/tmp/samba_rec_stop_mount"

#define SAMBA_EVENT_FOLDER	      "/tmp/smb_dir/Event"      	
#define SAMBA_EVENT_MOUNT_SUCCESS "/tmp/samba_event_mount_success"      
#define SAMBA_EVENT_MOUNT_FAIL    "/tmp/samba_event_mount_fail"         

#define SAMBA_EVENT_SUCCESS "/tmp/samba_event_success"
#define SAMBA_EVENT_FAIL    "/tmp/samba_event_fail"

#define SAMBA_EVENT_TEST_SUCCESS "/tmp/samba_event_test_success"
#define SAMBA_EVENT_TEST_FAIL    "/tmp/samba_event_test_fail"

#define DDNS_UPDATE_STATUS_FLAG   "/tmp/ipupdate_status_flag"	
#define DIPS_UPDATE_STATUS_FLAG   "/tmp/dips_status_flag"		
#define DIPS_UPDATE_STATUS_FLAG_BUSY   "/tmp/dips_status_flag_busy"		
#if HTTP_NOTIFY_FEATURE		
#define EVENT_HTTP_NOTIFY_STATUS_FLAG   "/tmp/event_http_notify_status_flag"		
#define EVENT_HTTP_NOTIFY_STATUS_FLAG_BUSY   "/tmp/event_http_notify_status_flag_busy"		
#define EVENT_HTTP_NOTIFY_CONFIG   "/tmp/event_http_notify_config"		
#define EVENT_HTTP_NOTIFY_CONFIG_BUSY   "/tmp/event_http_notify_config_busy"		
#endif						

#define OSD_align(value, align)   ((( (value) + ( (align) - 1 ) ) / (align) ) * (align) )	

#define OSD_ALIGN_FLOOR(value, align)   (( (value) / (align) ) * (align) )	
#define OSD_ALIGN_CEIL(value, align)    OSD_align(value, align) 			
#define OSD_ALIGN_X				2											
#define OSD_ALIGN_Y				2											
#define OSD_ALIGN_WIDTH			8											
#define OSD_ALIGN_HEIGHT		2											
#define OSD_ALIGN_THINKNESS		2											

#if UPNP_TMP_DIR
#define UPNPAV_PATH         "/tmp/upnpav"
#else
#define UPNPAV_PATH         "/mnt/firmware/upnpav"
#endif

#define BONJOUR_CONF_PATH   "/mnt/nand/bonjour_conf"

#define USB_DIR             "/tmp/usb_dir"
#define USB_MOUNT_SH        "/mnt/nand/usb_mount.sh"
#define USB_SDA             "/tmp/usb_sda"
#define USB_SDB             "/tmp/usb_sdb"
#define USB_ERROR_FLAG      "/tmp/usb_error_flag"
#define USB_SUCCESS_FLAG    "/tmp/usb_success_flag"
#define USB_BACKUP_SUCCESS  "/tmp/usb_backup_success"
#define USB_BACKUP_ERROR    "/tmp/usb_backup_error"
#define USB_RESTORE_SUCCESS "/tmp/usb_restore_success"
#define USB_RESTORE_ERROR   "/tmp/usb_restore_error"
#define USB_RESTORE_FILE_SUCCESS   "/tmp/usb_restore_file_sucess"
#define USB_RESTORE_FILE_ERROR     "/tmp/usb_restore_file error"
#define USB_BUSY_FLAG       "/tmp/usb_busy_flag"
#define USB_EVENT_DIR       "/tmp/usb_dir/Event"		
#define USB_RECORD_DIR      "/tmp/usb_dir/Record"		
#define USB_REC_DIR         "/tmp/usb_dir/Record"
#define USB_SUCCESS_STATUS   "/tmp/usb_success_status"
#define USB_ERROR_STATUS     "/tmp/usb_error_status"

#define SD_PATH_NODE	"/dev/mmcblk0"
#define SD_PATH_NODE_P1	"/dev/mmcblk0p1"
#define SD_CMD_MKDIR	  "mkdir -p /tmp/mmc_dir\n"						/* Dean - 20140324 */
#define SD_PATH_MOUNT	  "/tmp/mmc_dir"/*"/mnt/mmc/ipnc"*/		/* Dean - 20140324 */
#define SD_CMD_MOUNT	  "mount -t vfat /dev/mmcblk0 /tmp/mmc_dir\n"						/* Dean - 20140324 */
#define SD_CMD_MOUNT_P1	  "mount -t vfat /dev/mmcblk0p1 /tmp/mmc_dir\n"						/* Dean - 20140324 */
#define SD_CMD_UMOUNT	  "umount /tmp/mmc_dir\n"						/* Dean - 20140324 */
#define SD_CMD_FORMAT	  "mkdosfs /dev/mmcblk0\n"					/* Dean - 20140324 */
#define SD_CMD_FORMAT_P1	  "mkdosfs /dev/mmcblk0p1\n"					/* Dean - 20140324 */

#define SD_PATH_RECORD	  "/tmp/mmc_dir/Record"					/* Dean - 20140324 */
#define SD_PATH_EVENT	  "/tmp/mmc_dir/Event"					/* Dean - 20140324 */
#define SD_CMD_RECORD_MKDIR	  "mkdir -p /tmp/mmc_dir/Record\n"						/* Dean - 20140324 */
#define SD_CMD_EVENT_MKDIR	  "mkdir -p /tmp/mmc_dir/Event\n"						/* Dean - 20140324 */
#define SD_CMD_RECORD_RM	  "rm -f /tmp/mmc_dir/Record"					/* Dean - 20140324 */
#define SD_CMD_EVENT_RM	  "rm -f /tmp/mmc_dir/Event"					/* Dean - 20140324 */

#define MMC_STATUS_FLAG    "/tmp/mmc_status_flag"	
#define MMC_STATUS_FLAG_BUSY    "/tmp/mmc_status_flag_busy"	
#define MMC_DIR            "/tmp/mmc_dir"
#define MMC_EVENT_DIR    "/tmp/mmc_dir/Event"			
#define MMC_RECORD_DIR    "/tmp/mmc_dir/Record"			
#define MMC_REC_DIR        "/tmp/mmc_dir"						
//#define MMC_REC_BKP_DIR    "/tmp/mmc_dir/Backup"				
#define MMC_ERROR_FLAG     "/tmp/mmc_error_flag"
#define MMC_SUCCESS_FLAG   "/tmp/mmc_success_flag"
#define MMC_SUCCESS_FLAG_0 "/tmp/mmc_success_flag0"	
#define MMC_SUCCESS_FLAG_1 "/tmp/mmc_success_flag1"	
#define MMC_DIR_REMOVE_FLAG "/tmp/mmc_dir_remove_flag"	
#define MMC_BACKUP_SUCCESS "/tmp/mmc_backup_success"
#define MMC_BACKUP_ERROR   "/tmp/mmc_backup_error"
#define MMC_RESTORE_SUCCESS "/tmp/mmc_restore_success"
#define MMC_RESTORE_ERROR   "/tmp/mmc_restore_error"
#define MMC_BUSY_FLAG       "/tmp/mmc_busy_flag"
#define MMC_QUERY_FLAG       "/tmp/sdcard_query"        
#define MMC_EJECT_FLAG       "/tmp/mmc_eject_flag"
#define MMC_SUCCESS_STATUS   "/tmp/mmc_success_status"
#define MMC_ERROR_STATUS     "/tmp/mmc_error_status"
#define MMC_RESTORE_FILE_SUCCESS   "/tmp/mmc_restore_file_sucess"
#define MMC_RESTORE_FILE_ERROR     "/tmp/mmc_restore_file error"

#define JABBER_ERROR_FLAG   "/tmp/jabber_error_flag"
#define JABBER_SUCCESS_FLAG "/tmp/jabber_success_flag"
#define JABBER_ERROR        "/tmp/jabber_error"
#define JABBER_SUCCESS      "/tmp/jabber_success"

#define PROC_IFINET6_PATH "/proc/net/if_inet6"

#define WIS_AUDIO_DISABLE "/tmp/wis-streamer_audio_disable"

#if UPNP_TMP_DIR
#define LIVE_UPNPAV_FILE  "/tmp/upnpav/live.m3u"
#else
#define LIVE_UPNPAV_FILE  "/mnt/firmware/upnpav/live.m3u"
#endif

#if TV_OUT_FEATURE
#define TV_OUT_FLAG "/tmp/tv_out_flag"
#endif

#define RECORD_VIDEO_PARAM			"/tmp/rec_video_param"						/* AVI Record Msg - Dean - 20130530 */
#define AVI_RECORD_STATUS_FLAG		"/tmp/avi_record_status_flag"					/* AVI Record Msg - Dean - 20130530 */
#define AVI_RECORD_STATUS_FLAG_BUSY	"/tmp/avi_record_status_flag_busy"				/* AVI Record Msg - Dean - 20130530 */
#define AVI_RECORD_PARAM				"/tmp/avi_record_param"						/* AVI Record Msg - Dean - 20130530 */
#define AVI_RECORD_PARAM_BUSY			"/tmp/avi_record_param_busy"					/* AVI Record Msg - Dean - 20130530 */
#define AVI_RECORD_RUN					"/tmp/avi_record_run"							/* AVI Record Msg - Dean - 20130530 */
#define SAMBA_CHECK_BUSY				"/tmp/samba_check_busy"						/* AVI Record Msg - Dean - 20130530 */


#define NET_BW_INFO "/tmp/net_bw_info"

#define RECYCLE_DB_FILE	".recycle_file_db"
#define RECYCLE_DB_RECORD_FILE	".recycle_record_file_db"

//#define TITLE_DEFAULT				MODEL_NAME
#if EDIMAX_OEM
#define IP_DEFAULT					IPV4(192,168,2,3)
#define GATEWAY_DEAFULT				IPV4(192,168,2,1)
#define DNS_DEFAULT					IPV4(192,168,2,1)
#define STR_DEFAULT_IP              "192.168.2.3"
#define STR_DEFAULT_GATEWAY         "192.168.2.1"
#define STR_DEFAULT_DNS             "192.168.2.1"
#define STR_DEFAULT_DNS2            "192.168.2.2"				/* Add DNS Default - Dean - 20120912 */
#elif PLANET_OEM
#define IP_DEFAULT					IPV4(192,168,0,20)
#define GATEWAY_DEAFULT				IPV4(192,168,0,1)
#define DNS_DEFAULT					IPV4(192,168,0,1)
#define STR_DEFAULT_IP              "192.168.0.20"
#define STR_DEFAULT_GATEWAY         "192.168.0.1"
#define STR_DEFAULT_DNS             "192.168.0.1"
#define STR_DEFAULT_DNS2            "192.168.0.2"				/* Add DNS Default - Dean - 20120912 */
#elif DIGISOL_OEM
#define IP_DEFAULT					IPV4(192,168,2,99)
#define GATEWAY_DEAFULT				IPV4(192,168,2,1)
#define DNS_DEFAULT					IPV4(192,168,2,1)
#define STR_DEFAULT_IP              "192.168.2.99"
#define STR_DEFAULT_GATEWAY         "192.168.2.1"
#define STR_DEFAULT_DNS             "192.168.2.1"
#define STR_DEFAULT_DNS2            "192.168.2.2"				/* Add DNS Default - Dean - 20120912 */
#elif FUHO_OEM
#define IP_DEFAULT					IPV4(192,168,1,201)
#define GATEWAY_DEAFULT				IPV4(192,168,1,1)
#define DNS_DEFAULT					IPV4(192,168,1,1)
#define STR_DEFAULT_IP              "192.168.1.201"
#define STR_DEFAULT_GATEWAY         "192.168.1.1"
#define STR_DEFAULT_DNS             "192.168.1.1"
#define STR_DEFAULT_DNS2            "192.168.1.2"				/* Add DNS Default - Dean - 20120912 */
#elif AIRLIVE_OEM
#define IP_DEFAULT					IPV4(192,168,1,100)
#define GATEWAY_DEAFULT				IPV4(192,168,1,1)
#define DNS_DEFAULT					IPV4(192,168,1,1)
#define STR_DEFAULT_IP              "192.168.1.100"
#define STR_DEFAULT_GATEWAY         "192.168.1.1"
#define STR_DEFAULT_DNS             "192.168.1.1"
#define STR_DEFAULT_DNS2            "192.168.1.2"				/* Add DNS Default - Dean - 20120912 */
#elif HIQVIEW_OEM
#define IP_DEFAULT					IPV4(192,168,3,166)
#define GATEWAY_DEAFULT				IPV4(192,168,3,1)
#define DNS_DEFAULT					IPV4(168,95,1,1)
#define STR_DEFAULT_IP              "192.168.3.166"
#define STR_DEFAULT_GATEWAY         "192.168.3.1"
#define STR_DEFAULT_DNS             "168.95.1.1"
#define STR_DEFAULT_DNS2            "168.95.192.1"				/* Add DNS Default - Dean - 20120912 */
#elif HENTEK_OEM
#define IP_DEFAULT					IPV4(192,168,1,188)
#define GATEWAY_DEAFULT				IPV4(192,168,1,1)
#define DNS_DEFAULT					IPV4(168,95,1,1)
#define STR_DEFAULT_IP              "192.168.1.188"
#define STR_DEFAULT_GATEWAY         "192.168.1.1"
#define STR_DEFAULT_DNS             "168.95.1.1"
#define STR_DEFAULT_DNS2            "168.95.192.1"				/* Add DNS Default - Dean - 20120912 */
#else
#define IP_DEFAULT					IPV4(192,168,1,99)
#define GATEWAY_DEAFULT				IPV4(192,168,1,1)
#define DNS_DEFAULT					IPV4(192,168,1,1)
#define STR_DEFAULT_IP              "192.168.1.99"
#define STR_DEFAULT_GATEWAY         "192.168.1.1"
#define STR_DEFAULT_DNS             "192.168.1.1"
#define STR_DEFAULT_DNS2            "192.168.1.2"				/* Add DNS Default - Dean - 20120912 */
#endif

#define NETMASK_DEFAULT				IPV4(255,255,255,0)
#define HTTP_PORT_DEFAULT			80
#define HTTPS_PORT_DEFAULT			443
#define HTTPS_PORT_MODE			    1
#define DHCP_ENABLE_DEFAULT			1
#define DEFAULTTIMEZONENAME			"GMT+8"
#if PCI_OEM
#define NTP_TIMEZONE_DEFAULT		46 //Tokyo
#define NTP_SERVER_DEFAULT			"pool.ntp.org"
#else
#define NTP_TIMEZONE_DEFAULT		4 //20
#define NTP_SERVER_DEFAULT			"tw.pool.ntp.org"
#endif
#define NTP_FREQUENCY_DEFAULT		0
#define TIME_FORMAT_DEFAULT			0
#define DAYLIGHT_DEFAULT			0
#define FQDN_DEFAULT				""
#define DEVICE_TYPE_DEFAULT			0x1687
#define MAC_DEFAULT					{0x12,0x34,0x56,0x78,0x9a,0xbc}
#define FTP_SERVER_IP_DEFAULT		""//"192.168.1.1"
#define FTP_USERNAME_DEFAULT		""
#define FTP_PASSWORD_DEFAULT		""
#define FTP_FOLDERNAME_DEFAULT		"" //"default_folder"
#define FTP_IMAGEAMOUNT_DEFAULT 	5
#define FTP_PID_DEFAULT				0
#define SMTP_SERVER_IP_DEFAULT		""//"192.168.1.1"
#define SMTP_SERVER_PORT_DEFAULT	25
#define SMTP_AUTHENTICATION_DEFAULT	0
#define SMTP_USERNAME_DEFAULT		""
#define SMTP_PASSWORD_DEFAULT		""
#define SMTP_SNEDER_EMAIL_DEFAULT	""
#define SMTP_RECEIVER_EMAIL_DEFAULT	""
#define SMTP_CC_DEFAULT				""
#define SMTP_SUBJECT_DEFAULT		MODEL_NAME
#define SMTP_TEXT_DEFAULT			"alarm mail."
#define SMTP_ATTACHMENTS_DEFAULT	5
#define SMTP_VIEW_DEFAULT			0
#define RESERVE_VALUE				""

#if FINEST_OEM
#define MPEG4_RESOLUTION_DEF		4 //5-->VGA 3-->720P
#define LIVE_RESOLUTION_DEF			4 //VGA
#elif PLANET_OEM
#define MPEG4_RESOLUTION_DEF		0 // 5-->VGA 3-->720P
#define LIVE_RESOLUTION_DEF			5 //VGA
#else
#ifdef SNS_OV9712
#define MPEG4_RESOLUTION_DEF		3 //5-->VGA 3-->720P 1-->1080P
#else
#define MPEG4_RESOLUTION_DEF		1 //5-->VGA 3-->720P 1-->1080P
#endif
#define LIVE_RESOLUTION_DEF			5 //5->VGA
#endif
#define MPEG4_RESOLUTION2_DEF		6 //6->QVGA
#define MPEG4_RESOLUTION3_DEF		3 //6->QVGA
#define MPEG4QUALITY_DEF			1 //0-->hight 1-->normal
#define MOBQUALITY_DEF			    1 //0-->hight 1-->normal
#define JPEGQUALITY_DEF				1 //0-->hight 1-->normal
#define DEWARP_DEF				    0 //source image 0-->disable 1-->enable, dewarp
#define DEWARP_DEF1				    1 //eptz 0-->disable 1-->enable
#define ROTATION_DEF				0
#define MIRROR_DEF					0
#define ENCRYPT_DEF					0
#define NOISEFLT_DEF				0
#define VIDSTB1_DEF 				0
#define VIDSTB2_DEF 				0
#define FDETECT_DEF 				1 //RS,20091120
#define REGUSR_DEF					"" //"SMAX"
#define DEMOCFG_DEF					0
#define	OSDSTREAM_DEF				0
#define	OSDWINNUM_DEF				0
#define	OSDWIN_DEF					0
#define	HISTOGRAM_DEF				0
#define GBCE_DEF					0
#define	VS_DEFAULT					0
#define	LDC_DEFAULT					0
#define	VNF_DEFAULT					0
#define	VNF_MODE_DEFAULT			2
#define	VNF_STRENGTH_DEFAULT		0
#define DYNRANGE_DEF				0
#define DYNRANGESTR_DEF				1

#if PROVIDEO_OEM
#define	OSDTEXT_DEF					"IPDevice"
#else
#define	OSDTEXT_DEF					"IPCAM"
#endif
#define	DUMMY_DEF					0
#if PLANET_OEM
#define	MJPEG_DISABLE_DEF 			1
#else // not PLANET_OEM
#define	MJPEG_DISABLE_DEF 			0
#endif
// add by kenny chiu 20121212 for exposure setting default value
//ITN  begin
#define SENSORTYPE_DEF                                     0
#define AEMODE_DEF                                            0
#define AUTOIRIS_DEF		                           0 //			autoiris;  //itn
#define VIEWDR_DEF							0 //			viewdr; //itn
#define XDNR_DEF                                                  0 //xdnr; itn
#define SENSITIVITYUP_DEF	                            0  //	sensitivityup;	//itn
#define SLOWSHUTTER_DEF                                  0
#define HUE_DEF			0 //hue;	 itn
#define GAMMA_DEF		0	//gamma;	 itn
#define DAYNIGHTMODE_DEF        0			//daynightmode; itn
#define MANUAL_RGAIN_DEF 0			//manual_rgain; itn
#define MANUAL_BGAIN_DEF 0			//manual_bgain; itn
//ITN end
#define AGC_D_VALUE_MAX_DEF			3
#define AGC_D_VALUE_MIN_DEF			0
#define AGC_N_VALUE_MAX_DEF			3
#define AGC_N_VALUE_MIN_DEF			0
#define SHUTTER_D_TIME_MAX_DEF		4
#define SHUTTER_D_TIME_MIN_DEF		12
#define SHUTTER_N_TIME_MAX_DEF		4
#define SHUTTER_N_TIME_MIN_DEF		12
#define FLICKERLESS_MODE_DEF		0
#define COLOR_BAR_DEF				0
#define EXPOSURE_MODE_DEF			0 // 0 : auto, 1 : manual
#define EXPOSURE_VALUE_D_DEF		120
#define EXPOSURE_VALUE_N_DEF		120
// add by kenny chiu 20121212 for exposure setting default value
#define MOBILE_FRAMERATE_DEF		4
#define MOBILE_RATECONTROL_DEF		0
#define MOBILE_QUALITY_DEF			1  // normal
#define MOBILE_BITRATE_DEF			1000000
#define AUDIO_BROADCAST_PORT        AUDIO_SPEECH_PORT

#define FTP_PORT_DEFAULT			21
#define AEW_SWITCH_DEFAULT		    2  //1 //1: APPRO2A  2: TI2A
#if EDIMAX_OEM
#define AEW_TYPE_DEFAULT			3  //Iris+AWB+AE modify by kenny chiu 20110512
#elif DIGISOL_OEM
#define AEW_TYPE_DEFAULT			3  //Iris+AWB+AE modify by kenny chiu 20110512
#else
#define AEW_TYPE_DEFAULT			3 // 7//  3  //Iris+AWB+AE modify by kenny chiu 20110512
#endif
#define SUPPORTMPEG4_DEFAULT		1
#define IMAGEFORMAT_DEFAULT			0
#if TANTOS_OEM
#define IMAGESOURCE_DEFAULT			1  // PAL
#else
#define IMAGESOURCE_DEFAULT			0
#endif
#define MAX_FRAMERATE_DEFAULT		30
#define DEVICENAME_DEFAULT			MODEL_NAME
#define DEFAULTSTORAGE_DEFAULT		1
#define DEFAULTCARDGETHTM_DEFAULT	"sdget.htm"
#define BRANDURL_DEFAULT			""//"www.smax.com.tw"
#define BRANDNAME_DEFAULT			BRAND_NAME // MODEL_NAME
#define RTP_MULTICAST_SUPPORT_DEFAULT		1	//Add by Leon 20110428
#define RTP_USCORERTSP_USCORETCP_DEFAULT	1	//Add by Leon 20110428
#define RTP_USCORETCP_DEFAULT				1	//Add by Leon 20110428

#define CONFIG_NAME_DEFAULT			"backup.tar.gz"//Add by Leon 20110428
#define SUPPORTTSTAMP_DEFAULT		1
#define	MPEG41XSIZE_DEFAULT			640  //VGA
#define	MPEG41YSIZE_DEFAULT			480  //VGA
#define	MPEG42XSIZE_DEFAULT			176  //QCIF
#define	MPEG42YSIZE_DEFAULT			144  //QCIF
#define JPEGXSIZE_DEFAULT			640  //VGA
#define JPEGYSIZE_DEFAULT			480  //VGA
#define SUPPORTMOTION_DEFAULT		2
#define SUPPORTWIRELESS_DEFAULT		0
#define SERVICEFTPCLIENT_DEFAULT	1
#define SERVICESMTPCLIENT_DEFAULT	1
#define SERVICEPPPOE_DEFAULT		0
#define SERVICESNTPCLIENT_DEFAULT	1
#define SERVICEDDNSCLIENT_DEFAULT	0
#define SUPPORTMASKAREA_DEFAULT		0
#define MAXCHANNEL_DEFAULT			1
#define SUPPORTRS485_DEFAULT		0
#define SUPPORTRS232_DEFAULT		1
#define LAYOUTNUM_DEFAULT			0
#define SUPPORTMUI_DEFAULT			0
#define MUI_DEFAULT					0
#define SUPPORTSEQUENCE_DEFAULT		0
#define QUADMODESELECT_DEFAULT		-1
#define SERVICEIPFILTER_DEFAULT		0
#define OEMFLAG0_DEFAULT			1
#define	SUPPORTDNCONTROL_DEFAULT	0
#define SUPPORTAVC_DEFAULT			0
#define SUPPORTAUDIO_DEFAULT		1
#define SUPPORTPTZPAGE_DEFAULT		1
#if 0 // modify by kenny chiu n20130613 per frank.lin
#if AIRLIVE_OEM
#define WHITEBALANCE_DEFAULT		0 // 1 /* auto white balance */ modify by kenny chiu 20120229
#elif MODAL_SIU7127
#define WHITEBALANCE_DEFAULT		0 //0:Outdoor
#elif MODAL_SIVD7137
#define WHITEBALANCE_DEFAULT		0 //0:Outdoor
#else
#define WHITEBALANCE_DEFAULT		2 //2:Auto /* auto white balance */ modify by kenny chiu 20120229
#endif
#else
#if MODAL_PT || MODAL_SIFD5130 || MODAL_SIFD7130 || MODAL_SICPT501U
#define WHITEBALANCE_DEFAULT		2 // 2 :Auto /* auto white balance */ modify by kenny chiu 20120229
#else
#define WHITEBALANCE_DEFAULT		1 //0:Outdoor 1: auto
#endif
#endif
#define DAY_NIGHT_DEFAULT			0	/* Default: Day mode */ //(0:day, 1:night) Joseph 20140905
#define LOCALDISPLAY_DEFAULT		0
#define TV_CABLE_DEFAULT			1  // 0 : modify by kenny chiu 20110810
#define BINNING_DEFAULT				0
#define BACKLIGHT_CONTROL_DEFAULT	1
#define BACKLIGHT_VALUE_DEFAULT		128

#define STREAMTYPE_DEFAULT			0

#define VCODECCOMBO_DEFAULT			0

#ifdef IMGS_AR0331
#if FINEST_OEM  // Add by kenny chiu 20120926
#define BRIGHTNESS_DEFAULT			125
#define CONTRAST_DEFAULT			130
#define SATURATION_DEFAULT			100//70
#define SHARPNESS_DEFAULT			150
#else
#define BRIGHTNESS_DEFAULT			128
#define CONTRAST_DEFAULT			128
#define SATURATION_DEFAULT			128
#define SHARPNESS_DEFAULT			128
#endif
#elif IMGS_ITN
#define BRIGHTNESS_DEFAULT		    128
#define CONTRAST_DEFAULT			  0
#define SATURATION_DEFAULT		    128
#define SHARPNESS_DEFAULT			 80
#else // not AR0331
#if FINEST_OEM  // Add by kenny chiu 20120926
#define BRIGHTNESS_DEFAULT			128
#define CONTRAST_DEFAULT			128
#define SATURATION_DEFAULT			128
#define SHARPNESS_DEFAULT			140
#else
#define BRIGHTNESS_DEFAULT			128
#define CONTRAST_DEFAULT			128
#define SATURATION_DEFAULT			128
#define SHARPNESS_DEFAULT			128
#endif

#endif

#if SMAX_BRAND && EDIMAX_CLOUD_FEATURE
#define EDIMAX_CLOUD_DEFAULT 1
#else
#define EDIMAX_CLOUD_DEFAULT 0
#endif
//#define CODEC_DEFAULT				1
//#define VMODE_DEFAULT				1
#define VCODECMODE_DEFAULT			3
#define VCODECCOMBO_DEFAULT			0
#define VCODECRES_DEFAULT			4
#define SUPPORTSTREAM1_DEFAULT		1
#define SUPPORTSTREAM2_DEFAULT		1
#define SUPPORTSTREAM3_DEFAULT		1
#define SUPPORTSTREAM4_DEFAULT		0
#define SUPPORTSTREAM5_DEFAULT		1
#define SUPPORTSTREAM6_DEFAULT		0

#define RATECONTROL_DEFAULT			0	/* Set to CVBR */
#define FRAMERATE1_DEFAULT			0
#define FRAMERATE2_DEFAULT			0
#define FRAMERATE3_DEFAULT			0
#define FRAMERATE4_DEFAULT			0

//#define RATECONTROL_DEFAULT			0 //1
//#define MPEG41FRMRATE_DEFAULT		0
//#define MPEG42FRMRATE_DEFAULT		0
//#define MPEG43FRMRATE_DEFAULT		0
#define JPEGFRMRATE_DEFAULT			30
#define MPEG41GOP_DEFAULT		    30  //Add by Joseph 20131119
#define MPEG41BITRATE_DEFAULT		1500000//4000000
#define MPEG42BITRATE_DEFAULT		512000
#define MPEG43BITRATE_DEFAULT		1500000
#define MOBILEBITRATE_DEFAULT		512000

#define DHCP_CONFIG_DEFAULT			0
#define DISCOVERY_MODE              0
#define NO_SCHEDULE					{0,1,{0,0,0},{0,0,0}}
#define SCHEDULE_DEFAULT			{NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE,NO_SCHEDULE}
#define LOSTALARM_DEFAULT			0
#define SDAENABLE_DEFAULT			0
#define ALARMDURATION_DEFAULT		0
#define AFTPENABLE_DEFAULT			0
#define ASMTPENABLE_DEFAULT			0
#define AVIDURATION_DEFAULT			0
#define AVIFORMAT_DEFAULT			0
#define FTPFILEFORMAT_DEFAULT		0
#define SDFILEFORMAT_DEFAULT		0
#define ATTFILEFORMAT_DEFAULT		0
//#define SUPPORTSTREAM1_DEFAULT		0
//#define SUPPORTSTREAM2_DEFAULT		0
//#define SUPPORTSTREAM3_DEFAULT		0
//#define SUPPORTSTREAM4_DEFAULT		0
//#define SUPPORTSTREAM5_DEFAULT		0
//#define SUPPORTSTREAM6_DEFAULT		0
#define ADVMODE_DEF 				0  //365 Specify,RS
#define M41SFEAT_DEF 				0  //365 Specify,RS
#define M42SFEAT_DEF				0  //365 Specify,RS
#define JPGSFEAT_DEF				0  //365 Specify,RS
#define ASMTPATTACH_DEFAULT			0
#define RFTPENABLE_DEFAULT			0
#define SD_INSERT_DEFAULT	        0
#define SDRENABLE_DEFAULT			0

#define AUDIOENABLE_DEFAULT	        0
#define AUDIOPORT_DEFAULT	        AUDIO_SPEECH_PORT

#define MOTIONENABLE_DEFAULT		0
#define MOTIONCENABLE_DEFAULT		0
#define MOTIONLEVEL_DEFAULT		    0
#define MOTIONCVALUE_DEFAULT	   50
#define MOTIONBLOCK_DEFAULT  	   "0000000000"
//#define MOTIONBLOCK_DEFAULT	   "0006660000"
#define MOTION_ALARM_DEFAULT  0

#define EWDR_DEFAULT          0 // add by kenny chiu 20131210

#define PTZZOOMIN_DEFAULT	"0"//"zoomin"
#define PTZZOOMOUT_DEFAULT	"0"//"zoomout"
#define PTZPANUP_DEFAULT	"0"//"up"
#define PTZPANDOWN_DEFAULT	"0"//"down"
#define PTZPANLEFT_DEFAULT	"0"//"left"
#define PTZPANRIGHT_DEFAULT	"0"//"right"
#define EPTZMODE_DEFAULT    0//defined by frank for Geo
//GIO
#define	GIOINENABLE_DEFAULT	       0
#define	GIOINTYPE_DEFAULT	       0
#define	GIOOUTENABLE_DEFAULT	   0
#define	GIOOUTTYPE_DEFAULT	       0

#define DATEFORMAT_DEFAULT 		   2
#define TSTAMPENABLE_DEFAULT       0
#define TSTAMPFORMAT_DEFAULT       0
#define AUDIOINVOLUME_DEFAULT      0 //2
#define ALARMSTATUS_DEFAULT	       0
#define MULTICAST_ENABLE_DEFAULT   0
#define WIS_AUDIO_DISABLE "/tmp/wis-streamer_audio_disable"
#define REC_CIFS_TYPE  1

/* authority AUTHORITY_NONE = 9 */

#define NONE_ACOUT		  {"", "", 9}

#if EDIMAX_OEM
#define ADMIN_ACOUT		  {"admin", "1234", 0}
#elif FUHO_OEM
#define ADMIN_ACOUT		  {"admin", "9999", 0}
#elif PCI_OEM
#define ADMIN_ACOUT		  {"admin", "password", 0}
#elif AIRLIVE_OEM
#define ADMIN_ACOUT		  {"admin", "airlive", 0}
#else
#define ADMIN_ACOUT		  {"admin", "admin", 0}
#endif

#define ACOUNT_DEFAULT			{ADMIN_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT,NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT}
#define EXTRO_ACOUNT_DEFAULT	{NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT, NONE_ACOUT}
#define ACOUNT_TIME_DEFAULT		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define SD_INSERT_DEFAULT	0
#define	GIOINENABLE_DEFAULT	0
#define	GIOINTYPE_DEFAULT	  0
#define	GIOOUTENABLE_DEFAULT	0
#define	GIOOUTTYPE_DEFAULT	  0
#define CUR_LOG_DEFAULT     	{"", {0, 0, 0, 0, 0, 0, 0, 0, 0}}
#define CUR_DMVA_LOG_DEFAULT	{"", "", "",0,0,{0, 0, 0, 0, 0, 0, 0, 0, 0}}
#define TSTAMPENABLE_DEFAULT 0
#define TSTAMPFORMAT_DEFAULT 0
#define AUDIOINVOLUME_DEFAULT 0 //2    // by Larry
#define ALARMSTATUS_DEFAULT	  0
#define MULTICAST_ENABLE_DEFAULT	0
#define WIS_AUDIO_DISABLE "/tmp/wis-streamer_audio_disable"
#define REC_CIFS_TYPE  1

#define LINKSYS

#define OSD_align(value, align)   ((( (value) + ( (align) - 1 ) ) / (align) ) * (align) )	/* Privacy Mask Alignment - Dean - 20131015 */

#define OSD_ALIGN_FLOOR(value, align)   (( (value) / (align) ) * (align) )	/* Privacy Mask Alignment - Dean - 20131015 */
#define OSD_ALIGN_CEIL(value, align)    OSD_align(value, align) 			/* Privacy Mask Alignment - Dean - 20131015 */
#define OSD_ALIGN_X				2											/* Privacy Mask Alignment - Dean - 20131015 */
#define OSD_ALIGN_Y				2											/* Privacy Mask Alignment - Dean - 20131015 */
#define OSD_ALIGN_WIDTH			8											/* Privacy Mask Alignment - Dean - 20131015 */
#define OSD_ALIGN_HEIGHT		2											/* Privacy Mask Alignment - Dean - 20131015 */
#define OSD_ALIGN_THINKNESS		2											/* Privacy Mask Alignment - Dean - 20131015 */

enum
{
    VIDEO_D1=1,			//720 * 576
    VIDEO_4CIF,			//704 * 576
    VIDEO_CIF,			//352 * 288
    VIDEO_QCIF,			//176 * 144
    VIDEO_VGA,			//640 * 480
    VIDEO_QVGA			//320 * 240
};

#define STATUS_FLAG_NO_STATUS_FILE  (-1)		/* Add Save Status Flag - Dean - 20120528 */

/* Add DDNS Log Flag - Begin - Dean - 20120528 */
enum {
	DDNS_RETURN_CODE_FAIL = 0,
	DDNS_RETURN_CODE_SUCCESS,
	DDNS_RETURN_CODE_BAD_HOSTNAME,
	DDNS_RETURN_CODE_BAD_PASSWORD,
	DDNS_RETURN_CODE_BAD_USER,
	DDNS_RETURN_CODE_INVALID_IP,
	DDNS_RETURN_CODE_MAC_NOT_VALID,
	DDNS_RETURN_CODE_NO_UPDATE_NEEDED,		/* DDNS Log Modification - Dean - 20120601 *//* Add DDNS Provider PCI - Dean - 20120601 */
	DDNS_RETURN_CODE_CONNECT_FAIL,			/* DDNS Log Modification - Dean - 20120601 *//* Add DDNS Provider PCI - Dean - 20120601 */
	DDNS_RETURN_CODE_CONNECTING,			/* Add DDNS Connecting - Dean - 20120605 */
	DDNS_RETURN_CODE_NO_ACTION, 			/* Add DDNS Connecting - Dean - 20120605 */
	DDNS_RETURN_CODE_COMPLETE,				/* Add DDNS Connecting - Dean - 20120605 */
	DDNS_RETURN_CODE_BAD_USER_PASSWD,		/* Add DDNS Provider NoIP - Dean - 20120705 */
	DDNS_RETURN_CODE_TOTAL
};
#define DDNS_RETURN_CODE_LEN 16
#define DDNS_RETURN_CODE_LIST {"FAIL", "Success", "Bad Hostname", "Bad Password", "Bad User", "Invalid IP", "MAC Not Valid", "NoUpdateNeeded", "Connect Fail", "Connecting", "No Action", "Complete", "Bad UserPWD"}		/* DDNS Log Modification - Dean - 20120601 *//* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Connecting - Dean - 20120605 *//* Add DDNS Provider NoIP - Dean - 20120705 */
/* Add DDNS Log Flag - End - Dean - 20120528 */

// Modify DDNS Provider & Add DDNS Provider Planet - Begin - Dean - 20120411
#ifdef DDNS_PROVIDER_LIST_DEFAULT
enum{
	DDNS_PROVIDER_DYNDNS = 0,
	DDNS_PROVIDER_NOIP,			/* Add DDNS Provider NoIP - Dean - 20120705 */
	DDNS_PROVIDER_TOTAL,
//	DDNS_PROVIDER_TZO,          /*remove by Joseph 20140522*/
//	DDNS_PROVIDER_QDNS,				/* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_PROVIDER_FREEDNS,			/* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_PROVIDER_PLANET,			/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_PROVIDER_PCI				/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 */
};

enum{
	DDNS_DOMAIN_DYNDNS,
	DDNS_DOMAIN_NOIP,				/* Add DDNS Provider NoIP - Dean - 20120705 */
	DDNS_DOMAIN_TOTAL,
//	DDNS_DOMAIN_TZO,                /*remove by Joseph 20140522*/
//	DDNS_DOMAIN_PLANET,
//	DDNS_DOMAIN_PCI_LUNA,
//	DDNS_DOMAIN_PCI_MARS,
//	DDNS_DOMAIN_PCI_PLANEX,
//	DDNS_DOMAIN_PCI_SUN,
};

#define DDNS_PROVIDER_LIST			{DDNS_PROVIDER_STR_DYNDNS,  DDNS_PROVIDER_STR_NOIP}	/* Add DDNS Provider NoIP - Dean - 20120705  modify by Joseph for remove toz.com 20140522  DDNS_PROVIDER_STR_TZO*/
//#define DDNS_PROVIDER_IDX_LIST	{DDNS_STR_IDX_0, DDNS_STR_IDX_1, DDNS_STR_IDX_2, DDNS_STR_IDX_3, DDNS_STR_IDX_4}	/* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_PROVIDER_WEB_ABBR_LIST	{DDNS_PROVIDER_STR_WEB_ABBR_DYNDDS,  DDNS_PROVIDER_STR_WEB_ABBR_NOIP}	/* Add DDNS Abbreviation - Dean - 20120611 *//* Add DDNS Provider NoIP - Dean - 20120705  modify by Joseph for remove toz.com 20140522  DDNS_PROVIDER_STR_WEB_ABBR_TZO,*/
#define DDNS_PROVIDER_WEB_LIST		{DDNS_PROVIDER_STR_WEB_DYNDDS, DDNS_PROVIDER_STR_WEB_NOIP}	/* Add DDNS Update DateTime - Dean - 20120604 *//* Add DDNS Provider NoIP - Dean - 20120705 modify by Joseph for remove toz.com 20140522 */
#define DDNS_PROVIDER_WEB			(DDNS_PROVIDER_TOTAL)	/* Add DDNS Provider NoIP - Dean - 20120705 */

//#define DDNS_DOMAIN_LIST			{DDNS_DOMAIN_STR_DYNDNS, DDNS_DOMAIN_STR_TZO, DDNS_DOMAIN_STR_NOIP}		/* Add DDNS Provider NoIP - Dean - 20120705 */
//#define DDNS_DOMAIN_IDX_LIST		{DDNS_STR_IDX_0, DDNS_STR_IDX_1, DDNS_STR_IDX_2}						/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_DOMAIN_PROVIDER_WEB_LIST	{DDNS_PROVIDER_DYNDNS, DDNS_PROVIDER_NOIP}	/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 modify by Joseph for remove toz.com 20140522  DDNS_PROVIDER_TZO*/
#define DDNS_DOMAIN_WEB_LIST		{DDNS_DOMAIN_STR_DYNDNS, DDNS_DOMAIN_STR_NOIP}		/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 modify by Joseph for remove toz.com 20140522  DDNS_DOMAIN_STR_TZO*/
#define DDNS_DOMAIN_WEB				DDNS_DOMAIN_TOTAL										/* Add DDNS Provider PCI - Dean - 20120601 */

#endif
#ifdef DDNS_PROVIDER_LIST_PLANET
enum{
	DDNS_PROVIDER_DYNDNS = 0,
	DDNS_PROVIDER_TZO,
	DDNS_PROVIDER_NOIP,			/* Add DDNS Provider NoIP - Dean - 20120705 */
	DDNS_PROVIDER_PLANET,
	DDNS_PROVIDER_TOTAL,		/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_PROVIDER_QDNS,		/* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_PROVIDER_FREEDNS,		/* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_PROVIDER_PCI 			/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 */
};

enum{
	DDNS_DOMAIN_DYNDNS,
	DDNS_DOMAIN_TZO,
	DDNS_DOMAIN_NOIP, 			/* Add DDNS Provider NoIP - Dean - 20120705 */
	DDNS_DOMAIN_PLANET,
	DDNS_DOMAIN_TOTAL,
//	DDNS_DOMAIN_PCI_LUNA,		/* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_DOMAIN_PCI_MARS,		/* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_DOMAIN_PCI_PLANEX,	/* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_DOMAIN_PCI_SUN,		/* Add DDNS Provider NoIP - Dean - 20120705 */
};

#define DDNS_PROVIDER_LIST			{DDNS_PROVIDER_STR_DYNDNS, DDNS_PROVIDER_STR_TZO, DDNS_PROVIDER_STR_NOIP, DDNS_PROVIDER_STR_PLANET}	/* Add DDNS Provider NoIP - Dean - 20120705 */
//#define DDNS_PROVIDER_IDX_LIST	{DDNS_STR_IDX_0, DDNS_STR_IDX_1, DDNS_STR_IDX_2, DDNS_STR_IDX_3, DDNS_STR_IDX_4, DDNS_STR_IDX_5}	/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_PROVIDER_WEB_ABBR_LIST	{DDNS_PROVIDER_STR_WEB_ABBR_DYNDDS, DDNS_PROVIDER_STR_WEB_ABBR_TZO, DDNS_PROVIDER_STR_WEB_ABBR_NOIP, DDNS_PROVIDER_STR_WEB_ABBR_PLANET}	/* Add DDNS Abbreviation - Dean - 20120611 *//* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_PROVIDER_WEB_LIST		{DDNS_PROVIDER_STR_WEB_DYNDDS, DDNS_PROVIDER_STR_WEB_TZO, DDNS_PROVIDER_STR_WEB_NOIP, DDNS_PROVIDER_STR_WEB_PLANET}	/* Add DDNS Update DateTime - Dean - 20120604 *//* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_PROVIDER_WEB			(DDNS_PROVIDER_TOTAL)

//#define DDNS_DOMAIN_LIST			{DDNS_DOMAIN_STR_DYNDNS, DDNS_DOMAIN_STR_TZO, DDNS_DOMAIN_STR_NOIP, DDNS_DOMAIN_STR_PLANET}		/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 */
//#define DDNS_DOMAIN_IDX_LIST		{DDNS_STR_IDX_0, DDNS_STR_IDX_1, DDNS_STR_IDX_2, DDNS_STR_IDX_3}								/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_DOMAIN_PROVIDER_WEB_LIST	{DDNS_PROVIDER_DYNDNS, DDNS_PROVIDER_TZO, DDNS_PROVIDER_NOIP, DDNS_PROVIDER_PLANET}	/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_DOMAIN_WEB_LIST		{DDNS_DOMAIN_STR_DYNDNS, DDNS_DOMAIN_STR_TZO, DDNS_DOMAIN_STR_NOIP, DDNS_DOMAIN_STR_PLANET}		/* Add DDNS Provider PCI - Dean - 20120601 *//* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_DOMAIN_WEB				DDNS_DOMAIN_TOTAL																/* Add DDNS Provider NoIP - Dean - 20120705 */

#endif
/* Add DDNS Provider PCI - Begin - Dean - 20120601 */
#ifdef DDNS_PROVIDER_LIST_PCI
enum{
	DDNS_PROVIDER_DYNDNS = 0,
	DDNS_PROVIDER_TZO,
	DDNS_PROVIDER_NOIP,			/* Add DDNS Provider NoIP - Dean - 20120705 */
	DDNS_PROVIDER_PCI,
	DDNS_PROVIDER_TOTAL,
//	DDNS_PROVIDER_QDNS,		/* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_PROVIDER_FREEDNS,		/* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_PROVIDER_PLANET		/* Add DDNS Provider NoIP - Dean - 20120705 */
};

enum{
	DDNS_DOMAIN_DYNDNS,
	DDNS_DOMAIN_TZO,
	DDNS_DOMAIN_NOIP,			/* Add DDNS Provider NoIP - Dean - 20120705 */
	DDNS_DOMAIN_PCI_LUNA,
	DDNS_DOMAIN_PCI_MARS,
	DDNS_DOMAIN_PCI_PLANEX,
	DDNS_DOMAIN_PCI_SUN,
	DDNS_DOMAIN_TOTAL,
//	DDNS_DOMAIN_PLANET,		/* Add DDNS Provider NoIP - Dean - 20120705 */
};

#define DDNS_PROVIDER_LIST			{DDNS_PROVIDER_STR_DYNDNS, DDNS_PROVIDER_STR_TZO, DDNS_PROVIDER_STR_NOIP, DDNS_PROVIDER_STR_PCI}	/* Add DDNS Provider NoIP - Dean - 20120705 */
//#define DDNS_PROVIDER_IDX_LIST	{DDNS_STR_IDX_0, DDNS_STR_IDX_1, DDNS_STR_IDX_2, DDNS_STR_IDX_3, DDNS_STR_IDX_4, DDNS_STR_IDX_5}	/* Add DDNS Provider NoIP - Dean - 20120705 *//
#define DDNS_PROVIDER_WEB_ABBR_LIST	{DDNS_PROVIDER_STR_WEB_ABBR_DYNDDS, DDNS_PROVIDER_STR_WEB_ABBR_TZO, DDNS_PROVIDER_STR_WEB_ABBR_NOIP, DDNS_PROVIDER_STR_WEB_ABBR_PCI}	/* Add DDNS Abbreviation - Dean - 20120611 *//* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_PROVIDER_WEB_LIST		{DDNS_PROVIDER_STR_WEB_DYNDDS, DDNS_PROVIDER_STR_WEB_TZO, DDNS_PROVIDER_STR_WEB_NOIP, DDNS_PROVIDER_STR_WEB_PCI}	/* Add DDNS Update DateTime - Dean - 20120604 *//* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_PROVIDER_WEB			(DDNS_PROVIDER_TOTAL)

//#define DDNS_DOMAIN_LIST			{DDNS_DOMAIN_STR_DYNDNS, DDNS_DOMAIN_STR_TZO, DDNS_DOMAIN_STR_NOIP, DDNS_DOMAIN_STR_PCI_LUNA, DDNS_DOMAIN_STR_PCI_MARS, DDNS_DOMAIN_STR_PCI_PLANEX, DDNS_DOMAIN_STR_PCI_SUN}	/* Add DDNS Provider NoIP - Dean - 20120705 */
//#define DDNS_DOMAIN_IDX_LIST		{DDNS_STR_IDX_0, DDNS_STR_IDX_1, DDNS_STR_IDX_2, DDNS_STR_IDX_3, DDNS_STR_IDX_4, DDNS_STR_IDX_5, DDNS_STR_IDX_6}		/* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_DOMAIN_PROVIDER_WEB_LIST	{DDNS_PROVIDER_DYNDNS, DDNS_PROVIDER_TZO, DDNS_PROVIDER_NOIP, DDNS_PROVIDER_PCI, DDNS_PROVIDER_PCI, DDNS_PROVIDER_PCI, DDNS_PROVIDER_PCI}	/* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_DOMAIN_WEB_LIST		{DDNS_DOMAIN_STR_DYNDNS, DDNS_DOMAIN_STR_TZO, DDNS_DOMAIN_STR_NOIP, DDNS_DOMAIN_STR_PCI_LUNA, DDNS_DOMAIN_STR_PCI_MARS, DDNS_DOMAIN_STR_PCI_PLANEX, DDNS_DOMAIN_STR_PCI_SUN}	/* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_DOMAIN_WEB				DDNS_DOMAIN_TOTAL
#endif
/* Add DDNS Provider PCI - End - Dean - 20120601 */
/* Add DDNS Provider PCI/Planet - Begin - Dean - 20120601 *//* Add DDNS Provider PCI - Dean - 20120601 */
#ifdef DDNS_PROVIDER_LIST_ALL
enum{
	DDNS_PROVIDER_DYNDNS = 0,
	DDNS_PROVIDER_TZO,
	DDNS_PROVIDER_NOIP,			/* Add DDNS Provider NoIP - Dean - 20120705 */
	DDNS_PROVIDER_PLANET,
	DDNS_PROVIDER_PCI,
	DDNS_PROVIDER_TOTAL,
//	DDNS_PROVIDER_QDNS,		/* Add DDNS Provider NoIP - Dean - 20120705 */
//	DDNS_PROVIDER_FREEDNS,		/* Add DDNS Provider NoIP - Dean - 20120705 */
};

enum{
	DDNS_DOMAIN_DYNDNS,
	DDNS_DOMAIN_TZO,
	DDNS_DOMAIN_NOIP,			/* Add DDNS Provider NoIP - Dean - 20120705 */
	DDNS_DOMAIN_PLANET,
	DDNS_DOMAIN_PCI_LUNA,
	DDNS_DOMAIN_PCI_MARS,
	DDNS_DOMAIN_PCI_PLANEX,
	DDNS_DOMAIN_PCI_SUN,
	DDNS_DOMAIN_TOTAL,
};

#define DDNS_PROVIDER_LIST			{DDNS_PROVIDER_STR_DYNDNS, DDNS_PROVIDER_STR_TZO, DDNS_PROVIDER_STR_NOIP, DDNS_PROVIDER_STR_PLANET, DDNS_PROVIDER_STR_PCI}	/* Add DDNS Provider NoIP - Dean - 20120705 */
//#define DDNS_PROVIDER_IDX_LIST	{DDNS_STR_IDX_0, DDNS_STR_IDX_1, DDNS_STR_IDX_2, DDNS_STR_IDX_3, DDNS_STR_IDX_4}		/* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_PROVIDER_WEB_ABBR_LIST	{DDNS_PROVIDER_STR_WEB_ABBR_DYNDDS, DDNS_PROVIDER_STR_WEB_ABBR_TZO, DDNS_PROVIDER_STR_WEB_ABBR_NOIP, DDNS_PROVIDER_STR_WEB_ABBR_PLANET, DDNS_PROVIDER_STR_WEB_ABBR_PCI}	/* Add DDNS Abbreviation - Dean - 20120611 *//* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_PROVIDER_WEB_LIST	{DDNS_PROVIDER_STR_WEB_DYNDDS, DDNS_PROVIDER_STR_WEB_TZO, DDNS_PROVIDER_STR_WEB_NOIP, DDNS_PROVIDER_STR_WEB_PLANET, DDNS_PROVIDER_STR_WEB_PCI}	/* Add DDNS Update DateTime - Dean - 20120604 *//* Add DDNS Provider NoIP - Dean - 20120705 *//
#define DDNS_PROVIDER_WEB			(DDNS_PROVIDER_TOTAL)

//#define DDNS_DOMAIN_LIST			{DDNS_DOMAIN_STR_DYNDNS, DDNS_DOMAIN_STR_TZO, DDNS_DOMAIN_STR_NOIP, DDNS_DOMAIN_STR_PLANET, DDNS_DOMAIN_STR_PCI_LUNA, DDNS_DOMAIN_STR_PCI_MARS, DDNS_DOMAIN_STR_PCI_PLANEX, DDNS_DOMAIN_STR_PCI_SUN}		/* Add DDNS Provider NoIP - Dean - 20120705 */
//#define DDNS_DOMAIN_IDX_LIST		{DDNS_STR_IDX_0, DDNS_STR_IDX_1, DDNS_STR_IDX_2, DDNS_STR_IDX_3, DDNS_STR_IDX_4, DDNS_STR_IDX_5, DDNS_STR_IDX_6, DDNS_STR_IDX_7}		/* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_DOMAIN_PROVIDER_WEB_LIST	{DDNS_PROVIDER_DYNDNS, DDNS_PROVIDER_TZO, DDNS_PROVIDER_NOIP, DDNS_PROVIDER_PLANET, DDNS_PROVIDER_PCI, DDNS_PROVIDER_PCI, DDNS_PROVIDER_PCI, DDNS_PROVIDER_PCI}	/* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_DOMAIN_WEB_LIST		{DDNS_DOMAIN_STR_DYNDNS, DDNS_DOMAIN_STR_TZO, DDNS_DOMAIN_STR_NOIP, DDNS_DOMAIN_STR_PLANET, DDNS_DOMAIN_STR_PCI_LUNA, DDNS_DOMAIN_STR_PCI_MARS, DDNS_DOMAIN_STR_PCI_PLANEX, DDNS_DOMAIN_STR_PCI_SUN}		/* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_DOMAIN_WEB			DDNS_DOMAIN_TOTAL
#endif
/* Add DDNS Provider PCI - End - Dean - 20120601 *//* Add DDNS Provider PCI - Dean - 20120601 */

#define DDNS_PROVIDER_MIN			0
#define DDNS_PROVIDER_MAX			(DDNS_PROVIDER_WEB - 1)		/* Add DDNS Provider NoIP - Dean - 20120705 */

#define DDNS_PROVIDER_STR_LEN		8
#define DDNS_PROVIDER_STR_WEB_LEN	24

#define DDNS_PROVIDER_STR_DYNDNS	"dyndns"
#define DDNS_PROVIDER_STR_TZO		"tzo"
#define DDNS_PROVIDER_STR_PLANET	"planet"
#define DDNS_PROVIDER_STR_QDNS		"qdns"
#define DDNS_PROVIDER_STR_FREEDNS	"freedns"
#define DDNS_PROVIDER_STR_PCI		"pci"			/* Add DDNS Provider PCI - Dean - 20120601 */
#define DDNS_PROVIDER_STR_NOIP		"noip"			/* Add DDNS Provider NoIP - Dean - 20120705 */

#if 0	/* Add DDNS Provider NoIP - Dean - 20120705 */
#define DDNS_STR_IDX_0		"0"
#define DDNS_STR_IDX_1		"1"
#define DDNS_STR_IDX_2		"2"
#define DDNS_STR_IDX_3		"3"
#define DDNS_STR_IDX_4		"4"
#define DDNS_STR_IDX_5		"5"
#define DDNS_STR_IDX_6		"6"
#define DDNS_STR_IDX_7		"7"						/* Add DDNS Provider NoIP - Dean - 20120705 */
#endif	/* Add DDNS Provider NoIP - Dean - 20120705 */

#define DDNS_PROVIDER_STR_WEB_DYNDDS	"DynDNS.org"
#define DDNS_PROVIDER_STR_WEB_TZO		"TZO.com"
#define DDNS_PROVIDER_STR_WEB_PLANET	"Planetddns.com"
#define DDNS_PROVIDER_STR_WEB_QDNS		"3322.Com"
#define DDNS_PROVIDER_STR_WEB_FREEDNS	"Freedns.org"
#define DDNS_PROVIDER_STR_WEB_PCI		"Planex CyberGate"		/* Add DDNS Provider PCI - Dean - 20120601 */
#define DDNS_PROVIDER_STR_WEB_NOIP		"No-ip"					/* Add DDNS Provider NoIP - Dean - 20120705 */

#define DDNS_PROVIDER_STR_WEB_ABBR_DYNDDS	"d0"				/* Add DDNS Abbreviation - Dean - 20120611 */
#define DDNS_PROVIDER_STR_WEB_ABBR_TZO		"t0"				/* Add DDNS Abbreviation - Dean - 20120611 */
#define DDNS_PROVIDER_STR_WEB_ABBR_PLANET	"p0"				/* Add DDNS Abbreviation - Dean - 20120611 */
#define DDNS_PROVIDER_STR_WEB_ABBR_QDNS		"q0"				/* Add DDNS Abbreviation - Dean - 20120611 */
#define DDNS_PROVIDER_STR_WEB_ABBR_FREEDNS	"f0"				/* Add DDNS Abbreviation - Dean - 20120611 */
#define DDNS_PROVIDER_STR_WEB_ABBR_PCI		"p1"				/* Add DDNS Abbreviation - Dean - 20120611 */
#define DDNS_PROVIDER_STR_WEB_ABBR_NOIP		"n1"				/* Add DDNS Provider NoIP - Dean - 20120705 */

#define DDNS_DOMAIN_MIN			0
#define DDNS_DOMAIN_MAX			(DDNS_DOMAIN_TOTAL - 1)

#define DDNS_DOMAIN_STR_DYNDNS			""					/* Add DDNS Provider PCI - Dean - 20120601 */

#define DDNS_DOMAIN_STR_TZO				""					/* Add DDNS Provider PCI - Dean - 20120601 */

#define DDNS_DOMAIN_STR_PLANET			"planetddns.com"		/* Add DDNS Provider PCI - Dean - 20120601 */

#define DDNS_DOMAIN_STR_PCI_LUNA		"luna.ddns.vc"			/* Add DDNS Provider PCI - Dean - 20120601 */
#define DDNS_DOMAIN_STR_PCI_MARS		"mars.ddns.vc"		/* Add DDNS Provider PCI - Dean - 20120601 */
#define DDNS_DOMAIN_STR_PCI_PLANEX		"planex.ddns.vc"		/* Add DDNS Provider PCI - Dean - 20120601 */
#define DDNS_DOMAIN_STR_PCI_SUN			"sun.ddns.vc"			/* Add DDNS Provider PCI - Dean - 20120601 */

#define DDNS_DOMAIN_STR_NOIP			""					/* Add DDNS Provider NoIP - Dean - 20120705 */

/* Onvif */
#define ONVIF_PROFILE_NUM 3
#define dVE_Name "VE_Default"
#define dVE_UseCount 3 // 1 modify by Kenny Chiu 20151029
#define dVE_token "VE_Default"
#define dVE_Encoding 2 //tt__VideoEncoding__JPEG=0,tt__VideoEncoding__MPEG4=1,tt__VideoEncoding__H264=2
#define dVE_ResH 1080 //720
#define dVE_ResW 1920 //1280
#define dVE1_ResH 480
#define dVE1_ResW 640
#define dVE_Quality 2
#define dVE_FrameRateLimit 30
#define dVE_EncodingInterval 0
#define dVE_BitrateLimit 3000
#define dVE_stream2_BitrateLimit 1000
#define dVE_GovLength 30
#define dVE_Profile 0
#define dVE_IPV4Address "232.128.1.99"
#define dVE_Port 5560
#define dVE_TTL 15
#define dVE_AutoStart 1
#define dVE_SessionTimeout 60000
//
#define dAS_Name "AS_Default"
#define dAS_UseCount 1
#define dAS_token "AS_Default"
#define dAS_SourceToken "AS_Default"
//
#define dAE_Name "AE_Default"
#define dAE_UseCount 3 // 1 modify by Kenny Chiu 20151029
#define dAE_token "AE_Default"
#define dAE_Encoding 2 //0
#define dAE_Bitrate 64 //modify by joseph 20130130  kbps
#define dAE_SampleRate 16 // 16 : for AAC 8 : for G711 //modify by joseph 20130130 kHz
#define dAE_IPV4Address "232.128.1.99"
#define dAE_port 5562
#define dAE_TTL 15
#define dAE_AutoStart 1
#define dAE_SessionTimeout 6000
//
#define dVS_width  1920
#define dVS_height 1080


#define COM_PROF		"", "", 0
#define VSC_PROF		"", 0, "", "", 0, 0, 0, 0
#define ASC_PROF		"", 0, "", ""
#define VEC_PROF		"", "", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 720000
#define MULC_PROF		0, "", "", 0, 0, 720000
#define AEC_PROF		"", "", 0, 0, 0, 0
#define VAC_PROF		"", "", 0,
#define VAENGINE		0, 0, "", "", 0, "", "", ""
#define VRENGINE		0, 0, "", "", 0, "", "", ""
#define VOC_PROF		"",0,""
#define PTZ_PROF		"", "", 0, "", "", "", "", "", "", "", 0, 0, "", 0, "", 0, "", 0, 0, 0, 0, "", "", "", 0, 0, 0, 0
#define EXT_PROF		"", 0, "", "", "", 0, "", 0, ""
#define MDC_PROF		"" , 0, "", 0, 720000
#define NON_PROF		{COM_PROF,{VSC_PROF},{ASC_PROF},{VEC_PROF,{MULC_PROF}},{AEC_PROF},{VAC_PROF{VAENGINE},{VRENGINE}},{VOC_PROF},PTZ_PROF,EXT_PROF,\
							PresetE_PROFD,{MDC_PROF,{MULC_PROF}}}
#define COM_PROFD		"Default_Profile", "Default_Token", 1
#define COM_PROFD2		"subStream", "Default_Token2", 1
#define COM_PROFD3		"mobileStream", "Default_Token3", 1

//#define VSC_PROFD		"DVS_Name", 2, "DVS_Token", "0", 0, 0, dVS_width , dVS_height
#define VSC_PROFD		"DVS_Name", dVE_UseCount, "DVS_Token", "0", 0, 0, dVS_width , dVS_height
#define ASC_PROFD		AUDIO_CODEC_TYPE, 1, AUDIO_CODEC_TYPE, AUDIO_CODEC_TYPE // modify by Kenny Chiu 20151013
#define VEC_PROFD		"DE_Name1", "DE_Token1", 1, dVE_Quality, dVE_Encoding, dVE_ResW, dVE_ResH, dVE_FrameRateLimit, dVE_EncodingInterval, dVE_BitrateLimit, 30, 0, 30, 0, dVE_SessionTimeout
#define MULC_PROFD		0, dVE_IPV4Address, "", dVE_Port, dVE_TTL, dVE_AutoStart
#define AEC_PROFD		"DAE_Name", AUDIO_CODEC_TYPE, dAE_UseCount, dAE_Bitrate, dAE_SampleRate, dAE_Encoding// modify by Kenny Chiu 20151013
#define VAC_PROFD		"video_analytics", "video_analytics", 1
#define VAENGINED		0, 0, "", "", 0, "", "", ""
#define VRENGINED		0, 0, "", "", 0, "", "", ""
#define VOC_PROFD		"video_output", 1, "video_output"
#define PTZ_PROFD		"", "", 0, "", "", "", "", "", "", "", 0, 0, "", 0, "", 0, "", 0, 0, 0, 0, "", "", "", 0, 0, 0, 0
#define EXT_PROFD		"", 0, "", "", "", 0, "", 0, ""
#define MDC_PROFD		"MD_name" , 1, "MD_token", 0, 720000

#define VEC_PROFD1		"DE_Name1", "DE_Token1", 1, dVE_Quality, 2, dVE_ResW, dVE_ResH, dVE_FrameRateLimit, dVE_EncodingInterval, dVE_BitrateLimit, 30, 0, 30, 0, dVE_SessionTimeout
#define VEC_PROFD2		"DE_Name2", "DE_Token2", 1, dVE_Quality, 0, dVE1_ResW, dVE1_ResH, dVE_FrameRateLimit, dVE_EncodingInterval, dVE_BitrateLimit, 30, 0, 30, 0, dVE_SessionTimeout
#define VEC_PROFD3		"DE_Name3", "DE_Token3", 1, dVE_Quality, 2, dVE1_ResW, dVE1_ResH, dVE_FrameRateLimit, dVE_EncodingInterval, dVE_stream2_BitrateLimit, 30, 0, 30, 0, dVE_SessionTimeout
#define PresetE_One		{"" , "", 0 ,0}
#define PresetE_PROFD	{PresetE_One,PresetE_One,PresetE_One,PresetE_One,PresetE_One,PresetE_One,PresetE_One,PresetE_One}
#define DEFAULT_PROF	{COM_PROFD,{VSC_PROFD},{ASC_PROFD},{VEC_PROFD1,{MULC_PROFD}},{AEC_PROFD,{MULC_PROFD}},{VAC_PROFD,{VAENGINED},{VRENGINED}},{VOC_PROFD},\
							PTZ_PROFD,EXT_PROFD,PresetE_PROFD,{MDC_PROFD,{MULC_PROFD}}}
#define DEFAULT_PROF2	{COM_PROFD2,{VSC_PROFD},{ASC_PROFD},{VEC_PROFD2,{MULC_PROFD}},{AEC_PROFD,{MULC_PROFD}},{VAC_PROFD,{VAENGINED},{VRENGINED}},{VOC_PROFD},\
							PTZ_PROFD,EXT_PROFD,PresetE_PROFD,{MDC_PROFD,{MULC_PROFD}}}
#define DEFAULT_PROF3	{COM_PROFD3,{VSC_PROFD},{ASC_PROFD},{VEC_PROFD3,{MULC_PROFD}},{AEC_PROFD,{MULC_PROFD}},{VAC_PROFD,{VAENGINED},{VRENGINED}},{VOC_PROFD},\
							PTZ_PROFD,EXT_PROFD,PresetE_PROFD,{MDC_PROFD,{MULC_PROFD}}}


//#define DEFAULT_PROF	{COM_PROFD,{VSC_PROFD},{ASC_PROFD},{VEC_PROFD,{MULC_PROFD}},{AEC_PROFD},{VAC_PROFD,{VAENGINED},{VRENGINED}},{VOC_PROFD},
//						PTZ_PROFD,EXT_PROFD,{MDC_PROFD,{MULC_PROFD}}}

#if 0 // for edimax±M¥Î EDIMAX_OEM
#define ONVIF_PROFILE                   {DEFAULT_PROF, DEFAULT_PROF3, NON_PROF, NON_PROF}
#else
#define ONVIF_PROFILE                   {DEFAULT_PROF, DEFAULT_PROF2, DEFAULT_PROF3, NON_PROF}
#endif

#define	SCOPE_CN	"taiwan"
#define ONVIF_SCOPES			{7,{"onvif://www.onvif.org/Profile/Streaming","onvif://www.onvif.org/type/video_encoder","onvif://www.onvif.org/type/ptz","onvif://www.onvif.org/type/audio_encoder",\
                                 "onvif://www.onvif.org/hardware/"MODEL_NAME,"onvif://www.onvif.org/location/country/"SCOPE_CN,\
                                 "onvif://www.onvif.org/name/"MODEL_NAME},1}
#define RELAYS_PROFD			{0 ,{"alarm", 0, 10000, 0}}
#define RELAY_NON_PROF			{0 ,{"", 0, 0, 0}}
#define RELAYS_CONFIG			{RELAYS_PROFD, RELAY_NON_PROF, RELAY_NON_PROF}//, RELAY_NON_PROF}
#define IMAGING_CONFD			{"0" ,0, 1, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 0, 1, 0, 50, 50}
#define IMAGING_CONFG			{"" ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,""}
#define IMAGING_CONF			{IMAGING_CONFD,IMAGING_CONFG,IMAGING_CONFG,IMAGING_CONFG}
#define VEC_CONFIG              {{VEC_PROFD1,{MULC_PROFD}},{VEC_PROFD2,{MULC_PROFD}},{VEC_PROFD3,{MULC_PROFD}}}
#define VSC_CONFIG             {VSC_PROFD}
#define RestartState        0
//#define ONVIF_UTIL             {MAX_RES_2M,WIFI_FEATURE,IPNCPTZ_FEATURE,AUDIO_FEATURE,VEC_CONFIG,VSC_CONFIG,RestartState,RESERVE_ITEM_STR}
#define ONVIF_UTIL             {MAX_RES_2M,WIFI_FEATURE,0,AUDIO_FEATURE,VEC_CONFIG,VSC_CONFIG,RestartState,RESERVE_ITEM_STR} // modify by Kenny Chiu 20151013

/* End-Onvif */

// Modify DDNS Provider & Add DDNS Provider Planet - End - Dean - 20120411

/* Add DDNS Update DateTime - Begin - Dean - 20120604 */
#define TIMEZONE_NUM		57
#define TIMEZONE_STR_LEN	16
#define TIMEZONE_STR_LIST	{TIMEZONE_GMT_N_12_00, \
							TIMEZONE_GMT_N_11_00, \
							TIMEZONE_GMT_N_10_00, \
							TIMEZONE_GMT_N_09_00, \
							TIMEZONE_GMT_N_08_00, \
							TIMEZONE_GMT_N_07_00, TIMEZONE_GMT_N_07_00, \
							TIMEZONE_GMT_N_06_00, TIMEZONE_GMT_N_06_00, TIMEZONE_GMT_N_06_00, \
							TIMEZONE_GMT_N_05_00, TIMEZONE_GMT_N_05_00, TIMEZONE_GMT_N_05_00, \
							TIMEZONE_GMT_N_04_00, TIMEZONE_GMT_N_04_00, TIMEZONE_GMT_N_04_00, \
							TIMEZONE_GMT_N_03_00, TIMEZONE_GMT_N_03_00, \
							TIMEZONE_GMT_N_02_00, \
							TIMEZONE_GMT_N_01_00, \
							TIMEZONE_GMT_P_00_00, TIMEZONE_GMT_P_00_00, \
							TIMEZONE_GMT_P_01_00, TIMEZONE_GMT_P_01_00, TIMEZONE_GMT_P_01_00, TIMEZONE_GMT_P_01_00, \
							TIMEZONE_GMT_P_02_00, TIMEZONE_GMT_P_02_00, TIMEZONE_GMT_P_02_00, TIMEZONE_GMT_P_02_00, TIMEZONE_GMT_P_02_00, TIMEZONE_GMT_P_02_00, \
							TIMEZONE_GMT_P_03_00, TIMEZONE_GMT_P_03_00, TIMEZONE_GMT_P_03_00, \
							TIMEZONE_GMT_P_04_00, TIMEZONE_GMT_P_04_00, \
							TIMEZONE_GMT_P_05_00, TIMEZONE_GMT_P_05_00, \
							TIMEZONE_GMT_P_05_30, \
							TIMEZONE_GMT_P_06_00, \
							TIMEZONE_GMT_P_07_00, \
							TIMEZONE_GMT_P_08_00, TIMEZONE_GMT_P_08_00, TIMEZONE_GMT_P_08_00, TIMEZONE_GMT_P_08_00, \
							TIMEZONE_GMT_P_09_00, TIMEZONE_GMT_P_09_00, TIMEZONE_GMT_P_09_00, \
							TIMEZONE_GMT_P_10_00, TIMEZONE_GMT_P_10_00, TIMEZONE_GMT_P_10_00, TIMEZONE_GMT_P_10_00, TIMEZONE_GMT_P_10_00, \
							TIMEZONE_GMT_P_11_00, \
							TIMEZONE_GMT_P_12_00, TIMEZONE_GMT_P_12_00}

#define TIMEZONE_GMT_N_12_00		"(GMT-12:00)"
#define TIMEZONE_GMT_N_11_00		"(GMT-11:00)"
#define TIMEZONE_GMT_N_10_00		"(GMT-10:00)"
#define TIMEZONE_GMT_N_09_00		"(GMT-09:00)"
#define TIMEZONE_GMT_N_08_00		"(GMT-08:00)"
#define TIMEZONE_GMT_N_07_00		"(GMT-07:00)"
#define TIMEZONE_GMT_N_06_00		"(GMT-06:00)"
#define TIMEZONE_GMT_N_05_00		"(GMT-05:00)"
#define TIMEZONE_GMT_N_04_00		"(GMT-04:00)"
#define TIMEZONE_GMT_N_03_30		"(GMT-03:30)"
#define TIMEZONE_GMT_N_03_00		"(GMT-03:00)"
#define TIMEZONE_GMT_N_02_00		"(GMT-02:00)"
#define TIMEZONE_GMT_N_01_00		"(GMT-01:00)"
#define TIMEZONE_GMT_P_00_00		"(GMT+00:00)"
#define TIMEZONE_GMT_P_01_00		"(GMT+01:00)"
#define TIMEZONE_GMT_P_02_00		"(GMT+02:00)"
#define TIMEZONE_GMT_P_03_00		"(GMT+03:00)"
#define TIMEZONE_GMT_P_03_30		"(GMT+03:30)"
#define TIMEZONE_GMT_P_04_00		"(GMT+04:00)"
#define TIMEZONE_GMT_P_04_30		"(GMT+04:30)"
#define TIMEZONE_GMT_P_05_00		"(GMT+05:00)"
#define TIMEZONE_GMT_P_05_30		"(GMT+05:30)"
#define TIMEZONE_GMT_P_06_00		"(GMT+06:00)"
#define TIMEZONE_GMT_P_07_00		"(GMT+07:00)"
#define TIMEZONE_GMT_P_08_00		"(GMT+08:00)"
#define TIMEZONE_GMT_P_09_00		"(GMT+09:00)"
#define TIMEZONE_GMT_P_10_00		"(GMT+10:00)"
#define TIMEZONE_GMT_P_11_00		"(GMT+11:00)"
#define TIMEZONE_GMT_P_12_00		"(GMT+12:00)"

#define AUDIO_BUFFER_FRAME_PER_SECOND	72

#define GIO_SD_DETECT				86	/*Low True*/			
#define GIO_SD_WRITE_PROTECT		85						

#define SDCARD_UMOUNT_GENEARL	0						
#define SDCARD_UMOUNT_FORMAT	1					
#define SDCARD_UMOUNT_EJECT		2						

typedef enum{
	MMC_PATH_ID_MMCBLK0 = 0,
	MMC_PATH_ID_MMCBLK0P1,
	MMC_PATH_ID_MMCBLK1,
	MMC_PATH_ID_MMCBLK1P1,
	MMC_PATH_ID_TOTAL
}MMC_PATH_ID;

#define MMC_MSG_NUM_LIST { \
	SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK0_SUCCESS, \
	SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK0_SUCCESS, \
	SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK1_SUCCESS, \
	SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK1_SUCCESS, \
}

#define MMC_PATH_LIST_LEN 16
#define MMC_PATH_LIST { \
	"mmcblk0", \
	"mmcblk0p1", \
	"mmcblk1", \
	"mmcblk1p1", \
}
#define MMC_NODE_PATH_LIST_LEN 16
#define MMC_NODE_PATH_LIST { \
	"/dev/mmcblk0", \
	"/dev/mmcblk0p1", \
	"/dev/mmcblk1", \
	"/dev/mmcblk1p1", \
}
/* SD Card Mount - Dean - 20121213 - End */

/* SD Card Log - Dean - 20121213 - Begin */
#define SDCARD_MOUNT_WAIT_COUNT_MAX 10
#define SDCARD_MSG_OP_KEEP	0
#define SDCARD_MSG_OP_SEND	1

#define SDCARD_MSG_OP_SAVE_OFF	0
#define SDCARD_MSG_OP_SAVE_ON	1

#define SDCARD_STR_DATABASE_FILENAME ".sdcard_db"
#define SDCARD_STR_DATABASE_WRITE_ATTR "wb"
#define SDCARD_STR_DATABASE_READ_ATTR "rb"

#define SDCARD_ACTION_RETRY		2
#define SDCARD_ACTION_SUCCESS		1
#define SDCARD_ACTION_ERROR		0
#define SDCARD_ACTION_CARD_BUSY	-1		/* SD Card Log - End - Dean - 20120521 */
#define SDCARD_ACTION_FILE_BUSY	-2		/* SD Card File Busy - Dean - 20120704 */

typedef enum{
	SDCARD_MSG_NUM_NONE = 0,
	SDCARD_MSG_NUM_SAVE_MMC_SUCCESS_FLAG_0_FAIL,
	SDCARD_MSG_NUM_SAVE_MMC_SUCCESS_FLAG_1_FAIL,
	SDCARD_MSG_NUM_MAKE_DIR_EVENT_FAIL,
	SDCARD_MSG_NUM_MAKE_DIR_RECORD_FAIL,
	SDCARD_MSG_NUM_TOUCH_BUSY_FLAG_FAIL,
	SDCARD_MSG_NUM_TOUCH_BUSY_FLAG_BLK0_FAIL,
	SDCARD_MSG_NUM_TOUCH_BUSY_FLAG_BLK1_FAIL,
	SDCARD_MSG_NUM_FIX_FILE_FAIL,
	SDCARD_MSG_NUM_FIX_FILE_CARD_BUSY,
	SDCARD_MSG_NUM_PLAY_FAIL,
	SDCARD_MSG_NUM_PLAY_CARD_BUSY,
	SDCARD_MSG_NUM_DELETE_FAIL,
	SDCARD_MSG_NUM_DELETE_CARD_BUSY,
	SDCARD_MSG_NUM_DELETE_FILE_BUSY,
	SDCARD_MSG_NUM_DOWNLOAD_FAIL,
	SDCARD_MSG_NUM_DOWNLOAD_CARD_BUSY,
	SDCARD_MSG_NUM_DOWNLOAD_FILE_BUSY,
	SDCARD_MSG_NUM_LIST_FAIL,
	SDCARD_MSG_NUM_LIST_CARD_BUSY,
	SDCARD_MSG_NUM_QUERY_FAIL,
	SDCARD_MSG_NUM_QUERY_CARD_BUSY,
	SDCARD_MSG_NUM_SPACE_FAIL,
	SDCARD_MSG_NUM_SPACE_CARD_BUSY,
	SDCARD_MSG_NUM_FORMAT_FAIL,
	SDCARD_MSG_NUM_FORMAT_CARD_BUSY,
	SDCARD_MSG_NUM_EJECT_FAIL,
	SDCARD_MSG_NUM_EJECT_CARD_BUSY,
	SDCARD_MSG_NUM_UMOUNT_FAIL,
	SDCARD_MSG_NUM_UMOUNT_CARD_BUSY,
	SDCARD_MSG_NUM_MOUNT_FAIL,
	SDCARD_MSG_NUM_MOUNT_CARD_BUSY,
	SDCARD_MSG_NUM_MOUNT_FAIL_MAKE_REC_DIR,
	SDCARD_MSG_NUM_MOUNT_FAIL_OPEN_PARTITION_TABLE,
	SDCARD_MSG_NUM_MOUNT_FAIL_MKNOD_0,
	SDCARD_MSG_NUM_MOUNT_FAIL_MKNOD_0_P1,
	SDCARD_MSG_NUM_MOUNT_FAIL_MKNOD_1,
	SDCARD_MSG_NUM_MOUNT_FAIL_MKNOD_1_P1,
	SDCARD_MSG_NUM_MOUNT_FAIL_NO_PARTITIONS,
	SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_FAIL,
	SDCARD_MSG_NUM_WRONG_PATH_ID,
	SDCARD_MSG_NUM_NO_CARD,
	SDCARD_MSG_NUM_CARD_PROTECT,
	SDCARD_MSG_NUM_CARD_BUSY,
	SDCARD_MSG_NUM_STATUS_UMOUNT,
	SDCARD_MSG_NUM_CARD_EXISTS,
	SDCARD_MSG_NUM_CARD_WRITABLE,
	SDCARD_MSG_NUM_STATUS_MOUNT,
	SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK0_SUCCESS,
	SDCARD_MSG_NUM_CHECK_PARTITION_TABLE_BLK1_SUCCESS,
	SDCARD_MSG_NUM_MOUNT_SUCCESS,
	SDCARD_MSG_NUM_UMOUNT_SUCCESS,
	SDCARD_MSG_NUM_EJECT_SUCCESS,
	SDCARD_MSG_NUM_FORMAT_SUCCESS,
	SDCARD_MSG_NUM_SPACE_SUCCESS,
	SDCARD_MSG_NUM_QUERY_SUCCESS,
	SDCARD_MSG_NUM_LIST_SUCCESS,
	SDCARD_MSG_NUM_DOWNLOAD_SUCCESS,
	SDCARD_MSG_NUM_DELETE_SUCCESS,
	SDCARD_MSG_NUM_PLAY_SUCCESS,
	SDCARD_MSG_NUM_FIX_FILE_SUCCESS,
	SDCARD_MSG_NUM_MESSAGE_TOTAL
}SDCARD_MSG_NUM;

typedef enum{
	SDCARD_MSG_ID_NONE = -250,
	SDCARD_MSG_ID_SAVE_MMC_SUCCESS_FLAG_0_FAIL = -239,
	SDCARD_MSG_ID_SAVE_MMC_SUCCESS_FLAG_1_FAIL,
	SDCARD_MSG_ID_MAKE_DIR_EVENT_FAIL,
	SDCARD_MSG_ID_MAKE_DIR_RECORD_FAIL,
	SDCARD_MSG_ID_TOUCH_BUSY_FLAG_FAIL,
	SDCARD_MSG_ID_TOUCH_BUSY_FLAG_BLK0_FAIL,
	SDCARD_MSG_ID_TOUCH_BUSY_FLAG_BLK1_FAIL,
	SDCARD_MSG_ID_FIX_FILE_FAIL = -219,
	SDCARD_MSG_ID_FIX_FILE_CARD_BUSY,
	SDCARD_MSG_ID_PLAY_FAIL = -209,
	SDCARD_MSG_ID_PLAY_CARD_BUSY,
	SDCARD_MSG_ID_DELETE_FAIL = -199,
	SDCARD_MSG_ID_DELETE_CARD_BUSY,
	SDCARD_MSG_ID_DELETE_FILE_BUSY,
	SDCARD_MSG_ID_DOWNLOAD_FAIL = -189,
	SDCARD_MSG_ID_DOWNLOAD_CARD_BUSY,
	SDCARD_MSG_ID_DOWNLOAD_FILE_BUSY,
	SDCARD_MSG_ID_LIST_FAIL = -179,
	SDCARD_MSG_ID_LIST_CARD_BUSY,
	SDCARD_MSG_ID_QUERY_FAIL = -169,
	SDCARD_MSG_ID_QUERY_CARD_BUSY,
	SDCARD_MSG_ID_SPACE_FAIL = -159,
	SDCARD_MSG_ID_SPACE_CARD_BUSY,
	SDCARD_MSG_ID_FORMAT_FAIL = -149,
	SDCARD_MSG_ID_FORMAT_CARD_BUSY,
	SDCARD_MSG_ID_EJECT_FAIL = -139,
	SDCARD_MSG_ID_EJECT_CARD_BUSY,
	SDCARD_MSG_ID_UMOUNT_FAIL = -129,
	SDCARD_MSG_ID_UMOUNT_CARD_BUSY,
	SDCARD_MSG_ID_MOUNT_FAIL = -119,
	SDCARD_MSG_ID_MOUNT_CARD_BUSY,
	SDCARD_MSG_ID_MOUNT_FAIL_MAKE_REC_DIR,
	SDCARD_MSG_ID_MOUNT_FAIL_OPEN_PARTITION_TABLE,
	SDCARD_MSG_ID_MOUNT_FAIL_MKNOD_0,
	SDCARD_MSG_ID_MOUNT_FAIL_MKNOD_0_P1,
	SDCARD_MSG_ID_MOUNT_FAIL_MKNOD_1,
	SDCARD_MSG_ID_MOUNT_FAIL_MKNOD_1_P1,
	SDCARD_MSG_ID_MOUNT_FAIL_NO_PARTITIONS,
	SDCARD_MSG_ID_CHECK_PARTITION_TABLE_FAIL = -100,
	SDCARD_MSG_ID_WRONG_PATH_ID,
	SDCARD_MSG_ID_NO_CARD,
	SDCARD_MSG_ID_CARD_PROTECT,
	SDCARD_MSG_ID_CARD_BUSY,
	SDCARD_MSG_ID_STATUS_UMOUNT = -1,
	SDCARD_MSG_ID_CARD_EXISTS = 0,
	SDCARD_MSG_ID_CARD_WRITABLE,
	SDCARD_MSG_ID_STATUS_MOUNT,
	SDCARD_MSG_ID_CHECK_PARTITION_TABLE_BLK0_SUCCESS,
	SDCARD_MSG_ID_CHECK_PARTITION_TABLE_BLK1_SUCCESS,
	SDCARD_MSG_ID_MOUNT_SUCCESS,
	SDCARD_MSG_ID_UMOUNT_SUCCESS,
	SDCARD_MSG_ID_EJECT_SUCCESS,
	SDCARD_MSG_ID_FORMAT_SUCCESS,
	SDCARD_MSG_ID_SPACE_SUCCESS,
	SDCARD_MSG_ID_QUERY_SUCCESS,
	SDCARD_MSG_ID_LIST_SUCCESS,
	SDCARD_MSG_ID_DOWNLOAD_SUCCESS,
	SDCARD_MSG_ID_DELETE_SUCCESS,
	SDCARD_MSG_ID_PLAY_SUCCESS,
	SDCARD_MSG_ID_FIX_FILE_SUCCESS,
	SDCARD_MSG_ID_MESSAGE_TOTAL
}SDCARD_MSG_ID;

#define MMC_NODE_ERROR_CODE_LIST { \
	SDCARD_MSG_NUM_MOUNT_FAIL_MKNOD_0, \
	SDCARD_MSG_NUM_MOUNT_FAIL_MKNOD_0_P1, \
	SDCARD_MSG_NUM_MOUNT_FAIL_MKNOD_1, \
	SDCARD_MSG_NUM_MOUNT_FAIL_MKNOD_1_P1, \
}

#define SDCARD_MSG_STR_LEN 48
#define SDCARD_MSG_STR_MAX									"											"
#define SDCARD_MSG_STR_NONE									"[SDCard]: Do Nothing !!!"
#define SDCARD_MSG_STR_SAVE_MMC_SUCCESS_FLAG_0_FAIL		"[SDCard]: Save Mount Success Flag(BLK0) Fail !!!"
#define SDCARD_MSG_STR_SAVE_MMC_SUCCESS_FLAG_1_FAIL		"[SDCard]: Save Mount Success Flag(BLK1) Fail !!!"
#define SDCARD_MSG_STR_MAKE_DIR_EVENT_FAIL					"[SDCard]: Make Dir Event Fail !!!"
#define SDCARD_MSG_STR_MAKE_DIR_RECORD_FAIL					"[SDCard]: Make Dir Record Fail !!!"
#define SDCARD_MSG_STR_TOUCH_BUSY_FLAG_FAIL					"[SDCard]: Save Busy Flag Fail !!!"
#define SDCARD_MSG_STR_TOUCH_BUSY_FLAG_BLK0_FAIL			"[SDCard]: Save Busy Flag(BLK0) Fail !!!"
#define SDCARD_MSG_STR_TOUCH_BUSY_FLAG_BLK1_FAIL			"[SDCard]: Save Busy Flag(BLK1) Fail !!!"
#define SDCARD_MSG_STR_FIX_FILE_FAIL							"[SDCard]: Fix File Fail !!!"
#define SDCARD_MSG_STR_FIX_FILE_CARD_BUSY					"[SDCard]: Fix File - Card Busy !!!"
#define SDCARD_MSG_STR_PLAY_FAIL								"[SDCard]: Play Fail !!!"
#define SDCARD_MSG_STR_PLAY_CARD_BUSY						"[SDCard]: Play - Card Busy !!!"
#define SDCARD_MSG_STR_DELETE_FAIL							"[SDCard]: Delete Fail !!!"
#define SDCARD_MSG_STR_DELETE_CARD_BUSY						"[SDCard]: Delete - Card Busy !!!"
#define SDCARD_MSG_STR_DELETE_FILE_BUSY						"[SDCard]: Delete - File Busy !!!"
#define SDCARD_MSG_STR_DOWNLOAD_FAIL						"[SDCard]: Download Fail !!!"
#define SDCARD_MSG_STR_DOWNLOAD_CARD_BUSY					"[SDCard]: Download - Card Busy !!!"
#define SDCARD_MSG_STR_DOWNLOAD_FILE_BUSY					"[SDCard]: Download - File Busy !!!"
#define SDCARD_MSG_STR_LIST_FAIL								"[SDCard]: List Fail !!!"
#define SDCARD_MSG_STR_LIST_CARD_BUSY						"[SDCard]: List - Card Busy !!!"
#define SDCARD_MSG_STR_QUERY_FAIL							"[SDCard]: Search Fail !!!"
#define SDCARD_MSG_STR_QUERY_CARD_BUSY						"[SDCard]: Search - Card Busy !!!"
#define SDCARD_MSG_STR_SPACE_FAIL								"[SDCard]: Check Space Fail !!!"
#define SDCARD_MSG_STR_SPACE_CARD_BUSY						"[SDCard]: Check Space - Card Busy !!!"
#define SDCARD_MSG_STR_FORMAT_FAIL							"[SDCard]: Format Fail !!!"
#define SDCARD_MSG_STR_FORMAT_CARD_BUSY					"[SDCard]: Format - Card Busy !!!"
#define SDCARD_MSG_STR_EJECT_FAIL								"[SDCard]: Eject Fail !!!"
#define SDCARD_MSG_STR_EJECT_CARD_BUSY						"[SDCard]: Eject - Card Busy !!!"
#define SDCARD_MSG_STR_UMOUNT_FAIL							"[SDCard]: Unmount Fail !!!"
#define SDCARD_MSG_STR_UMOUNT_CARD_BUSY					"[SDCard]: Unmount - Card Busy !!!"
#define SDCARD_MSG_STR_MOUNT_FAIL							"[SDCard]: Mount Fail !!!"
#define SDCARD_MSG_STR_MOUNT_CARD_BUSY						"[SDCard]: Mount - Card Busy !!!"
#define SDCARD_MSG_STR_MOUNT_FAIL_MAKE_REC_DIR				"[SDCard]: Mount Fail - Make REC Dir!!!"
#define SDCARD_MSG_STR_MOUNT_FAIL_OPEN_PARTITION_TABLE		"[SDCard]: Mount Fail - Open Partition Table!"
#define SDCARD_MSG_STR_MOUNT_FAIL_MKNOD_0					"[SDCard]: Mount Fail - MKNOD 0!!!"
#define SDCARD_MSG_STR_MOUNT_FAIL_MKNOD_0_P1				"[SDCard]: Mount Fail - MKNOD 0P1!!!"
#define SDCARD_MSG_STR_MOUNT_FAIL_MKNOD_1					"[SDCard]: Mount Fail - MKNOD 1!!!"
#define SDCARD_MSG_STR_MOUNT_FAIL_MKNOD_1_P1				"[SDCard]: Mount Fail - MKNOD 1P1!!!"
#define SDCARD_MSG_STR_MOUNT_FAIL_NO_PARTITIONS				"[SDCard]: Mount Fail - No Partitions!!!"
#define SDCARD_MSG_STR_CHECK_PARTITION_TABLE_FAIL			"[SDCard]: Check Partiton Table Fail !!!"
#define SDCARD_MSG_STR_WRONG_PATH_ID						"[SDCard]: Wrong Path ID !!!"
#define SDCARD_MSG_STR_NO_CARD								"[SDCard]: NO Card !!!"
#define SDCARD_MSG_STR_CARD_PROTECT							"[SDCard]: Card Protect !!!"
#define SDCARD_MSG_STR_CARD_BUSY								"[SDCard]: Card Busy !!!"
#define SDCARD_MSG_STR_STATUS_UMOUNT						"[SDCard]: Status - Unmount !!!"
#define SDCARD_MSG_STR_CARD_EXISTS							"[SDCard]: Card Exists !!!"
#define SDCARD_MSG_STR_CARD_WRITABLE							"[SDCard]: Card Writable !!!"
#define SDCARD_MSG_STR_STATUS_MOUNT							"[SDCard]: Status - Mount !!!"
#define SDCARD_MSG_STR_CHECK_PARTITION_TABLE_BLK0_SUCCESS	"[SDCard]: Check PartitonTable(BLK0) Success !"
#define SDCARD_MSG_STR_CHECK_PARTITION_TABLE_BLK1_SUCCESS	"[SDCard]: Check PartitonTable(BLK1) Success !"
#define SDCARD_MSG_STR_MOUNT_SUCCESS						"[SDCard]: Mount Success !!!"
#define SDCARD_MSG_STR_UMOUNT_SUCCESS						"[SDCard]: Unmount Success !!!"
#define SDCARD_MSG_STR_EJECT_SUCCESS							"[SDCard]: Eject Success !!!"
#define SDCARD_MSG_STR_FORMAT_SUCCESS						"[SDCard]: Format Success !!!"
#define SDCARD_MSG_STR_SPACE_SUCCESS							"[SDCard]: Check Space Success !!!"
#define SDCARD_MSG_STR_QUERY_SUCCESS						"[SDCard]: Search Success !!!"
#define SDCARD_MSG_STR_LIST_SUCCESS							"[SDCard]: List Success !!!"
#define SDCARD_MSG_STR_DOWNLOAD_SUCCESS					"[SDCard]: Download Success !!!"
#define SDCARD_MSG_STR_DELETE_SUCCESS						"[SDCard]: Delete Success !!!"
#define SDCARD_MSG_STR_PLAY_SUCCESS							"[SDCard]: Play Success !!!"
#define SDCARD_MSG_STR_FIX_FILE_SUCCESS						"[SDCard]: Fix File Success !!!"

#define SDCARD_MSG_LIST { \
	{SDCARD_MSG_ID_NONE, 									SDCARD_MSG_STR_NONE,									SDCARD_MSG_OP_KEEP}, \
	{SDCARD_MSG_ID_SAVE_MMC_SUCCESS_FLAG_0_FAIL, 			SDCARD_MSG_STR_SAVE_MMC_SUCCESS_FLAG_0_FAIL,			SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_SAVE_MMC_SUCCESS_FLAG_1_FAIL, 			SDCARD_MSG_STR_SAVE_MMC_SUCCESS_FLAG_1_FAIL,			SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MAKE_DIR_EVENT_FAIL, 					SDCARD_MSG_STR_MAKE_DIR_EVENT_FAIL,					SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MAKE_DIR_RECORD_FAIL, 					SDCARD_MSG_STR_MAKE_DIR_RECORD_FAIL,					SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_TOUCH_BUSY_FLAG_FAIL, 					SDCARD_MSG_STR_TOUCH_BUSY_FLAG_FAIL,					SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_TOUCH_BUSY_FLAG_BLK0_FAIL, 				SDCARD_MSG_STR_TOUCH_BUSY_FLAG_BLK0_FAIL,				SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_TOUCH_BUSY_FLAG_BLK1_FAIL, 				SDCARD_MSG_STR_TOUCH_BUSY_FLAG_BLK1_FAIL,				SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_FIX_FILE_FAIL,							SDCARD_MSG_STR_FIX_FILE_FAIL,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_FIX_FILE_CARD_BUSY,						SDCARD_MSG_STR_FIX_FILE_CARD_BUSY,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_PLAY_FAIL,								SDCARD_MSG_STR_PLAY_FAIL,								SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_PLAY_CARD_BUSY,							SDCARD_MSG_STR_PLAY_CARD_BUSY,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_DELETE_FAIL,								SDCARD_MSG_STR_DELETE_FAIL,								SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_DELETE_CARD_BUSY,						SDCARD_MSG_STR_DELETE_CARD_BUSY,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_DELETE_FILE_BUSY,						SDCARD_MSG_STR_DELETE_FILE_BUSY,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_DOWNLOAD_FAIL,							SDCARD_MSG_STR_DOWNLOAD_FAIL,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_DOWNLOAD_CARD_BUSY,					SDCARD_MSG_STR_DOWNLOAD_CARD_BUSY,					SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_DOWNLOAD_FILE_BUSY,					SDCARD_MSG_STR_DOWNLOAD_FILE_BUSY,					SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_LIST_FAIL,								SDCARD_MSG_STR_LIST_FAIL,								SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_LIST_CARD_BUSY,							SDCARD_MSG_STR_LIST_CARD_BUSY,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_QUERY_FAIL,								SDCARD_MSG_STR_QUERY_FAIL,								SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_QUERY_CARD_BUSY,						SDCARD_MSG_STR_QUERY_CARD_BUSY,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_SPACE_FAIL,								SDCARD_MSG_STR_SPACE_FAIL,								SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_SPACE_CARD_BUSY,						SDCARD_MSG_STR_SPACE_CARD_BUSY,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_FORMAT_FAIL,							SDCARD_MSG_STR_FORMAT_FAIL,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_FORMAT_CARD_BUSY,						SDCARD_MSG_STR_FORMAT_CARD_BUSY,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_EJECT_FAIL,								SDCARD_MSG_STR_EJECT_FAIL,								SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_EJECT_CARD_BUSY,						SDCARD_MSG_STR_EJECT_CARD_BUSY,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_UMOUNT_FAIL,							SDCARD_MSG_STR_UMOUNT_FAIL,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_UMOUNT_CARD_BUSY,						SDCARD_MSG_STR_UMOUNT_CARD_BUSY,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MOUNT_FAIL,								SDCARD_MSG_STR_MOUNT_FAIL,								SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MOUNT_CARD_BUSY,						SDCARD_MSG_STR_MOUNT_CARD_BUSY,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MOUNT_FAIL_MAKE_REC_DIR,				SDCARD_MSG_STR_MOUNT_FAIL_MAKE_REC_DIR,				SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MOUNT_FAIL_OPEN_PARTITION_TABLE,		SDCARD_MSG_STR_MOUNT_FAIL_OPEN_PARTITION_TABLE,		SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MOUNT_FAIL_MKNOD_0,					SDCARD_MSG_STR_MOUNT_FAIL_MKNOD_0,					SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MOUNT_FAIL_MKNOD_0_P1,					SDCARD_MSG_STR_MOUNT_FAIL_MKNOD_0_P1,					SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MOUNT_FAIL_MKNOD_1,					SDCARD_MSG_STR_MOUNT_FAIL_MKNOD_1,					SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MOUNT_FAIL_MKNOD_1_P1,					SDCARD_MSG_STR_MOUNT_FAIL_MKNOD_1_P1,					SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_MOUNT_FAIL_NO_PARTITIONS,				SDCARD_MSG_STR_MOUNT_FAIL_NO_PARTITIONS,				SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_CHECK_PARTITION_TABLE_FAIL,				SDCARD_MSG_STR_CHECK_PARTITION_TABLE_FAIL,				SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_WRONG_PATH_ID,							SDCARD_MSG_STR_WRONG_PATH_ID,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_NO_CARD,								SDCARD_MSG_STR_NO_CARD,								SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_CARD_PROTECT,							SDCARD_MSG_STR_CARD_PROTECT,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_CARD_BUSY,								SDCARD_MSG_STR_CARD_BUSY,								SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_STATUS_UMOUNT,							SDCARD_MSG_STR_STATUS_UMOUNT,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_CARD_EXISTS,							SDCARD_MSG_STR_CARD_EXISTS,							SDCARD_MSG_OP_KEEP}, \
	{SDCARD_MSG_ID_CARD_WRITABLE,							SDCARD_MSG_STR_CARD_WRITABLE,							SDCARD_MSG_OP_KEEP}, \
	{SDCARD_MSG_ID_STATUS_MOUNT,							SDCARD_MSG_STR_STATUS_MOUNT,							SDCARD_MSG_OP_KEEP}, \
	{SDCARD_MSG_ID_CHECK_PARTITION_TABLE_BLK0_SUCCESS,		SDCARD_MSG_STR_CHECK_PARTITION_TABLE_BLK0_SUCCESS,	SDCARD_MSG_OP_KEEP}, \
	{SDCARD_MSG_ID_CHECK_PARTITION_TABLE_BLK1_SUCCESS,		SDCARD_MSG_STR_CHECK_PARTITION_TABLE_BLK1_SUCCESS,	SDCARD_MSG_OP_KEEP}, \
	{SDCARD_MSG_ID_MOUNT_SUCCESS, 						SDCARD_MSG_STR_MOUNT_SUCCESS,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_UMOUNT_SUCCESS, 						SDCARD_MSG_STR_UMOUNT_SUCCESS,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_EJECT_SUCCESS,							SDCARD_MSG_STR_EJECT_SUCCESS,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_FORMAT_SUCCESS, 						SDCARD_MSG_STR_FORMAT_SUCCESS,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_SPACE_SUCCESS, 							SDCARD_MSG_STR_SPACE_SUCCESS,							SDCARD_MSG_OP_KEEP}, \
	{SDCARD_MSG_ID_QUERY_SUCCESS, 							SDCARD_MSG_STR_QUERY_SUCCESS,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_LIST_SUCCESS, 							SDCARD_MSG_STR_LIST_SUCCESS,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_DOWNLOAD_SUCCESS, 						SDCARD_MSG_STR_DOWNLOAD_SUCCESS,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_DELETE_SUCCESS, 						SDCARD_MSG_STR_DELETE_SUCCESS,						SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_PLAY_SUCCESS, 							SDCARD_MSG_STR_PLAY_SUCCESS,							SDCARD_MSG_OP_SEND}, \
	{SDCARD_MSG_ID_FIX_FILE_SUCCESS, 						SDCARD_MSG_STR_FIX_FILE_SUCCESS,						SDCARD_MSG_OP_SEND}, \
}
typedef struct {
	int	MsgId;
	char MsgStr[SDCARD_MSG_STR_LEN];
	char MsgOp;
}SDCardMsg;

#define REC_TO_SDCARD			1							
#define REC_TO_SAMBA			1
#define REC_TO_USB				0
#define REC_TO_TMP				0							

#define TEST_RECORD  0

/* Record Media - Dean - 20130530 - Begin */
#define DEF_SDCARD_RECORD_RECYCLE_FILE	1
#define DEF_SAMBA_RECORD_RECYCLE_FILE	1
#define DEF_USB_RECORD_RECYCLE_FILE		0
#define DEF_TMP_RECORD_RECYCLE_FILE		0

typedef enum
{
	DISK_MEDIA_SDCARD = 0,
	DISK_MEDIA_SAMBA,
	DISK_MEDIA_USB,
	DISK_MEDIA_TMP,
	DISK_MEDIA_TOTAL
}DISK_MEDIA;

#define RECORD_MEDIA_NONE		0x00	
#define RECORD_MEDIA_CLEAR		0x00	
#define RECORD_MEDIA_ENABLE		0x01	
#define RECORD_MEDIA_SDCARD		0x02	
#define RECORD_MEDIA_SAMBA		0x04	
#define RECORD_MEDIA_USB		0x08	
#define RECORD_MEDIA_TMP		0x10	
#define RECORD_MEDIA_DISABLE	0x1E	
#define RECORD_MEDIA_ALL		0x1F	
#define RECORD_MEDIA_DEFAULT	RECORD_MEDIA_SDCARD		
#define RECORD_MEDIA_CHECK_TIME	10		

#define DISK_MEDIA_STR_LEN			64
#define DISK_MEDIA_STR_LIST			{"SDCard", "Samba", "USB", "TMP"}
#define DISK_MEDIA_STR_PATH_LIST	{MMC_DIR, SAMBA_REC_DIR, USB_DIR, TEMP_FOLDER}

#define REC_VIDEO_TO_MEDIA_LIST_NUM		DISK_MEDIA_TOTAL
#define REC_VIDEO_TO_MEDIA_LIST_DEF		{REC_TO_SDCARD, REC_TO_SAMBA, REC_TO_USB, REC_TO_TMP}
#define REC_VIDEO_TO_MEDIA_LIST_VALUE	{RECORD_MEDIA_SDCARD, RECORD_MEDIA_SAMBA, RECORD_MEDIA_USB, RECORD_MEDIA_TMP}
#define REC_VIDEO_TO_MEDIA_LIST_RECYCLE	{DEF_SDCARD_RECORD_RECYCLE_FILE, DEF_SAMBA_RECORD_RECYCLE_FILE, DEF_USB_RECORD_RECYCLE_FILE, DEF_TMP_RECORD_RECYCLE_FILE}
#define REC_VIDEO_TO_MEDIA_LIST_DB		{DB_MEDIA_SDCARD_RECORD, DB_MEDIA_SAMBA_RECORD, DB_MEDIA_USB_RECORD, DB_MEDIA_TMP_RECORD}
#define REC_VIDEO_TO_MEDIA_LIST			{DISK_MEDIA_SDCARD, DISK_MEDIA_SAMBA, DISK_MEDIA_USB, DISK_MEDIA_TMP}
/* Samba - Record - Dean - 20130125 - End */
/* Record Media - Dean - 20130530 - End */

#define FILE_ACTION_ERROR		-1
#define FILE_ACTION_SUCCESS		 1
#define FILE_ACTION_FAIL		 0
#define FILE_ACTION_BUSY		-2			

#define FILE_RETRY_COUNT_MAX	20			
#define FILE_RETRY_COUNT_LOOP_TIME 300000	

typedef enum{
	FILE_OP_NONE = 0,
	FILE_OP_READ,
	FILE_OP_WRITE,
	FILE_OP_TOTAL
}FILE_OP;

#define FILE_OP_RETRY_COUNT_MAX			10
#define FILE_OP_RETRY_COUNT_LOOP_TIME	100000

#define DB_STR_DATABASE_FILENAME		".db_file"
#define DB_STR_DATABASE_BACKUP_FILENAME	".db_backup"		
#define DB_STR_DATABASE_WRITE_ATTR	    "wb"
#define DB_STR_DATABASE_READ_ATTR		"rb"

#define DB_STR_FILE_INFO_SET_FORMAT	"%07d%010d%s"
#define DB_STR_FILE_INFO_GET_FORMAT	"%07d%010d%s"

#define DB_FILE_NUM_MAX				10000

#define DB_FILE_INFO_TEMP_LEN			4		

#define DB_FILE_INFO_SIZE_LEN			7
#define DB_FILE_INFO_TIME_LEN			10
#define DB_FILE_INFO_NAME_LEN			63		
#define DB_FILE_INFO_DATA_LEN			(DB_FILE_INFO_SIZE_LEN + DB_FILE_INFO_TIME_LEN + DB_FILE_INFO_NAME_LEN)	

#define DB_FILE_INFO_SIZE_SHIFT			0
#define DB_FILE_INFO_TIME_SHIFT			(DB_FILE_INFO_SIZE_SHIFT + DB_FILE_INFO_SIZE_LEN)
#define DB_FILE_INFO_NAME_SHIFT		(DB_FILE_INFO_TIME_SHIFT + DB_FILE_INFO_TIME_LEN)

#define DB_FILE_PATH_LEN_MAX			128
#define DB_FILE_FULL_NAME_LEN_MAX		(DB_FILE_INFO_NAME_LEN + DB_FILE_PATH_LEN_MAX)

#define DB_FILE_TIME_VALUE_MIN			0
#define DB_FILE_TIME_VALUE_MAX		9999999999

#define DB_MEDIA_DIR_LEN	64

typedef enum
{
	DB_CHECK_MAIN_FILE = 0,
	DB_CHECK_BACKUP_FILE,
	DB_CHECK_SEARCH_FILE,
	DB_CHECK_FILE_TOTAL
}DB_CHECK_FILE;

#define DB_CHECK_FILE_NAME_LEN	16
#define DB_CHECK_FILE_NAME_LIST	{"file", "backup", "search"}
#define DB_FILE_SEARCH_PATH		"/tmp"							
#define DB_FILE_SEARCH_SDCARD_EVENT		".db_sdevent"
#define DB_FILE_SEARCH_SDCARD_RECORD	".db_sdrecord"
#define DB_FILE_SEARCH_SAMBA_EVENT		".db_sambaevent"
#define DB_FILE_SEARCH_SAMBA_RECORD		".db_sambarecord"

#define DB_MEDIA_DIR_LIST_NUM	((REC_TO_SDCARD + REC_TO_SAMBA + REC_TO_USB + REC_TO_TMP) * 2)
#if (REC_TO_SDCARD && REC_TO_SAMBA && REC_TO_USB && REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {MMC_EVENT_DIR, MMC_RECORD_DIR, SAMBA_EVENT_FOLDER, SAMBA_REC_FOLDER, USB_EVENT_DIR, USB_RECORD_DIR, TEMP_FOLDER, TEMP_FOLDER}
#elif (!REC_TO_SDCARD && REC_TO_SAMBA && REC_TO_USB && REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {SAMBA_EVENT_FOLDER, SAMBA_REC_FOLDER, USB_EVENT_DIR, USB_RECORD_DIR, TEMP_FOLDER, TEMP_FOLDER}
#elif (REC_TO_SDCARD && !REC_TO_SAMBA && REC_TO_USB && REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {MMC_EVENT_DIR, MMC_RECORD_DIR, USB_EVENT_DIR, USB_RECORD_DIR, TEMP_FOLDER, TEMP_FOLDER}
#elif (REC_TO_SDCARD && REC_TO_SAMBA && !REC_TO_USB && REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {MMC_EVENT_DIR, MMC_RECORD_DIR, SAMBA_EVENT_FOLDER, SAMBA_REC_FOLDER, TEMP_FOLDER, TEMP_FOLDER}
#elif (REC_TO_SDCARD && REC_TO_SAMBA && REC_TO_USB && !REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {MMC_EVENT_DIR, MMC_RECORD_DIR, SAMBA_EVENT_FOLDER, SAMBA_REC_FOLDER, USB_EVENT_DIR, USB_RECORD_DIR}
#elif (!REC_TO_SDCARD && !REC_TO_SAMBA && REC_TO_USB && REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {USB_EVENT_DIR, USB_RECORD_DIR, TEMP_FOLDER, TEMP_FOLDER}
#elif (!REC_TO_SDCARD && REC_TO_SAMBA && !REC_TO_USB && REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {SAMBA_EVENT_FOLDER, SAMBA_REC_FOLDER, TEMP_FOLDER, TEMP_FOLDER}
#elif (!REC_TO_SDCARD && REC_TO_SAMBA && REC_TO_USB && !REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {SAMBA_EVENT_FOLDER, SAMBA_REC_FOLDER, USB_EVENT_DIR, USB_RECORD_DIR}
#elif (REC_TO_SDCARD && !REC_TO_SAMBA && !REC_TO_USB && REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {MMC_EVENT_DIR, MMC_RECORD_DIR, TEMP_FOLDER, TEMP_FOLDER}
#elif (REC_TO_SDCARD && !REC_TO_SAMBA && REC_TO_USB && !REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {MMC_EVENT_DIR, MMC_RECORD_DIR, USB_EVENT_DIR, USB_RECORD_DIR, }
#elif (REC_TO_SDCARD && REC_TO_SAMBA && !REC_TO_USB && !REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {MMC_EVENT_DIR, MMC_RECORD_DIR, SAMBA_EVENT_FOLDER, SAMBA_REC_FOLDER}
#elif (!REC_TO_SDCARD && !REC_TO_SAMBA && !REC_TO_USB && REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {TEMP_FOLDER, TEMP_FOLDER}
#elif (!REC_TO_SDCARD && !REC_TO_SAMBA && REC_TO_USB && !REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {USB_EVENT_DIR, USB_RECORD_DIR}
#elif (!REC_TO_SDCARD && REC_TO_SAMBA && !REC_TO_USB && !REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {SAMBA_EVENT_FOLDER, SAMBA_REC_FOLDER}
#elif (REC_TO_SDCARD && !REC_TO_SAMBA && !REC_TO_USB && !REC_TO_TMP)
#define DB_MEDIA_DIR_LIST {MMC_EVENT_DIR, MMC_RECORD_DIR}
#else
#define DB_MEDIA_DIR_LIST {MMC_EVENT_DIR}
#endif

typedef enum
{
	DB_FLAG_BUSY = 0,
	DB_FLAG_TOTAL
}DB_FLAG;

typedef enum
{
#if REC_TO_SDCARD
	DB_MEDIA_SDCARD_EVENT = 0,
	DB_MEDIA_SDCARD_RECORD,
#endif
#if REC_TO_SAMBA
#if (!REC_TO_SDCARD)
	DB_MEDIA_SAMBA_EVENT = 0,
#else
	DB_MEDIA_SAMBA_EVENT,
#endif
	DB_MEDIA_SAMBA_RECORD,
#endif
#if REC_TO_USB
#if ((!REC_TO_SDCARD) && (!REC_TO_SAMBA))
	DB_MEDIA_USB_EVENT = 0,
#else
	DB_MEDIA_USB_EVENT,
#endif
	DB_MEDIA_USB_RECORD,
#endif
#if REC_TO_TMP
#if ((!REC_TO_SDCARD) && (!REC_TO_SAMBA) && (!REC_TO_USB))
	DB_MEDIA_TMP_EVENT = 0,
#else
	DB_MEDIA_TMP_EVENT,
#endif
	DB_MEDIA_TMP_RECORD,
#endif
	DB_MEDIA_SEARCH_EVENT,				
	DB_MEDIA_SEARCH_RECORD,				
	DB_MEDIA_TOTAL
}DB_MEDIA;

#if !REC_TO_SDCARD
#define DB_MEDIA_SDCARD_EVENT	(DB_MEDIA_TOTAL + 1)
#define DB_MEDIA_SDCARD_RECORD	(DB_MEDIA_TOTAL + 2)
#endif
#if !REC_TO_SAMBA
#define DB_MEDIA_SAMBA_EVENT	(DB_MEDIA_TOTAL + 3)
#define DB_MEDIA_SAMBA_RECORD	(DB_MEDIA_TOTAL + 4)
#endif
#if !REC_TO_USB
#define DB_MEDIA_USB_EVENT		(DB_MEDIA_TOTAL + 5)
#define DB_MEDIA_USB_RECORD		(DB_MEDIA_TOTAL + 6)
#endif
#if !REC_TO_TMP
#define DB_MEDIA_TMP_EVENT		(DB_MEDIA_TOTAL + 7)
#define DB_MEDIA_TMP_RECORD		(DB_MEDIA_TOTAL + 8)
#endif

typedef enum
{
	DB_COUNT_CHECK_RECORD,
	DB_COUNT_CHECK_EVENT,
	DB_COUNT_CHECK_MANAGE,
	DB_COUNT_CHECK_TOTAL,
}DB_COUNT_CHECK;

typedef enum
{
	DB_CHECK_FNAME_SINGLE = 0,
	DB_CHECK_FNAME_ALL,
	DB_CHECK_FNAME_TOTAL
}DB_CHECK_FNAME;

typedef struct {
	char		size[DB_FILE_INFO_SIZE_LEN];
	char		time[DB_FILE_INFO_TIME_LEN];
	char		fname[DB_FILE_INFO_NAME_LEN];
} DBItemInfo;

typedef struct {
	int		total;
	int		record;				
	long	totalsize;			
	char	reserved[DB_FILE_INFO_TEMP_LEN];
} DBListHead;

typedef struct {
	DBListHead head;
	char		DBPath[DB_MEDIA_DIR_LEN];			
	char		DBSearchPath[DB_MEDIA_DIR_LEN];		
	char		flag[DB_FLAG_TOTAL];
	char		KeepList[DB_FILE_NUM_MAX + 1];
	DBItemInfo InfoList[DB_FILE_NUM_MAX + 1];
} DBListInfo;

#define DB_RETRY_COUNT_MAX 10
#define DB_RETRY_COUNT_LOOP_TIME 300000

typedef enum
{
	DB_KEEP_LIST_OP_REMOVE = 0,
	DB_KEEP_LIST_OP_ADD,
	DB_KEEP_LIST_OP_TOTAL
}DB_KEEP_LIST_OP;

#define DB_INFO_LIST_OP_SHIFT_LIST {DB_FILE_INFO_TIME_SHIFT, DB_FILE_INFO_NAME_SHIFT}

typedef enum
{
	DB_INFO_LIST_OP_SEARCH_DATE = 0,
	DB_INFO_LIST_OP_SEARCH_NAME,
	DB_INFO_LIST_OP_TOTAL
}DB_INFO_LIST_OP;

typedef enum
{
	DB_FLAG_OP_OFF = 0,
	DB_FLAG_OP_ON,
	DB_FLAG_OP_TOTAL
}DB_FLAG_OP;

typedef enum
{
	DB_RECYCLE_OP_FREESPACE = 0,
	DB_RECYCLE_OP_AMOUNT,
	DB_RECYCLE_OP_RECORD,			
	DB_RECYCLE_OP_TOTAL
}DB_RECYCLE_OP;

#define DB_ACTION_ERROR		-1
#define DB_ACTION_SUCCESS		1
#define DB_ACTION_FAIL			0
#define DB_ACTION_BUSY			2
#define DB_ACTION_UPDATE		3		

typedef enum{
	DB_FILE_NONE				= 0,
	DB_FILE_READ				= 1,
	DB_FILE_WRITE				= 2,
} DB_FILE_ACTION;

typedef enum{
	DB_READ_DATA_ACTION_NONE	= 0x00,
	DB_READ_DATA_ACTION_NUMBER	= 0x01,
	DB_READ_DATA_ACTION_SPACE	= 0x02,			
	DB_READ_DATA_ACTION_DATA	= 0x10,			
	DB_READ_DATA_ACTION_SEARCH	= 0x11,			
	DB_READ_DATA_ACTION_DATA_ALL= 0x20,			
	DB_READ_DATA_ACTION_SEARCH_ALL= 0x21,		
} DB_READ_DATA_ACTION;

typedef enum{
	DB_MAINTAIN_DATA_ACTION_BACKUP	= 0,
	DB_MAINTAIN_DATA_ACTION_RESTORE	= 1,
	DB_MAINTAIN_DATA_ACTION_REPAIR	= 2,
	DB_MAINTAIN_DATA_ACTION_SEARCH	= 3,		
	DB_MAINTAIN_DATA_ACTION_ALL					
} DB_MAINTAIN_DATA_ACTION;

typedef enum{
	UPLOAD_LOG_NONE = 0,
	UPLOAD_LOG_START,
	UPLOAD_LOG_END,
	UPLOAD_LOG_FTP_ACTION,		
	UPLOAD_LOG_FTP_DISABLE,		
	UPLOAD_LOG_MESSAGE_TOTAL
}UPLOAD_LOG_MESSAGE;

#if DIPS_FEATURE

#define DIPS_PROVIDER_STR_IDX_0		"0"
#define DIPS_PROVIDER_STR_IDX_1		"1"
#define DIPS_PROVIDER_STR_IDX_2		"2"
#define DIPS_PROVIDER_STR_IDX_3		"3"
#define DIPS_PROVIDER_STR_IDX_4		"4"

enum{
	DIPS_ID_BYTE_VENDOR_0 = 0,
	DIPS_ID_BYTE_VENDOR_1,
	DIPS_ID_BYTE_MAC_0,
	DIPS_ID_BYTE_MAC_1,
	DIPS_ID_BYTE_MAC_2,
	DIPS_ID_BYTE_MAC_3,
	DIPS_ID_BYTE_MAC_4,
	DIPS_ID_BYTE_MAC_5,
	DIPS_ID_BYTE_CHECK_SUM,
	DIPS_ID_BYTE_ADD_N,
	DIPS_ID_BYTE_TOTAL
};

#define DIPS_PROVIDER_STR_LEN		8
#define DIPS_PROVIDER_STR_AIRLIVE	"Airlive"
#define DIPS_VENDOR_STR_SMAX		"05"
#define DIPS_PROVIDER_STR_WEB_LEN	16

enum{
	DIPS_PROVIDER_AIRLIVE = 0,
	DIPS_PROVIDER_TOTAL,
};

#define DIPS_PROVIDER_MIN			0
#define DIPS_PROVIDER_MAX			(DIPS_PROVIDER_TOTAL - 1)

#define DIPS_PROVIDER_LIST			{DIPS_PROVIDER_STR_AIRLIVE}
#define DIPS_PROVIDER_IDX_LIST		{DIPS_PROVIDER_STR_IDX_0}
#define DIPS_PROVIDER_WEB_LIST		{DIPS_PROVIDER_STR_AIRLIVE}
#define DIPS_PROVIDER_WEB			(DIPS_PROVIDER_TOTAL)

enum {
	DIPS_RETURN_CODE_FAIL = 0,
	DIPS_RETURN_CODE_SUCCESS,
	DIPS_RETURN_CODE_CONNECTING,	
	DIPS_RETURN_CODE_NO_ACTION, 	
	DIPS_RETURN_CODE_COMPLETE,		
	DIPS_RETURN_CODE_TOTAL
};
#define DIPS_RETURN_CODE_LEN 16
#define DIPS_RETURN_CODE_LIST {"FAIL", "Success"}
#endif


#define SPACE_REQUIRED_SIZE_RECORD (512*1024)//(50*1024*1024)			
#define SPACE_REQUIRED_SIZE_RECORD_RECYCLE (50*1024)//(50*1024*1024)	
#define SPACE_REQUIRED_SIZE_EVENT (600*1024)//(9*1024*1024)				
#define SPACE_REQUIRED_SIZE_EVENT_RECYCLE (10*1024)//(9*1024*1024)	

#if HTTP_NOTIFY_FEATURE
enum{
	EVENT_HTTP_NOTIFY_RETURN_CODE_FAIL,
	EVENT_HTTP_NOTIFY_RETURN_CODE_SUCCESS,
	EVENT_HTTP_NOTIFY_RETURN_CODE_TOTAL
};

#define EVENT_HTTP_NOTIFY_RETURN_CODE_LEN 16
#define EVENT_HTTP_NOTIFY_RETURN_CODE_LIST {"FAIL", "Success"}
#endif

#define RECORD_CHECK_DISK_SPACE_FAIL		-3
#define RECORD_CHECK_DISK_SPACE_EMPTY		-2
#define RECORD_CHECK_DISK_SPACE_NOT_ENOUGH	-1
#define RECORD_CHECK_DISK_SPACE_ENOUGH		 1

enum {
	AVI_REC_PARAM_FILENAME_SDCARD = 1,
	AVI_REC_PARAM_FILENAME_SAMBA,
	AVI_REC_PARAM_FILENAME_USB,
	AVI_REC_PARAM_FILENAME_TMP,
	AVI_REC_PARAM_REC_TO_SDCARD,
	AVI_REC_PARAM_REC_TO_SAMBA,
	AVI_REC_PARAM_REC_TO_USB,
	AVI_REC_PARAM_REC_TO_TMP,
	AVI_REC_PARAM_REC_SIZE,
	AVI_REC_PARAM_RESOLUTION,
	AVI_REC_PARAM_STREAM_FRAME_ID,
	AVI_REC_PARAM_FRAMERATE,
	AVI_REC_PARAM_AUDIO_ENABLE,
	AVI_REC_PARAM_TIMESTAMP_ENABLE,
	AVI_REC_PARAM_TOTAL,
}AVI_REC_PARAMETER;

#define AVI_REC_INFO_FNAME_LEN_MAX 240
typedef struct {
	char		enable;
	char		strFilename[AVI_REC_INFO_FNAME_LEN_MAX];
	char		reserved[15];
} AviRecordMediaInfo;

typedef struct {
	int 	maxsize;
	int 	resolution;
	int 	streamtype;
	int 	framerate;
	int 	audioenable;
	int 	timestamp;
	long	filetime;								
	AviRecordMediaInfo media[DISK_MEDIA_TOTAL];		
	char	reserved[24];
} AviRecordInfo;

typedef struct {
	int 	resolution;
	int 	aviype;
	int 	framerate;
	char	reserved[12];
} RecordVideoInfo;

#define AVI_RECORD_MSG_SUCCESS	1

#define AVI_RECORD_MSG_OP_KEEP	0
#define AVI_RECORD_MSG_OP_SEND	1

typedef enum{
	AVI_RECORD_MSG_ID_NONE = -250,
	AVI_RECORD_MSG_ID_OPEN_SDCARD_AVI_FILE_FAIL = -239,
	AVI_RECORD_MSG_ID_OPEN_SAMBA_AVI_FILE_FAIL,
	AVI_RECORD_MSG_ID_OPEN_USB_AVI_FILE_FAIL,
	AVI_RECORD_MSG_ID_OPEN_TMP_AVI_FILE_FAIL,
	AVI_RECORD_MSG_ID_OPEN_SDCARD_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_ID_OPEN_SAMBA_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_ID_OPEN_USB_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_ID_OPEN_TMP_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_ID_GET_SDCARD_CONFIG_FILE_FAIL = -229,
	AVI_RECORD_MSG_ID_GET_SAMBA_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_ID_GET_USB_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_ID_GET_TMP_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_ID_GET_VIDEO_INERFACE_FAIL = -219,
	AVI_RECORD_MSG_ID_GET_AUDIO_INERFACE_FAIL,
	AVI_RECORD_MSG_ID_WRT_VIDEO_FRAME_TO_SDCARD_FAIL = -209,
	AVI_RECORD_MSG_ID_WRT_AUDIO_FRAME_TO_SDCARD_FAIL,
	AVI_RECORD_MSG_ID_WRT_VIDEO_FRAME_TO_SAMBA_FAIL,
	AVI_RECORD_MSG_ID_WRT_AUDIO_FRAME_TO_SAMBA_FAIL,
	AVI_RECORD_MSG_ID_WRT_VIDEO_FRAME_TO_USB_FAIL,
	AVI_RECORD_MSG_ID_WRT_AUDIO_FRAME_TO_USB_FAIL,
	AVI_RECORD_MSG_ID_WRT_VIDEO_FRAME_TO_TMP_FAIL,
	AVI_RECORD_MSG_ID_WRT_AUDIO_FRAME_TO_TMP_FAIL,
	AVI_RECORD_MSG_ID_GET_SHARED_MEM_FAIL = -199,
	AVI_RECORD_MSG_ID_GET_INFO_FAIL,
	AVI_RECORD_MSG_ID_GET_SHARED_MEM_SAMBA_FAIL,
	AVI_RECORD_MSG_ID_GET_REC_VIDEO_PARAM_FAIL = -189,
	AVI_RECORD_MSG_ID_GET_AVI_REC_PARAM_FAIL,
	AVI_RECORD_MSG_ID_FREE_MEMORY_NOT_ENOUGH = -179,
	AVI_RECORD_MSG_ID_FAIL = -1,
	AVI_RECORD_MSG_ID_SUCCESS = 0,
	AVI_RECORD_MSG_ID_SAVE_TO_SDCARD_SUCCESS,
	AVI_RECORD_MSG_ID_SAVE_TO_SAMBA_SUCCESS,
	AVI_RECORD_MSG_ID_SAVE_TO_USB_SUCCESS,
	AVI_RECORD_MSG_ID_SAVE_TO_TMP_SUCCESS,
	AVI_RECORD_MSG_ID_WRT_VIDEO_FRAME_SUCCESS,
	AVI_RECORD_MSG_ID_WRT_AUDIO_FRAME_SUCCESS,
	AVI_RECORD_MSG_ID_TOTAL
}AVI_RECORD_MSG_ID;

typedef enum{
	AVI_RECORD_MSG_NUM_NONE = 0,
	AVI_RECORD_MSG_NUM_OPEN_SDCARD_AVI_FILE_FAIL,
	AVI_RECORD_MSG_NUM_OPEN_SAMBA_AVI_FILE_FAIL,
	AVI_RECORD_MSG_NUM_OPEN_USB_AVI_FILE_FAIL,
	AVI_RECORD_MSG_NUM_OPEN_TMP_AVI_FILE_FAIL,
	AVI_RECORD_MSG_NUM_OPEN_SDCARD_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_NUM_OPEN_SAMBA_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_NUM_OPEN_USB_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_NUM_OPEN_TMP_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_NUM_GET_SDCARD_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_NUM_GET_SAMBA_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_NUM_GET_USB_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_NUM_GET_TMP_CONFIG_FILE_FAIL,
	AVI_RECORD_MSG_NUM_GET_VIDEO_INERFACE_FAIL,
	AVI_RECORD_MSG_NUM_GET_AUDIO_INERFACE_FAIL,
	AVI_RECORD_MSG_NUM_WRT_VIDEO_FRAME_TO_SDCARD_FAIL,
	AVI_RECORD_MSG_NUM_WRT_AUDIO_FRAME_TO_SDCARD_FAIL,
	AVI_RECORD_MSG_NUM_WRT_VIDEO_FRAME_TO_SAMBA_FAIL,
	AVI_RECORD_MSG_NUM_WRT_AUDIO_FRAME_TO_SAMBA_FAIL,
	AVI_RECORD_MSG_NUM_WRT_VIDEO_FRAME_TO_USB_FAIL,
	AVI_RECORD_MSG_NUM_WRT_AUDIO_FRAME_TO_USB_FAIL,
	AVI_RECORD_MSG_NUM_WRT_VIDEO_FRAME_TO_TMP_FAIL,
	AVI_RECORD_MSG_NUM_WRT_AUDIO_FRAME_TO_TMP_FAIL,
	AVI_RECORD_MSG_NUM_GET_SHARED_MEM_FAIL,
	AVI_RECORD_MSG_NUM_GET_INFO_FAIL,
	AVI_RECORD_MSG_NUM_GET_SHARED_MEM_SAMBA_FAIL,
	AVI_RECORD_MSG_NUM_GET_REC_VIDEO_PARAM_FAIL,
	AVI_RECORD_MSG_NUM_GET_AVI_REC_PARAM_FAIL,
	AVI_RECORD_MSG_NUM_FREE_MEMORY_NOT_ENOUGH,
	AVI_RECORD_MSG_NUM_FAIL,
	AVI_RECORD_MSG_NUM_SUCCESS,
	AVI_RECORD_MSG_NUM_SAVE_TO_SDCARD_SUCCESS,
	AVI_RECORD_MSG_NUM_SAVE_TO_SAMBA_SUCCESS,
	AVI_RECORD_MSG_NUM_SAVE_TO_USB_SUCCESS,
	AVI_RECORD_MSG_NUM_SAVE_TO_TMP_SUCCESS,
	AVI_RECORD_MSG_NUM_WRT_VIDEO_FRAME_SUCCESS,
	AVI_RECORD_MSG_NUM_WRT_AUDIO_FRAME_SUCCESS,
	AVI_RECORD_MSG_NUM_TOTAL
}AVI_RECORD_MSG_NUM;

#define AVI_RECORD_MSG_STR_LEN 48
#define AVI_RECORD_MSG_STR_NONE								"[AVI]: None !!!"
#define AVI_RECORD_MSG_STR_OPEN_SDCARD_AVI_FILE_FAIL		"[AVI]: Open SDCard Avi File Fail !!!"
#define AVI_RECORD_MSG_STR_OPEN_SAMBA_AVI_FILE_FAIL			"[AVI]: Open Samba Avi File Fail !!!"
#define AVI_RECORD_MSG_STR_OPEN_USB_AVI_FILE_FAIL			"[AVI]: Open USB Avi File Fail !!!"
#define AVI_RECORD_MSG_STR_OPEN_TMP_AVI_FILE_FAIL			"[AVI]: Open TMP Avi File Fail !!!"
#define AVI_RECORD_MSG_STR_OPEN_SDCARD_CONFIG_FILE_FAIL		"[AVI]: Open SDCard Config File Fail !!!"
#define AVI_RECORD_MSG_STR_OPEN_SAMBA_CONFIG_FILE_FAIL		"[AVI]: Open Samba Config File Fail !!!"
#define AVI_RECORD_MSG_STR_OPEN_USB_CONFIG_FILE_FAIL		"[AVI]: Open USB Config File Fail !!!"
#define AVI_RECORD_MSG_STR_OPEN_TMP_CONFIG_FILE_FAIL		"[AVI]: Open TMP Config File Fail !!!"
#define AVI_RECORD_MSG_STR_GET_SDCARD_CONFIG_FILE_FAIL		"[AVI]: Get SDCard Config File Fail !!!"
#define AVI_RECORD_MSG_STR_GET_SAMBA_CONFIG_FILE_FAIL		"[AVI]: Get Samba Config File Fail !!!"
#define AVI_RECORD_MSG_STR_GET_USB_CONFIG_FILE_FAIL			"[AVI]: Get USB Config File Fail !!!"
#define AVI_RECORD_MSG_STR_GET_TMP_CONFIG_FILE_FAIL			"[AVI]: Get TMP Config File Fail !!!"
#define AVI_RECORD_MSG_STR_GET_VIDEO_INERFACE_FAIL			"[AVI]: Get Video Interface Fail !!!"
#define AVI_RECORD_MSG_STR_GET_AUDIO_INERFACE_FAIL			"[AVI]: Get Audio Interface Fail !!!"
#define AVI_RECORD_MSG_STR_WRT_VIDEO_FRAME_TO_SDCARD_FAIL	"[AVI]: Write Video to SDCard Fail !!!"
#define AVI_RECORD_MSG_STR_WRT_AUDIO_FRAME_TO_SDCARD_FAIL	"[AVI]: Write Audio to SDCard Fail !!!"
#define AVI_RECORD_MSG_STR_WRT_VIDEO_FRAME_TO_SAMBA_FAIL	"[AVI]: Write Video to Samba Fail !!!"
#define AVI_RECORD_MSG_STR_WRT_AUDIO_FRAME_TO_SAMBA_FAIL	"[AVI]: Write Audio to Samba Fail !!!"
#define AVI_RECORD_MSG_STR_WRT_VIDEO_FRAME_TO_USB_FAIL		"[AVI]: Write Video to USB Fail !!!"
#define AVI_RECORD_MSG_STR_WRT_AUDIO_FRAME_TO_USB_FAIL		"[AVI]: Write Audio to USB Fail !!!"
#define AVI_RECORD_MSG_STR_WRT_VIDEO_FRAME_TO_TMP_FAIL		"[AVI]: Write Video to TMP Fail !!!"
#define AVI_RECORD_MSG_STR_WRT_AUDIO_FRAME_TO_TMP_FAIL		"[AVI]: Write Audio to TMP Fail !!!"
#define AVI_RECORD_MSG_STR_GET_SHARED_MEM_FAIL				"[AVI]: Get Shared Memory Fail !!!"
#define AVI_RECORD_MSG_STR_GET_INFO_FAIL					"[AVI]: Get Info Fail !!!"
#define AVI_RECORD_MSG_STR_GET_SHARED_MEM_SAMBA_FAIL		"[AVI]: Get Shared Memory(Samba) Fail !!!"
#define AVI_RECORD_MSG_STR_GET_REC_VIDEO_PARAM_FAIL			"[AVI]: Get Rec Video Parameter Fail !!!"
#define AVI_RECORD_MSG_STR_GET_AVI_REC_PARAM_FAIL			"[AVI]: Get Avi Record Parameter Fail !!!"
#define AVI_RECORD_MSG_STR_FREE_MEMORY_NOT_ENOUGH			"[AVI]: Free memory is not enough !!!"
#define AVI_RECORD_MSG_STR_FAIL								"[AVI]: Fail !!!"
#define AVI_RECORD_MSG_STR_SUCCESS							"[AVI]: Success !!!"
#define AVI_RECORD_MSG_STR_SAVE_TO_SDCARD_SUCCESS			"[AVI]: Save AVI to SDCard Success !!!"
#define AVI_RECORD_MSG_STR_SAVE_TO_SAMBA_SUCCESS			"[AVI]: Save AVI to Samba Success !!!"
#define AVI_RECORD_MSG_STR_SAVE_TO_USB_SUCCESS				"[AVI]: Save AVI to USB Success !!!"
#define AVI_RECORD_MSG_STR_SAVE_TO_TMP_SUCCESS				"[AVI]: Save AVI to TMP Success !!!"
#define AVI_RECORD_MSG_STR_WRT_VIDEO_FRAME_SUCCESS			"[AVI]: Write Video Success !!!"
#define AVI_RECORD_MSG_STR_WRT_AUDIO_FRAME_SUCCESS			"[AVI]: Write Audio Success !!!"

#define AVI_RECORD_MSG_LIST { \
	{AVI_RECORD_MSG_ID_NONE, 								AVI_RECORD_MSG_STR_NONE,								AVI_RECORD_MSG_OP_KEEP}, \
	{AVI_RECORD_MSG_ID_OPEN_SDCARD_AVI_FILE_FAIL, 			AVI_RECORD_MSG_STR_OPEN_SDCARD_AVI_FILE_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_OPEN_SAMBA_AVI_FILE_FAIL, 			AVI_RECORD_MSG_STR_OPEN_SAMBA_AVI_FILE_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_OPEN_USB_AVI_FILE_FAIL, 				AVI_RECORD_MSG_STR_OPEN_USB_AVI_FILE_FAIL,				AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_OPEN_TMP_AVI_FILE_FAIL, 				AVI_RECORD_MSG_STR_OPEN_TMP_AVI_FILE_FAIL,				AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_OPEN_SDCARD_CONFIG_FILE_FAIL, 		AVI_RECORD_MSG_STR_OPEN_SDCARD_CONFIG_FILE_FAIL,		AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_OPEN_SAMBA_CONFIG_FILE_FAIL, 		AVI_RECORD_MSG_STR_OPEN_SAMBA_CONFIG_FILE_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_OPEN_USB_CONFIG_FILE_FAIL, 			AVI_RECORD_MSG_STR_OPEN_USB_CONFIG_FILE_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_OPEN_TMP_CONFIG_FILE_FAIL, 			AVI_RECORD_MSG_STR_OPEN_TMP_CONFIG_FILE_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_SDCARD_CONFIG_FILE_FAIL, 		AVI_RECORD_MSG_STR_GET_SDCARD_CONFIG_FILE_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_SAMBA_CONFIG_FILE_FAIL, 			AVI_RECORD_MSG_STR_GET_SAMBA_CONFIG_FILE_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_USB_CONFIG_FILE_FAIL, 			AVI_RECORD_MSG_STR_GET_USB_CONFIG_FILE_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_TMP_CONFIG_FILE_FAIL, 			AVI_RECORD_MSG_STR_GET_TMP_CONFIG_FILE_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_VIDEO_INERFACE_FAIL, 			AVI_RECORD_MSG_STR_GET_VIDEO_INERFACE_FAIL,				AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_AUDIO_INERFACE_FAIL,				AVI_RECORD_MSG_STR_GET_AUDIO_INERFACE_FAIL,				AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_WRT_VIDEO_FRAME_TO_SDCARD_FAIL,		AVI_RECORD_MSG_STR_WRT_VIDEO_FRAME_TO_SDCARD_FAIL,		AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_WRT_AUDIO_FRAME_TO_SDCARD_FAIL,		AVI_RECORD_MSG_STR_WRT_AUDIO_FRAME_TO_SDCARD_FAIL,		AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_WRT_VIDEO_FRAME_TO_SAMBA_FAIL,		AVI_RECORD_MSG_STR_WRT_VIDEO_FRAME_TO_SAMBA_FAIL,		AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_WRT_AUDIO_FRAME_TO_SAMBA_FAIL,		AVI_RECORD_MSG_STR_WRT_AUDIO_FRAME_TO_SAMBA_FAIL,		AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_WRT_VIDEO_FRAME_TO_USB_FAIL,			AVI_RECORD_MSG_STR_WRT_VIDEO_FRAME_TO_USB_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_WRT_AUDIO_FRAME_TO_USB_FAIL,			AVI_RECORD_MSG_STR_WRT_AUDIO_FRAME_TO_USB_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_WRT_VIDEO_FRAME_TO_TMP_FAIL,			AVI_RECORD_MSG_STR_WRT_VIDEO_FRAME_TO_TMP_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_WRT_AUDIO_FRAME_TO_TMP_FAIL,			AVI_RECORD_MSG_STR_WRT_AUDIO_FRAME_TO_TMP_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_SHARED_MEM_FAIL,					AVI_RECORD_MSG_STR_GET_SHARED_MEM_FAIL,					AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_INFO_FAIL,						AVI_RECORD_MSG_STR_GET_INFO_FAIL,						AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_SHARED_MEM_SAMBA_FAIL,			AVI_RECORD_MSG_STR_GET_SHARED_MEM_SAMBA_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_REC_VIDEO_PARAM_FAIL,			AVI_RECORD_MSG_STR_GET_REC_VIDEO_PARAM_FAIL,			AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_GET_AVI_REC_PARAM_FAIL,				AVI_RECORD_MSG_STR_GET_AVI_REC_PARAM_FAIL,				AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_FREE_MEMORY_NOT_ENOUGH,				AVI_RECORD_MSG_STR_FREE_MEMORY_NOT_ENOUGH,				AVI_RECORD_MSG_OP_SEND}, \
	{AVI_RECORD_MSG_ID_FAIL,								AVI_RECORD_MSG_STR_FAIL,								AVI_RECORD_MSG_OP_KEEP}, \
	{AVI_RECORD_MSG_ID_SUCCESS,								AVI_RECORD_MSG_STR_SUCCESS,								AVI_RECORD_MSG_OP_KEEP}, \
	{AVI_RECORD_MSG_ID_SAVE_TO_SDCARD_SUCCESS,				AVI_RECORD_MSG_STR_SAVE_TO_SDCARD_SUCCESS,				AVI_RECORD_MSG_OP_KEEP}, \
	{AVI_RECORD_MSG_ID_SAVE_TO_SAMBA_SUCCESS,				AVI_RECORD_MSG_STR_SAVE_TO_SAMBA_SUCCESS,				AVI_RECORD_MSG_OP_KEEP}, \
	{AVI_RECORD_MSG_ID_SAVE_TO_USB_SUCCESS,					AVI_RECORD_MSG_STR_SAVE_TO_USB_SUCCESS,					AVI_RECORD_MSG_OP_KEEP}, \
	{AVI_RECORD_MSG_ID_SAVE_TO_TMP_SUCCESS,					AVI_RECORD_MSG_STR_SAVE_TO_TMP_SUCCESS,					AVI_RECORD_MSG_OP_KEEP}, \
	{AVI_RECORD_MSG_ID_WRT_VIDEO_FRAME_SUCCESS,				AVI_RECORD_MSG_STR_WRT_VIDEO_FRAME_SUCCESS,				AVI_RECORD_MSG_OP_KEEP}, \
	{AVI_RECORD_MSG_ID_WRT_AUDIO_FRAME_SUCCESS,				AVI_RECORD_MSG_STR_WRT_AUDIO_FRAME_SUCCESS,				AVI_RECORD_MSG_OP_KEEP}, \
}

typedef struct {
	int	MsgId;
	char MsgStr[AVI_RECORD_MSG_STR_LEN];
	char MsgOp;
}AviRecordMsg;

#define IMAGE_DIRECTION_X 0
#define IMAGE_DIRECTION_Y 1
#define IMAGE_DIRECTION_TOTAL 2
#define IMAGE_RESOLUTION_ITEM_MAX	 7

#define IMAGE_RESOLUTION_1	{{2048, 1536}, {1920,1088}, {1280, 960}, {1280, 736}, {736, 480}, {640, 480}, {320, 256}}
#define IMAGE_RESOLUTION_2	{{2048, 1536}, {1920,1080}, {1280, 960}, {1280, 720}, {720, 480}, {640, 480}, {320, 240}}

int Image_Resize(int iPreCaptureID, int iNewCaptureID, int iDir, int iValue);
void Image_Align(int CaptureID, int iDir, int* iValue, int* iValue1);		

#define PORT_USEDLIST_UPDATE_FLAG	"/tmp/UpdateUsedPortList"
#define PORT_USEDLIST_FILENAME		"/tmp/UsedPortList"
#define PORT_USEDLIST_TIMEOUT		30
typedef struct {
	char Proto[16];
	unsigned int Recv_Q;
	unsigned int Send_Q;
	char LocalAddress[64];
	char ForeignAddress[64];
	char Stat[16];
}NETSTAT_INFO;

#define SERVICE_CMD_LEN_MAX		64
#define SERVICE_CMD_NULL			""
#define SERVICE_CMD_SYSTEM_SERVER	"system_server &"
#define SERVICE_CMD_AV_SERVER		"av_server &"
#define SERVICE_CMD_WEB_SERVER	    "boa -c /etc &"
#define SERVICE_CMD_WIS_STREAMER    "wis-streamer &"
#define SERVICE_CMD_AVI_RECORD	    "/bin/avi_rec"
#define SERVICE_CMD_AVI_EVENT		"/bin/avi_event"
#define SERVICE_CMD_SYSTEM_CALL 	"/bin/system_call"
#define SERVICE_CMD_FINDER_SERVER	"finder_server eth1 &"
#define SERVICE_CMD_FW_UPODATE_DAEMON  "fw_update_daemon &"
#define SERVICE_CMD_SW_WATCH_DOG    "/bin/sw_watch_dog &"
#define SERVICE_CMD_PS				"ps"								
#define SERVICE_CMD_AV_SERVER_KILL	"killall -2 av_server"				
#define SERVICE_CMD_SW_WATCH_DOG_KILL	"killall -2 sw_watch_dog"		
#define SERVICE_CMD_BOA_KILL			"killall -9 boa"				
#define SERVICE_CMD_WIS_STREAMER_KILL	"killall -9 wis-streamer"				
#define SERVICE_CMD_AVI_RECORD_KILL 	"killall -15 avi_rec"				
#define SERVICE_CMD_AVI_EVENT_KILL	    "killall -15 avi_event"				
#define SERVICE_CMD_SYSTEM_CALL_KILL	"killall -15 system_call"				

typedef struct {
	unsigned int PID;
	char User[64];
	char VSZ[8];
	char Stat[8];
	char Command[64];
}SERVICE_CMD_INFO;

#endif   /* __SYSTEM_DEFAULT_H__ */

#if defined (__cplusplus)
}
#endif