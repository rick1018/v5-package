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
#ifndef __IPNC_VER_H__
#define __IPNC_VER_H__
#if defined (__cplusplus)
extern "C" {
#endif

//#define DM368_APP_VERSION "DM368 IPNC REF DESIGN VERSION 3.00.00.10"
//#define DM365_APP_VERSION "DM365 IPNC REF DESIGN VERSION 3.00.00.10"
#include "svn_ver.h"

#define REL_UNIQUE_ID 	(0x310)

#include "sys_env_type.h"
//#include "system_default.h" /* Modify DDNS Provider & Add DDNS Provider Planet - Compile - Remove - 20120412 */
#include "ipnc_feature.h"
#if IPNC_PELCOD_FEATURE
#include "pelcod_util.h"
#endif
//#include "patrol_util.h"
#define FFLAG_LDC	0x02
#define FFLAG_SNF	0x04
#define FFLAG_TNF	0x08
#define OVERLAY_DEFAULT_SIZE "160|64"
#define MOTION_TRIGGER_PERIOD 5 // 2 /* 20 *//* Modify Event - 20120620 - Dean */

#define KERNEL_COMMOM_BUILD_VERSION "2.6.36.x"
#define KERNEL_DEF_VERSION "1.00.00" 

#define SYS_DEF_VERSION "1.00.00" 

#define Onvif_Resoultion 1

#define NPAPI_VERSION		"1.0.0.15"			/* Dean - 20140317 20140515 */ 

#define ONVIF_TESTTOOL_VER	"13.12"

// defined for Model and Version control start
enum _modelNum
{
	SVS100=0x0001,
	SIMB7000,
	SIB7000=0x0101,
	SIMB8000
};
// defined for Model and Version control end
//#if WIFI_FEATURE
//#define MAIN_MODEL "SICPT500W"
//#else
//#define MAIN_MODEL "SIB7000"
//#endif
// =======================
// model area start : add by kenny chiu 20110520
// =======================
// No Brand Area
#undef MODEL_NAME
#if MODAL_PT
#if WIFI_FEATURE
#define MODEL_NAME "5MP_IP_PTW"
#else
#define MODEL_NAME "5MP_IP_PT"
#endif // end of if WIFI_FEATURE
#elif MODAL_SICPT501U
#define MODEL_NAME "SICPT501U"
#elif MODAL_SIB7000
#define MODEL_NAME "5MP_IP_BOX"
#elif MODAL_SIU7110
#define MODEL_NAME "5MP_BULLET_10"
#elif MODAL_SIU7120
#define MODEL_NAME "5MP_BULLET_20"
#elif MODAL_SIU7130
#define MODEL_NAME "5MP_BULLET_30"
#elif MODAL_SIFD7130
#define MODEL_NAME "3MP_IP_FD"    // SIFD7130
#elif MODAL_SIVD7137
#define MODEL_NAME "3MP_IP_VD"    // SIVD7137
#elif MODAL_SIVD7137E
#define MODEL_NAME "3MP_IP_VD"    // SIVD7137E
#elif MODAL_SIU5110
#define MODEL_NAME "2MP_BULLET_10"  // SIU5110
#elif MODAL_BU3100
#define MODEL_NAME "2MP_BULLET_11"  // BU3100
#elif MODAL_VD3100
#define MODEL_NAME "2MP_BULLET_11"  // VD3100
#elif MODAL_FD3000
#define MODEL_NAME "2MP_BULLET_11"  // FD3000
#elif MODAL_SIU7117
#define MODEL_NAME "5MP_BULLET_17"  // SIU7117
#elif MODAL_SIU7127
#define MODEL_NAME "5MP_BULLET_27"  // SIU7127
#else
#define MODEL_NAME "5MP_IP_M"
#endif



//#define MODEL_NAME "5MP_BULLET_40"
//#define MODEL_NAME "3MP_IP_BOX"    // SIB7000

//#define MODEL_NAME "SIU7100"
//#define MODEL_NAME "SIU7100G"
//#define MODEL_NAME "SIU7110"
//#define MODEL_NAME "SIU7117"
//#define MODEL_NAME "SIU7120"
//#define MODEL_NAME "SIU7127"
//#define MODEL_NAME "SIU7130"
//#define MODEL_NAME "SIU7137"
//#define MODEL_NAME "SIVD7137"
//#define MODEL_NAME "SIMB8040"

#define MANUFACTURER_DEFAULT "N/A"
#define HARDWARE_ID_DEFAULT  "0000-00"

//#define DEFAULT_LOGO "img/logo.gif"
//#define DEFAULT_LOGO "img/logo_atek.gif"

#define DEFAULT_LOGO	"img/logo_none.gif"
#define BRAND_URL		""
#define COMPANY_NAME	""
#define CAB_FILE_NAME	"SStreamVideo.cab"
#define BRAND_NAME		""

#define CAB_FILE_VERSION "1.0.0.15"

#define SEC_8021X_PATH  "/opt/wpa_supplicant"//charles lin wifi
#define VIR_8021X_PATH  "/var/run/wpa_supplicant"

#define JEFF_OEM 0
// OEM 參數設定專區 Start
#define SMAX_BRAND 1
#define EDIMAX_OEM 0
#define PROVIDEO_OEM 0
#define IVIEW_OEM 0
#define COP_OEM 0

#define FINEST_OEM 0
#define DIGISOL_OEM 0
#define PLANET_OEM 0
#define LIMIX_OEM 0
#define FUHO_OEM 0
#define LOGILINK_OEM 0
#define WEGIN_OEM 0
#define AIRLIVE_OEM 0
#define NEOTECH_OEM 0
#define ALPHA_OEM 0
#define HENTEK_OEM 0
#define HIQVIEW_OEM 0
#define UDM_OEM 0
#define FITI_OEM 0
#define SURPEDIA_OEM 0
#define TANTOS_OEM 0
#define ANTEI_OEM 0
#define HUNTCCTV_OEM 0
#define PCI_OEM 0

#define DDNS_PROVIDER_LIST_DEFAULT	/* Modify DDNS Provider & Add DDNS Provider Planet - Dean - 20120411*/
#define FIRMWARE_VERSION	SYS_DEF_VERSION // ""			/* Modify DDNS Provider & Add DDNS Provider Planet - Dean - 20120416*/
#define EVENT_HTTP_NOTIFY_STR	"Event Notify"	/* Modify Event - 20120620 - Dean */

#define MAX_PRE_ALARM_TIME  5
#define MAX_POST_ALARM_TIME 5

#if SMAX_BRAND  // for SMAX Brand專用
#undef BRAND_NAME
#define BRAND_NAME		"SMAX"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "SMAX"

#define EDIMAX_CLOUD_VENDOR_ID 37

#undef MJPG_DISABLE
#define MJPG_DISABLE 0
#undef EDIMAX_CLOUD_FEATURE
#define EDIMAX_CLOUD_FEATURE 0

#undef MODEL_NAME
#if MODAL_SIB7000
#define MODEL_NAME "SIB7000"	// Smax SIB7000
#elif MODAL_SIB7137
#define MODEL_NAME "SIB7137"	// Smax SIB7137
#elif MODAL_SIU7110
#define MODEL_NAME "SIU7110"	// Smax SIU7110
#elif MODAL_SIU7110A
#define MODEL_NAME "SIU7110A"	// Smax SIU7110S
#elif MODAL_SIU7120
#define MODEL_NAME "SIU7120"    // Smax SIU7120
#elif MODAL_PT
#if WIFI_FEATURE
#define MODEL_NAME "SICPT500W"	// Smax 3M PT wifi
#else
#define MODEL_NAME "SICPT500"	// Smax 3M PT w/o POE
#endif
#elif MODAL_SICPT501U
#define MODEL_NAME "SICPT501U"
#elif MODAL_SIU5110
#define MODEL_NAME "SIU5110"    // Smax SIU5110
#elif MODAL_SIU5130
#define MODEL_NAME "SIU5130"    // Smax SIU5130
#elif MODAL_SIFD5130
#define MODEL_NAME "SIFD5130"    // Smax SIFD5130
#elif MODAL_BU3100
#define MODEL_NAME "BU3100"  // BU3100
#elif MODAL_VD3100
#define MODEL_NAME "VD3100"  // BU3100
#elif MODAL_FD3000
#define MODEL_NAME "FD3000"  // BU3100
#elif MODAL_SIFD7130
#define MODEL_NAME "SIFD7130"    // Smax SIFD7130
#elif MODAL_SIU7323
	#define MODEL_NAME "SIU7323" // smax SIU7323
#elif MODAL_SIB7323
	#define MODEL_NAME "SIB7323" // smax SIB7323
#elif MODAL_SIMB8303
#define MODEL_NAME "SIM8303"    // Smax ITN
#elif MODAL_SIU7117
#define MODEL_NAME "SIU7117"    // Smax SIU7117
#elif MODAL_SIU7127
#define MODEL_NAME "SIU7127"    // Smax SIU7127
#elif MODAL_SIVD7137
#define MODEL_NAME "SIVD7137"    // Smax SIVD7137
#elif MODAL_SIVD7137E
#define MODEL_NAME "SIVD7137E"    // Smax SIVD7137E
#elif MODAL_AE1
#define MODEL_NAME "AE1"    // Smax MT7620(AE1)
#elif MODAL_AU3
#define MODEL_NAME "VCT_S900"    // Smax MT7628(AU3)
#endif

#undef BRAND_URL
#define BRAND_URL "www.smax.com.tw"

#undef COMPANY_NAME
#define COMPANY_NAME "SMAX Technology CO., LTD."

#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo.gif"

#endif

#if EDIMAX_OEM  // for edimax專用
/*
#undef SYS_DEF_VERSION
#ifdef IMGS_AR0331
#define SYS_DEF_VERSION "1.02.03A"
#else
#define SYS_DEF_VERSION "1.02.04"
#endif
*/
#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "EDIMAX"

#undef MODEL_NAME
#if MODAL_SIB7000
#define MODEL_NAME "NC-213E" // Edimax SIB7000 with PoE
#elif MODAL_PT
#if WIFI_FEATURE
#define MODEL_NAME "PT-31W"    // edimax 3M PT wifi
#else
#define MODEL_NAME "PT-31E"    // edimax 3M PT with POE
#endif
#elif MODAL_SIU5110
#define MODEL_NAME "IR-012E"    // Edimax SIU5110
#elif MODAL_SIU7110
#define MODEL_NAME "IR-113E"    // Edimax SIU7110
#elif MODAL_SIU7120
#define MODEL_NAME "IR-123E"    // Edimax SIU7120
#elif MODAL_SIFD7130
#define MODEL_NAME "ND-233E"    // Edimax SIFD7130
#elif MODAL_SIVD7137
#define MODEL_NAME "VD-233ED"    // Edimax SIVD7137
#endif

#undef BRAND_NAME
#define BRAND_NAME		"EDIMAX"

#undef BRAND_URL
#define BRAND_URL "www.edimax.com.tw"

#undef COMPANY_NAME
#define COMPANY_NAME "EDIMAX Technology CO., LTD."

#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo_edimax.gif"

#undef MAX_PRE_ALARM_TIME
#define MAX_PRE_ALARM_TIME  3
#undef MAX_POST_ALARM_TIME
#define MAX_POST_ALARM_TIME 3

//#undef BRAND_NAME
//#define BRAND_NAME "EDIMAX"
#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */

#endif

#if PROVIDEO_OEM  // for 丕文專用

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "PROVIDEO"

#undef MODEL_NAME
#if MODAL_SIB7000
#define MODEL_NAME "DIPC-H264-7"  // SIB7000
#elif MODAL_SIU7110
#define MODEL_NAME "SD-738MP-ICR"  // SIU7110
//#define MODEL_NAME "SD-738MP-ICR-MVF"  // SIU7110
#else
#define MODEL_NAME "H264_MP_38"  // SIMB8000
#endif

#undef BRAND_URL
#define BRAND_URL "www.provideo.com.tw"

#undef COMPANY_NAME
#define COMPANY_NAME "PROVIDEO CO., LTD."

#undef CAB_FILE_NAME
#define CAB_FILE_NAME "ICLAXPlayer.cab"
#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */
#endif

#if IVIEW_OEM   // for I-View專用
#undef BRAND_NAME
#define BRAND_NAME		"I-View"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "I-View"

#undef MODEL_NAME
#if MODAL_SIB7000
#define MODEL_NAME "EC-3MIP"   // SIB7000
#else
#define MODEL_NAME "IR-3MIP"  // SIMB8000
#endif

#undef BRAND_URL
#define BRAND_URL "www.i-view.com.tw"

#undef COMPANY_NAME
#define COMPANY_NAME "I-View Communication Inc."
//#undef CAB_FILE_NAME
//#define CAB_FILE_NAME "SStreamVideo.cab"
#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */
#endif

#if COP_OEM   // for COP專用

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT ""

#undef MODEL_NAME
//#define MODEL_NAME "IR-3MIP"  // SIMB8000
#define MODEL_NAME ""   // SIB7000

#undef BRAND_URL
#define BRAND_URL ""

#undef COMPANY_NAME
#define COMPANY_NAME ""
//#undef CAB_FILE_NAME
//#define CAB_FILE_NAME "SStreamVideo.cab"
#endif

#if FINEST_OEM   // for 宏臣專用

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "Fine"

#define WITH_LOGO 1

#undef MODEL_NAME
#if WITH_LOGO
#if MODAL_SIB7000
#define MODEL_NAME "ACM-B3000"   // SIB7000
#elif MODAL_PT
#define MODEL_NAME "ACM-PT300"   // SICPT500
#elif MODAL_SIU7120
#define MODEL_NAME "ACM-R3002"   // SIU7120 SIU7127
#elif MODAL_SIU7127
#define MODEL_NAME "ACM-R3002"   // SIU7120 SIU7127
#elif MODAL_SIFD7130
#define MODEL_NAME "ACM-V3001"   // SIFD7130
#elif MODAL_SIVD7137
#define MODEL_NAME "ACM-V3002"   // SIVD7137
#elif MODAL_SIU7323
	#define MODEL_NAME "ACM-R3006"   // SIU7323
#elif MODAL_SIB7323
	#define MODEL_NAME "ACM-B3000"   // SIB7323
#elif MODAL_SIMB8303
	#define MODEL_NAME "ACM-R3006"   // SIMB8303
#elif MODAL_SIU5110
#define MODEL_NAME "ACM-R2001"   // SIU5110
#endif
#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo_fine.gif"
#else // not WITH_LOGO
#if MODAL_SIB7000
#define MODEL_NAME "3MP_Box"   // SIB7000
#elif MODAL_PT
#define MODEL_NAME "3MP_PT"   // SICPT500
#elif MODAL_SIU7127
#define MODEL_NAME "3MP_IR"   // SIU7120 SIU7127
#elif MODAL_SIFD7130
#define MODEL_NAME "3MP_FD"   // SIFD7130
#elif MODAL_SIVD7137
#define MODEL_NAME "3MP_VD"   // SIVD7137
#elif MODAL_SIU7323
	#define MODEL_NAME "ACM-R3006"   // SIU7323
#elif MODAL_SIB7323
	#define MODEL_NAME "ACM-B3000"   // SIB7323
#elif MODAL_SIMB8303
	#define MODEL_NAME "ACM-R3006"   // SIMB8303
#elif MODAL_SIU5110
#define MODEL_NAME "ACM-R2001"   // SIU5110
#endif
#endif
#undef BRAND_NAME
#define BRAND_NAME		"Fine"

#undef BRAND_URL
#define BRAND_URL ""

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */

#undef COMPANY_NAME
#define COMPANY_NAME "Fine"
//#undef CAB_FILE_NAME
//#define CAB_FILE_NAME "SStreamVideo.cab"
#undef MJPG_DISABLE
#define MJPG_DISABLE 0

#endif

#if DIGISOL_OEM   // for
#undef BRAND_NAME
#define BRAND_NAME		"DIGISOL"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "DIGISOL"

#undef MODEL_NAME
#if MODAL_SIB7000
#define MODEL_NAME "DG-SC4600I"   // SIB7000
//#define MODEL_NAME "DG-SC4600PI"   // SIB7000 with PoE
#elif MODAL_PT
//#define MODEL_NAME "DG-SC3800"	// SICPT500
#define MODEL_NAME "DG-SC3800P"	// SICPT500 with PoE
#undef IPNC_ENVIRONMENT
#define IPNC_ENVIRONMENT 1
#elif MODAL_SIU7110
#define MODEL_NAME "DG-SC5800PI"   // SIU71000 with PoE
#elif MODAL_SIFD7130
#define MODEL_NAME "DG-SC6820PI"   // SIFD7130 with PoE
#elif MODAL_SIVD7137
#define MODEL_NAME "DG-SC8860VPI"   // SIVD7137 with PoE
#elif MODAL_SIU5110
#define MODEL_NAME "DG-SC5800PIE"   // SIU5110 with PoE
#endif

#undef BRAND_URL
#define BRAND_URL "www.digisol.com"

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */

#undef COMPANY_NAME
#define COMPANY_NAME "Digisol"
//#undef CAB_FILE_NAME
//#define CAB_FILE_NAME "SStreamVideo.cab"
#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo.gif"
#endif

#if PLANET_OEM   // for 普萊德專用

#undef BRAND_NAME
#define BRAND_NAME		"PLANET"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "PLANET"

#undef MODEL_NAME
#if MODAL_SIB7000
#define MODEL_NAME "ICA-HM127"   // SIB7000
#elif MODAL_SIU7110
#define MODEL_NAME "ICA-HM317"   // SIU7100
#undef SD_FEATURE
#define SD_FEATURE 0
#elif MODAL_PT
#define MODEL_NAME "ICA-HM227W"   // SICPT500W
#endif
#undef BRAND_URL
#define BRAND_URL "www.planet.com.tw"

#undef COMPANY_NAME
#define COMPANY_NAME "PLANET Technology Corp."
#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo.gif"

#undef DDNS_PROVIDER_LIST_DEFAULT	/* Modify DDNS Provider & Add DDNS Provider Planet - Dean - 20120411*/
#define DDNS_PROVIDER_LIST_PLANET	/* Modify DDNS Provider & Add DDNS Provider Planet - Dean - 20120411*/

#undef EVENT_HTTP_NOTIFY_STR					/* Modify Event - 20120620 - Dean */
#define EVENT_HTTP_NOTIFY_STR	"HttpNotify"	/* Modify Event - 20120620 - Dean */

#undef WIFI_ChannelEn
#define WIFI_ChannelEn 1 //alice_modify_2012_6_29_v1

#undef SYS_DEF_VERSION
#define SYS_DEF_VERSION "1.01.94"

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 1	/* Add Event Modification - Http Notify - 20120619 - Dean */

#endif

#if LIMIX_OEM   // for 創碁專用

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT ""

#undef MODEL_NAME
#define MODEL_NAME "LID-P30"   // SICPT500

#undef BRAND_URL
#define BRAND_URL "www.limix.com.tw"

#undef COMPANY_NAME
#define COMPANY_NAME "LIMIX Technology Co., Ltd."
#endif

#if FUHO_OEM   // for馥鴻專用

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "FUHO"

#undef MODEL_NAME
//#define MODEL_NAME "FIB-TI500"   // SIB7000
#define MODEL_NAME "FIO-TI520"   // SIU7110

#undef BRAND_URL
#define BRAND_URL "www.fuho.com.tw"

#undef COMPANY_NAME
#define COMPANY_NAME "FUHO"
#endif

#if LOGILINK_OEM   // for LOGILINK專用
#undef BRAND_NAME
#define BRAND_NAME		"LogiLink"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "LogiLink"

#undef MODEL_NAME
#if MODAL_PT
#if WIFI_FEATURE
#define MODEL_NAME "WC0009"   // SICPT500W
#else
#define MODEL_NAME "WC0008"   // SICPT500
#endif
#endif
//#define MODEL_NAME "WC0050"   // SIB7000
//#define MODEL_NAME "WC0052"   // SIU7110

#undef BRAND_URL
#define BRAND_URL "www.logilink.com"

#undef COMPANY_NAME
#define COMPANY_NAME "LogiLink"

#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo_LogiLink.gif"

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */
#endif

#if AIRLIVE_OEM   // for AIRLIFE_OEM專用

#undef BRAND_NAME
#define BRAND_NAME		"Airlive"

#undef MODEL_NAME
#define MODEL_NAME "BU-3025"   // SIU7110
//#define MODEL_NAME "BU-3025IVS"   // SIU7130 with IVS

//#undef DEFAULT_LOGO
//#define DEFAULT_LOGO "img/logo.gif"
#undef COMPANY_NAME
#define COMPANY_NAME "Airlive"

#undef DIPS_FEATURE			/* DIPS_FEATURE - Dean - 20120530 */
#define DIPS_FEATURE	1	/* DIPS_FEATURE - Dean - 20120530 */

#undef BRAND_URL
#define BRAND_URL "www.airlive.com"

#define AIRLIVE_IP_FINDER	1		/* Airlive IP Finder - Dean - 20120703 */

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 1	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef AGC_FEATURE
#define AGC_FEATURE             1
#undef MANUAL_SHUTTER_FEATURE
#define MANUAL_SHUTTER_FEATURE  1

#endif

#if NEOTECH_OEM   // for NEOTECH_OEM專用

#undef MODEL_NAME
#define MODEL_NAME "HBC-3M-V6"   // SIB7000 with DI2

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 1	/* Add Event Modification - Http Notify - 20120619 - Dean */

#endif

#if ALPHA_OEM

#endif

#if PCI_OEM   // for PCI		/* Add PCI_OEM - Begin - Dean - 20120601 *//* Add DDNS Provider PCI - Dean - 20120601 */

#undef BRAND_NAME
#define BRAND_NAME		"PCI"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "PCI"

#undef MODEL_NAME
#define MODEL_NAME "CS-WMV04N2"  // SICPT500W

#undef BRAND_URL
#define BRAND_URL "www.planex.com.jp"

#undef COMPANY_NAME
#define COMPANY_NAME "PLANEX Communications Inc."
#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo.gif"

#undef DDNS_PROVIDER_LIST_DEFAULT
#define DDNS_PROVIDER_LIST_PCI

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */

#undef UPLOAD_FEATURE
#define UPLOAD_FEATURE 0

#endif	/* Add PCI_OEM - End - Dean - 20120601 *//* Add DDNS Provider PCI - End - Dean - 20120601 */

#if HENTEK_OEM   // for HENTEK

#undef BRAND_NAME
#define BRAND_NAME		"HENTEK"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "HENTEK"

#undef MODEL_NAME
#if MODAL_SIB7000
#define MODEL_NAME "HK-BC303IP"  // SIB7000
#elif MODAL_SIU7110
#define MODEL_NAME "HK-NS30HY11S-1"  // SIU7110
#elif MODAL_PT
#if WIFI_FEATURE
#define MODEL_NAME "HK-NS30PHB11S-2"  // SICPT500
#else
#define MODEL_NAME "HK-NS30PHB11S-3"  // SICPT500W
#endif
#elif MODAL_SIU7120
#define MODEL_NAME "HK-BC304IR"  // SIU7120
#elif MODAL_SIFD7130
#define MODEL_NAME "HK-DS301IR"  // SIFD7130
#endif
#undef BRAND_URL
#define BRAND_URL "www.hentek.cn"

#undef COMPANY_NAME
#define COMPANY_NAME ""
#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo_hentek.gif"

#endif
// OEM 參數設定專區 End

#if HIQVIEW_OEM   // for HENTEK

#undef BRAND_NAME
#define BRAND_NAME		"hiQview"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "hiQview"

#undef MODEL_NAME
#if MODAL_SIB7000
#define MODEL_NAME "HIQ-6473"  // SIB7000
#elif MODAL_SIU7110
#define MODEL_NAME "HIQ-6483"  // SIU7110
#elif MODAL_SIU7120
#define MODEL_NAME "HIQ-6484"  // SIU7120
#elif MODAL_SIVD7137
#define MODEL_NAME "HIQ-5483"  // SIVD7137
#endif
#undef BRAND_URL
#define BRAND_URL "www.hiqview.com"

#undef COMPANY_NAME
#define COMPANY_NAME "hiQview"

#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo.gif"

#undef SYS_DEF_VERSION
#define SYS_DEF_VERSION "V3.1.3.2"

#undef EVENT_HTTP_NOTIFY_STR					/* Modify Event - 20120620 - Dean */
#define EVENT_HTTP_NOTIFY_STR	"HttpNotify"	/* Modify Event - 20120620 - Dean */

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 1	/* Add Event Modification - Http Notify - 20120619 - Dean */

#endif

#if UDM_OEM   // for HENTEK
#undef BRAND_NAME
#define BRAND_NAME		"UDM Group"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "UDM Group"

#undef MODEL_NAME
#if MODAL_SIB7000
#define MODEL_NAME "AV-7000"  // SIB7000
#elif MODAL_SIU7120
#define MODEL_NAME "AV-7120"  // SIU7110
#elif MODAL_PT
#define MODEL_NAME "AV-500E"  // SICPT500
#endif
#undef BRAND_URL
#define BRAND_URL "www.udmgroup.com"

#undef COMPANY_NAME
#define COMPANY_NAME ""

#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo_none.gif"

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */
#endif

#if FITI_OEM   // for HENTEK
#undef BRAND_NAME
#define BRAND_NAME		"FitiVision"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "FitiVision"

#undef MODEL_NAME
#define MODEL_NAME "VC130-OM"  // SIB7000

#undef BRAND_URL
#define BRAND_URL "www.fitivision.com"

#undef COMPANY_NAME
#define COMPANY_NAME "FitiVision"

#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo_DG_none.gif"
#endif

#if SURPEDIA_OEM   // for SURPEDIA
#undef BRAND_NAME
#define BRAND_NAME		"Surpedia"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "Surpedia"

#undef MODEL_NAME
//#define MODEL_NAME "SIU6110"  // SIU6110
#define MODEL_NAME "PVM04"  // SIU6120

#undef BRAND_URL
#define BRAND_URL "www.surpedia.com"

#undef COMPANY_NAME
#define COMPANY_NAME "Surpedia"

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */

#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo_none.gif"
#endif

#if TANTOS_OEM   // for TANTOS
#undef BRAND_NAME
#define BRAND_NAME		"TANTOS"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "TANTOS"

#undef MODEL_NAME
#define MODEL_NAME "TSi-PTZ311F"  // SICPT500

//#undef BRAND_URL
//#define BRAND_URL "www.surpedia.com"

#undef COMPANY_NAME
#define COMPANY_NAME "TANTOS"

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */

#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo_tantos.gif"
#endif

#if ANTEI_OEM   // for ANTEI
#undef BRAND_NAME
#define BRAND_NAME		"ANTEI"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "ANTEI"

#undef MODEL_NAME
#define MODEL_NAME "3MP_IP_M"  // SIMB8040

//#undef BRAND_URL
//#define BRAND_URL "www.surpedia.com"

#undef COMPANY_NAME
#define COMPANY_NAME "ANTEI"

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */

#undef DEFAULT_LOGO
#define DEFAULT_LOGO "img/logo_none.gif"
#endif

#if HUNTCCTV_OEM   // for HUNTCCTV

#undef BRAND_NAME
#define BRAND_NAME		"HuntCCTV"

#undef MANUFACTURER_DEFAULT
#define MANUFACTURER_DEFAULT "HuntCCTV"

#undef MODEL_NAME
#if MODAL_SIB7000
#define MODEL_NAME "HTP-TSMG"  // SIB7000
#elif MODAL_PT
#define MODEL_NAME "HTP-T3MPT"  // SICPT500W
#elif MODAL_SIU7120
#define MODEL_NAME "HTP-T3MG28H"  // SIU7120
#elif MODAL_SIFD7130
#define MODEL_NAME "HTP-T3MG28L"  // SIFD7130
#elif MODAL_SIVD7137
#define MODEL_NAME "HTP-T3MG28D"  // SIVD7137
#endif
#undef BRAND_URL
#define BRAND_URL "http://www.huntcctv.com/"

#undef COMPANY_NAME
#define COMPANY_NAME "HuntCCTV"

#undef HTTP_NOTIFY_FEATURE
#define HTTP_NOTIFY_FEATURE 0	/* Add Event Modification - Http Notify - 20120619 - Dean */
#undef HTTP_NOTIFY_CONFIG_FILE
#define HTTP_NOTIFY_CONFIG_FILE 0	/* Http Event Notify Config - Dean - 20121101 */

#endif

#define DELIM "."
#define DEFAULT_SN                  "SMTC0000000"
#define DEFAULT_PID                 "V00"
#define DEFAULT_MAC                 "00:12:54:6B:78:03"
#define APP_VERSION_DEFAULT         SYS_DEF_VERSION //__DATE__ //"ver.1.0.1"
#define APP_VERSION_DATE_DEFAULT    __DATE__ //"2012-04-26"
#define BOOT_VERSION_DEFAULT        APP_VERSION_DEFAULT
#define BOOT_VERSION_DATE_DEFAULT   APP_VERSION_DATE_DEFAULT
#define HARDWARE_VERSION_DEFAULT    "v0.00"
#define SYSTEM_OBJECT_ID_DEFAULT    "SYS_ID_0.0.0.1"

#define IPCAM_LOCATION_DEFAULT      "TAIWAN"

#define BONJOUR_NAME_DEFAULT        "Camera"
#define CAMERA_NAME_DEFAULT         "Camera"
//#define BONJOUR_NAME_DEFAULT        "SMAX-SIB7000P"

#define SNMP_READCOMMUNITY_DEFAULY  "public"
#define SNMP_WRITECOMMUNITY_DEFAULY "private"

#define MAX_IPFILTER_NUMBER  20

//#define PTZ_SPEED_DEFAULT 2
#define PTZ_SPEED_DEFAULT "2|2|2"
#define MAX_PTZ_SPEED 3
#define MAX_PTZ_GOTO  7

//#define JABBER_MSG_DEFAULT "Warning: Event happening from SIB7000 !!"

#define OVERLAY_IMAGE_TEXT MODEL_NAME
#define JABBER_MSG_DEFAULT MODEL_NAME
#define SERVER_HOSTNAME    MODEL_NAME


#ifdef LIMIT_LOG_NUM
#define MAX_LOG_NUM   100 //500
#endif

#define MAX_PTZ_PRESET 10

#define MAX_MD_NUM     3
#define MAX_MD_STRING_NUM     32

#define MD_BLOCK_DEFAULT "000"
#define MB_COORINATES    "0,0,0,0,0,50"

//#define SEC_8021X_PATH  "/opt/ipnc/wpa_supplicant"
//#define VIR_8021X_PATH  "/var/run/wpa_supplicant"

#define UPNP_TRAVERAL_PORT 4500

#define SCHEDULE_NUM       10
#define SCHEDULE_NUM_ADD   SCHEDULE_NUM+1

#define SCH_DAY_DEFAULT        "00000000"
#define SCH_ALL_DAY_DEFAULT    "11111111"
#define SCH_NAME_DEFAULT       "Schedule"

#define POST_ALARM_DEFAULT  5

#define SNMP_USERNAME   MODEL_NAME
#define SNMP_AUTH_PW    MODEL_NAME
#define SNMP_PRIVACY_PW MODEL_NAME

#define AUDIO_IN_VOLUME 5
#define MICROPHONE_VOLUME 50
#define SPEAKER_VOLUME 50

#define DATE_TIME_MODE 1

#define GLOBAL_PTZ_PATROL 1

#define DEFAULT_GAIN 0 // 7 modify by Kenny Chiu per frank.lin to set derault valuse as 0dB

#define UPNP_PORTMAPPING 0

#define AUDIO_SPEECH_PORT 5432

#define WIZARD_SETUP_FLAG 0

int  xml_cmd_dhcp_enable;
int  Set_MaxUserLogin;
int  Set_ServerityUser;
char AddressType[32];
char facilityType[128];

int mExtraShareMemId;
int mMDInfoMemId;

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
}Network_Mode;

