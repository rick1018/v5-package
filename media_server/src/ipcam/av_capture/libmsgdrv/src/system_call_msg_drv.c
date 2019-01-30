/* ===========================================================================
* @file system_call_msg_drv.c
*
* @path $(IPNCPATH)\util
*
* @desc
* .
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */
#include <stdio.h>
#include <string.h>
#include <share_mem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <Msg_Def.h>
#include <system_call_msg_drv.h>

#ifdef DEBUG
#define DBG(fmt, args...)	printf("SystemCallMsgDrv: Debug\n" fmt, ##args)
#define ERR(fmt, args...)	printf("SystemCallMsgDrv: Error\n" fmt, ##args)
#else
#define DBG(fmt, args...)
#define ERR(fmt, args...)	
#endif

#define MSG_NOWAIT 1 //by Larry

static int gProcId = MSG_TYPE_MSG1;
static int g_SystemCallQid = -1;

void SetSystemCallProcId(int proc_id)
{
	if(proc_id < MSG_TYPE_MSG1 || proc_id >= MSG_TYPE_END){
		ERR("Error Process ID\n");
		gProcId = MSG_TYPE_MSG1;
		return;
	} else
		gProcId = proc_id;
}
/**
* @brief Save message ID in alarm message driver

* Save a message ID to alarm message driver. This ID will be used to identify
*  who is communicating with alarm manager. This function is old method, please
*  use AlarmDrvInit(int proc_id) instead.
* @param iqid [I ] message ID
*/
void SystemCallSaveQid(int iqid)
{
	g_SystemCallQid = iqid;
}
/**
* @brief Initialize alarm message driver.

* Initialize alarm message driver. Please see \ref ALARM_MSG_DRV_HOW to learn more.
* @note This API must be used before use any other alarm message driver API.
* @param proc_id [I ] Message ID(Which was define in \ref Alarm_Msg_Def.h) to initialize alarm message driver.
* @retval 0 Success.
* @retval -1 Fail.
*/
int SystemCallDrvInit(int proc_id)
{
	if((g_SystemCallQid = Msg_Init(SYSTEM_CALL_KEY)) < 0){
		ERR("SystemCallDrvInit - Message queue init fail\n");
		return -1;
	}
	gProcId = proc_id;
	return g_SystemCallQid;
}
/**
* @brief Cleanup alarm message driver.

* This API shoud be called at the end of process.
*/
int SystemCallDrvExit()
{
	gProcId = MSG_TYPE_MSG1;
	g_SystemCallQid = -1;
	return 0;
}
/**
* @brief Send quit command to alarm manager.

* This command will make alarm manager stop running. After you called this, all
*  the other process can't get system information because alarm manager is down.
*/
void SendSystemCallQuitCmd()
{
	SYSTEM_CALL_MSG_BUF msgbuf;
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = 0;
	msgbuf.event = SYSTEM_CALL_EVENT_QUIT;
	
	DBG("%s\n",__func__);
#if MSG_NOWAIT  //Add by Kenny Chiu 20121123
	msgsnd(g_SystemCallQid, &msgbuf, sizeof(msgbuf)-sizeof(long), IPC_NOWAIT);/*send msg1*/
#else
	msgsnd(g_SystemCallQid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
#endif
}
//RS 0429 Modify,Event_t

void SendSystemCallCmd(int index, System_Call_Event_t event, char* strCmd, int iCmdLen)
{
	SYSTEM_CALL_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = 0;
	msgbuf.event = event;
	msgbuf.index = index;
	msgbuf.iCmdLen = iCmdLen;
	strncpy(msgbuf.strCmd, strCmd, iCmdLen);
	
	/*fprintf(stderr,"%s %d %d (%d)%s\n",__func__, msgbuf.event, msgbuf.index, iCmdLen, msgbuf.strCmd);*/
#if MSG_NOWAIT  //by Larry
	msgsnd(g_SystemCallQid, &msgbuf, sizeof(msgbuf)-sizeof(long), IPC_NOWAIT);/*send msg1*/
#else
	msgsnd(g_SystemCallQid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
#endif
}

/**
* @brief Send  alarm reset trigger command to alarm manager
*/
void SendSystemCallResetTrigger()
{
	SYSTEM_CALL_MSG_BUF msgbuf;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.src = 0;
	msgbuf.event = SYSTEM_CALL_EVENT_RESET;
	
	DBG("%s\n",__func__);	
	msgsnd(g_SystemCallQid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
}

