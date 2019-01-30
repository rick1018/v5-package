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
* @file File_Msg_Def.h
* @brief Definition of the message ID to communicate with file manager.

* This file lists all the message ID that will be used to initialize file
*  message driver.
* @warning Be sure that one message ID can only be used by one process.
* @note When you add a process to use file message driver, please add a new
*  definition here. And MSG_TYPE_MSG10 is the maximum.

* Add message ID may like following.
* @code
#define FILE_NEW_MSG		MSG_TYPE_MSG6
* @endcode
*/
#ifndef _FILE_MSG_DEF_H_
#define _FILE_MSG_DEF_H_

#if defined (__cplusplus)
extern "C" {
#endif

/* MSG_TYPE_MSG1 is reserved for server */
/**
 * @addtogroup FILE_MSG_DRV
* @{*/
	
/*Defined for the input params (iProcId) of  InitFileMsgDrv(int key,int iProcId)
   the values are important as the msgtype of message queue.
   the input param "key" for the mesage ID of queue and sharememory key. 
*/

#if 1
#define FILE_SYS_MSG		MSG_TYPE_MSG2 ///< iProcId  used in system server to communicate with file manager.
#define FILE_BOA_MSG		MSG_TYPE_MSG3 ///< iProcId  used in BOA to communicate with file manager.
#define FILE_STREAM_MSG		MSG_TYPE_MSG4 ///< iProcId  used in stream server to communicate with file manager.
#define FILE_MOTION_MSG		MSG_TYPE_MSG5 ///< iProcId  used by motion detection to communicate with file manager.
#define FILE_CLOSE_MSG    	MSG_TYPE_MSG6///
#define FILE_FINDER_MSG		MSG_TYPE_MSG7 ///< iProcId  used by finder server to communicate with file manager.
#define FILE_REC_VIDEO_MSG	MSG_TYPE_MSG8

#else
#define FILE_SYS_MSG		MSG_TYPE_MSG2 ///< message ID used in system server to communicate with file manager.
#define FILE_BOA_MSG		MSG_TYPE_MSG3 ///< message ID used in BOA to communicate with file manager.
#define FILE_STREAM_MSG		MSG_TYPE_MSG4 ///< message ID used in stream server to communicate with file manager.
#define FILE_MOTION_MSG		MSG_TYPE_MSG5 ///< message ID used by motion detection to communicate with file manager.
#define FILE_CLOSE_MSG    	MSG_TYPE_MSG13///by Larry
#define FILE_FINDER_MSG		MSG_TYPE_MSG9 ///< message ID used by finder server to communicate with file manager.
/** @} */
#define FILE_REC_VIDEO_MSG	MSG_TYPE_MSG20
#endif
#if defined (__cplusplus)
}
#endif

#endif