typedef struct{
	char ddns_mode;
#if EXTRA_INFO_V2
	char reserved1[3];
#endif
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
}DDNS_Mode;

typedef struct{
	char upnp_mode;
	char upnp_discover;
	unsigned short upnp_traversal;
	unsigned char  upnp_portmapping;
	char upnp_avenable;
	char bonjour_mode;
	char friendly_name[64];
	char live_domain[128];
}Bonjour_Mode;

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
}Notification_Mode;

typedef struct{
	char sip_mode;
	char sip_phonenumber[16];
	short sip_port;
	char sip_ipaddress[64];
	char sip_loginid[64];
	char sip_loginpw[64];
}SIP_MODE;

typedef struct{
	char eventconfig;	/* Event -> Media -> 0:H.264, 1:One Snapshot *//* Modify Event - 20120620 - Dean */
	char prealarm;
	char postalarm;
}FTPALARM_MODE;

typedef struct{
	char ipf_mode;
	char ip4_mode[MAX_IPFILTER_NUMBER];
	char ip6_mode[MAX_IPFILTER_NUMBER];
	int  ip_number;
	char ip_type[MAX_IPFILTER_NUMBER];
	char ip_start[MAX_IPFILTER_NUMBER][64];
	char ip_end[MAX_IPFILTER_NUMBER][64];
}IPFILTER_MODE;

