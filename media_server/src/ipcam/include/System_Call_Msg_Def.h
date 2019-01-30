/* ===========================================================================
* @path $(IPNCPATH)\include
*
* @desc
* .
* Copyright (c) Fitivision Inc.  2009,RS,DM365/DM355,Verify All Pass
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
/**
* @file Alarm_Msg_Def.h
* @brief Definition of the message ID to communicate with alarm manager.

* This file lists all the message ID that will be used to initialize alarm
*  message driver.
* @warning Be sure that one message ID can only be used by one process.
* @note When you add a process to use alarm message driver, please add a new
*  definition here.

* Add message ID may like following.
* @code
#define ALARM_NEW_MSG		MSG_TYPE_MSG5  //RS MDY
* @endcode
*/
#ifndef _SYSTEM_CALL_MSG_DEF_H_
#define _SYSTEM_CALL_MSG_DEF_H_

/* MSG_TYPE_MSG1 is reserved for server */
/**
 * @addtogroup SYSTEM_CALL_MSG_DRV
* @{*/
#define SYSTEM_CALL_SYS_MSG           MSG_TYPE_MSG21 /* message ID used in system server to communicate with system call manager */

typedef enum {
	SYSTEM_CALL_FUNCTION_CMD = 0,
	SYSTEM_CALL_FUNCTION_TOTAL,
}SystemCallFunctionType;

#define SCH_SYSTEM_CALL_QUEUE_COUNT (SCH_SYSTEM_CALL_EVENT_TOTAL + 1)	/* Modify Event - 20120620 - Dean */

/** @} */
#endif

