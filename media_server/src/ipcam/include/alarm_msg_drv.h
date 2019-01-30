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
* @file alarm_msg_drv.h
* @brief Alarm message driver
*/
#ifndef _ALARM_MSG_DRV_H_
#define _ALARM_MSG_DRV_H_
#if defined (__cplusplus)
extern "C" {
#endif

#include <Msg_Def.h>
#include <sys_env_type.h>

// Add Event Trigger Check - 20120508 - Begin - Dean
#define REC_EVENT_TRIG_MOTION_DETECT	0x100
#define REC_EVENT_TRIG_DIGITAL_IN_1		0x200
#define REC_EVENT_TRIG_DIGITAL_IN_2		0x400
#define REC_EVENT_TRIG_IVS				0x800
#define REC_EVENT_TRIG_OV				0x1000
#define REC_EVENT_TRIG_SYSTEM_REBOOT	0x2000
#define REC_EVENT_TRIG_TAMPER			0x4000	/* TAMPER_FEATURE - Dean - 20131007 */
// Add Event Trigger Check - 20120508 - End - Dean

/* Event Modify - 20120620 - Begin - Dean */
#define REC_EVENT_MEDIA_TYPE_ONE_SNAPSHOT	0x1
#define REC_EVENT_MEDIA_TYPE_VIDEO_H264		0x0
/* Event Modify - 20120620 - End - Dean */

/**
* @defgroup ALARM_MSG_DRV Alarm message driver

* This is the communication interface of alarm manager. Every process needs a
*  message ID (which is defined in @ref Alarm_Msg_Def.h) to communicate with
* alarm manager. More detail please refer to (@ref ALARM_MSG_DRV_HOW).
* @{
*/
/* This function should be called at process innitial !!! */
int AlarmDrvInit(int proc_id);
int AlarmDrvExit();
/* API */
void SendAlarmQuitCmd();
void SendAlarmMotionrDetect(int Serial);
void SendAlarmAudioTrigger();
void SendMotionAlarmAudio(int alarmaudio_duration, int alarm_audio_enable);
void SendAlarmResetTrigger();
//void SendAlarmInTrigger();					/* Remove - Record Media - Dean - 20130530 */
//void SendAlarmEthTrigger();
//void SendAlarmSchedule(int index);				/* Remove - Record Media - Dean - 20130530 */
//void SendAlarmScheduleEnd();					/* Remove - Record Media - Dean - 20130530 */
void SendAlarmTrigger(int index,Event_t event,int alarmaudio_duration, int alarm_audio_enable);//RS ADD
/* DMVA ALARM EVENT */
void SendDMVAAlarmEventDetect(int EvtType,char *fileName);

#if defined (__cplusplus)
}
#endif

/** @} */
#endif /* _ALARM_MSG_DRV_H_ */
/**
* @page ALARM_MSG_DRV_HOW How to use alarm message driver?
* 1. Add new message ID in \ref Alarm_Msg_Def.h if needed.\n
* 2. Call the API AlarmDrvInit(int proc_id) with the ID you added to initialize alarm
*  message driver.\n
* 3. Now you can use any alarm message driver functions as you want.\n
* 4. Call the API AlarmDrvExit() to cleanup alarm message driver.
* @section ALARM_MSG_DRV_HOW_EX Example
* @code
#include <alarm_msg_drv.h>
int main()
{
	SysInfo *pSysInfo;
	if( AlarmDrvInit(ALARM_SYS_MSG) < 0)
		return -1;
		// more alarm message driver API
	AlarmDrvExit();
	return 0;
}
* @endcode
*/