typedef struct{
	char  https_mode;
	char  http_mode;
	int   https_port;
}HTTPS_MODE;

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
} IMAGE_SETTING_MODE;

// add by kenny chiu 20130422 start
typedef struct{
	char ipmode;
	struct in_addr	ip; ///< IP address in static IP mode
	struct in_addr	netmask; ///< netmask in static IP mode
	struct in_addr	gateway; ///< gateway in static IP mode
	struct in_addr	dns; ///< DNS IP in static IP mode
	char ipv6_addr[64];
	unsigned short ipv6_prefix;
	char pppoeenable;
	char pppoeaccount[32];
	char pppoepwd[32];
	__u8 ntp_timezone; ///< current time zone
	__u8 daylight_time; ///< daylight saving time mode
} DEFAULT_BACKUP;
// add by kenny chiu 20130422 end


typedef struct{
     int status;
}CABLE_INFO;
//alice_modify_onvif_code_2011
#if IVS_FEATURE
#define IVS_MOTION_CONFIG_PATH		"/mnt/nand/"
#define IVS_MOTION_CONFIG_FILENAME	"motion_conf.ini"
#define IVS_MOTION_CONFIG_FULL_FILENAME	"/mnt/nand/motion_conf.ini"

#define IVS_ZONE_CTRL_POINT_MAX		9
typedef struct{
	int enable;
	int showResult;
	int warn;
	int colorNormal;
	int colorWarning;
	int wallorGround;					// Type - Wall or Ground - 0:Wall, 1:Ground
	int direction;						// Direction - 0:Positive(clockwise), 1:Negative(counterclockwise)
	int objMinSize_X;
	int objMinSize_Y;
	int pointnum;
	int point_X[IVS_ZONE_CTRL_POINT_MAX+1];
	int point_Y[IVS_ZONE_CTRL_POINT_MAX+1];
}IVS_ZONE;

