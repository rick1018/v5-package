#ifndef _AV_SERVER_CONFIG_H_
#define _AV_SERVER_CONFIG_H_

//#include <osa.h>

#define AVSERVER_MAX_STREAMS (4)
#define AVSERVER_MAX_ENCS (3) //RS,DEF
#define AVSERVER_MAX_AU_STREAMS (9) //RS,DEF
#define AVSERVER_CAPTURE_RAW_IN_MODE_ISIF_IN        (0)
#define AVSERVER_CAPTURE_RAW_IN_MODE_DDR_IN         (1)

#define MAX_STRING_LENGTH 22

#define char bool
#define unsigned int Uint32
#define unsigned short Uint16

typedef struct {

	Bool   captureEnable;
	Uint32 samplingRate;
	Uint16 codecType;
	Uint16 codecBiType; //RS ,ADD
	Bool   fileSaveEnable;

} AUDIO_Config;

typedef struct {

	Uint16 captureStreamId;
	Bool   fdEnable;
	Bool   frIdentify;         /* ANR - FR */
	Bool   frRegUsr;
	Bool   frDelUsr;
	Bool   privacyMaskEnable;
	Bool   fdROIEnable;

} VIDEO_FaceDetectConfig;

typedef struct {

	Uint16 captureStreamId;
	Uint16 width;
	Uint16 height;
	Uint16 expandH;
	Uint16 expandV;

} VIDEO_DisplayConfig;

typedef enum{                       //Defined by Rajiv:  SWOSD Test

	SWOSD_BASIC = 0,
	SWOSD_DETAIL,
	SWOSD_COMPLETE

}SWOSD_Type ;

typedef enum{
	SWOSD_FMT_YMD = 1,
	SWOSD_FMT_MDY,
	SWOSD_FMT_DMY
}SWOSD_DATE_FMT;

typedef enum{
	AEWB_NONE = 0,
	AEWB_APPRO,
	AEWB_TI
}AEWB_VENDOR;


typedef struct{

	Bool swosdEnable;
	Bool swosdStringUpdate;
	SWOSD_Type  swosdType;
	SWOSD_DATE_FMT swosdDateFmt;
	char swosdDispString[MAX_STRING_LENGTH];
	Bool swosdTextEnable;
	Bool swosdLogoEnable;
	Bool swosdTimeEnable;
	Bool swosdFREnable[8];   //ANR - FR
	Bool swosdHistEnable;
} VIDEO_SWOSDConfig;   //Defined by Rajiv:  SWOSD Test

typedef struct {

	Uint16 width;
	Uint16 height;
	Bool   ldcEnable;
	Bool   snfEnable;
	Bool   tnfEnable;
	//Bool   vsEnable;

	Bool   flipH;
	Bool   flipV;

	int numEncodes;
	int encodeStreamId[AVSERVER_MAX_STREAMS];

	Uint32 frameSkipMask;

	VIDEO_SWOSDConfig  swosdConfig;       //Defined by Rajiv:  SWOSD Test

} VIDEO_CaptureConfig;

typedef struct {

	Uint16 captureStreamId;
	Uint16 cropWidth;
	Uint16 cropHeight;
	Uint32 frameRateBase;
	Uint32 frameSkipMask;
	Uint16 codecType;
	Uint32 codecBitrate;
	Bool   encryptEnable;
	Bool   fileSaveEnable;
	Bool   motionVectorOutputEnable;
	Int16  qValue;
	Uint16 rateControlType;

} VIDEO_EncodeConfig;

/* Add Extend Structure - Dean - 20130802 - Begin */
typedef struct {
	char encodePreset;	  					/* H264_SVC_FEATURE - Dean - 20130719 */
	char newH264SVCConfig;	  				/* H264_SVC_FEATURE - Dean - 20130719 */
	char numTemporalLayers;   				/* H264_SVC_FEATURE - Dean - 20130719 */
	char svcSyntaxEnable;	  				/* H264_SVC_FEATURE - Dean - 20130719 */
} VIDEO_EncodeConfig_Extend;
/* Add Extend Structure - Dean - 20130802 - End */

typedef struct {

	int mem_layou_set;

} STREAM_Config;

typedef struct {

	int    sensorMode;
	int    sensorFps;

	//Bool   vstabTskEnable;
	Bool   ldcTskEnable;
	Bool   vnfTskEnable;

	Bool   aewbEnable;
	Bool   fbEnable; 
	Uint16 fbRate;    
	int    aewbType;
	int    aewbVendor;
	int    aewb;
	int    gbceEnable;
	int    captureRawInMode;
	Bool   captureSingleResize;

	Bool   privacyMaskEnable;
	int    captureYuvFormat;

	Uint16 numCaptureStream;
	Uint16 numEncodeStream;

	VIDEO_CaptureConfig     captureConfig[AVSERVER_MAX_STREAMS];
	VIDEO_EncodeConfig      encodeConfig[AVSERVER_MAX_STREAMS];

	VIDEO_FaceDetectConfig  faceDetectConfig;

	VIDEO_DisplayConfig     displayConfig;
	AUDIO_Config            audioConfig;
	STREAM_Config			streamConfig;

	Bool winmodeEnable;
	Bool fxdROIEnable;
	Bool vnfDemoEnable;
	Bool newROIconfig;
	int  OSD_startX;  //RS,20091229
	int  OSD_startY;  //RS,20091229
	int  OSD_type;    //RS,20091231,0:CISCO
	int  DateTime_Opaque; //RS,20100225
	int  OSD_TEXT_Opaque; //RS,20100225
	int  OSD_Trans;  //RS,20100225
    Bool   afEnable;
	int    autoFocusVal;  		   

  //Extended,RS
	//int iMainCaptureID; 		/* Remove - Add Extend Structure - Dean - 20130802 */// add by dean huang	- video ivs - 20111209 IVS_FEATURE
	
} AVSERVER_Config;

/* Add Extend Structure - Dean - 20130802 - Begin */
typedef struct {
	VIDEO_EncodeConfig_Extend  encodeConfig[AVSERVER_MAX_STREAMS];
	int  iMainCaptureID;
	char bRunResize;														/* Overlay Image Modification - Dean - 20130731 */
} AVSERVER_Config_Extend;
/* Add Extend Structure - Dean - 20130802 - End */

extern int gAutoIris; // add by kenny chiu 20110617
extern int gWDR; // Cato@20120508
#endif  /*  _AV_SERVER_CONFIG_H_  */

