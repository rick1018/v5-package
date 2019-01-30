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
* @file Sys_Msg_Def.h
* @brief System message ID definition
* @warning Be sure that one message ID can only be used by one process.
* @see File_Msg_Def.h
*/
#if defined (__cplusplus)
extern "C" {
#endif

#ifndef _SYS_MSG_DEF_H_
#define _SYS_MSG_DEF_H_
/**
* @addtogroup SYS_MSG_DRV
* @{
*/
/* MSG_TYPE_MSG1 is reserved for server */

/*Defined for the input params (iProcId) of  SysDrvInit(int iProcId)
   the values are important as the msgtype of message queue.
   the input param "key" for the mesage ID of queue and sharememory key. 
*/

#if 1 //for redefine by frank
//following value are the processs id to get/set share memory offset, and are the input params of SysDrvInit().
//following total numbers are important for MAX_SHARE_PROC defined in Share_mem.h

#define SYS_BOA_MSG		MSG_TYPE_MSG2 ///< iProcIdused in BOA process to communicate with system server. 
#define SYS_UPNP_MSG	MSG_TYPE_MSG3 ///< iProcIdused in UPNP process to communicate with system server. 
#define SYS_FTP_MSG		MSG_TYPE_MSG4 ///< iProcId used in FTP client to communicate with system server. 
#define SYS_SMTP_MSG	MSG_TYPE_MSG5 ///< iProcId used in SMTP client to communicate with system server. 
#define SYS_BOOT_MSG	MSG_TYPE_MSG6 ///<iProcId used in boot_proc to communicate with system server.
#define SYS_IPCHECK_MSG	MSG_TYPE_MSG7 //
#define NTP_CLIENT_MSG	MSG_TYPE_MSG8 //
#define SYS_CLOSE_MSG	MSG_TYPE_MSG9 //
#define SYS_FINDER_MSG	MSG_TYPE_MSG10//< iProcId used in finder server to communicate with system server.
#define REC_VIDEO_MSG   MSG_TYPE_MSG11

#else
#define SYS_BOA_MSG		MSG_TYPE_MSG2 ///< message ID used in BOA process to communicate with system server. 
#define SYS_UPNP_MSG		MSG_TYPE_MSG20 ///< message ID used in UPNP process to communicate with system server. 
#define SYS_FTP_MSG			MSG_TYPE_MSG4 ///< message ID used in FTP client to communicate with system server. 
#define SYS_SMTP_MSG		MSG_TYPE_MSG5 ///< message ID used in SMTP client to communicate with system server. 
#define SYS_BOOT_MSG		MSG_TYPE_MSG6 ///< message ID used in boot_proc to communicate with system server.
//#define SYS_UPDATE_MSG		MSG_TYPE_MSG6 //by Larry
#define SYS_IPCHECK_MSG		MSG_TYPE_MSG7 //by Larry
#define NTP_CLIENT_MSG		MSG_TYPE_MSG11 //by Larry
#define SYS_CLOSE_MSG		MSG_TYPE_MSG14 //by Larry
#define SYS_FINDER_MSG		MSG_TYPE_MSG9 ///< message ID used in finder server to communicate with system server.

#define REC_VIDEO_MSG		MSG_TYPE_MSG19
#end
/**
* @}
*/
#endif
#endif

#if defined (__cplusplus)
}
#endif