#define IVS_OBJECT_NAME_LENGTH_MAX	24
#define IVS_OBJECT_CTRL_POINT_MAX		2
typedef struct{
	int enable;
	int show;
	int color;
	int warn;
	char name[IVS_OBJECT_NAME_LENGTH_MAX];
	int width;
	int height;
	int point_X;
	int point_Y;
}IVS_OBJECT;

enum _sensitivityPara
{
	SENSITIVITY_PARA_DARK_Y = 0,
	SENSITIVITY_PARA_BRIGHT_Y,
	SENSITIVITY_PARA_DARK_U,
	SENSITIVITY_PARA_BRIGHT_U,
	SENSITIVITY_PARA_DARK_V,
	SENSITIVITY_PARA_BRIGHT_V,
	SENSITIVITY_PARA_TOTAL,
};

typedef struct{
	int NIRPara[SENSITIVITY_PARA_TOTAL];			// Near Infrared (NIR) Mode (DarkY, BrightY, DarkU, BrihgtU, DarkV, BrightV)
	int NormalPara[SENSITIVITY_PARA_TOTAL];		// Normal Lighting Mode (DarkY, BrightY, DarkU, BrihgtU, DarkV, BrightV)
	int NIREnable;
	float NIRAreaRatio;
}IVS_SENSITIVITY;

