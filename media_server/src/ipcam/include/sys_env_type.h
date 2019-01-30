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
* @file sys_env_type.h
* @brief System evironment structure and Global definition.
*/
#if defined (__cplusplus)
extern "C" {
#endif

#ifndef __SYS_ENV_TYPE_H__
#define __SYS_ENV_TYPE_H__

#include <asm/types.h>
#include <netinet/in.h>
#include <time.h>
#include <Dmva_scheduler.h>

#include "ipnc_feature.h"
#include "patrol_util.h"
#include <Msg_Def.h>

/*
MAGIC_NUM log
0x9A131904
0x56313930
0x56323030
0x56313600
*/
#define MAGIC_NUM			0x30323637 //0x9A131904

#define SUCCESS 0
#define FAIL	-1

#define GUI_RELOAD_TIME_IN_SECS 	(14)
#define BOOT_PROC_TIMEOUT_CNT		(2000)

#define SYS_ENV_SIZE		sizeof(SysInfo)
#define MAX_LOG_PAGE_NUM	20
#define NUM_LOG_PER_PAGE	20
#define LOG_ENTRY_SIZE		sizeof(LogEntry_t)

#define MAX_DMVA_STRING_LENGTH  		32
#define MAX_DMVA_LABEL_STRING_LENGTH  	16
#define MAX_DMVA_SAVELOAD_SETS     		5

#define MAX_DOMAIN_NAME			40 		///< Maximum length of domain name. Ex: www.xxx.com
#define MAX_LANCAM_TITLE_LEN	128 		///< Maximum length of LANCAM title.
#define OSDTEXTLEN				24
#define MAX_FQDN_LENGTH			256 	///< Maximum length of FQDN.
#define MAX_NAME_LENGTH			256 	///< Maximum length of normal string.
#define MAX_STRING_LENGTH	256 ///< Maximum length of normal string.
#define MAX_OSD_STREAMS			3 		///< Maximum length of normal string.
#define MAX_CODEC_STREAMS		4 		///< Maximum length of normal string.
#define MAX_STREAM_NAME			75  //25///< Maximum length of normal string.
#define MAC_LENGTH				6 		///< Length of MAC address.
#define SCHDULE_NUM				10 		///< How many schedules will be stored in system.
#define MAX_FILE_NAME			256		///< Maximum length of file name.

#define ACOUNT_NUM				21 		///< How many acounts which are stored in system.
#define USER_LEN				33 		///< Maximum of acount username length.
#define MIN_USER_LEN			4 		///< Maximum of acount username length.
#define PASSWORD_LEN			33 		///< Maximum of acount password length.
#define MIN_PASSWORD_LEN		4 		///< Maximum of acount password length.
#define PASSWORD_ZERO           0               ///< minimum length of password>
#if 0
#define IP_STR_LEN				20		///< IP string length
#else
#define IP_STR_LEN				64		///< IP string length
#endif
#define MAX_LENGTH              20
//#define MOTION_BLK_LEN			(4) 	///< Motion block size
#define MOTION_BLK_LEN			(10) 	///< Motion block size

#define OSD_MSG_FALSE			(0)
#define OSD_MSG_TRUE			(1)
#define OSD_PARAMS_UPDATED		(4)

/*
 The following flags is defined for alarmstatus.
 Modify these sould also check where the alarmstatus is used.
 Be sure that you also check your web UI coding.
*/
#define FLG_UI_EXT 				(1 << 0)
#define FLG_UI_MOTION 			(1 << 1)
#define FLG_UI_ETH 				(1 << 2)
#define FLG_UI_AUDIO			(1 << 3)
#define FLG_UI_RECORD 			(1 << 4)
#define FLG_UI_AVI 				(1 << 5)
#define FLG_UI_JPG 				(1 << 6)
#define FLG_UI_DI_1 				(1 << 7)
#define FLG_UI_DI_2 				(1 << 8)

#define LOGIN_TIMEOUT		1800 	/* Set for 30 min */

#define INVALID_IP			0

#define ETH_PPPOE	"ppp0"
#define ETH_ETH0	"eth0"
#define ETH_ETH1	"eth1"
#define ETH_ETH2	"eth2"
#define ETH_RA0		"ra0"
#define ETH_BR0		"br0"
#define ETH_NAME	ETH_ETH1
#define ETH_NAME_LOCAL ETH_NAME":avahi"

#define USB_PATH			"/proc/usb_appro"
#define USB_CONNECT_PATH	"/proc/usb_connect"
#define USB_CFG_FILE		"/mnt/ramdisk/NET.TXT"

/* Low : on */
#define LED_ON					0
#define LED_OFF					1
#define LED_BLINKING		    2

#define GIO_HIGH	           1
#define GIO_LOW		           0
#define IRLED_CTRL   		  46 //23

//#if IRCUT_GIO_GROUP == 1  //SIB7000
//#define GIO_IRCUT_DETECT		41
//#define GIO_IRCUT_CTL1			40
//#define GIO_IRCUT_CTL2			39
//#else
#define GIO_IRCUT_DETECT		47//22
#define GIO_IRCUT_CTL1			53 //45//48
//#define GIO_IRCUT_CTL2			48 //45
//#endif
#define GIO_USB_CTRL         62
#define GIO_HW_RESET	  	 61  //WPS
#define GIO_ACT_LED          44	 /*	2 */		
#define GIO_PWR_LED          43	 /* 0 */	
#define GIO_LAN_LED		     42  /* 5 */

#define GIO_LED_1		 GIO_ACT_LED  
#define GIO_LED_2		 GIO_PWR_LED 
#define GIO_LED_3		 GIO_LAN_LED 

#define GIO_TV_CONNECT			7
#define GIO_ALARMOUT 			61
#define GIO_ALARMIN 			31
#define GIO_ALARMOUT_HIGH   1
#define GIO_ALARMOUT_LOW    0

#define GIO_ALARMRESET 			35
#define GIO_ALARMRESET_OLD 		80
#define GIO_RESET_TO_DEFAULT	79 /*Low trigger*/
#define GIO_DHCP				84

#define H264_1_PORTNUM	(8557)
#define H264_2_PORTNUM	(8556)
#define MPEG4_1_PORTNUM	(8553)
#define MPEG4_2_PORTNUM	(8554)
#define MJPEG_PORTNUM	(8555)

#define MAX_PROF        4
#define MAX_PROF_TOKEN  20
#define MAX_CONF_TOKEN  20
#define MAX_IP_ADDR     30
#define MAX_RELAYS      3
#define MAX_IMAGING     4
#define dMaxNumPreset   8

typedef enum{
//	CHIP_NONE = -1,
	CHIP_DM365 = 0,
	CHIP_DM368,
	CHIP_DM368E,
	CHIP_DM369,
	CHIP_DMVA1,
	CHIP_DMVA2,
	CHIP_MT7620,
	CHIP_MAX
}CHIP_DM36X;

typedef enum{
	MPEG4_CODEC = 0,
	H264_CODEC,
	MJPEG_CODEC,
	NO_CODEC
}CODEC_TYPE;

enum {
	CODEC_COMB_SINGLE_H264,	// SINGLE H.264
	CODEC_COMB_SINGLE_MPEG4,	// SINGLE MPEG4
	CODEC_COMB_MEGAPIXEL,	//MegaPixel
	CODEC_COMB_H264_JPEG,	// H.264 + JPEG
	CODEC_COMB_MPEG4_JPEG,	// MPEG4 + JPEG
	CODEC_COMB_DUAL_H264,	// DUAL H.264
	CODEC_COMB_DUAL_MPEG4,	// DUAL MPEG4
	CODEC_COMB_H264_MPEG4,	// H.264 + MPEG4
	CODEC_COMB_TRIPLE_H264,	// Dual H.264 + MJPEG
	CODEC_COMB_TRIPLE_MPEG4,	// Dual MPEG4 + MJPEG
	CODEC_COMB_NUM
};

typedef enum{
	BINNING = 0,
	SKIPPING,
	WINDOW
}SENSOR_MODE;

typedef enum{
	AUTO = 0,
	CUSTOM,
	SVC
}ENCPRESET_MODE;

/* Onvif Structures */
/* Onvif - Video source configuration structure */

typedef struct
{
	char	Vname[MAX_PROF_TOKEN]; //video source configuration name
	int		Vcount;         // count for number of users
	char	Vtoken[MAX_CONF_TOKEN];//video source configuration token
	char	Vsourcetoken[MAX_PROF_TOKEN];
	int		windowx;             //x cordinate of window
	int		windowy;             //y cordinate of window
	int		windowwidth;         //width
	int		windowheight;        //height
	__u8	reserve[40];
}Add_VideoSource_configuration;

/* Onvif - Video multicast configuration structure */
typedef struct
{
	int		nIPType;                //ip address IPv4/IPv6
	char	IPv4Addr[MAX_IP_ADDR];	//ipv4 address
	char	IPv6Addr[MAX_IP_ADDR];	//ipv6 address
	int		port;                   //port number
	int		ttl;                    //TTL
	int		autostart;              //autostart
	__u8	reserve[24];
}Video_Multicast_configuration;

/* Onvif - Video encoder configuration structure */
typedef struct
{
	char                                   VEname[MAX_PROF_TOKEN] ;                 //video encoder name
	char                                   VEtoken[MAX_CONF_TOKEN];                 //video encoder token
	int                                    VEusercount;            //video encoder user count
	float                                  VEquality;              //video encoder quality
	int                                    Vencoder;             //video encoder for jpeg, mpeg, h.264
	int                                    Rwidth;               //resolution width
	int                                    Rheight;                //resolution height
	int                                    frameratelimit;        //frameratelimit
	int                                    encodinginterval;       //encodinginterval
	int                                    bitratelimit;            //bitratelimit
	int                                    Mpeggovlength;              //mpeg GoVLength
	int                                    Mpegprofile;                 //mpegprofile SP/ASP
	int                                    H264govlength;               //H264 govlength
	int                                    H264profile;                 //H264 profile  baseline/main/extended/high
	long                                   sessiontimeout;
	Video_Multicast_configuration          Multicast_Conf;
	__u8        reserve[40];

}Add_VideoEncoder_configuration;

/* Onvif - Video analyticengine configuration structure */
typedef struct
{
	int		VAMsize;                 //video analytic module size
	int		VAsizeitem;              //size of simple items
	char	VASname[MAX_LENGTH];     //simple item name
	char	VASvalue[MAX_LENGTH];    //simple item value
	int		VAEsize;                 //Element size
	char	VAEname[MAX_LENGTH];     //element name
	char	Aconfigname[MAX_LENGTH];
	char	Aconfigtype[MAX_LENGTH];
	__u8	reserve[40];

}Video_AnalyticEngine;

/* Onvif - Video ruleengine configuration structure */
typedef struct
{

	int		VARsize;				//video analytic rule size
	int		VARSsizeitem;			//size of simple items for ruleengine
	char	VARSname[MAX_LENGTH];	//simple item name for rule engine
	char	VARSvalue[MAX_LENGTH];	//simple item value for rule engine
	int		VAREsize;				//Element size for rule engine
	char	VAREname[MAX_LENGTH];	//element name for rule engine
	char	Rconfigname[MAX_LENGTH];
	char	Rconfigtype[MAX_LENGTH];

	__u8	reserve[40];
}Video_RuleEngine;

/* Onvif - VideoAnalytic configuration structure */
typedef struct
{
	char					VAname[MAX_PROF_TOKEN];   //video analytic name
	char					VAtoken[MAX_CONF_TOKEN];  //video analytic token
	int						VAusercount;              //video analytic user count
	Video_AnalyticEngine	VAE;
	Video_RuleEngine		VRE;
	__u8					reserve[40];
} Add_VideoAnalytics_Configuration;

typedef struct
{
	char	name[MAX_CONF_TOKEN];
	int		usecount;
	char	token[MAX_CONF_TOKEN];
	__u8	reserve[40];
} Add_VideoOutput_Configuration;

/* Onvif - Audio configuration structure */
typedef struct
{
	char							Aname[MAX_LENGTH];			//audio name
	int								Ausecount;					//audio user count
	char							Atoken[MAX_LENGTH];			//audio token
	char							Asourcetoken[MAX_LENGTH];	//audio source token
	Video_Multicast_configuration	AEMulticast_Conf;
	__u8        reserve[40];
}Add_AudioSource_Configuration;

typedef struct
{
	char							AEname[MAX_LENGTH];                     //audio encoder name
	char							AEtoken[MAX_LENGTH];                   //audio encoder token
	int								AEusercount;               //audio encoder user count
	int								bitrate;                    //Bitrate
	int								samplerate;                //Samplerate
	int								AEencoding;
	Video_Multicast_configuration	AEMulticast_Conf;
	__u8        reserve[40];
}Add_AudioEncoder_Configuration;

typedef struct
{
	char							MDname[MAX_LENGTH];
	int								MDusecount;
	char							MDtoken[MAX_LENGTH];
	int								MDanalytics;
	long							sessiontimeout;
	Video_Multicast_configuration	VMC;
	__u8        reserve[40];
}Metadata_configuration;


typedef struct {
	char	PresetName[MAX_PROF_TOKEN];
	char	PresetToken[MAX_PROF_TOKEN];
	int		x;
	int		y;
	__u8	reserve[24];
} PresetEn;

/* Onvif - Profile Structure*/
typedef struct
{
	char								profilename[MAX_PROF_TOKEN];       //name of profile
	char								profiletoken[MAX_CONF_TOKEN];      //token of profile
	int									fixed;             //profile fixed or not
	Add_VideoSource_configuration		AVSC;

	Add_AudioSource_Configuration		AASC;

	Add_VideoEncoder_configuration		AESC;

	Add_AudioEncoder_Configuration		AAEC;

	Add_VideoAnalytics_Configuration	AVAC;
	Add_VideoOutput_Configuration		AVOC;
	/* PTZ configuration */
	char								PTZname[MAX_PROF_TOKEN];                  //ptz name
	char								PTZtoken[MAX_CONF_TOKEN];                 //ptz token
	int									PTZusercount;             //ptz user count
	char								PTZnodetoken[MAX_CONF_TOKEN];             //ptz nade token
	char								DefaultAbsolutePantTiltPositionSpace[MAX_LENGTH];         //default absolute pant tilt position space
	char								DefaultAbsoluteZoomPositionSpace[MAX_LENGTH];             //default absolute zoom position space
	char								DefaultRelativePanTiltTranslationSpace[MAX_LENGTH];       //default relative pan tilt translation space
	char								DefaultRelativeZoomTranslationSpace[MAX_LENGTH];          //default relative zoom translation space
	char								DefaultContinuousPanTiltVelocitySpace[MAX_LENGTH];         //Default Continuous PanTilt Velocity Space
	char								DefaultContinuousZoomVelocitySpace[MAX_LENGTH];            //Default Continuous Zoom Velocity Space
	float								PTZspeedx;                                    //pan tilt speed
	float								PTZspeedy;                                    // pan tilt speed
	char								PTZspeedspace[MAX_LENGTH];                                // pan tilt speed
	float								PTZzoom;                                      //ptz zoom
	char								PTZzoomspce[MAX_LENGTH];                                  //ptz zoom space
	long*								DefaultPTZTimeout;                            //default time out for ptz
	char								PANURI[MAX_LENGTH];                                        //pan tilt limit uri
	float								PTZrangeminx;                                     //ptz min x  range
	float								PTZrangemaxx;                                     //ptz max x range
	float								PTZrangeminy;                                     //ptz min y range
	float								PTZrangemaxy;                                      //ptz max y range
	char								PTZzoomURI[MAX_IP_ADDR];                                        //zoom uri
	char								PTZMname[MAX_LENGTH];                                           //meta data name
	char								PTZMtoken[MAX_CONF_TOKEN];                                         //meta data token
	int									PTZMusercount;                                      //meta data user count
	int									PTZfilterstatus;                                    //ptz filter status
	int									PTZfilterposition;                                  //ptz filter position
	int									PTZManalytics;                                      //analytics
	/* Extension */
	char								AExname[MAX_PROF_TOKEN];                           //extension audio output configuration name
	int									AExusecount;                       //extension audio output configuration user count
	char								AExtoken[MAX_CONF_TOKEN];                          //extension audio output configuration token
	char								AExoutputtoken[MAX_CONF_TOKEN];                    //extension audio output configuration otput token
	char								AExsendprimacy[MAX_LENGTH];                    //extension audio output configuration primacy
	int									AExoutputlevel;                      //extension audio output configuration level
	char								AExDname[MAX_LENGTH];                            //audio decoder name
	int									AExDusecount;                       //audio decoder user count
	char								AExDtoken[MAX_LENGTH];                         //audio decoder token
	/* metadata configuration */
	PresetEn							PresetE[dMaxNumPreset];                                        //preset
	Metadata_configuration				MDC;
	__u8        reserve[40];

}Onvif_profile;

/* Onvif Scopes structures*/
typedef struct {
/*	char type[3][100];
	char name[50];
	char location[3][100];
	char hardware[50]; */
	int  scope_size;
	char scope[20][100];
	int  medataver;
	char reserved[2048];
} Onvif_Scopes;


typedef struct {
	char token[MAX_CONF_TOKEN];
	int  relaymode;
	long delaytime;
	int  idlestate;
	//Relay_conf set_relay_conf;
	__u8 reserve[40];
} Onvif_Relay_in;

typedef struct
{
	int	relaylogicalstate;
	Onvif_Relay_in onvif_relay_in;
	__u8        reserve[40];
}Onvif_Relay;

/**
 * Imaging structure
 */
typedef struct
{
	char	Isourcetoken[MAX_LENGTH];
	int		BacklightCompensationMode;
	float	BackLightLevel;
	float	Brightness;
	float	Saturation;
	float	Contrast;
	float	ExposureMode;
	float	ExposurePriority;
	float	bottom;
	float	top;
	float	right;
	float	left;
	float	MinExposureTime;
	float	MaxExposureTime;
	float	MinGain;
	float	MaxGain;
	float	MinIris;
	float	MaxIris;
	float	ExposureTime;
	float	Gain;
	float	Iris;
	int		AutoFocusMode ;
	float	DefaultSpeed;
	float	NearLimit;
	float	FarLimit;
	int		IrCutFilterMode;
	float	Sharpness;
	int		WideDynamicMode;
	float	WideDMLevel;
	int		WhiteBalanceMode;
	float	CrGain;
	float	CbGain;
	__u8	reserve[16];  // add by kenny chiu 20130201
} Imaging_Conf;

typedef struct
{
	__u8 Max_Rrs_2M;
	__u8 Wifi_feature;
    __u8 PTZ_feature;
    __u8 AUDIO_feature;
	Add_VideoEncoder_configuration AESC[3];
	Add_VideoSource_configuration  AVSC;
    __u8 restartState;
	__u8 reserve[80];
}Onvif_Util;

typedef struct
{
	int				PrifileNum;
	int				RelayNum;
	Onvif_profile	Profile[MAX_PROF];
	Onvif_Scopes	Scopes;
	Onvif_Relay		Relay[MAX_RELAYS];
	Imaging_Conf	Imaging[MAX_IMAGING];
	Onvif_Util		Util;
} Onvif_Info;
/* End-Onvif Structures */

/**

* @brief Infomation of network status and user settings.
*/
typedef struct
{
	struct in_addr	ip; ///< IP address in static IP mode
	struct in_addr	netmask; ///< netmask in static IP mode
	struct in_addr	gateway; ///< gateway in static IP mode
	struct in_addr	dns; ///< Primary DNS IP in static IP mode
    struct in_addr	dns2; ///< Secondary DNS IP in static IP mode
	__u16		http_port; ///< HTTP port in web site.
	__u16		https_port; ///< HTTP port in web site.
	int			dhcp_enable; ///< current DHCP status.
	int			dhcp_config; ///< DHCP status 
    __u8        discovery_mode;         ///<set 0 for discoverable and 1 for non-discoverable>
	char		ntp_server[MAX_DOMAIN_NAME+1]; ///< NTP server FQDN
	__u8		ntp_timezone; ///< current time zone
	__u8		ntp_frequency; ///< NTP server query frequence
	__u8		time_format; ///< time display format
	__u8		daylight_time; ///< daylight saving time mode
	__u8		MAC[MAC_LENGTH]; ///< hardware MAC address
	__u8		mpeg4resolution; ///< MPEG4_1 resolution
	__u8		mpeg42resolution; ///< MPEG4_2 resolution  for mobile
	__u8		mpeg43resolution; ///< MPEG4_2 resolution  for ePTZ
	__u8		liveresolution; ///< live resolution for stream 2
	__u8		mpeg4quality; ///< MPEG4 quality
	__u8		mobquality; ///< Mobile quality
	__u8		jpegquality; ///< jpeg quality //modify by Joseph no use 20140528
	__u8		supportmpeg4; ///< support MPEG4
	__u8		imageformat; ///< image format
	__u8		imagesource; ///< image source (NTSC/PAL)
	__u8		defaultstorage; ///< default storage
	char		defaultcardgethtm[MAX_NAME_LENGTH]; ///< default card get page
	char		brandurl[MAX_NAME_LENGTH]; ///< brand URL
	char		brandname[MAX_NAME_LENGTH]; ///< brand name
	__u8		supporttstamp; ///< support time stamp
	__u8		supportmotion; ///< support motion detection
	__u8		supportwireless; ///< support wireless
	__u8		serviceftpclient; ///< ftp client support
	__u8		servicesmtpclient; ///< smtp client support
	__u8		servicepppoe; ///< PPPoE support
	__u8		servicesntpclient; ///< sntp client support
	__u8		serviceddnsclient; ///< DNS client support
	__u8		supportmaskarea; ///< mask area support
	__u8		maxchannel; ///< max channel
	__u8		supportrs485; ///< RS485 support
	__u8		supportrs232; ///< RS232 support
	__u8		layoutnum; ///< layout No.
	__u8		supportmui; ///< support MUI
	__u8		mui; ///< MUI
	__u8		supportsequence; ///< support sequence
	__s8		quadmodeselect; ///< quadmode select
	__u8		serviceipfilter; ///< service IP filter
	__u8		oemflag0; ///< OEM flag
	__u8		supportdncontrol; ///< Daynight control support
	__u8		supportavc; ///< avc support
	__u8		supportaudio; ///< support audio
	__u8		supportptzpage; ///< PTZ support
	__u8		multicast_enable; ///< multicast enable
	__u8		audioenable; ///< audio enable
	__u16       audioport; // add by kenny chiu 20131119 :default is 5432
    __u8        https_mode;  //add by Joseph 20140409
	char		timezone[255];//added by frank for MTK platform.    
	__u8        reserve[142];//[144]; // modify by kenny chiu 20131119 : 2 bytes for audioport// add by kenny chiu 20130201
	
} Network_Config_Data;

/**
* @brief Infomation about ftp configuration.
*/
typedef struct
{
	char	servier_ip[MAX_DOMAIN_NAME+1]; ///< FTP server address
	char	username[USER_LEN]; ///< FTP login username
	char	password[PASSWORD_LEN]; ///< FTP login password
	char	foldername[MAX_FILE_NAME]; ///< FTP upload folder
	int		image_acount; ///< Image count
	int		pid; ///< PID
	__u16	port; ///< FTP port
 	__u8	rftpenable; ///< RFTP enable
	__u8	ftpfileformat; ///< file format
	__u8	reserve[8];  // add by kenny chiu 20130201
} Ftp_Config_Data;

typedef struct
{
	char	Isourcetoken_t[MAX_LENGTH];
	int		BacklightCompensationMode_t;
	float	BackLightLevel_t;
	float	Brightness_t;
	float	Saturation_t;
	float	Contrast_t;
	float	ExposureMode_t;
	float	ExposurePriority_t;
	float	bottom_t;
	float	top_t;
	float	right_t;
	float	left_t;
	float	MinExposureTime_t;
	float	MaxExposureTime_t;
	float	MinGain_t;
	float	MaxGain_t;
	float	MinIris_t;
	float	MaxIris_t;
	float	ExposureTime_t;
	float	Gain_t;
	float	Iris_t;
	int		AutoFocusMode_t ;
	float	DefaultSpeed_t;
	float	NearLimit_t;
	float	FarLimit_t;
	int		IrCutFilterMode_t;
	float	Sharpness_t;
	int		WideDynamicMode_t;
	float	WideDMLevel_t;
	int		WhiteBalanceMode_t;
	float	CrGain_t;
	float	CbGain_t;
	__u8	reserve[16];  // add by kenny chiu 20130201
} Imaging_Conf_in;

typedef struct
{
	int				position;
	Imaging_Conf_in	imaging_conf_in;
}Imaging_Conf_tmp;
/**
* @brief Infomation about SMTP configuration.
*/
typedef struct
{
	char	servier_ip[MAX_DOMAIN_NAME+1]; ///< SMTP server address
	__u16	server_port; ///< SMTP server port
	char	username[USER_LEN]; ///< SMTP username
	char	password[PASSWORD_LEN]; ///< SMTP password
	__u8	authentication; ///< SMTP authentication
	char	sender_email[MAX_NAME_LENGTH]; ///< sender E-mail address
	char	receiver_email[MAX_NAME_LENGTH]; ///< receiver E-mail address
	char	CC[MAX_NAME_LENGTH]; ///< CC E-mail address
	char	subject[MAX_NAME_LENGTH]; ///< mail subject
	char	text[MAX_NAME_LENGTH]; ///< mail text
	__u8	attachments; ///< mail attachment
	__u8	view; ///< smtp view
	__u8	asmtpattach; ///< attatched file numbers
	__u8	attfileformat; ///< attachment file format
	__u8	reserve[16];  // add by kenny chiu 20130201
} Smtp_Config_Data;

/**
* @brief custom data structure for time.
*/
typedef struct{
	__u8	nHour;	///< Hour from 0 to 23.
	__u8	nMin;	///< Minute from 0 to 59.
	__u8	nSec;	///< Second from 0 to 59.
} Time_t;

/**
* @brief custom data structure for schedule entry.
*/
typedef struct{
	__u8	bStatus;	///< schedule status ( 0:disable 1:enable }
	__u8	nDay;		///< schedule day of week (1:Mon 2:Tue 3:Wed 4:Thr 5:Fri 6:Sat 7:Sun 8:Everyday)
	Time_t	tStart;		///< schedule start time
	Time_t	tDuration;	///< schedule duration
} Schedule_t;

typedef enum{
	MAIN_STREAN = 0,
	MOBILE_STREAM,
	STREAM3,
	JPEG_STREAM,
	STREAM5,
	STREAM6,
	MAX_STREAM_NUM
} STREAM_INDEX;

typedef struct {
	__u8  SupportStream; // 0 : not support, 1 : H264, 2 : JPEG, 3 : MPEG4, 4 : H265s
	__u8  Resolution;
	__u8  FrameRate;
	__u8  RateControl; // 0 : VBR, 1 : CBR
	__u32 Bitrate;
	__u8  Quality;
	__u8  Feature;
	__u8  GOP;
	__u8  reserved[5];
} STREAM_INFO; // Total size is 16 Bytes

typedef struct { // retired now 20151116
	__u8	Mode; // 0 : disable, 1 : 3gpp w/0 audio, 2 : 3gpp w/ audio
	__u8	Framerate; // add by kenny chiu 20130424
	__u8	RateControl; // add by kenny chiu 20130424 0:VBR, 1;CBR
 	__u8	Quality; // add by kenny chiu 20130424  0:Very High, 1:High, 2:Normal, 3:Low, 4:Very low
	int		Bitrate; // add by kenny chiu 20130424  512K:4000K
} Mobile_Info;

/**
* @brief IPCAM configuration data.
*/
typedef struct
{
	char			title[MAX_LANCAM_TITLE_LEN+1];	///< camera title
	char			hostname[MAX_FILE_NAME];        ///< host name
	__u8			nWhiteBalance;					///< white balance mode
	__u8			nDayNight;						///< Daynight mode
	__u8			nTVcable;						///< TV-out setting
	__u8			nBinning;						///< binning/skipping setting
	__u8			nBacklightControl;				///< backlight control setting
	__u8			nBackLight;						///< backlight value
	__u8			nBrightness;					///< brightness value
	__u8			nContrast;						///< contrast value
	__u8			nSaturation;					///< saturation value
	__u8			nSharpness;						///< sharpness value
	__u8            lostalarm;						///< ethernet lost alarm
	__u8			bSdaEnable;						///< alarm save into SD card
	__u8			bAFtpEnable;					///< alarm upload via FTP
	__u8			bASmtpEnable;					///< alarm upload via SMTP
	__u8 			nStreamType;
//	__u8			nVideocodec;					///< video codec
//	__u8			nVideomode;						///< video mode
	__u8			nVideocodecmode;				///< selected codec mode
	__u8			nVideocombo;
	__u8			nVideocodecres;					///< selected codec resolution
	__u8			nRateControl;					///< bitrate control setting
//	__u8			nFrameRate1;					///< MPEG4_1 frame rate
//	__u8			nFrameRate2;					///< MPEG4_2 frame rate
//	__u8			nFrameRate3;					///< JPEG frame rate
	__u8			nMpeg41frmrate;					///< MPEG4_1 frame rate
	__u8			nMpeg42frmrate;					///< MPEG4_2 frame rate
	__u8			nMpeg43frmrate;					///< MPEG4_3 frame rate
	__u8			nJpegfrmrate;					///< JPEG frame rate
	__u8			njpegquality; 					///< jpeg quality
	__u8			ndewarp; 					    ///< H264_1 dewarp
	__u8			ndewarp3;					    ///< H264_3 dewarp (ePTZ)
	int				nMpeg41bitrate;					///< MPEG4_1 bitrate
	int				nMpeg42bitrate;					///< MPEG4_2 bitrate
	int				nMpeg43bitrate;					///< MPEG4_3 bitrate
	int				nMobilebitrate;			 	    ///< Mobile bitrate
	int				Mpeg41Xsize;					///< MPEG4_1 horizontal resolution
	int				Mpeg41Ysize;					///< MPEG4_1 vertical resolution
	int				Mpeg42Xsize;					///< MPEG4_2 horizontal resolution
	int				Mpeg42Ysize;					///< MPEG4_2 vertical resolution
	int				JpegXsize;						///< JPEG horizontal resolution
	int				JpegYsize;						///< JPEG vertical resolution
	int				Avc1Xsize;						///< H264_1 horizontal resolution
	int				Avc1Ysize;						///< H264_1 vertical resolution
	int				Avc2Xsize;						///< H264_2 horizontal resolution
	int				Avc2Ysize;						///< H264_2 vertical resolution
	__u8			Supportstream1;					///< support stream1 (JPEG)
	__u8			Supportstream2;					///< support stream2 (MPEG4_1)
	__u8			Supportstream3;					///< support stream3 (MPEG4_2)
	__u8			Supportstream4;					///< support stream4
	__u8			Supportstream5;					///< support stream5 (AVC_1)
	__u8			Supportstream6;					///< support stream6 (AVC_2)
	__u8			nAlarmDuration;					///< alarm duration
	__u8			nAEWswitch;						///< 2A engine selection
	__u8			gioinenable;					///< GIO input enable
	__u8			giointype;						///< GIO input type
	__u8			giooutenable;					///< GIO output enable
	__u8			gioouttype;						///< GIO output type
	__u8            tstampenable;					///< enable timestamp
	__u8            speakerenable;					///< speaker enable
	__u8            aviduration;					///< avi record duration
	__u8			aviformat;						///< avirecord format.
	__u16			alarmstatus;					///< Current alarm status.
	//__u8			rotation; 						///< rotation
	__u8			mirror; 						///< mirroring
	__u8		    AdvanceMode; 				    ///< video stabilization (ON/OFF)
	__u8		    M41SFeature; 					///< video stabilization (ON/OFF)
	__u8			M42SFeature; 					///< noise filter
	__u8			JPGSFeature;					///< video codec
	__u8		    fdetect; 					    ///< facedetect (ON/OFF),used for OV
	__u8			democfg;						///< demo config
	__u8			regusr[MAX_LANCAM_TITLE_LEN+1];	///< register user name
	__u8			osdstream;						///< register user name
	__u8			osdwinnum;						///< register user name
	__u8			osdwin;							///< register user name
	__u8			osdtext[OSDTEXTLEN];			///< register user name
	__u8			nAEWtype;						///< 2A engine selection
	__u8			histogram;						///< Histogram switch ON/OFF
	__u8			dynRange;						///< Dynamic Range switch ON/OFF
	__u8			maxexposure;
	__u8			maxgain;
	__u8            dateformat;
	__u8            tstampformat;
	__u8            dateposition;
	__u8            timeposition;
	__u8			rs485config;
	__u8			nClickSnapFilename[OSDTEXTLEN];	///< click snap filename
	__u8			nClickSnapStorage;				///< click snap storage setting
	__u8			nRateControl1;					///< stream1 bitrate control setting
	__u8			nRateControl2;					///< stream2 bitrate control setting
	__u8			nRateControl3;					///< stream3 bitrate control setting
	__u8			nEncryptVideo;   				///< encrypt video setting (ON/OFF)
	__u8		    nLocalDisplay; 			        ///< local display video enable/disable
	__u8            nAlarmEnable;                   ///< alarm enable/disable setting
	__u8		    nExtAlarm; 			            ///< external trigger alarm
	__u8		    nAlarmAudioPlay; 			    ///< alarm audio play enable/disable
	__u8		    nAlarmAudioFile; 			    ///< alarm audio file
	__u8		    nScheduleRepeatEnable; 			///< schedule record repeat enable/disable
	__u8			nScheduleNumWeeks;   		    ///< scheduled number of weeks
	__u8		    nScheduleInfiniteEnable; 		///< schedule infinite times enable/disable
	__u8			alarmlocal;
	__u8			recordlocal;
	__u8			expPriority;
	__u8			codectype1;
	__u8			codectype2;
	__u8			codectype3;
	Schedule_t		aSchedules[SCHDULE_NUM];		///< schedule data
	int				schedCurDay;
	int				schedCurYear;
	__u8			reloadFlag;
	__u8			chipConfig;
	__u8			modelname[OSDTEXTLEN];
	int				frameRateVal1;
	int				frameRateVal2;
	int				frameRateVal3;
	__u8			dummy;							///< dummy for end of config
	Network_Config_Data	net;						///< network status and user settings
	__u8			linearwdr;
	int			flickerdetect;
	__u8			mjpgdisable;					///< MJPEG disable flag
	__u8			agc;
	__u8			agc_min;    //AGC Min
	__u8			agc1;
	__u8			agc_min1;   //AGC Min for night
	__u8			shuttertime;
	__u8			shuttertime_min; //exposure time minima
	__u8			shuttertime1;
	__u8			shuttertime_min1; //exposure time minima
	__u8			flickerlessmode;
	__u8			colorbar;
	__u8			exposuremode;    //exposure mode
	__u8			exposurevalue;   //exposure target value day
	__u8			exposurevalue1;  //exposure target value

	Mobile_Info     mobileInfo; // reserved this 8 bytes space to used in future
    __u8 			nMpeg41GOP;
    __u8 			eWdrMode; // add by kenny chiu 20131210
    __u8 			swwatchdogenable;
	__u8			swwatchdog_alarm;	  //swwatchdog alarm av_server:1 wis-streamer:2
	__u8			fix_wb;
	__u8			edimax_cloud;
	__u8            reserve[100];//[102];//[106];  // modify by kenny chiu 20131210
} Lancam_Config_Data;


typedef struct
{
	__u8			vsEnable;
	__u8			ldcEnable;
	__u8			vnfEnable;
	__u8			vnfMode;
	__u8			vnfStrength;
	__u8			dynRange;						///< Dynamic Range switch ON/OFF
	__u8			dynRangeStrength;				///< Dynamic Range Strength
	__u8            reserve[16];
} Advanced_Mode;


typedef struct
{
  char       hostname_t[MAX_FILE_NAME];
}hostname_tmp;

/**
* @brief SD card configuration data.
*/
typedef struct
{
   __u8			sdfileformat;	//< file format saved into SD card
   __u8			sdrenable;		//< enable SD card recording
   __u8			sdinsert;		//< SD card inserted
   __u32        schAviRun;		//< 1 - ON;0 - OFF
   __u8			reserve[25];	// add by kenny chiu 20130201
}Sdcard_Config_Data;

/**
* @brief IPCAM user account data.
*/
typedef struct
{
	char	user[USER_LEN];			///< username
	char	password[PASSWORD_LEN];	///< password
	__u8	authority;				///< user authority
//	__u8    reserve[29];   // add by kenny chiu 20130201
}Acount_t;

/**
* @brief motion detection configuration data.
*/
#define MD_SENSITIVITY 50//100 //for RS
#define MD_PERCENTAGE  50
#define MD_NAME "Motion Window"
#define MAX_MD_NUM     3
#define MAX_MD_STRING_NUM     32

typedef struct
{
	__u8	motionenable;				///< motion detection enable
	__u8	motioncenable;				///< customized sensitivity enable
	__u8	motioncvalue;				///< customized sensitivity value
	__u8	motionblock_coordinates[MAX_MD_STRING_NUM];
	__u8	motionblock[MOTION_BLK_LEN];    ///< motion detection block data
	__u8	percentage;
	__u8	indicator;
	char	motionname[15];
}Motion_Data;
//Motion_Data md_info[MAX_MD_NUM];

typedef struct
{
	char	enable;
	short	left;
	short	top;
	short	right;
	short	bottom;
	short	sen;
}Motion_Str_Data;
Motion_Str_Data MD_Str_Data;

typedef struct
{
  __u8      motionenable;					///< motion detection enable
  __u8      motioncenable;					///< customized sensitivity enable
  __u8      motionlevel;					///< predefined sensitivity level
  __u8      motioncvalue;					///< customized sensitivity value
  __u8	 	motionblock[MOTION_BLK_LEN];	///< motion detection block data
  Motion_Data 		  md_data[MAX_MD_NUM];				  /* Dean - 20140325 */
  __u8	    reserve[18]; // add by kenny chiu 20130201
}Motion_Config_Data;

typedef struct
{
  __u8      audioON;
  __u8      audioenable;
  __u8      audiomode;				///< motion detection enable
  __u8      audioinvolume;				///< customized sensitivity enable
  __u8		codectype;
  __u8      samplerate;				///< predefined sensitivity level
  __u8      bitrate;				///< customized sensitivity value
  __u32     bitrateValue;			///< customized sensitivity value
  __u8 		alarmlevel;
  __u8 		alarmON;
  __u8	 	audiooutvolume;	        ///< motion detection block data
  __u8      audiorecvenable;    	///< audio receiver enable
  __u8      audioServerIp[MAX_NAME_LENGTH];        ///< audio server ip address
}Audio_Config_Data;

typedef struct
{
	__u8	dstampenable;					///< stream1 date stamp enable (ON/OFF)
	__u8	tstampenable;   				///< stream1 time stamp enable (ON/OFF)
	__u8	nLogoEnable;					///< stream1 logo enable (ON/OFF)
	__u8	nLogoPosition;   				///< stream1 logo position
	__u8	nTextEnable;					///< stream1 text enable (ON/OFF)
	__u8	nTextPosition;   				///< stream1 text position
	__u8	overlaytext[OSDTEXTLEN];		///< register user name
    __u8	nDetailInfo;
}OSD_config;

typedef struct
{
	__u32 ipRatio;
	__u8 fIframe;
	__u8 qpInit;
	__u8 qpMin;
	__u8 qpMax;
	__u8 meConfig;
	__u8 packetSize;
	__u8 unkown1;
	__u8 unkown2;
}CodecAdvParam;

typedef struct _ROI_Param{
	__u32 startx;
	__u32 starty;
	__u32 width;
	__u32 height;
}ROI_Param;

typedef struct _CodecROIParam{
	__u32 		numROI;
  	__u32     	prevwidth;
  	__u32     	prevheight;
	ROI_Param	roi[3];
}CodecROIParam;

/**
* @brief motion detection configuration data.
*/
typedef struct
{
  __u8	    fdetect; 					    ///< facedetect (ON/OFF)
  __u32     startX;
  __u32     startY;
  __u32     width;
  __u32     height;
  __u32     prevwidth;
  __u32     prevheight;
  __u8      fdconflevel;
  __u8      fddirection;
  __u8      frecog;
  __u8	    frconflevel;
  __u8	    frdatabase;
  __u8      pmask;
  __u8      maskoption;
}Face_Config_Data;

typedef struct
{
  __u32 curRuleNo;
  __u32 numRules;
  __u8  ruleName[MAX_DMVA_SAVELOAD_SETS][MAX_DMVA_STRING_LENGTH+2];
}Dmva_save_load_rules_t;

/**
* @brief DMVA configuration data.
*/
typedef struct
{
  __u32	dmvaenable_sys; 					    ///< dmvaenable (ON/OFF)
  __u32	dmvaexptalgoframerate_sys; 			    ///< video (fps/N): 3 means 30/3 = 10 fps
  __u32	dmvaexptalgodetectfreq_sys;			    ///< 10 means once in 10 frames that go to DMVA
  __u32	dmvaexptevtrecordenable_sys;		    ///< 0: Off, 1: On
  __u32 dmvaexptSmetaTrackerEnable_sys;         ///< 0: Off, 1:On

  /* DMVA Video Analytics Main Page */
  __u32 dmvaAppCamID_sys;
  __u32 dmvaAppGetSchedule_sys;
  __u32 dmvaAppCfgFD_sys;
  __u32 dmvaAppCfgCTDImdSmetaTzOc_sys; /* Bit 4: OC, Bit 3: TZ, Bit 2: SMETA, Bit 1: IMD, Bit 0: CTD */
  __u32 dmvaAppDisplayOptions_sys; /* Bit 3: Object Bounding Box, Bit 2: Zones, Bit 1: TimeStamp, Bit 0: MetaData Overlay*/
  __u32 dmvaAppEventRecordingVAME_sys;/* Bit 3: Video Stream, Bit 2: Audio Stream, Bit 1: MetaData Stream, Bit 0: Event Log */
  __u32 dmvaAppEventListActionPlaySendSearchTrash_sys;/* Bit 3: Play, Bit 2: Send Email, Bit 1: Search, Bit 0: Trash*/
  __u32 dmvaAppEventListSelectEvent_sys;
  __u32 dmvaAppEventListArchiveFlag_sys;
  __u32 dmvaAppEventListEventType_sys;
  __u32 dmvaAppEventListStartMonth_sys;
  __u32 dmvaAppEventListStartDay_sys;
  __u32 dmvaAppEventListStartTime_sys;
  __u32 dmvaAppEventListEndMonth_sys;
  __u32 dmvaAppEventListEndDay_sys;
  __u32 dmvaAppEventListEndTime_sys;
  __u8  dmvaAppEventListTimeStamp_sys[MAX_DMVA_STRING_LENGTH];
  __u8  dmvaAppEventListEventDetails_sys[MAX_DMVA_STRING_LENGTH];

  /* DMVA ADVANCED SETTINGS PAGE */
  __u32  dmvaObjectMetaData_sys;
  __u32  dmvaOverlayPropertiesFontType_sys;
  __u32  dmvaOverlayPropertiesFontSize_sys;
  __u32  dmvaOverlayPropertiesFontColor_sys;
  __u32  dmvaOverlayPropertiesBoxColor_sys;
  __u32  dmvaOverlayPropertiesCentroidColor_sys;
  __u32  dmvaOverlayPropertiesActiveZoneColor_sys;
  __u32  dmvaOverlayPropertiesInactiveZoneColor_sys;
  __u32  dmvaOverlayPropertiesVelocityColor_sys;
  __u32  dmvaEnvironmentProfileLoad_sys;
  __u32  dmvaEnvironmentProfileStore_sys;
  __u32  dmvaEnvironmentProfileClear_sys;
  __u32  dmvaEnvironmentProfileState_sys;
  __u8  dmvaEnvironmentProfileDesc_sys[MAX_DMVA_STRING_LENGTH];

  /* Camera Tamper Detect Parameters */
  __u32 dmvaAppCfgTDEnv1_sys;/* Detect Lights On to Off */
  __u32 dmvaAppCfgTDEnv2_sys;/* Detect Lights On to Off */
  __u32 dmvaAppCfgTDSensitivity_sys;/* 1 to 5: Low to High */
  __u32 dmvaAppCfgTDResetTime_sys; /* Number of seconds */

  /* Trip Zone Parameters */
  __u32 dmvaAppTZSensitivity_sys; /* 1 to 5: Low to High */
  __u32 dmvaAppTZPersonMinWidth_sys;
  __u32 dmvaAppTZPersonMinHeight_sys;
  __u32 dmvaAppTZVehicleMinWidth_sys;
  __u32 dmvaAppTZVehicleMinHeight_sys;
  __u32 dmvaAppTZPersonMaxWidth_sys;
  __u32 dmvaAppTZPersonMaxHeight_sys;
  __u32 dmvaAppTZVehicleMaxWidth_sys;
  __u32 dmvaAppTZVehicleMaxHeight_sys;
  __u32 dmvaAppTZDirection_sys; /* 4: Bi-direction, 2: Zone 1 to Zone 2, 1: Zone 2 to Zone 1 */
  __u32 dmvaAppTZEnable_sys;

  __u32 dmvaAppTZZone1_ROI_numSides_sys;
  __u32 dmvaAppTZZone2_ROI_numSides_sys;

  __u32 dmvaAppTZZone1_x[17];
  __u32 dmvaAppTZZone1_y[17];
  __u32 dmvaAppTZZone2_x[17];
  __u32 dmvaAppTZZone2_y[17];

  __u8 dmvaAppTZSave_sys[MAX_DMVA_STRING_LENGTH+2];
  __u32 dmvaAppTZLoad_sys;
  __u32 dmvaAppTZPresentAdjust_sys;

  /* Object Counting Parameters */
  __u32 dmvaAppOCSensitivity_sys; /* 1 to 5: Low to High */
  __u32 dmvaAppOCObjectWidth_sys;
  __u32 dmvaAppOCObjectHeight_sys;
  __u32 dmvaAppOCDirection_sys;/* 4: Bi-direction, 2: Zone 1 to Zone 2, 1: Zone 2 to Zone 1 */
  __u32 dmvaAppOCEnable_sys;
  __u32 dmvaAppOCLeftRight_sys;
  __u32 dmvaAppOCTopBottom_sys;

  __u8 dmvaAppOCSave_sys[MAX_DMVA_STRING_LENGTH+2];
  __u32 dmvaAppOCLoad_sys;

  /* Streaming META Parameters */
  __u32 dmvaAppSMETASensitivity_sys; /* 1 to 5: Low to High */
  __u32 dmvaAppSMETAEnableStreamingData_sys;
  __u32 dmvaAppSMETAStreamBB_sys;
  __u32 dmvaAppSMETAStreamVelocity_sys;
  __u32 dmvaAppSMETAStreamHistogram_sys;
  __u32 dmvaAppSMETAStreamMoments_sys;
  __u32 dmvaAppSMETAPresentAdjust_sys;	/* 8: Person present, 4: Present vehicle, 2: View Person, 1: View vehicle */
  __u32 dmvaAppSMETAPersonMinWidth_sys;
  __u32 dmvaAppSMETAPersonMinHeight_sys;
  __u32 dmvaAppSMETAPersonMaxWidth_sys;
  __u32 dmvaAppSMETAPersonMaxHeight_sys;
  __u32 dmvaAppSMETAVehicleMinWidth_sys;
  __u32 dmvaAppSMETAVehicleMinHeight_sys;
  __u32 dmvaAppSMETAVehicleMaxWidth_sys;
  __u32 dmvaAppSMETAVehicleMaxHeight_sys;
  __u32 dmvaAppSMETANumZones_sys;
  __u32 dmvaAppSMETAZone1_ROI_numSides_sys;
  __u32 dmvaAppSMETAZone1_x[17];
  __u32 dmvaAppSMETAZone1_y[17];

  __u8 dmvaSMETAZone_1_Label_sys[MAX_DMVA_LABEL_STRING_LENGTH];

  __u8 dmvaAppSMETASave_sys[MAX_DMVA_STRING_LENGTH+2];
  __u32 dmvaAppSMETALoad_sys;
  __u32 dmvaAppSMETAEnableVelocityObjId_sys;

  /* IMD Parameters */
  __u32 dmvaAppIMDSensitivity_sys; /* 1 to 5: Low to High */
  __u32 dmvaAppIMDPresentAdjust_sys; /* 8: Person present, 4: Present vehicle, 2: View Person, 1: View vehicle */
  __u32 dmvaAppIMDPersonMinWidth_sys;
  __u32 dmvaAppIMDPersonMinHeight_sys;
  __u32 dmvaAppIMDVehicleMinWidth_sys;
  __u32 dmvaAppIMDVehicleMinHeight_sys;
  __u32 dmvaAppIMDPersonMaxWidth_sys;
  __u32 dmvaAppIMDPersonMaxHeight_sys;
  __u32 dmvaAppIMDVehicleMaxWidth_sys;
  __u32 dmvaAppIMDVehicleMaxHeight_sys;
  __u32 dmvaAppIMDNumZones_sys; /* 1 to 4 */
  __u32 dmvaAppIMDZoneEnable_sys; /* Bit 3: Zone 3 enable, Bit 2: Zone 2 enable, Bit 1: Zone 1 enable, Bit 0: Zone 0 enable*/

  __u32 dmvaAppIMDZone1_ROI_numSides_sys;
  __u32 dmvaAppIMDZone2_ROI_numSides_sys;
  __u32 dmvaAppIMDZone3_ROI_numSides_sys;
  __u32 dmvaAppIMDZone4_ROI_numSides_sys;

  __u32 dmvaAppIMDZone1_x[17];
  __u32 dmvaAppIMDZone1_y[17];
  __u32 dmvaAppIMDZone2_x[17];
  __u32 dmvaAppIMDZone2_y[17];
  __u32 dmvaAppIMDZone3_x[17];
  __u32 dmvaAppIMDZone3_y[17];
  __u32 dmvaAppIMDZone4_x[17];
  __u32 dmvaAppIMDZone4_y[17];

  __u8 dmvaIMDZone_1_Label_sys[MAX_DMVA_LABEL_STRING_LENGTH];
  __u8 dmvaIMDZone_2_Label_sys[MAX_DMVA_LABEL_STRING_LENGTH];
  __u8 dmvaIMDZone_3_Label_sys[MAX_DMVA_LABEL_STRING_LENGTH];
  __u8 dmvaIMDZone_4_Label_sys[MAX_DMVA_LABEL_STRING_LENGTH];

  __u8 dmvaAppIMDSave_sys[MAX_DMVA_STRING_LENGTH+2];
  __u32 dmvaAppIMDLoad_sys;

  __u32 dmvaEventDeleteIndex;
  __u32 dmvaEventDeleteStartIndex;
  __u32 dmvaEventDeleteEndIndex;
  __u32 dmvaEventDeleteAll;

  /* DMVA Enc Roi Enable */
  __u32 dmvaAppEncRoiEnable_sys;

  /* TZ/OC box min/max size limit */
  __u32 dmvaTZOCMinWidthLimit_sys;
  __u32 dmvaTZOCMinHeightLimit_sys;
  __u32 dmvaTZOCMaxWidthLimit_sys;
  __u32 dmvaTZOCMaxHeightLimit_sys;

  /* IMD/SMETA box min/max size limit */
  __u32 dmvaIMDSMETAPeopleMinWidthLimit_sys;
  __u32 dmvaIMDSMETAPeopleMinHeightLimit_sys;
  __u32 dmvaIMDSMETAPeopleMaxWidthLimit_sys;
  __u32 dmvaIMDSMETAPeopleMaxHeightLimit_sys;

  __u32 dmvaIMDSMETAVehicleMinWidthLimit_sys;
  __u32 dmvaIMDSMETAVehicleMinHeightLimit_sys;
  __u32 dmvaIMDSMETAVehicleMaxWidthLimit_sys;
  __u32 dmvaIMDSMETAVehicleMaxHeightLimit_sys;

  /* save/load rules */
  Dmva_save_load_rules_t dmvaSaveLoadTZ;
  Dmva_save_load_rules_t dmvaSaveLoadIMD;

  Dmva_save_load_rules_t dmvaSaveLoadOC;
  Dmva_save_load_rules_t dmvaSaveLoadSMETA;

  /* Dmva event serch */
  __u32 dmvaEventSearchListStartIndex;
  __u32 dmvaEventSearchListEndIndex;

  /* DMVA Schedule Manager */
  DMVA_scheduleMgr_t dmvaScheduleMgr;

  __u32 sdUmountFlag;

}Dmva_Config_Data;

typedef struct
{
  __u16     width;
  __u16     height;
  __u16		portnum;
  __u8		name[MAX_STREAM_NAME];
  __u8 		portname[MAX_STREAM_NAME];
}StreamParam;

/**
* @brief structure for PTZ control data.
*/
typedef struct
{
  char      ptzdata[300];
}Ptz_Data;


/**
* @brief PTZ configuration data.
*/
typedef struct
{
	char	ptzzoomin[6];	///< zoom-in
	char	ptzzoomout[7];	///< zoom-out
	char	ptzpanup[2];	///< pan-up
	char	ptzpandown[4];	///< pan-down
	char	ptzpanleft[4];		///< pan-left
	char	ptzpanright[5];		///< pan-right
	int		eptzmdoe;//0:manual eptz mode, 1: autotracking 2:autopatrol
	char    reserve[36]; // add by kenny chiu 20130201
}Ptz_Config_Data;

/**
* @brief event log data structure.
*/
typedef struct LogEntry_t{
	char event[50];		///< event description
	struct tm time;		///< event log time
}LogEntry_t;

/**
* @brief event log data structure.
*/
typedef struct DmvaLogEntry_t{
	__u8  eventListEventDetails_sys[MAX_DMVA_STRING_LENGTH];///< event description
	__u8  eventListTimeStamp_sys[MAX_DMVA_STRING_LENGTH];///< event log time string
	__u8  eventListFileName[MAX_DMVA_STRING_LENGTH];///< event file name string    AYK - 0310
	__u32 eventListArchiveFlag_sys;
	__u32 eventListEventType_sys;
	struct tm time;		///< event log time
}DmvaLogEntry_t;
/**
* @brief user settings structure.
*/
typedef struct{
	char id[USER_LEN];
	char password[PASSWORD_LEN];
	char remote_ip[IP_STR_LEN];
}login_data_t;

typedef struct{
	char	user_id[USER_LEN];
	int		authority;
}get_user_authority_t;

typedef struct{
	char	user_id[USER_LEN];
	char	password[PASSWORD_LEN];
	__u8	authority;
}add_user_t;

/* Onvif Structures */
typedef struct {
	char profile_name[20];
	char profile_token[20];
}media_profile;


typedef struct
{
	char	Vname_t[MAX_PROF_TOKEN];
	int		Vcount_t;			// count for number of users
	char	Vtoken_t[MAX_CONF_TOKEN];		//video source configuration token
	char	Vsourcetoken_t[MAX_PROF_TOKEN];	//video source profile token
	int		windowx_t;			//x cordinate of window
	int		windowy_t;			//y cordinate of window
	int		windowwidth_t;		//width
	int		windowheight_t;		//height
}video_source_conf_in;

typedef struct
{
	int						position;  //position of profile
	video_source_conf_in	video_source_conf_in_t;
}video_source_conf;

typedef struct
{
	int		nIPType_t;               //ip address IPv4/IPv6
	char	IPv4Addr_t[MAX_IP_ADDR];	//ipv4 address
	char	IPv6Addr_t[MAX_IP_ADDR];	//ipv6 address
	int		port_t;                   //port number
	int		ttl_t;                    //TTL
	int		autostart_t;              //autostart
}Video_Multicast_configuration_in;

typedef struct
{
	char								VEname_t[MAX_PROF_TOKEN] ;                 //video encoder name
	char								VEtoken_t[MAX_CONF_TOKEN];                 //video encoder token
	int									VEusercount_t;            //video encoder user count
	float								VEquality_t;              //video encoder quality
	int									Vencoder_t;             //video encoder for jpeg, mpeg, h.264
	int									Rwidth_t;               //resolution width
	int									Rheight_t;                //resolution height
	int									frameratelimit_t;        //frameratelimit
	int									encodinginterval_t;       //encodinginterval
	int									bitratelimit_t;            //bitratelimit
	int									Mpeggovlength_t;              //mpeg GoVLength
	int									Mpegprofile_t;                 //mpegprofile SP/ASP
	int									H264govlength_t;               //H264 govlength
	int									H264profile_t;                 //H264 profile  baseline/main/extended/high
	long								sessiontimeout_t;
	Video_Multicast_configuration_in	Multicast_Conf_t;

}video_encoder_conf_in;

typedef struct
{
	int						position;
	video_encoder_conf_in	video_encoder_conf_in_t;
}video_encoder_conf;

typedef struct
{
	char	VAname_t[MAX_PROF_TOKEN];                    //video analytic name
	char	VAtoken_t[MAX_CONF_TOKEN];                  //video analytic token
	int		VAusercount_t;              //video analytic user count
	int		VARsize_t;                 //video analytic rule size
	int		VARSsizeitem_t;               //size of simple items for ruleengine
	char	VARSname_t[MAX_LENGTH];                   //simple item name for rule engine
	char	VARSvalue_t[MAX_LENGTH];                 //simple item value for rule engine
	int		VAREsize_t;                 //Element size for rule engine
	char	VAREname_t[MAX_LENGTH];                   //element name for rule engine
	char	Rconfigname_t[MAX_LENGTH];
	char	Rconfigtype_t[MAX_LENGTH];
	int		VAMsize_t;                  //video analytic module size
	int		VAsizeitem_t;               //size of simple items
	char	VASname_t[MAX_LENGTH];                   //simple item name
	char	VASvalue_t[MAX_LENGTH];                 //simple item value
	int		VAEsize_t;                 //Element size
	char	VAEname_t[MAX_LENGTH];                   //element name
	char	Aconfigname_t[MAX_LENGTH];
	char	Aconfigtype_t[MAX_LENGTH];
}video_analytics_conf_in;

typedef struct
{
	int						position;
	video_analytics_conf_in	video_analytics_conf_t;
}video_analytics_conf;

typedef struct
{
	char	name[MAX_CONF_TOKEN];
	int		usecount;
	char	token[MAX_CONF_TOKEN];
}video_output_conf_in;

typedef struct
{
	int						position;
	video_output_conf_in	video_output_conf_t;
}video_output_conf;

typedef struct {
	char	type_t[3][100];
	char	name_t[50];
	char	location_t[3][100];
	char	hardware_t[50];
} Onvif_Scopes_t;

typedef struct{
	char	token_t[MAX_LENGTH];
	int		relaymode_t;
	long	delaytime_t;
	int		idlestate_t;
} relay_conf_t;

typedef struct
{
	int				position;
	int				logicalstate_t;
	relay_conf_t	relay_conf_in_t;
}relay_conf;
typedef struct
{
	char	Aname_t[MAX_LENGTH];		//audio name
	int		Ausecount_t;				//audio user count
	char	Atoken_t[MAX_LENGTH];		//audio token
	char	Asourcetoken_t[MAX_LENGTH];	//audio source token
}Add_AudioSource_Configuration_t;

typedef struct
{
	int								position;
	Add_AudioSource_Configuration_t	Add_AudioSource_in;
}Add_AudioSource_Configuration_tmp;

typedef struct
{
	char	AEname_t[MAX_LENGTH];	//audio encoder name
	char	AEtoken_t[MAX_LENGTH];	//audio encoder token
	int		AEusercount_t;			//audio encoder user count
	int		bitrate_t;				//Bitrate
	int		samplerate_t;			//Samplerate
	int		AEencoding_t;
}Add_AudioEncoder_Configuration_t;

typedef struct
{
	int position;
	Video_Multicast_configuration_in multicast_in;
	Add_AudioEncoder_Configuration_t Add_AudioEncoder_in;
}Add_AudioEncoder_Configuration_tmp;

typedef struct
{
	char								MDname_t[MAX_LENGTH];
	int									MDusecount_t;
	char								MDtoken_t[MAX_LENGTH];
	int									MDanalytics_t;
	long								sessiontimeout_t;
	Video_Multicast_configuration_in	VMC_t;
}Metadata_configuration_in;

typedef struct
{
	int							position;
	Metadata_configuration_in	MDC_in;
}Metadata_configuration_tmp;

/* End-Onvif Structures */

typedef struct{
	char qos_mode;
	char qos_type;
	char qos_video_dscp;
	char qos_audio_dscp;
	char cos_mode;
	short cos_vlanid;
	char cos_videopriority;
	char cos_audiopriority;
	char cos_address[64];
	char snmp_mode;
	char snmp_type;
	char snmp_username[64];
	char snmp_username_privacy;
	char snmp_auth;
	char snmp_privacy;
	char snmp_auth_pw[64];
	char snmp_privacy_pw[64];
	char snmp_readcommunity[64];
	char snmp_writecommunity[64];
}NetworkMode;

typedef struct{
	char pppoe_mode;
	char username[64];
	char password[64];
}PPPoEMode;

typedef struct{
	char ddns_mode;
	char provider[64];
	char hostname[64];
	char username[64];
	char password[32];		/* 64 byte -> 32 byte - Dean - 20120419 */
	int  period_time;		/* Modify DDNS Provider & Add DDNS Provider Planet - DDNS Update Period - 600~86400  - Dean - 20120419 */
	int  updatetime_sec;	/* Add Update Time - Dean - 20120601 */
	int  updatetime_min;	/* Add Update Time - Dean - 20120601 */
	int  updatetime_hour;	/* Add Update Time - Dean - 20120601 */
	int  updatetime_mday;	/* Add Update Time - Dean - 20120601 */
	int  updatetime_mon;	/* Add Update Time - Dean - 20120601 */
	int  updatetime_year;	/* Add Update Time - Dean - 20120601 */
	char connecting;		/* Add DDNS Connecting - Dean - 20120605 */
	char reserver[3];		/* Add Resever - Dean - 20120419 */
}DDNSMode;

typedef struct{
	char upnp_mode;
	char upnp_discover;
	unsigned short upnp_traversal;
	unsigned char  upnp_portmapping;
	char upnp_avenable;
	char bonjour_mode;
	char friendly_name[64];
	char live_domain[128];
}BonjourMode;

typedef struct{
	char mail_mode;
	char mail_sendto[64];
	char mail_subject[64];
	char tcp_mode;
	char tcp_addr[64];
	unsigned short tcp_port;
	char http_mode;
	char http_url[64];
	char http_username[64];
	char http_password[64];
}NotificationMode;

typedef struct{
	char sip_mode;
	char sip_phonenumber[16];
	short sip_port;
	char sip_ipaddress[64];
	char sip_loginid[64];
	char sip_loginpw[64];
}SIPMODE;

typedef struct{
	char eventconfig;	/* Event -> Media -> 0:H.264, 1:One Snapshot *//* Modify Event - 20120620 - Dean */
	char prealarm;
	char postalarm;
}FTPALARMMODE;

#define MAX_IPFILTER_NUMBER  20
typedef struct{
	char ipf_mode;
	char ip4_mode[MAX_IPFILTER_NUMBER];
	char ip6_mode[MAX_IPFILTER_NUMBER];
	int  ip_number;
	char ip_type[MAX_IPFILTER_NUMBER];
	char ip_start[MAX_IPFILTER_NUMBER][64];
	char ip_end[MAX_IPFILTER_NUMBER][64];
}IPFILTERMODE;

typedef struct{
	char  https_mode;
	char  http_mode;
	int   https_port;
}HTTPSMODE;

typedef struct{
	char  mirrortype;
	char  overlaydate;
	char  overlaytext;
	char  imagetext[32];
	char  overlayimage;
	char  overlayimage_type;	// 0->Cisco Logo, 1->Custom Image
	char  overlayimage_file[64];
	char  timestamp;
	short overlayimage_xpos;
	short overlayimage_ypos;
} IMAGESETTINGMODE;

#define DEFAULT_GROUP_ADDRESS "232.128.1.99"
typedef struct{
	char ipmode;
	char dns2ip[32];
	char group_addr[32];
	char ipv6_addr[64];
	unsigned short ipv6_prefix;
	unsigned short rtspport;
	unsigned short rtpport;
	unsigned short videoport;
	unsigned short audioport;
	unsigned char ttl;
	unsigned short rtpdataport;
	char multicastenable;
	char pppoeenable;
	char pppoeaccount[32];
	char pppoepwd[32];
	char ddnsenable;
	char ddnsprovider;
	char ddnshostname[31];		/* Add DDNS Provider PCI - Dean - 20120601 */
	char ddnsdomain;			/* Add DDNS Provider PCI - Dean - 20120601 */
	char ddnsaccount[32];
	char ddnspassword[32];
}GENERAL_NET_MODE;

/* Event Modify - Begin - 20120620 - Dean */
typedef enum{
	SCH_ALARM_MOTION = 0,
	SCH_ALARM_DI_1,
	SCH_ALARM_DI_2,
#if EVENT_REBOOT
	SCH_ALARM_REBOOT,
#endif
#if Intelligence_FEATURE
	SCH_ALARM_OV,
#endif
#if IVS_FEATURE
	SCH_ALARM_IVS,
#endif
	SCH_ALARM_TOTAL
}SchAlarmEvent;
/* Event Modify - End - 20120620 - Dean */

#if 1
typedef struct SchAlarmQ_t{
	struct SchAlarmQ_t *pNext;
	Event_t		event; ///< Message command ID.
	int			index; ///< To tell witch schedule is.
	int			serial;///< Motion detect JPEG serial.
	int			alarmaudio_duration;///<Play alarm audio base on duration.
	int			alarmaudio_enable;///<Restart the alarm audio.
	int			alarmsettings[SCH_ALARM_TOTAL];	/* Modify Event - 20120620 - Dean */
}SchAlarmQ_t;

typedef struct LogQ_t{
	struct LogQ_t *pNext;
	LogEntry_t log_msg;
} LogQ_t;

#if 0
/* Record Media - Dean - 20130530 - Begin */
typedef struct _SchSystemCallQ_t{
	struct SchSystemCallQ_t *pNext;
	System_Call_Event_t		event; ///< Message command ID.
	int			index; ///< To tell witch schedule is.
	int			iCmdLen;
	char			strCmd[1024];
	char			reserved[512];
	long			NotUse;		/* For sizeof(msgbuf)-sizeof(long) - Dean - 20130530 */
}SchSystemCallQ_t;
/* Record Media - Dean - 20130530 - End */
#endif
#endif

/* Add Event Modification - Http Notify - Begin - 20120619 - Dean */
/* Event Modify - Remove - Begin - 20120620 - Dean */
//#if HTTP_NOTIFY_FEATURE
//typedef enum{
//	EVENT_HTTP_NOTIFY_MOTION = 0,
//	EVENT_HTTP_NOTIFY_DI_1,
//	EVENT_HTTP_NOTIFY_DI_2,
//	EVENT_HTTP_NOTIFY_TOTAL
//}EventHttpNotifyId;
//#endif
/* Event Modify - Remove - End - 20120620 - Dean */

#if HTTP_NOTIFY_FEATURE
typedef enum{
	EVENT_HTTP_NOTIFY_DI_TRIG_OPEN = 0,
	EVENT_HTTP_NOTIFY_DI_TRIG_CLOSE,
	EVENT_HTTP_NOTIFY_DI_TRIG_CHANGE,
	EVENT_HTTP_NOTIFY_DI_TRIG_TOTAL
}EventHttpNotifyDITrig;

#define EVENT_TRIG_DI_CHAR_LIST	{'0', '1', '2'}	/* Modify Event - 20120620 - Dean */
#define EVENT_TRIG_DI_VALUE_LIST	{EVENT_HTTP_NOTIFY_DI_TRIG_OPEN, EVENT_HTTP_NOTIFY_DI_TRIG_CLOSE, EVENT_HTTP_NOTIFY_DI_TRIG_CHANGE}	/* Modify Event - 20120620 - Dean */

/* Add Enable/Disable - Begin - Dean - 20121012 */
typedef enum{
	EVENT_HTTP_NOTIFY_TRIG_DISABLE = 0,
	EVENT_HTTP_NOTIFY_TRIG_ENABLE,
	EVENT_HTTP_NOTIFY_TRIG_TOTAL
}EventHttpNotifyTrigEnable;

#define EVENT_TRIG_ENABLE_CHAR_LIST	{'0', '1'}
/* Add Enable/Disable - End - Dean - 20121012 */

typedef struct{
	char enable;
	char trig;			/* Modify Event - 20120620 - Dean */
	char reserved1[2];	/* Modify Event - 20120620 - Dean */
	char url[64];
	char port[8];
	char cgi[64];
	char event[16];
	char time[32];
	char reserved2[32];	/* Modify Event - 20120620 - Dean */
}EVENT_HTTP_NOTIFY;
#endif
/* Add Event Modification - Http Notify - End - 20120619 - Dean */

typedef struct{
	char samba_server_ip[64];
	char samba_path[128];
	char samba_mount_path[128];
	char samba_username[16];
	char samba_password[16];
	char http_url[32];
	char http_username[16];
	char http_password[16];
	char jabber_im_server[32];
	char jabber_login_id[32];
	char jabber_login_pw[32];
	char jabber_send_to[32];
	char jabber_message[64];
	char ftp_enable;
	char smtp_enable;
	char samba_enable;
	char jabber_enable;
#if HTTP_NOTIFY_FEATURE								/* Add Event Modification - Http Notify - 20120619 - Dean */
	EVENT_HTTP_NOTIFY http_notify[SCH_ALARM_TOTAL];	/* Add Event Modification - Http Notify - 20120619 - Dean *//* Modify Event - 20120620 - Dean */
#endif												/* Add Event Modification - Http Notify - 20120619 - Dean */
}EVENT_SERVER;

typedef struct{
	char sec8021_x_enable;
	char sec8021_x_enable_tmp;
	char sec8021_x_protocol;
	char sec8021_x_username[32];
	char sec8021_x_password[32];
	char sec8021_x_ca_file[64];
	char sec8021_x_cert_file[64];
	char sec8021_x_privkey_file[64];
	char sec8021_x_privkey_password[32];
	char sec8021_x_tunnel_protocol;
}SEC8021_X;

typedef struct{
	char time_type;
	char restart_type;
	short log_num;
}XML_INFO;

typedef struct{
	unsigned char bStatus;	///< schedule status ( 0:disable 1:enable }
	char reserved[3];  /* Record Media - Dean - 20130703 */
	unsigned char nDay[8];	///< schedule day of week (Sun Mon Tue Wed Thr Fri Sat)
	Time_t tStart;		      ///< schedule start time
	Time_t tDuration;	    ///< schedule duration
} ScheduleStatus;

#define DEFAULT_EVENT_PRIORITY 1 //0->high 1->normal 2->low
#define DEFAULT_EVENT_WINDOWS "000"
typedef struct {
	char enable_event;
	char priority;
	char trigger_motion_detection;
	char trigger_digital_in_mode1;
	char trigger_digital_in_mode2;
	char trigger_digital_in_type1;
	char trigger_digital_in_type2;
#if Intelligence_FEATURE
	char trigger_ov_alert;
#endif // Intelligence_FEATURE
#if IVS_FEATURE
	char trigger_ivs_alert;
#endif // IVS_FEATURE
	char trigger_periodically_mode;
	char trigger_periodically_min;
	char trigger_system_boot;
	char trigger_schedule_time_mode;
	ScheduleStatus schedule_time;
	char trigger_motion_in_window[3];
	char enable_ftp;
	char enable_jabber;
	char enable_email;
	char event_preset_mode;
	char event_preset_position;
	char enable_samba;
	char enable_usb;
	char trigger_digital_out_mode;
	char trigger_digital_out_sec;
	char event_name[24];
	char trigger_time_gap;		/* TAMPER_FEATURE Event - Dean - 20131007 */
	char trigger_tamper_alert;	/* TAMPER_FEATURE - Dean - 20131007 */
	char reserved[6];
} EVENT_SCHEDULE;

typedef struct{
	char	ptzzoomin;	  ///< zoom-in
	char	ptzzoomout;	  ///< zoom-out
	char	ptzfocusin;
	char	ptzfocusout;
	char	ptzfocusauto;
	char	ptzpanup;   	///< pan-up
	char	ptzpandown;	///< pan-down
	char	ptzpanleft;	///< pan-left
	char	ptzpanright;	///< pan-right
	char	ptzpanhome;
	char	ptzspeed[16];
	char	ptzgoto;
}PTZ_DATA;

typedef struct
{
	unsigned char audioinvolume;
	unsigned char microphonevolume;
	unsigned char speakervolume;
	unsigned char gain;
}AudioInfo;

#define DEFAULT_SMTP_PORT 25
typedef struct{
	unsigned char auth;
	unsigned char ssl_mode;
	unsigned short port;
	unsigned char starttls;
}SMTP_INFO;

#define DEFAULT_SYSLOG_PORT 514
typedef struct{
	char enable;
	unsigned char ip[32];
	unsigned short port;
}SYSLOG_INFO;

#define RECORD_PATH_DEFAULT     "Record"
#define DEFAULT_MAX_SPACE_SIZE  250  //samba disk total space
#define DEFAULT_MAX_RECORD_SIZE  10  //file max size
typedef struct{
	char enable;
	char samba_server_ip[64];
	char samba_path[64]; 
	char reserved1[3]; 
	int  samba_space_max;	//total max space
	char reserved2[57];                  
	char samba_mount_path[128];
	char samba_username[16];
	char samba_password[16];
	short max_size;        //file max size
	ScheduleStatus schedule_time;
	char timestamp;
	char reserved[15];  /* Record Media - Dean - 20130703 */
}SAMBA_RECORD;

typedef struct
{
	unsigned int status;
	unsigned int maxsize;
	unsigned int enable;
}SAMBA_INFO;
SAMBA_INFO samba_info;

#define dPeriodMin 0
#define dPeriodHour 1

typedef struct{
	char enable;
	char MediaType;
	char MediaFileTm;
	char MediaPrefixName[128];
	char Mediatimestamp;
	char PeriodValue;
	char PeriodUnit;
}UPLOAD_RECORD;

#if IPNC_PELCOD_FEATURE
#define MIN_PAN_SPEED			0
#define MAX_PAN_SPEED			9
#define MIN_TILT_SPEED			0
#define MAX_TILT_SPEED			9
#define MAX_PRESET_NUM			8
#define MAX_PRESET_NAME_LEN		16
#define MAX_PATROL_SIZE			20
#define HOME_PRESET				0x10
#define MAX_NUM_EXTCMD			5
#define RS485_CMD_STR_LEN		16
#define RS485_EXT_NAME_LEN		16
#define MAX_TOUR_SIZE			20

typedef struct {
	char	enable;
	int		baudrate;
	int		databits;
	int		parity;
	int		stopbit;
	char	addr;
	char	pan_speed;
	char	tilt_speed;
	char	preset[MAX_PRESET_NUM][MAX_PRESET_NAME_LEN+1];
	char	patrol[MAX_PATROL_SIZE];
	char	cHome[RS485_CMD_STR_LEN+1];
	char	cUp[RS485_CMD_STR_LEN+1];
	char	cDown[RS485_CMD_STR_LEN+1];
	char	cLeft[RS485_CMD_STR_LEN+1];
	char	cRight[RS485_CMD_STR_LEN+1];
	char	cExtname[MAX_NUM_EXTCMD][RS485_EXT_NAME_LEN+1];
	char	cExtcmd[MAX_NUM_EXTCMD][RS485_CMD_STR_LEN+1];
} RS485;
#endif	//IPNC_PELCOD_FEATURE

#if IPNC_IRCUT_FEATURE
#define IRCUT_MODE_AUTO		0
#define IRCUT_MODE_NIGHT	1
#define IRCUT_MODE_DAY		2
#define IRCUT_MODE_SCHEDULE	3
#define IRCUT_MODE_LOWLIGHT_IRCUT	4
#define IRCUT_MODE_LOWLIGHT	5
#define IRCUT_MIN_TIME		0
#define IRCUT_MAX_TIME		1439
typedef struct {
	char	mode;
	char	externallightsensor; // 0 : disable, 1 : DI1, 2 : DI2...
	char	trigger; // 0 : Low Trigger, 1 : High Trigger
	char	delaytime;
	short	sche_start_minute;
	short	sche_end_minute;
	char	lux;
	char	lux2;
	char	reserved[2];
} IRCUT_INFO;
#endif	//IPNC_IRCUT_FEATURE

#if Intelligence_FEATURE
typedef struct {
	char	enable;
} OBJECTVIDEO;
#endif 	//Intelligence_FEATURE

#if WIFI_FEATURE
#define WPA_SUPPLICANT_CONF	"/mnt/nand/wpa_supplicant.conf"

#define WIFI_MAX_SSID_LEN		32
#define WIFI_MAX_NUM_WEP_KEY	4
#define WIFI_MAX_WEP_KEY_LEN	26
#define WIFI_MAX_PSK_LEN		64
//alice_modify_2012_3_15_v1
//alice_modify_2012_3_27_v1
typedef struct
{
	char	mode;		// infrastructure/ad-hoc
	char	channel;	// 1~14
	char	security;	// none, WEP, WPA/WPA2
	char	wepauth;	// Open, Share
	char	wepkeyfmt;	// ascii_64, ascii_128, hex_64, hex_128
	char	wepkeyidx;	// 0~3
	char	wcountryreg;// 1, 2, 3
	char	wbandwidth; //alice_modify_2012_6_29_v1
	char	ssid[WIFI_MAX_SSID_LEN + 1];
	char	wepkeystr[WIFI_MAX_NUM_WEP_KEY][WIFI_MAX_WEP_KEY_LEN + 1];
	char	psk[WIFI_MAX_PSK_LEN + 1];

} WIFI_Config_t;
typedef struct
{
	WIFI_Config_t wifiConfig;
	char	enable;		// 0:disabled, 1:enable
	char	pin_ssid[WIFI_MAX_SSID_LEN + 1];//charles lin wifi6
	char	wifitestbegin;	// 1, 2, 3
	//char	wpstestbegi;	// 1, 2, 3
} WIFI;

#define WIFI_WPS_NONE			0
#define WIFI_WPS_PIN			1
#define WIFI_WPS_PBC			2

#define WIFI_MODE_INFRA			0
#define WIFI_MODE_ADHOC			1

//Authentication Mode
#define WIFI_WEP_AUTH_NONE		0
#define WIFI_WEP_AUTH_OPEN		1
#define WIFI_WEP_AUTH_SHARE		2
#define WIFI_WEP_AUTH_WPA_PSK	3
#define WIFI_WEP_AUTH_WPA2_PSK	4
#define WIFI_WEP_AUTH_WPA_NONE	5

//Encryption Type
#define WIFI_WEP_ENCR_NONE		0
#define WIFI_WEP_ENCR_WEP		1
#define WIFI_WEP_ENCR_TKP		2
#define WIFI_WEP_ENCR_AES		3

#define WIFI_WEP_ENC_ASCII_64	5
#define WIFI_WEP_ENC_HEX_64		10
#define WIFI_WEP_ENC_ASCII_128	13
#define WIFI_WEP_ENC_HEX_128	26

//alice_modify_2012_3_27_v1
#define WIFI_DEFAULT_SSID	"IPCAMTEST"
#define WIFI_FCC 0
#define WIFI_ETSI 1
#define WIFI_JP 5
//alice_modify_2012_3_27_v1
//Factory Test Function Wifi
#define WIFI_TEST_START 1
#define WIFI_TEST_END 0
//alice_modify_2012_6_29_v1
#define WIFI_BAND_WIDTH_20 0
#define WIFI_BAND_WIDTH_40 1
//alice_modify_2012_6_29_v1
#endif //WIFI_FEATURE

//alice_modify_2012_3_15_v1
#if UPLOAD_FEATURE
#define UPLOAD_ENABLE 0
#define UPLOAD_MEDIA_TYPE 1
#define UPLOAD_MEDIAFILE_TM 3
#define UPLOAD_MEDIA_PREFIX_NAME MODEL_NAME
#define UPLOAD_MEDIA_TIMESTAMP 0
#define UPLOAD_PERIOD_VALUE 1
#define UPLOAD_PERIOD_UNIT 0
#endif

typedef struct{
	int  bandwidth;
	char type[8];
}NET_INFO;

typedef struct{
	int Pos_x;
	int Pos_y;
}PTZACTPOS;

#define SCHEDULE_NUM       10
#define SCHEDULE_NUM_ADD   SCHEDULE_NUM+1
#define MAX_PTZ_PRESET 10


/* PRIVACY_MASK_FEATURE - Dean - 20130801 - Begin */
#if 1 //PRIVACY_MASK_FEATURE
#define PRIVACY_MASK_CMD_STR_LEN			256
//#define PRIVACY_MASK_ZONE_MAX				3			/* Privacy Mask Modification - Dean - 20131007 */
#define PRIVACY_MASK_ZONE_CTRL_POINT_MAX	2
#define PRIVACY_MASK_ZONE_TITLE_STR_LEN		16
#define PRIVACY_MASK_ZONE_TITLE_STR_DEFAULT	"Mask"		/* Privacy Mask Modification - Dean - 20131007 */
typedef struct
{
	__u8	enable;
	__u8	color;
	__u8	reserved2[2];  // add by kenny chiu 20131119
	short	point_X[PRIVACY_MASK_ZONE_CTRL_POINT_MAX];
	short	point_Y[PRIVACY_MASK_ZONE_CTRL_POINT_MAX];
	char	title[PRIVACY_MASK_ZONE_TITLE_STR_LEN];
	char	Reserved[20];
}PRIVACY_MASK_ZONE_INFO;

typedef struct
{
	unsigned char	enable;
	unsigned char	reserved2[3];
	PRIVACY_MASK_ZONE_INFO zone[PRIVACY_MASK_ZONE_MAX];
	char			Reserved[12];
}PRIVACY_MASK_INFO;
#endif
/* PRIVACY_MASK_FEATURE - Dean - 20130801 - End */
/* TAMPER_FEATURE - Dean - 20131007 - Begin */
#if 1 //TAMPER_FEATURE
#define TAMPER_CMD_STR_LEN				256

/* TAMPER_FEATURE - Dean - 20131007 - Begin */
#if 1//TAMPER_FEATURE
#define TAMPER_ENABLE_DEFAULT 0
#define TAMPER_SENSITIVITY_LEVEL_MAX 100
#define TAMPER_SENSITIVITY_DEFAULT 90
#define TAMPER_DURATION_DEFAULT 10
#define TAMPER_TRIGGER_GAP_DEFAULT 5
#define TAMPER_FOCUS_VALUE_DEFAULT 0
#endif
/* TAMPER_FEATURE - Dean - 20131007 - End */

typedef struct
{
	unsigned char		enable;
	unsigned char		sensitivity;
	unsigned char 		reserved2[2];
	unsigned int		detect_duration;
	unsigned int		focus_value;
	char				Reserved[20];
}TAMPER_INFO;
#endif
/* TAMPER_FEATURE - Dean - 20131007 - End */
/* Add for auto port mapping by Joseph 20140212 */
typedef struct
{
	unsigned short		public_http_port;
	unsigned short		public_rtsp_port;
	unsigned short		public_udp_rtsp_port;
	char				Reserved[16];
}AUTO_UPNP_MAPPING_INFO;

typedef struct __FZ_Info
{
	int posIndexOfFocus;
	int posIndexOfZoom;
} FZ_INFO;

#define VER_EXTRA_INFO2 0x00000001
typedef struct __Extra_Info2{
	unsigned long st_version;  // struct version : 0x00000001
	PRIVACY_MASK_INFO 		pm_info;			/* PRIVACY_MASK_FEATURE - Dean - 20130801 */
	TAMPER_INFO 			tamper_info;		/* TAMPER_FEATURE - Dean - 20131007 */
	char reserved[3892]; //[3900]
}Extra_Info2; // total size 4096

typedef struct __Extra_Info{
	char sn[12];
	char pid[12];
	char mac[64];
	char app_version[64];
	char app_version_date[64];
	char boot_version[64];
	char boot_version_date[64];
	char hardware_version[8];
	char system_object_id[64];
	char ipcam_location[64];
	char camera_name[17];
	char ipcam_led;
	NetworkMode			network_mode;
	PPPoEMode			pppoe_mode;
	DDNSMode			ddns_mode;
	BonjourMode			bonjour_mode;
	NotificationMode	notification_mode;
	SIPMODE				sip_mode;
	FTPALARMMODE		ftpalarm_mode;
	IPFILTERMODE		ipfilter_mode;
	HTTPSMODE			https_mode;
	IMAGESETTINGMODE	imageseting_mode;
	GENERAL_NET_MODE	general_net_mode;
	EVENT_SERVER		event_server;
	SEC8021_X			sec8021_x_mode;
	Acount_t			acounts[ACOUNT_NUM+1];
	XML_INFO			xml_info;
	EVENT_SCHEDULE		event_schedule[SCHEDULE_NUM_ADD];
	PTZ_DATA			ptz_data[MAX_PTZ_PRESET];
#if 1 // Onvif_FEATURE
	//alice_modify_onvif_code_2011
	PTZACTPOS			ptzactpos;
	int					ptz_mv_status;
	PresetList			Presetlist;
#endif
	char				ptz_auto_patrol;
	char				remote_ip4_addr[32];
	char				remote_ip6_addr[64];
	char				date_time_mode;
	int					event_schedule_number;
	char				detect_ip[64];
	char				streamformat;
	AudioInfo			audio_info;
	char				global_ptz_patrol;
	char				wizard_flag;
	SMTP_INFO			smtp_info;
	SYSLOG_INFO			syslog_info;
#if UPLOAD_FEATURE
	UPLOAD_RECORD	    upload_record;
#endif
#if IPNC_PELCOD_FEATURE
	RS485				rs485;
#endif	//IPNC_PELCOD_FEATURE
#if IPNC_IRCUT_FEATURE
	IRCUT_INFO			ircut;
#endif 	//IPNC_IRCUT_FEATURE
#if Intelligence_FEATURE
	OBJECTVIDEO			ov;
#endif	//Intelligence_FEATURE
#if WIFI_FEATURE
    WIFI			wifi;
#endif //WIFI_FEATURE
    NET_INFO net_info;
      //added by frank;
    char gpi1;
    char gpi2;
    char gpo;
    char vloss;
//#if FINEST_OEM
	char restart_busy;
//#endif
#if IVS_FEATURE
	IVS_INFO ivs_info;
#endif
#if DIPS_FEATURE				/* DIPS_FEATURE - Dean - 20120530 */
	DIPS_INFO dips_info;		/* DIPS_FEATURE - Dean - 20120530 */
#endif						/* DIPS_FEATURE - Dean - 20120530 */
	unsigned long st_version;  // struct version : 0x00000001
	PRIVACY_MASK_INFO 		pm_info;			/* PRIVACY_MASK_FEATURE - Dean - 20130801 */
	TAMPER_INFO 			tamper_info;		/* TAMPER_FEATURE - Dean - 20131007 */
	FZ_INFO fz_info;
	char reserved[4096];
}Extra_Info;

/* 20140317  - Event Tigger Info - Dean - Begin */
typedef struct
{
	unsigned char	enable;
	char			Reserved0[3];
	int				msg_id;
	int				result;
	unsigned int	file_max_size;
	char			filename[96];
	char			Reserved[12];
}Event_Trigger_Info;
/* 20140317  - Event Tigger Info - Dean - End */
/**
* @brief system info main data structure.
*/
typedef struct SysInfo{
	unsigned long relUniqueId;			///< IPCAM device type
//	unsigned int nprofile;              //number of media profiles
//	unsigned int nrelays;				//number of relay configs
	Acount_t	acounts[ACOUNT_NUM];	///< user account data
	unsigned short DeviceType;			///< IPCAM device type
	Motion_Config_Data motion_config;	///< motion detection configuration data
	Face_Config_Data  face_config;
	Audio_Config_Data audio_config;
	OSD_config osd_config[MAX_OSD_STREAMS];
	CodecAdvParam codec_advconfig[MAX_CODEC_STREAMS];
	CodecROIParam codec_roiconfig[MAX_CODEC_STREAMS];
	StreamParam stream_config[MAX_CODEC_STREAMS];
	Advanced_Mode	advanceMode;
	Ftp_Config_Data ftp_config;			///< IPCAM FQDN
	Smtp_Config_Data smtp_config;		///< ftp configuration data
	Sdcard_Config_Data sdcard_config;	///< SD card configuration data
	Lancam_Config_Data lan_config;		///< IPCAM configuration data
	Ptz_Config_Data ptz_config;			///< PTZ configuration data
	LogEntry_t	tCurLog;				///< event log
#if 0
    Onvif_profile   Oprofile[MAX_PROF];
    Onvif_Scopes    onvif_scopes;
	Onvif_Relay     onvif_relay[MAX_RELAYS];
	Imaging_Conf    imaging_conf[MAX_IMAGING];
    Onvif_Util      onvif_util;
#else
	Onvif_Info	Onvif;
#endif
	DmvaLogEntry_t	tCurDmvaLog;		///< dmva event log
	Dmva_Config_Data  dmva_config;      ///< DMVA configuration data
	__u32 mmcqdPid;                     ///< PID of mmcqd process
	char fqdn[MAX_FQDN_LENGTH];			///< IPCAM FQDN
	char devicename[MAX_STRING_LENGTH];	///< device name
	Event_Trigger_Info event_trigger_info;	/* 20140317  - Event Tigger Info - Dean */
	SAMBA_RECORD		samba_record;
	Extra_Info extrainfo; //Extra_Info
	AUTO_UPNP_MAPPING_INFO  auto_portmapping;  //auto port mapping Joseph 20140212  reserved[3892]
}SysInfo;

#endif /* __SYS_ENV_TYPE_H__ */

#if defined (__cplusplus)
}
#endif
