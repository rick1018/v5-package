/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
* Copyright (c) SMAX Technology Co., Ltd
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#ifndef __IPNC_FEATURE_H__
#define __IPNC_FEATURE_H__
#if defined (__cplusplus)
extern "C" {
#endif

#define DAY_NIGHT_FEATURE 0
#define ROTATION_FEATURE 0
#define FDETECT_FEATURE 0 //RS,20091120,For OV
#define IPMODULE_DEFINE 1  // add by kenny chiu 20110928
#define AWB_CABLICATION   2
#define IRCUT_GIO_GROUP   0 // add by kenny chiu 20111110
#define RESET_GPIO_FUNCTION 1 // add by kenny chiu 20111110
#define DIGITAL_INPUT_NUM 1 // 2 : SIB7000 , 1 else
#define DIGITAL_OUTPUT_NUM 1  // add by kenny chiu 20131217

#define RS485_FEATURE     1
#define AUDIO_FEATURE     1
#define EWDR_FEATURE 0
#define RTC_FEATURE  0

#define MJPG_ADJUSTABLE 0 
#define IPNC_IRDETECT_FEATURE 0 // for SIU7000 and SICPT500

#define TV_OUT_ENABLE 1   // only disable in PT

#define PTZ_ADVANCE_FEATURE 0  // always 0 in 3MP products 20111229
#define IPNC_PELCOD_FEATURE 1

#define AUTOIRIS_RESTART 0

#define VARI_FOCUS_LEN 0

#define DIPS_FEATURE 0		/* DIPS_FEATURE - Dean - 20120530 */
#define IVS_FEATURE 0		// add by dean huang - video ivs - 20111209
#define REC_AVI_AUDIO 1		/* AVI add Audio - Dean - 20120831 */
#define ROI_FEATURE 0
#define FOCUS_ZOOM_FEATURE 		0
#define SD_FEATURE  1		// add by dean huang - SD Card - 20120209
#define SDCARD_EJECT_ENABLE	1
#define PRIVACY_MASK_FEATURE	1 /* PRIVACY_MASK_FEATURE - Dean - 20130801 */
#define UPLOAD_FEATURE 0

#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#define HTTP_NOTIFY_CONFIG_FILE 1	/* Http Event Notify Config - Dean - 20121101 */

#define WDR_FEATURE 0
#define AUTOIRIS_RESTART 0

#define MJPG_DISABLE 0
#define Onvif_FEATURE 1

#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)
#define WIFI_ChannelEn 0 //alice_modify_2012_6_29_v1
#define WIFI_FEATURE 1

#define WIFI_DRV_VER_RT3070     1

#define AGC_FEATURE             0
#define FLICKERLESS_FEATURE     0
#define MANUAL_SHUTTER_FEATURE  0
#define DNR_FEATURE        0
#define COLORBAR_FEATURE        0
#define SLOW_SHUTTER_FEATURE        0
#define SENSUP_FEATURE        0
#define BLC_FEATURE        0
#define GAMMA_FEATURE        0

#define AP1512_FEATURE     0
#define EDIMAX_CLOUD_FEATURE 0

#define DIGITAL_INPUT_TYPE 2 // 0 : for old modal like SIB7000..., 1 : for new modal like SIU7110A ; add by Kenny Chiu 20130412
#define NOT_SUPPORT_IP6_TABLES	1  // add by kenny chiu 20121226

#define TR069_FEATURE 0     //add by Joseph 20131031
#define AUTO_PORTMAPPING_FEATURE   0     //add by Joseph 20140213
#define GOP_FEATURE   0     //add by Joseph 20131119
#define EXPOSURE_FEATURE        3 // bit 0 , day , bit 1 : night

#define MODAL_PT 	   0

#define MODAL_SIB7000  0

#define MODAL_SIU7110  0
#define MODAL_SIU7120  0
#define MODAL_SIFD7130 0

// Sensor AR0331 start
#define MODAL_SIB7137  0
#define MODAL_SIU7117  0
#define MODAL_SIU7127  0
#define MODAL_SIU7137  0
#define MODAL_SIVD7137 0
#define MODAL_SIVD7137E 0
// Sensor AR0331 end

// Sensor MT9P006 start
#define MODAL_SIU7110A 0

// new modal with USB
#define MODAL_SICPT501U 0
// Sensor MT9P006 end

#define MODAL_SIMB8040 0

#define MODAL_SIMB8303 0
#define MODAL_SIU7323  0	
#define MODAL_SIB7323  0	

#define MODAL_SIU5110  0
#define MODAL_SIU5130  0
#define MODAL_SIFD5130 0

// for SURPEDIA Sample
#define MODAL_SIU6110  0
#define MODAL_SIU6120  0

#define MODAL_BU3100  0
#define MODAL_VD3100  0
#define MODAL_FD3000  0

#define MODAL_AE1  0

#define MODAL_AU3  1

#if MODAL_PT  // 1 : meas Smax PT, 0 : otherwise
#if WIFI_FEATURE
#define MAIN_MODEL "SICPT500W"
#define MODEL_NAME "5MP_IP_PTW"
#else
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "5MP_IP_PT"
#endif
//#define IMGS_MICRON_MT9P031
#undef AGC_FEATURE
#define AGC_FEATURE             0
#undef FLICKERLESS_FEATURE
#define FLICKERLESS_FEATURE     0
#undef MANUAL_SHUTTER_FEATURE
#define MANUAL_SHUTTER_FEATURE  0
#undef COLORBAR_FEATURE
#define COLORBAR_FEATURE        0
#define AP1512_FEATURE        0

#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define PT_ENABLE 1
#define IPNCPTZ_FEATURE 1
#define PTZ_FEATURE 1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   0
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1

#undef IPNC_IRDETECT_FEATURE  // add by kenny chiu 20110928
#define IPNC_IRDETECT_FEATURE 1 // for SIU7000 and SICPT500
#undef VARI_FOCUS_LEN  // add by kenny chiu 20110928
#define VARI_FOCUS_LEN 0
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928//
#define DIGITAL_INPUT_NUM 2
#undef IPNC_IRCUT_FEATURE  // add by kenny chiu 20110928//
#define IPNC_IRCUT_FEATURE 0
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IPNC_ENVIRONMENT  // add by kenny chiu 20110928//
#define IPNC_ENVIRONMENT   0
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#elif MODAL_SICPT501U
#define MAIN_MODEL "SICPT501U"
#define MODEL_NAME "SICPT501U"
//#define IMGS_MICRON_MT9P031
#define MT9P006

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define PT_ENABLE 1
#define IPNCPTZ_FEATURE 1
#define PTZ_FEATURE 1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   2
//#define MODAL_PT 1
#undef IPNC_IRDETECT_FEATURE  // add by kenny chiu 20110928
#define IPNC_IRDETECT_FEATURE 0 // for SIU7000 and SICPT500
#undef VARI_FOCUS_LEN  // add by kenny chiu 20110928
#define VARI_FOCUS_LEN 0
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928//
#define DIGITAL_INPUT_NUM 2
#define IPNC_IRCUT_FEATURE 1
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IPNC_ENVIRONMENT  // add by kenny chiu 20110928//
#define IPNC_ENVIRONMENT   0
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef WIFI_FEATURE
#define WIFI_FEATURE 0

#undef WDR_FEATURE
#define WDR_FEATURE 1
#undef AGC_FEATURE
#define AGC_FEATURE             0
#undef FLICKERLESS_FEATURE
#define FLICKERLESS_FEATURE     0
#undef MANUAL_SHUTTER_FEATURE
#define MANUAL_SHUTTER_FEATURE  0
#undef COLORBAR_FEATURE
#define COLORBAR_FEATURE        0

#else  // none PT series

#define PT_ENABLE 0
#define IPNCPTZ_FEATURE 0
#define PTZ_FEATURE 0
#define IPNC_IRCUT_FEATURE 1
#undef IPNC_IRDETECT_FEATURE  // add by kenny chiu 20110928
#define IPNC_IRDETECT_FEATURE 0 // for SIU7000 and SICPT500

#undef WIFI_FEATURE  // add by kenny chiu 20110928
#define WIFI_FEATURE 0

#if MODAL_SIB7000  // only for SIB7000
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "SIB7000"
//#define IMGS_MICRON_MT9P031

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#define IPNC_AUTO_IRIS     1 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 2
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

/***** MODAL SIB7323 Define */
#elif MODAL_SIB7323  // SIB7323(Box ITN)
#define MAIN_MODEL "SIB7323"
#define MODEL_NAME "SIB7323"
#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#define IPNC_AUTO_IRIS     1 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   2
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 2
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0
#undef IRCUT_RESET
#define IRCUT_RESET 0
#undef VARI_FOCUS_LEN  // add by kenny chiu 20110928
#define VARI_FOCUS_LEN     1
#undef WDR_FEATURE
#define WDR_FEATURE 1
#undef MANUAL_GAIN_FEATURE
#define MANUAL_GAIN_FEATURE			1				/* ITN - Dean - 20120504*/
#undef AGC_FEATURE
#define AGC_FEATURE					1				/* ITN - Dean - 20120504*/
#undef DNR_FEATURE
#define DNR_FEATURE     1
#undef FLICKERLESS_FEATURE
#define FLICKERLESS_FEATURE     1
#undef MANUAL_SHUTTER_FEATURE
#define MANUAL_SHUTTER_FEATURE  1
#undef SLOW_SHUTTER_FEATURE
#define SLOW_SHUTTER_FEATURE    1
#undef SENSUP_FEATURE
#define SENSUP_FEATURE  1
#undef BLC_FEATURE
#define BLC_FEATURE     1
#undef COLORBAR_FEATURE
#define COLORBAR_FEATURE     1
#undef GAMMA_FEATURE
#define GAMMA_FEATURE     1
/***** MODAL SIU7137 Define */
#elif MODAL_SIU7137
#define MAIN_MODEL "SIU7137"
#define MODEL_NAME "SIU7137"
//#define IMGS_AR0331

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#undef IPNC_AUTO_IRIS
#define IPNC_AUTO_IRIS     1 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef AUTOIRIS_RESTART
#define AUTOIRIS_RESTART 1
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 2
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0
#undef WDR_FEATURE
#define WDR_FEATURE 1

/***** MODAL SIU7110 Define */
#elif  MODAL_SIU7110 // MODAL_SIU7110 
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "SIU7110"
//#define IMGS_MICRON_MT9P031

#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   0
#undef AUDIO_FEATURE
#define AUDIO_FEATURE 1 
//#undef MANUAL_SHUTTER_FEATURE
//#define MANUAL_SHUTTER_FEATURE  1
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

/***** MODAL SIU7117 Define */
#elif  MODAL_SIU7117
#define MAIN_MODEL "SIU7117"
#define MODEL_NAME "SIU7117"
//#define IMGS_AR0331

#define IPNC_IRCUT_FEATURE 1 // 1 : only for SIB7000,  0 : else
#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 1   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   2
#undef IPNC_AUTO_IRIS
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 1
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0
#undef WDR_FEATURE
#define WDR_FEATURE 1
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 2

/***** MODAL SIU7120 Define */
#elif  MODAL_SIU7120
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "SIU7120"
//#define IMGS_MICRON_MT9P031

#define IPNC_IRCUT_FEATURE 1 // 1 : only for SIB7000,  0 : else
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   0
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

/***** MODAL SIU5110 Define */
#elif MODAL_SIU5110
#define MAIN_MODEL "SIU5110"
#define MODEL_NAME "SIU5110"
//#define IMGS_MICRON_MT9P031

#undef MAX_RES_2M  // add by kenny chiu 20110928
#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 0
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 1

#undef RS485_FEATURE  // add by kenny chiu 20120803
#define RS485_FEATURE 0
#undef AUDIO_FEATURE  // add by kenny chiu 20120803
#define AUDIO_FEATURE 0
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

//***** MODAL SIU5130 Define //*/
#elif MODAL_SIU5130
#define MAIN_MODEL "SIU5130"
#define MODEL_NAME "SIU5130"
//#define IMGS_MICRON_MT9P031
#define MT9P006

#undef MAX_RES_2M  // add by kenny chiu 20110928
#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   2
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 0
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 0

#undef RS485_FEATURE  // add by kenny chiu 20120803
#define RS485_FEATURE 0
#undef AUDIO_FEATURE  // add by kenny chiu 20120803
#define AUDIO_FEATURE 0
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

//***** MODAL SIFD5130 Define //*/
#elif MODAL_SIFD5130
#define MAIN_MODEL "SIFD5130"
#define MODEL_NAME "SIFD5130"
//#define IMGS_MICRON_MT9P031
#define MT9P006

#undef MAX_RES_2M  // add by kenny chiu 20110928
#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   0 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   2
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 0
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0
#undef IPNC_IRCUT_FEATURE
#define IPNC_IRCUT_FEATURE 0 // 1 : only for SIB7000,  0 : else

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 0

#undef RS485_FEATURE  // add by kenny chiu 20120803
#define RS485_FEATURE 0
#undef AUDIO_FEATURE  // add by kenny chiu 20120803
#define AUDIO_FEATURE 0
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

//***** MODAL SIFD7130 Define //*/
#elif  MODAL_SIFD7130
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "SIFD7130"
//#define IMGS_MICRON_MT9P031

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#define IPNC_AUTO_IRIS     1 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 2
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 1
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 1

#undef WDR_FEATURE
#define WDR_FEATURE 1

/***** MODAL SIVD7137 Define */
#elif  MODAL_SIVD7137
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "SIVD7137"
//#define IMGS_AR0331

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef IPNC_AUTO_IRIS
#define IPNC_AUTO_IRIS     0// 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 2
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 1

#undef WDR_FEATURE
#define WDR_FEATURE 1
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 2

/***** MODAL SIVD7137E Define */
#elif  MODAL_SIVD7137E
#define MAIN_MODEL "SIVD7137E"
#define MODEL_NAME "SIVD7137E"
//#define IMGS_AR0331

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef IPNC_AUTO_IRIS
#define IPNC_AUTO_IRIS    1 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   2
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 2
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AP1512_FEATURE
#define AP1512_FEATURE  1

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 1

#undef WDR_FEATURE
#define WDR_FEATURE 1


/***** MODAL SIB7137 Define */
#elif  MODAL_SIB7137
#define MAIN_MODEL "SIB7137"
#define MODEL_NAME "SIB7137"
//#define IMGS_AR0331

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef IPNC_AUTO_IRIS
#define IPNC_AUTO_IRIS    1 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   2
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 2
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AP1512_FEATURE
#define AP1512_FEATURE  1

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 1

#undef WDR_FEATURE
#define WDR_FEATURE 1

/***** MODAL SIU7127 Define */
#elif  MODAL_SIU7127
#define MAIN_MODEL "SIU7127"
#define MODEL_NAME "SIU7127"

//#define IMGS_AR0331

#define IPNC_IRCUT_FEATURE 1 // 1 : only for SIB7000,  0 : else
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   0
#undef WDR_FEATURE
#define WDR_FEATURE 1
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0
/***** MODAL SIU7110A Define */
#elif  MODAL_SIU7110A
#define MAIN_MODEL "SIU7110A"
#define MODEL_NAME "SIU7110A"
//#define IMGS_MICRON_MT9P031
#define MT9P006

#define IPNC_IRCUT_FEATURE 1 // 1 : only for SIB7000,  0 : else
#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 1   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   2
#undef IPNC_AUTO_IRIS
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 1
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 2

/***** MODAL SIMB8303 Define */
#elif MODAL_SIMB8303  // SIMB8303(ITN)
//#define MAIN_MODEL "SIMB8303"
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "SIMB8303"
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#define IPNC_AUTO_IRIS     1 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   0
#undef IRCUT_RESET
#define IRCUT_RESET 0
#undef VARI_FOCUS_LEN  // add by kenny chiu 20110928
#define VARI_FOCUS_LEN     1
#undef WDR_FEATURE
#define WDR_FEATURE 1
#undef MANUAL_GAIN_FEATURE
#define MANUAL_GAIN_FEATURE			1				/* ITN - Dean - 20120504*/
#undef AGC_FEATURE
#define AGC_FEATURE					1				/* ITN - Dean - 20120504*/
#undef DNR_FEATURE
#define DNR_FEATURE     1
#undef FLICKERLESS_FEATURE
#define FLICKERLESS_FEATURE     1
#undef MANUAL_SHUTTER_FEATURE
#define MANUAL_SHUTTER_FEATURE  1
#undef SLOW_SHUTTER_FEATURE
#define SLOW_SHUTTER_FEATURE    1
#undef SENSUP_FEATURE
#define SENSUP_FEATURE  1
#undef BLC_FEATURE
#define BLC_FEATURE     1
#undef COLORBAR_FEATURE
#define COLORBAR_FEATURE     1
#undef GAMMA_FEATURE
#define GAMMA_FEATURE     1

/***** MODAL SIU7323 Define */
#elif MODAL_SIU7323  // SIU7323(Bullet ITN)
#define MAIN_MODEL "SIU7323"
#define MODEL_NAME "SIU7323"
#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#define IPNC_AUTO_IRIS     1 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   3
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 1
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0
#undef IRCUT_RESET
#define IRCUT_RESET 0
#undef VARI_FOCUS_LEN  // add by kenny chiu 20110928
#define VARI_FOCUS_LEN     1
#undef WDR_FEATURE
#define WDR_FEATURE 1
#undef MANUAL_GAIN_FEATURE
#define MANUAL_GAIN_FEATURE			1				/* ITN - Dean - 20120504*/
#undef AGC_FEATURE
#define AGC_FEATURE					1				/* ITN - Dean - 20120504*/
#undef DNR_FEATURE
#define DNR_FEATURE     1
#undef FLICKERLESS_FEATURE
#define FLICKERLESS_FEATURE     1
#undef MANUAL_SHUTTER_FEATURE
#define MANUAL_SHUTTER_FEATURE  1
#undef SLOW_SHUTTER_FEATURE
#define SLOW_SHUTTER_FEATURE    1
#undef SENSUP_FEATURE
#define SENSUP_FEATURE  1
#undef BLC_FEATURE
#define BLC_FEATURE     1
#undef COLORBAR_FEATURE
#define COLORBAR_FEATURE     1
#undef GAMMA_FEATURE
#define GAMMA_FEATURE     1

/***** MODAL SIMB8040 Define */
#elif MODAL_SIMB8040  // SIU7100 with IRCUT
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "SIMB8040"

//#define IMGS_MICRON_MT9P031
//#define MT9P006

#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   0
#undef VARI_FOCUS_LEN  // add by kenny chiu 20110928
#define VARI_FOCUS_LEN     1
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

/***** MODAL SIU6110 Define */
#elif MODAL_SIU6110  // SIU7100 with IRCUT
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "SIU6110"

//#define IMGS_MICRON_MT9P031

#undef IPNC_IRCUT_FEATURE
#define IPNC_IRCUT_FEATURE 0 // 1 : only for SIB7000,  0 : else
#undef IPNC_ENVIRONMENT
#define IPNC_ENVIRONMENT   0 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   0
#undef IPNC_AUTO_IRIS
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527

#undef MAX_RES_2M  // add by kenny chiu 20110928
#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)

#define DIGITAL_INPUT_NUM 0

#undef RS485_FEATURE  // add by kenny chiu 20120803
#define RS485_FEATURE 0
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

/***** MODAL SIU6120 Define */
#elif MODAL_SIU6120  // SIU7100 with IRCUT
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "SIU6120"

//#define IMGS_MICRON_MT9P031

#undef IPNC_IRCUT_FEATURE
#define IPNC_IRCUT_FEATURE 0 // 1 : only for SIB7000,  0 : else
#undef IPNC_ENVIRONMENT
#define IPNC_ENVIRONMENT   0 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   0
#undef IPNC_AUTO_IRIS
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527

#undef MAX_RES_2M  // add by kenny chiu 20110928
#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)

#define DIGITAL_INPUT_NUM 1

#undef AUDIO_FEATURE  // add by kenny chiu 20120803
#define AUDIO_FEATURE 0
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

#elif MODAL_BU3100
#define MAIN_MODEL "BU3100"
#define MODEL_NAME "BU3100"
//#define IMGS_MICRON_MT9P031

#undef MAX_RES_2M  // add by kenny chiu 20110928
#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)

#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 0
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 1

#undef RS485_FEATURE  // add by kenny chiu 20120803
#define RS485_FEATURE 0
#undef AUDIO_FEATURE  // add by kenny chiu 20120803
#define AUDIO_FEATURE 0
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

#elif MODAL_VD3100 
#define MAIN_MODEL "VD3100"
#define MODEL_NAME "VD3100"
//#define IMGS_MICRON_MT9P031

#undef MAX_RES_2M  // add by kenny chiu 20110928
#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)

#undef DAY_NIGHT_FEATURE  
#define DAY_NIGHT_FEATURE 1
#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 1
#undef DIGITAL_OUTPUT_NUM
#define DIGITAL_OUTPUT_NUM 1  // add by kenny chiu 20131217
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 1

#undef RS485_FEATURE  // add by kenny chiu 20120803
#define RS485_FEATURE 0
#undef AUDIO_FEATURE  // add by kenny chiu 20120803
#define AUDIO_FEATURE 3
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

#undef SDCARD_EJECT_ENABLE
#define SDCARD_EJECT_ENABLE	0

#elif MODAL_FD3000
#define MAIN_MODEL "FD3000"
#define MODEL_NAME "FD3000"
//#define IMGS_MICRON_MT9P031

#undef MAX_RES_2M  // add by kenny chiu 20110928
#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)

#undef IPNC_IRCUT_FEATURE
#define IPNC_IRCUT_FEATURE 0 // 1 : only for SIB7000,  0 : else
#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   1 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   1
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 1
#undef DIGITAL_OUTPUT_NUM
#define DIGITAL_OUTPUT_NUM 0  // add by kenny chiu 20131217
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 1

#undef RS485_FEATURE  // add by kenny chiu 20120803
#define RS485_FEATURE 0
#undef AUDIO_FEATURE  // add by kenny chiu 20120803
#define AUDIO_FEATURE 1

#undef SDCARD_EJECT_ENABLE
#define SDCARD_EJECT_ENABLE	1
#undef EXPOSURE_FEATURE
#define EXPOSURE_FEATURE 1

#elif MODAL_AE1
#define MAIN_MODEL "AE1"
#define MODEL_NAME "AE1"

#undef MAX_RES_2M  // add by kenny chiu 20110928
#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)

#undef EDIMAX_CLOUD_DEVICE_ID
#define EDIMAX_CLOUD_DEVICE_ID 37

#undef PT_ENABLE 
#define PT_ENABLE 1
#undef IPNCPTZ_FEATURE
#define IPNCPTZ_FEATURE 1
//#define PTZ_FEATURE 1
#define ZOOM_FEATURE  1

#undef WIFI_FEATURE  // add by kenny chiu 20110928
#define WIFI_FEATURE 0
#undef DAY_NIGHT_FEATURE  
#define DAY_NIGHT_FEATURE 0
#undef IPNC_IRCUT_FEATURE
#define IPNC_IRCUT_FEATURE 0 
#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   0 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   0
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 0
#undef DIGITAL_OUTPUT_NUM
#define DIGITAL_OUTPUT_NUM 0  // add by kenny chiu 20131217
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 1

#undef RS485_FEATURE  // add by kenny chiu 20120803
#define RS485_FEATURE 0
#undef AUDIO_FEATURE  // add by kenny chiu 20120803
#define AUDIO_FEATURE 1
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

#undef SDCARD_EJECT_ENABLE
#define SDCARD_EJECT_ENABLE	1
#undef EWDR_FEATURE
#define EWDR_FEATURE 1

#elif MODAL_AU3
#define MAIN_MODEL "AU3"
#define MODEL_NAME "AU3"

#undef MAX_RES_2M  // add by kenny chiu 20110928
#define MAX_RES_2M 1 //, 0 --> 3M(DM368),  1 --> 2M(DM365)

#undef EDIMAX_CLOUD_DEVICE_ID
#define EDIMAX_CLOUD_DEVICE_ID 37

#undef PT_ENABLE 
#define PT_ENABLE 1
#undef IPNCPTZ_FEATURE
#define IPNCPTZ_FEATURE 1
//#define PTZ_FEATURE 1
#define ZOOM_FEATURE  1

#undef WIFI_FEATURE  // add by kenny chiu 20110928
#define WIFI_FEATURE 0
#undef DAY_NIGHT_FEATURE  
#define DAY_NIGHT_FEATURE 0
#undef IPNC_IRCUT_FEATURE
#define IPNC_IRCUT_FEATURE 0 
#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 0
#define IPNC_ENVIRONMENT   0 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef AWB_CABLICATION  // add by kenny chiu 20110928
#define AWB_CABLICATION	   0
#undef TV_OUT_ENABLE  // add by kenny chiu 20110928
#define TV_OUT_ENABLE 0   // only enable when model is SIB7000 and SIMB8000 add by kenny 20110613
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#undef IRCUT_GIO_GROUP  // add by kenny chiu 20110928
#define IRCUT_GIO_GROUP   1
#undef DIGITAL_INPUT_NUM  // add by kenny chiu 20110928
#define DIGITAL_INPUT_NUM 0
#undef DIGITAL_OUTPUT_NUM
#define DIGITAL_OUTPUT_NUM 0  // add by kenny chiu 20131217
#undef RESET_GPIO_FUNCTION  // add by kenny chiu 20110928
#define RESET_GPIO_FUNCTION 0

#undef AUTOIRIS_RESTART  // add by kenny chiu 20110928
#define AUTOIRIS_RESTART 1

#undef RS485_FEATURE  // add by kenny chiu 20120803
#define RS485_FEATURE 0
#undef AUDIO_FEATURE  // add by kenny chiu 20120803
#define AUDIO_FEATURE 1
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

#undef SDCARD_EJECT_ENABLE
#define SDCARD_EJECT_ENABLE	1
#undef EWDR_FEATURE
#define EWDR_FEATURE 1

#else  // !MODAL_SIMB8040
#define MAIN_MODEL "SIB7000"
#define MODEL_NAME "SIMB8000"

//#define IMGS_MICRON_MT9P031
#undef IPMODULE_DEFINE  // add by kenny chiu 20110928
#define IPMODULE_DEFINE 1

#undef IPNC_IRCUT_FEATURE
#define IPNC_IRCUT_FEATURE 0 // 1 : only for SIB7000,  0 : else
#undef IPNC_ENVIRONMENT
#define IPNC_ENVIRONMENT   0 // 1 : with condiction, 0 : else  add by kenny 20110527
#undef IPNC_AUTO_IRIS
#define IPNC_AUTO_IRIS     0 // 1 : with auto iris, 0 : else  add by kenny 20110527
#define AGC_FEATURE             0
#undef FLICKERLESS_FEATURE
#define FLICKERLESS_FEATURE     0
#undef MANUAL_SHUTTER_FEATURE
#define MANUAL_SHUTTER_FEATURE  0
#undef COLORBAR_FEATURE
#define COLORBAR_FEATURE        0
#undef DIGITAL_INPUT_TYPE
#define DIGITAL_INPUT_TYPE 0

#endif  // endif MODAL_SIB7000

#endif  // endif MODAL_PT

#define IPNC_IRDETECT_MODE  0 // 0 for SIU7000B and 1 for SIMB8100
#define SD_PLAYBACK 0 // 0 : w/o playback, 1 : with media player, 2:  only download :  // not used now : replace by para_sdcard 20120510

#define VIDSTB_FEATURE 0
#define BINNING_FEATURE 0
#define UpnpPort_FEATURE 0
//#define BACKLIGHT_FEATURE 0
#define ECHO_FEATURE 0
#define DISCARD_FEATURE 0
#define INTELLIGENT_FEATURE 1 //RS,20100416,Dynamic Video Quality
#define AEWVendor 2  //RS, APPRO :1 , TI :2, RS :3
#define UPNP_TMP_DIR 1   //by Larry
#define TV_OUT_FEATURE 0 //by Larry
#define MOTION_1_99_MODE_OFF 1 //0: ON, 1:OFF
#define IPMODULE_FOR_CMOS 1 //Frank, need set GPIO38 to reset LOW voltage
//
// modify by kenny chiu 20110520
#define JPEG_VERY_HIGH	80 // 90 //60
#define JPEG_HIGH		70 // 80 //50
#define JPEG_NORMAL		60 // 70 //40
#define JPEG_LOW		50 // 60 //30
#define JPEG_VERY_LOW	40 // 50 //20

#define MPEG_VERY_HIGH	4
#define MPEG_HIGH		5
#define MPEG_NORMAL		6
#define MPEG_LOW		8
#define MPEG_VERY_LOW	12//10
#if (MAX_RES_2M == 1)  // add by kenny chiu 20130725
#define MAX_CBR_BITRATE 4000
#else
#define MAX_CBR_BITRATE 6000
#endif
//#define DEFINE_TVS  1
//#define DEFINE_FITI 0
#define DEFINE_801_1X 0
#define BP 66
#define MP 77
#define HP 100
#if MODAL_SIMB8040
#define PROFILE_USED MP// HP //MP  //RS,20100707
#else
#define PROFILE_USED HP //MP  //RS,20100707
#endif
//#define CVBS_OUT 1 //RS,20100708,New Taipei City

#define DEF_NEW_TAIPEI_CITY 1
#if defined (__cplusplus)
}
#endif
#endif