typedef struct{
	int warningPeriod;		// Camera Tamper Detection - Blind detection, Sudden lighting detection, Scene change detection
	float maxRealAreaRatio;
	float maxRectAreaRatio;
}IVS_SCENE_CHANGE;

typedef struct{
	int level;					// Small Vibration Rejection
}IVS_SMALL_VIBRATION_REJECTION;

typedef struct{
	float maxV;				// Shadow Rejection
}IVS_SHADOW_REJECTION;

typedef struct{
	int range_X;				// Merge Rectangle
	int range_Y;
}IVS_MERGE_RECT;

typedef struct{
	int widthMin;				// Min/Max Detected Object Size
	int heightMin;
	int widthMax;
	int heightMax;
	float effectPixelRatio;
	int updateTime;			// Background Model Updating - Time to update the detected stationary objetcts to background
}IVS_DETECTED_OBJECT;

typedef struct{
	int enable;
	int show;
	int warn;
}IVS_FUNCTION;

typedef struct{
	IVS_FUNCTION motionDetect;
	IVS_FUNCTION objectSizeFilter;
	IVS_FUNCTION forbiddenZone;
	IVS_FUNCTION uninterestedZone;
	IVS_FUNCTION shadowRejection;
	IVS_FUNCTION smallVibrationRejection;
	IVS_FUNCTION sceneChangeDetection;
	IVS_FUNCTION crowdDetection;
	IVS_DETECTED_OBJECT detectedObj;
	IVS_MERGE_RECT mergeRect;
	IVS_SENSITIVITY sensitivity;
	IVS_SHADOW_REJECTION shadowReject;
	IVS_SMALL_VIBRATION_REJECTION smallVibraReject;
	IVS_SCENE_CHANGE sceneChange;
}IVS_DETECT;

#define IVS_CMD_STR_LEN		64

#define IVS_ZONE_FORBIDDEN_MAX			12
#define IVS_ZONE_UNINTERESTED_MAX			4
#define IVS_ZONE_CROWD_MAX				2
#define IVS_OBJECT_DISPLAY_TITLE_MAX		1
#define IVS_OBJECT_DISPLAY_CROWD_MAX		2
#define IVS_OBJECT_SIZE_FILTER_MAX			4

typedef struct{
	unsigned char enable;
	unsigned char demo;
	unsigned char videoResoultionID;
	IVS_DETECT detect;
	IVS_ZONE zoneForbidden[IVS_ZONE_FORBIDDEN_MAX];
	IVS_ZONE zoneUninterested[IVS_ZONE_UNINTERESTED_MAX];
	IVS_ZONE zoneCrowd[IVS_ZONE_CROWD_MAX];
	IVS_OBJECT displayTitle[IVS_OBJECT_DISPLAY_TITLE_MAX];
	IVS_OBJECT displayCrowd[IVS_OBJECT_DISPLAY_CROWD_MAX];
	IVS_OBJECT sizeFilter[IVS_OBJECT_SIZE_FILTER_MAX];
}IVS_INFO;
#endif

/* DIPS_FEATURE - Begin - Dean - 20120530 */
#if 1 //DIPS_FEATURE
typedef struct{
	char dips_enable;
	char dips_mode;
	char dips_provider;
	char dips_connecting;		/* Add DIPS Connecting - Dean - 20120817 */
	char dips_id[16];
	char reserver[12];
}DIPS_INFO;
#endif
/* DIPS_FEATURE - End - Dean - 20120530 */

/* OVERLAY_IMAGE_FEATURE - Dean - 20130709 - Begin */
#define OVERLAY_IMAGE_CMD_STR_LEN				128
/* OVERLAY_IMAGE_FEATURE - Dean - 20130709 - End */

#define SAMBA_RECORD_SPACE_MAX_SIZE 10
#define PORT_USED_LIST_DATA_STR_LEN				256			/* Port Used List CGI - Dean - 20131021 */

typedef enum{
	SCH_SYSTEM_CALL_EVENT_RUN = 0,
	SCH_SYSTEM_CALL_EVENT_TOTAL
}SchSystemCallEvent;

typedef enum{
	SET_SN = 0x0000,
	SET_PID,
	SET_MAC,
	SET_APP_VER,
	SET_APP_VER_DATE,
	SET_BOOT_VER,
	SET_BOOT_VER_DATE,
	SET_HW_VER,
	SET_SYS_OBJ_ID,
	SET_IPCAM_LOCATION,
	SET_IPCAM_CAMERA_NAME,
	SET_IPCAM_LED,
	SET_QOS_MODE = 0x1000,
	SET_QOS_TYPE,
	SET_QOS_VIDEO_DSCP,
	SET_QOS_AUDIO_DSCP,
	SET_COS_MODE,
	SET_COS_VLANID,
	SET_COS_VIDEOPRIORITY,
	SET_COS_AUDIOPRIORITY,
	SET_COS_ADDRESS,
	SET_SNMP_MODE,
	SET_SNMP_TYPE,
	SET_SNMP_USERNAME,
	SET_SNMP_USERNAME_PRIVACY,
	SET_SNMP_AUTH,
	SET_SNMP_PRIVACY,
	SET_SNMP_AUTH_PW,
	SET_SNMP_PRIVACY_PW,
	SET_SNMP_READCOMMUNITY,
	SET_SNMP_WRITECOMMUNITY,
	SET_UPNP_MODE = 0x1100,
	SET_UPNP_DISCOVER,
	SET_UPNP_TRAVERSAL,
	SET_UPNP_PORTMAPPING,
	SET_UPNP_AVMODE,
	SET_BONJOUR_MODE,
	SET_FRIENDLY_NAME,
	SET_LIVE_DOMAIN,
	SET_MAIL_MODE = 0x1200,
	SET_MAIL_SENDTO,
	SET_MAIL_SUBJECT,
	SET_TCP_MODE,
	SET_TCP_ADDR,
	SET_TCP_PORT,
	SET_HTTP_MODE,
	SET_HTTP_URL,
	SET_HTTP_USERNAME,
	SET_HTTP_PASSWORD,
	SET_SIP_MODE = 0x1300,
	SET_SIP_PHONENUMBER,
	SET_SIP_PORT,
	SET_SIP_IPADDRESS,
	SET_SIP_LOGINID,
	SET_SIP_LOGINPW,
	SET_FTP_EVENTCONFIG = 0x1400,
	SET_FTP_PREALARM,
	SET_FTP_POSTALARM,
	SET_IPF_IPF_MODE = 0x1500,
	SET_IPF_IP4_MODE,
	SET_IPF_IP6_MODE,
	SET_IPF_IP_ADD_NUMBER,
	SET_IPF_IP_DEL_NUMBER,
	SET_IPF_IP_TYPE,
	SET_IPF_IP_START,
	SET_IPF_IP_END,
	SET_HTTPS_MODE = 0x1600,
	SET_HTTPS_PORT,
	SET_HTTP_STATUS,
	SET_MIRRORTYPE = 0x1700,
	SET_OVERLAYDATE,
	SET_OVERLAYTEXT,
	SET_IMAGETEXT,
	SET_OVERLAYIMAGE,
	SET_OVERLAYIMAGE_TYPE,
	SET_OVERLAYIMAGE_XPOS,
	SET_OVERLAYIMAGE_YPOS,
	SET_OVERLAYIMAGE_FILE,
	SET_OVERLAYIMAGE_CONFIG,	/* OVERLAY_IMAGE_FEATURE - Dean - 20130709 */
	SET_IPMODE = 0x1800,
	SET_DNS2IP,
	SET_IPV6_ADDRESS,
	SET_IPV6_PREFIX,
	SET_RTSP_PORT,
	SET_RTP_PORT,
	SET_VIDEO_PORT,
	SET_AUDIO_PORT,
	SET_TTL,
	SET_RTP_DATAPORT,
	SET_GROUP_ADDR,
	SET_MULTICAST_ENABLE,
	SET_PPPOE_ENABLE,
	SET_PPPOE_ACCOUNT,
	SET_PPPOE_PWD,
	SET_DDNS_ENABLE,
	SET_DDNS_PROVIDER,
	SET_DDNS_DOMAIN,			/* Add DDNS Provider PCI - Dean - 20120601 */
	SET_DDNS_HOSTNAME,
	SET_DDNS_ACCOUNT,
	SET_DDNS_PASSWORD,
	SET_DDNS_UPDATE_PERIOD, 	/* Modify DDNS Provider & Add DDNS Provider Planet - Dean - 20120411 */
	SET_DDNS_CONNECTING,		/* Add DDNS Connecting - Begin - Dean - 20120605 */
	SET_DIPS_ENABLE,			/* DIPS_FEATURE - Dean - 20120530 */
	SET_DIPS_PROVIDER,			/* DIPS_FEATURE - Dean - 20120530 */
	SET_DIPS_ID,				/* DIPS_FEATURE - Dean - 20120530 */
	SET_DIPS_CONNECTING,		/* Add DIPS Connecting - Dean - 20120817 */
	SET_RTSP_MULTI,
	SET_EVENT_SAMBA_SERVER_IP = 0x1900,
	SET_EVENT_SAMBA_PATH,
	SET_EVENT_SAMBA_MOUNT_PATH,
	SET_EVENT_SAMBA_USERNAME,
	SET_EVENT_SAMBA_PASSWORD,
	SET_EVENT_HTTP_URL,
	SET_EVENT_HTTP_USERNAME,
	SET_EVENT_HTTP_PASSWORD,
	SET_EVENT_JABBER_SERVER_IM_SERVER,
	SET_EVENT_JABBER_LOGIN_ID,
	SET_EVENT_JABBER_LOGIN_PW,
	SET_EVENT_JABBER_SEND_TO,
	SET_EVENT_JABBER_MESSAGE,
	SET_EVENT_FTP_ENABLE,
	SET_EVENT_SMTP_ENABLE,
	SET_EVENT_SAMBA_ENABLE,
	SET_EVENT_JABBER_ENABLE,
	SET_8021_X_MODE = 0x2000,
	SET_8021_X_MODE_TMP,
	SET_8021_X_PROTOCOL,
	SET_8021_X_USERNAME,
	SET_8021_X_PASSWORD,
	SET_8021_X_CA_FILE,
	SET_8021_X_CERT_FILE,
	SET_8021_X_PRIVKEY_FILE,
	SET_8021_X_PRIVKEY_PASSWORD,
	SET_8021_X_TUNNEL_PROTOCOL,
	SET_ACOUNT_INDEX = 0x2100,
	SET_ACOUNT_USER,
	SET_ACOUNT_PASSWORD,
	SET_ACOUNT_AUTHORITY,
	SET_ACOUNT_MODIFY,
	SET_ACOUNT_MODIFY_CHAR,
	SET_TIME_TYPE = 0x2200,
	SET_RESTART_TYPE,
	SET_LOG_NUM,
	SET_EVENT_INDEX_ADD = 0x2300,
	SET_EVENT_INDEX_DEL,
	SET_EVENT_INDEX,
	SET_ENABLE_EVENT,
	SET_TRIGGER_TIME_GAP,		/* TAMPER_FEATURE Event - Dean - 20131007 */
	SET_EVENT_NAME,
	SET_TRIGGER_MOTION_DETECTION,
	SET_TRIGGER_DIGITAL_IN_MODE1,
	SET_TRIGGER_DIGITAL_IN_TYPE1,
	SET_TRIGGER_DIGITAL_IN_MODE2,
	SET_TRIGGER_DIGITAL_IN_TYPE2,
	SET_TRIGGER_SCH_TIME_MODE,
	SET_TRIGGER_PERIODICALLY_MODE,
	SET_TRIGGER_PERIODICALLY_MIN,
	SET_SCH_TIME_STATUS,
	SET_SCH_TIME_DAY,
	SET_SCH_TIME_START,
	SET_SCH_TIME_DURATION,
	SET_ENABLE_EVENT_FTP,
	SET_ENABLE_EVENT_JABBER,
	SET_ENABLE_EVENT_EMAIL,
	SET_ENABLE_EVENT_SAMBA,
	SET_TRIGGER_DIGITAL_OUT_MODE,
	SET_TRIGGER_DIGITAL_OUT_SEC,
	SET_EVENT_PRESET_MODE,
	SET_EVENT_PRESET_POSITION,
	SET_ENABLE_EVENT_USB,
	SET_EVENT_CLEAN_DATA,
	SET_EVENT_CLEAN_ALL_DATA,
	SET_EVENT_WRITE_ALL_DATA,
  //SET_PTZ_ZOOM_IN = 0x2400,
  //SET_PTZ_ZOOM_OUT,
  //SET_PTZ_FOCUS_IN,
  //SET_PTZ_FOCUS_OUT,
  //SET_PTZ_FOCUS_AUTO,
	SET_PTZ_PAN_UP,
	SET_PTZ_PAN_DOWN,
	SET_PTZ_PAN_LEFT,
	SET_PTZ_PAN_RIGHT,
	SET_PTZ_PAN_HOME,
	SET_PTZ_SPEED,
	SET_PTZ_GOTO,
	SET_PTZ_PRESET,
	//alice_modify_onvif_code_2011
	SET_PTZ_PRESETLIST, // add by alice 2011-12-9
	SET_PTZ_ACTPOS, // add by alice 2011-12-9
	SET_PTZ_MVSTATUS, // add by alice 2011-12-9
	SET_PTZ_AUTO_PATROL,
	SET_REMOTE_IP4 = 0x2500,
	SET_REMOTE_IP6,
	SET_DATETIME_MODE = 0x2600,
	SET_MD_INDEX = 0x2700,
	SET_MD_ENABLE,
	SET_MD_SEN_MODE,
	SET_MD_SEN_VALUE,
	SET_MD_BLK_STR,
	SET_MD_BLK,
	SET_MD_PERCENTAGE,
	SET_MD_INDICATOR,
	SET_MD_NAME,
	SET_MD_WRITE_ALL_DATA,
	SET_DETECT_IP = 0x2800,
	SET_STREAM_FORMAT = 0x2900,
	SET_AUDIO_IN_VOLUME = 0x2a00,
	SET_MIRCOPHONE_VOLUME,
	SET_SPEAKER_VOLUME,
	SET_AUDIO_GAIN,
	SET_GLROBAL_PTZ_PATROL = 0x2b00,
	SET_WIZARD_FLAG = 0x2c00,
	SET_SMTP_AUTH_MODE = 0x2d00,
	SET_SMTP_SSL_MODE,
	SET_SMTP_PORT,
	SET_SMTP_STARTTLS,
	SET_SYSLOG_ENABLE = 0x2e00,
	SET_SYSLOG_IP,
	SET_SYSLOG_PORT,
	SET_USB_STATUS = 0x2f00,
	SET_SAMBA_REC_MODE = 0x3000,
	SET_SAMBA_REC_IP,
	SET_SAMBA_REC_PATH,
	SET_SAMBA_REC_MOUNT_PATH,
	SET_SAMBA_REC_USER,
	SET_SAMBA_REC_PW,
	SET_SAMBA_REC_SIZE,
	SET_SAMBA_REC_SPACE,
	SET_SAMBA_REC_SCH,
	SET_SAMBA_REC_TIME_MODE,
	SET_SAMBA_REC_TIME,
	SET_SAMBA_REC_DURATION,
	SET_SAMBA_REC_CLEAN_SCH,
	SET_SAMBA_REC_TIMESTAMP
#if IPNC_PELCOD_FEATURE
	,SET_RS485_ENABLE = 0x3100
	,SET_RS485_ADDRESS
	,SET_RS485_PAN_SPEED
	,SET_RS485_TILT_SPEED
	,SET_RS485_CUSTOMHOME
	,SET_RS485_CUSTOMUP
	,SET_RS485_CUSTOMDOWN
	,SET_RS485_CUSTOMLEFT
	,SET_RS485_CUSTOMRIGHT
	,SET_RS485_EXTCMD
	,SET_RS485_EXTNAME
	,SET_RS485_PRESET_NAME
	,CLEAN_RS485_PRESET_NAME
#endif	//IPNC_PELCOD_FEATURE
#if IPNC_IRCUT_FEATURE
	,SET_IRCUT_MODE = 0x3200
	,SET_IRCUT_SCHE_START
	,SET_IRCUT_SCHE_END
#endif	//IPNC_IRCUT_FEATURE
#if WIFI_FEATURE
	//alice_modify_2012_3_27_v1
	,SET_WIFI_ENABLE = 0x3300
	,SET_WIFI_WPS
	,SET_WIFI_SSID
	,SET_WIFI_PIN_SSID//charles lin wifi6
	,SET_WIFI_MODE
	,SET_WIFI_CHANNEL
	,SET_WIFI_SECURITY
	,SET_WIFI_WEP_AUTH
	,SET_WIFI_WEPKEY_FMT
	,SET_WIFI_WEPKEY_STR0
	,SET_WIFI_WEPKEY_STR1
	,SET_WIFI_WEPKEY_STR2
	,SET_WIFI_WEPKEY_STR3
	,SET_WIFI_WEPKEY_IDX
	,SET_WIFI_WPA_COUNTRYREGION
	,SET_WIFI_TEST_BEGIN ////alice_modify_2012_5_10_v1
	,SET_WIFI_PSK_STR
	,SET_WIFI_BAND_WIDTH
	//alice_modify_2012_3_27_v1
#endif //WIFI_FEATURE
#if IVS_FEATURE
	,UPDATE_IVS_FEATURE = 0x3400
	,SET_IVS_DEMO
#endif // IVS_FEATURE
#if HTTP_NOTIFY_FEATURE					/* Add Event Modification - Http Notify - 20120619 - Dean */
	,UPDATE_EVENT_HTTP_NOTIFY = 0x3500 /* Add Event Modification - Http Notify - 20120619 - Dean */
#endif									/* Add Event Modification - Http Notify - 20120619 - Dean */
#if	UPLOAD_FEATURE
	,SET_UPLOAD_ENABLE=0x3600
	,SET_UPLOAD_MEDIA_TYPE
	,SET_UPLOAD_MEDIA_TIME
	,SET_UPLOAD_PREFIX_NAME
	,SET_UPLOAD_TIMESTAMP
	,SET_UPLOAD_PERIOD_VALUE
	,SET_UPLOAD_PERIOD_UNIT
#endif
	,SET_PRIVACY_MASK_INFO = 0x3800
	,SET_TAMPER_INFO = 0x3A00
}EXTRA_INFO_CONTROL;

struct USER_INFO {
   int  user_id;
   char user_name[64];
   char remote_ip_addr[128];
   time_t start_time;
};

int CurrConnectNum;
typedef struct USER_INFO UserInfo;
UserInfo user_info[16];

struct USER_ARTH_INFO {
	int  user_id;
	char user_name[64];
};

typedef struct USER_ARTH_INFO UserArthInfo;
UserArthInfo user_arth_info[16];


struct THREEGPPINFO{
	int status;
	int without_audio;
	int with_audio;
};

typedef struct THREEGPPINFO ThreeGPPInfo;
ThreeGPPInfo thr_gpp_info;


typedef struct MULTIMEDIACONTROL{
	short         rtsp_port;
	unsigned char multicast_enable;
	unsigned char imagesource_type;
	unsigned char mobile_mode;
	unsigned char mpeg4_resolution;
	unsigned char mpeg4_rate_control;
	unsigned char mpeg4_quality;
	int           mpeg4_bitrate;
	unsigned char mpeg4_framerate;
	unsigned char mjpeg_resolution;
	unsigned char mjpeg_quality;
	unsigned char mjpeg_framerate;
	unsigned char audio_enable;
	unsigned char audio_resolution;
	unsigned char process_reset_mode; // 0 for non-restart, 1 for wis, 2 for all.
} MultiMediaControl;

#if defined (__cplusplus)
}
#endif

#endif
