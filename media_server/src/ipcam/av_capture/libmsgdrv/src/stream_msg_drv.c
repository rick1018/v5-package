/* ===========================================================================
* @file ApproDrvMsg.c
*
* @path $(IPNCPATH)/util/
*
* @desc Message driver for Appro_interface
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <string.h>
#include <stream_msg_drv.h>
#include <error.h>

//#include "cmem.h"
#include "sem_util.h"

static int gProcId = MSG_TYPE_MSG1;
static int qid;
static SemHandl_t hndlApproDrvSem = NULL;

#define PROC_MSG_ID	gProcId

#define ENABLE_CMEM	(1) //cato

//#define DEBUG
#ifdef __DEBUG
#define DBG(fmt, args...) fprintf(stderr, "[%s]%s(%d) Error: " fmt, __FILE__,__func__,__LINE__,## args)
#else
#define DBG(fmt, args...)
#endif
#define ERR(fmt, args...) fprintf(stderr, "[%s]%s(%d) Error: " fmt, __FILE__,__func__,__LINE__,## args)


/**
 * @brief set the message type id of the process
 *
 *
 * @param   proc_id    message type id of the proceess, defined at ipnc_app/include/Stream_Msg_Def.h
 *
 *
 *
 */
void ApproDrv_SetProcId(int proc_id)
{
	if(proc_id < MSG_TYPE_MSG1 || proc_id >= MSG_TYPE_END){
		gProcId = MSG_TYPE_MSG1;
		return;
	} else
		gProcId = proc_id;
}

/**
 * @brief set the message queue id of the process
 *
 *
 * @param   iqid    message queue id of the proceess
 *
 *
 *
 */
void ApproDrv_SaveQid(int iqid)
{
	qid = iqid;
}

int Testflag = 0;
/**
 * @brief Initiliztion of the message driver for Appro interface
 *
 *
 * @param   proc_id    message type id of the proceess, defined at ipnc_app/include/Stream_Msg_Def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int ApproDrvInit(int proc_id,int chn)
{
	fprintf(stderr, "%s: %x\n", __func__, hndlApproDrvSem);

	if(proc_id < MSG_TYPE_MSG1 || proc_id > MSG_TYPE_END){
		gProcId = MSG_TYPE_MSG1;
		return -1;
	}
	gProcId = proc_id;
	 
#if ENABLE_CMEM
	/* CMEM only one init is allowed in each process */
	if(Testflag==0)
	{
		if(CMEM_init(chn) < 0){
			gProcId = MSG_TYPE_MSG1;
			return -1;
		}
	}
#endif
	Testflag = 1;
	if(hndlApproDrvSem == NULL)
		hndlApproDrvSem = MakeSem();
	if(hndlApproDrvSem == NULL){
#if ENABLE_CMEM
		CMEM_exit();
#endif
		gProcId = MSG_TYPE_MSG1;
		return -1;
	}
	if((qid=Msg_Init(MSG_KEY)) < 0){
		DestroySem(hndlApproDrvSem);
		hndlApproDrvSem = NULL;
#if ENABLE_CMEM
		CMEM_exit();
#endif
		gProcId = MSG_TYPE_MSG1;
		return -1;
	}
	fprintf(stderr, "%s: gProcid:%d %d, %x\n", __func__, gProcId, qid,hndlApproDrvSem);	
	return 0;
}
/**
 * @brief Resource releasing of the message driver for Appro interface
 *
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int ApproDrvExit()
{
	fprintf(stderr, "%s: %d\n", __func__, gProcId);
	gProcId = MSG_TYPE_MSG1;
	DestroySem(hndlApproDrvSem);
	hndlApproDrvSem = NULL;
#if ENABLE_CMEM

	if( Testflag != 0 )
	{
	 	CMEM_exit();
	}
	Testflag = 0;;
	return 0;
#else
	return 0;
#endif
}

/**
 * @brief  Message driver for getting current frame information
 *
 *
* @param   fmt    stucture of frame information, defined at Msg_def.h
 *
 *
 *
 */
FrameInfo_t GetCurrentFrame(FrameFormat_t fmt)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_CUR_FRAME;
	msgbuf.frame_info.format = fmt;
	SemWait(hndlApproDrvSem);	
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
//    fprintf(stderr,"msgsend=%d, PROC_MSG_ID=%d, frame_info.format=%d\n",qid, PROC_MSG_ID, msgbuf.frame_info.format);			
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	if(msgbuf.ret != 0)
		msgbuf.frame_info.serial_no = -1;
	return msgbuf.frame_info;
}

/**
 * @brief  Message driver for waiting newframe (Do not use now)
 *
 *
* @param   fmt    stucture pointer of frame information, defined at Msg_def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
FrameInfo_t WaitNewFrame(FrameFormat_t fmt)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_NEW_FRAME;
	msgbuf.frame_info.format = fmt;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	if(msgbuf.ret != 0)
		msgbuf.frame_info.serial_no = -1;
	return msgbuf.frame_info;
}
/**
 * @brief  Message driver for getting memory information
 *
 *
 * @return MSG_MEM
 *
 *
 */
MSG_MEM GetPhyMem()
{
	MSG_BUF msgbuf;
	int timeout = 0;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_MEM;
	SemWait(hndlApproDrvSem);		
//do	
//{
 	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0); /*send msg1*/
#if 1	
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
#else
	while( (msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), msgbuf.Src, IPC_NOWAIT) < 0)  && (timeout++ < 3))
	{  
	  fprintf(stderr, "%s: msgbuf.Des =%d,msgbuf.Src =%d,msgbuf.cmd=%d\n",__func__,msgbuf.Des,msgbuf.Src,msgbuf.cmd); 
	  sleep(1);
  }
#endif	
//	fprintf(stderr,"[%s:%d][%d][%d] msgbuf.mem_info.addr = 0x%x, msgbuf.mem_info.size=%d\n",__func__,__LINE__,qid,timeout++,msgbuf.mem_info.addr,msgbuf.mem_info.size);
//}
//while(msgbuf.ret != 0);
//while(timeout <10);

	SemRelease(hndlApproDrvSem);
	
//	if(msgbuf.ret != 0)
//		msgbuf.mem_info.size = 0;

	return msgbuf.mem_info;
}

/**
 * @brief  Message driver for sending leave message (Do not use now)
 *
 *
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
void SendQuitCmd()
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = 0;
	msgbuf.cmd = MSG_CMD_QUIT;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	SemRelease(hndlApproDrvSem);

}
/**
 * @brief  Message driver for unLock frame
 *
 *
 * @param   pFrame    stucture pointer of frame information, defined at Msg_def.h
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
void UnlockFrame(FrameInfo_t *pFrame)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = 0;
	msgbuf.cmd = MSG_CMD_UNLOCK_FRAME;
	msgbuf.frame_info.format = pFrame->format;
	msgbuf.frame_info.serial_no = pFrame->serial_no;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	SemRelease(hndlApproDrvSem);
	return ;
}
/**
 * @brief  Message driver for Lock frame
 *
 *
 * @param   pFrame    stucture pointer of frame information, defined at Msg_def.h
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int LockFrame(FrameInfo_t *pFrame)
{
	int cnt = 0;
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_LOCK_FRAME;
	msgbuf.frame_info.format = pFrame->format;
	msgbuf.frame_info.serial_no = pFrame->serial_no;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	pFrame->size = msgbuf.frame_info.size;
	pFrame->width = msgbuf.frame_info.width;
	pFrame->height = msgbuf.frame_info.height;
	pFrame->offset = msgbuf.frame_info.offset;
	pFrame->quality = msgbuf.frame_info.quality;
	pFrame->flags = msgbuf.frame_info.flags;
	pFrame->frameType = msgbuf.frame_info.frameType;
	pFrame->timestamp = msgbuf.frame_info.timestamp;
	pFrame->temporalId = msgbuf.frame_info.temporalId;
	for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
	{
		pFrame->ref_serial[cnt] = msgbuf.frame_info.ref_serial[cnt];
	}

	return msgbuf.ret;
}
/**
 * @brief  Message driver for getting MPEG4 VOL data
 *
 *
 * @param   pVolBuf   ouput buffer for getting MPEG4 VOL data
 *
 * @param   fmt_type    frame type ID : FMT_MJPEG, FMT_MPEG4 , FMT_MPEG4_EXT, FMT_AUDIO, defined at Msg_def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int GetVolInfo(FrameInfo_t *pFrame, int fmt_type)
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_VOL;
	msgbuf.frame_info.format = fmt_type;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);

	pFrame->offset 	= msgbuf.frame_info.offset;
	pFrame->size 	= msgbuf.frame_info.size;

	return msgbuf.frame_info.size;
}

/**
 * @brief  Message driver for getting Media info
 *
 *
 * @param   pVolBuf   ouput buffer for getting Media info
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int GetMediaInfo(FrameInfo_t *pFrame)
{
	MSG_BUF msgbuf;
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_MEDIA_INFO;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);

	pFrame->format 	= msgbuf.frame_info.format;

	return msgbuf.frame_info.format;
}


/**
 * @brief  Message driver for Lock MPEG4 I-frame
 *
 *
 * @param   pFrame    stucture pointer of frame information, defined at Msg_def.h
 *
 * @param   fmt_type    frame type ID : FMT_MJPEG, FMT_MPEG4 , FMT_MPEG4_EXT, FMT_AUDIO, defined at Msg_def.h
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int LockMpeg4IFrame(FrameInfo_t *pFrame,int fmt_type)
{
	int cnt = 0;
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_AND_LOCK_IFRAME;
	msgbuf.frame_info.format = fmt_type;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	if(msgbuf.ret == -1)
		msgbuf.frame_info.serial_no = -1;
	else if(msgbuf.ret != 0)
		msgbuf.frame_info.serial_no = -2;
	pFrame->serial_no = msgbuf.frame_info.serial_no;
	pFrame->size = msgbuf.frame_info.size;
	pFrame->width = msgbuf.frame_info.width;
	pFrame->height = msgbuf.frame_info.height;
	pFrame->offset = msgbuf.frame_info.offset;
	pFrame->quality = msgbuf.frame_info.quality;
	pFrame->flags = msgbuf.frame_info.flags;
	pFrame->frameType = msgbuf.frame_info.frameType;
	pFrame->timestamp = msgbuf.frame_info.timestamp;
	pFrame->temporalId = msgbuf.frame_info.temporalId;
	for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
	{
		pFrame->ref_serial[cnt] = msgbuf.frame_info.ref_serial[cnt];
	}
	return msgbuf.frame_info.serial_no;
}

/**
 * @brief  day / night mode setting for UI
 *
 *
 * @param   nDayNight    0 : night mode 	1: day mode
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetDayNight(unsigned char nDayNight)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DAY_NIGHT;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nDayNight;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  Motion Enable mode setting for UI
 *
 *
 * @param   enableVal  0: Enable		1:Disable
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int ApproSetMotion(ApproMotionPrm* pMotionPrm)
{
	MSG_BUF msgbuf;
	void* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MOTION;
	ptr = (void*)&msgbuf.mem_info;
	memcpy(ptr, pMotionPrm, sizeof(ApproMotionPrm));
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SendFaceDetectMsg(FaceDetectParam* faceParam)
{
	MSG_BUF msgbuf;
	void* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_FDETECT;
	ptr = (void*)&msgbuf.mem_info;
	memcpy(ptr, faceParam, sizeof(FaceDetectParam));
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;

}
int SetDateTimeFormat(DateTimePrm* datetimeParam)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DATETIMEPRM;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, datetimeParam, sizeof(DateTimePrm));
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetOSDPrmMsg(int id, OSDPrm* osdprm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	if(id==0)
		msgbuf.cmd = MSG_CMD_SET_OSDPRM1;
	else if(id==1)
		msgbuf.cmd = MSG_CMD_SET_OSDPRM2;
	else if(id==2)
		msgbuf.cmd = MSG_CMD_SET_OSDPRM3;

	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, osdprm, sizeof(OSDPrm));
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetCodecROIMsg(int id, CodecROIPrm* codecROIPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	if(id==0)
		msgbuf.cmd = MSG_CMD_SET_ROIPRM1;
	else if(id==1)
		msgbuf.cmd = MSG_CMD_SET_ROIPRM2;
	else if(id==2)
		msgbuf.cmd = MSG_CMD_SET_ROIPRM3;

	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, codecROIPrm, sizeof(CodecROIPrm));
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetCodecAdvPrmMsg(int id, CodecAdvPrm* codecAdvPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	if(id==0)
		msgbuf.cmd = MSG_CMD_SET_ADVCODECPRM1;
	else if(id==1)
		msgbuf.cmd = MSG_CMD_SET_ADVCODECPRM2;
	else if(id==2)
		msgbuf.cmd = MSG_CMD_SET_ADVCODECPRM3;

	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, codecAdvPrm, sizeof(CodecAdvPrm));
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/* 20140225 */
/* OVERLAY_IMAGE_FEATURE - Dean - 20130709 - Begin */
int SetOverlayImageInfo(OverlayImagePrm* pOverlayImagePrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_OVERLAY_IMAGE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, pOverlayImagePrm, sizeof(OverlayImagePrm));
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
/* OVERLAY_IMAGE_FEATURE - Dean - 20130709 - End */

#if PRIVACY_MASK_FEATURE
/* PRIVACY_MASK_FEATURE - Dean - 20130801 - Begin */
int SetPrivacyMaskInfo(PrivacyMaskPrm* pPrivacyMaskPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PRIVACY_MASK;
	ptr = (unsigned char*)&msgbuf.pm_info;
	memcpy(ptr, pPrivacyMaskPrm, sizeof(PrivacyMaskPrm));
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
/* PRIVACY_MASK_FEATURE - Dean - 20130801 - End */
#endif
/* TAMPER_FEATURE - Dean - 20131007 - Begin */
#if TAMPER_FEATURE
int SetTamperInfo(TamperPrm* pTamperPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_TAMPER;
	ptr = (unsigned char*)&msgbuf.tamper_info;
	memcpy(ptr, pTamperPrm, sizeof(TamperPrm));
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
/* TAMPER_FEATURE - Dean - 20131007 - End */
/* TAMPER_FEATURE Event - Dean - 20131007 - Begin */
int SetEventInfo(EventPrm* pEventPrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EVENT;
	ptr = (unsigned char*)&msgbuf.event_info;
	memcpy(ptr, pEventPrm, sizeof(EventPrm));
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
/* TAMPER_FEATURE Event - Dean - 20131007 - End */
#endif
/* 20140225 */

/**
 * @brief  indoor / outdoor mode setting for UI
 *
 *
 * @param   nValue    0 : indoor	1: outdoor	2: Auto
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetWhiteBalance(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WHITE_BALANCE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetDisplayValue(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_LOCALDISPLAY;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetOSDEnable(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_OSDENABLE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  back light setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetBacklight(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_BACKLIGHT;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
/**
 * @brief  brightness setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetBrightness(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_BRIGHTNESS;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
/**
 * @brief  contrast setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetContrast(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_CONTRAST;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  Saturation setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetSaturation(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_SATURATION;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
/**
 * @brief  Sharpness setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetSharpness(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_SHARPNESS;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetSysRes(unsigned char stream, unsigned int nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	if (stream == 0)
		msgbuf.cmd = MSG_CMD_SET_RESOLUTION1;
	else if (stream == 1)
		msgbuf.cmd = MSG_CMD_SET_RESOLUTION2;
	else if (stream == 2)
		msgbuf.cmd = MSG_CMD_SET_RESOLUTION3;
	else
		msgbuf.cmd = MSG_CMD_SET_RESOLUTION4;

	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int Setdewarpmode(unsigned char stream, unsigned int nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	if (stream == 0)
		msgbuf.cmd = MSG_CMD_SET_MAIN_DEWARPMODE;
	else if (stream == 2)
		msgbuf.cmd = MSG_CMD_SET_3rd_DEWARPMODE;
	else
		return -1;//error

	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
/**
 * @brief  Bitrate setting for UI
 *
 * modify by frank for Geo
 * @param   stream    	0: stream1 1080P \n
*				1: stream2 D1 \n
*				2: stream3 720P
*				3: JPEG D1
*
 * @param   nValue    	128~6000
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetEncBitrate(unsigned char stream, int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	if (stream == 0)
		msgbuf.cmd = MSG_CMD_SET_BITRATE1;
	else if (stream == 1)
		msgbuf.cmd = MSG_CMD_SET_BITRATE2;
	else
		msgbuf.cmd = MSG_CMD_SET_BITRATE3;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  frame rate setting for UI
 *
 * modify by frank for Geo
 * @param   stream    	0: stream1 H264 1080P \n
*				1: stream2 Mobile/sub D1 \n
*				2: stream3 ePTZ 720P \n
*				3: stream4 JPEG D1\m
*
 * @param   nValue    	1000~30000 ( 1fps ~30fps )
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetStreamFramerate(unsigned char stream, unsigned int nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	if (stream == 0)
		msgbuf.cmd = MSG_CMD_SET_FRAMERATE1;
	else if (stream == 1)
		msgbuf.cmd = MSG_CMD_SET_FRAMERATE2;
	else if (stream == 2)
		msgbuf.cmd = MSG_CMD_SET_FRAMERATE3;
	else
		msgbuf.cmd = MSG_CMD_SET_FRAMERATE4;

	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/* 20140225 */
int SetEncFramerate(unsigned char stream, int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	if (stream == 0)
		msgbuf.cmd = MSG_CMD_SET_M41_FRAMERATE;
	else if (stream == 1)
		msgbuf.cmd = MSG_CMD_SET_M42_FRAMERATE;
	else
		msgbuf.cmd = MSG_CMD_SET_JPG_FRAMERATE;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
//Add by Joseph 20131118
#if GOP_FEATURE	//Add by Joseph 20131119
int SetM41Gop(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;
	fprintf(stderr, "SetM41Gop %d\n", nValue);
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_M41_GOP;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#endif
/* 20140225 */

/**
 * @brief  frame rate setting for UI
 *
 *
 * @param   nValue    	7500~30000 ( 7.5fps ~30fps )
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetJpgQuality(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_JPEG_QUALITY;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/* 20140225 */
#if ROTATION_FEATURE
int SetRot(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_ROTATION;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf,sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#endif
#if FDETECT_FEATURE
int SetFDetect(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_FDETECT;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#endif
/* 20140225 */

int SetMirr(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MIRROR;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetOSDWindow(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_OSDWINDOW;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  2A enable/disable setting for UI
 *
 *
 * @param   nValue    	0: disable \n
 *				1: enable
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetImage2AType(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_TYPE_2A;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int Set2APriority(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_2A_PRIORITY;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/* 20140225 */
int SetImage2A(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SWITCH_2A;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
/* 20140225 */

/**
 * @brief  TV setting for UI
 *
 *
 * @param   nValue    	0: NTSC \n
 *				1: PAL
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetTvSystem(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_TV_SYSTEM;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#if IPNCPTZ_FEATURE
/**
 * @brief  PTZ  setting  for UI
 *
 *
 * @param   nValue    	1: PTZ_PANTILT -move(UI)\n
*					2: PTZ_PANTILT - moveto(OCX)\n
*					3: PTZ_AUTOTRACKING start \n
*					4: PTZ_AUTOTRACKING stop \n
*					5: PTZ_ROATE \n
*					6: PTZ_ZOOM\n
*					7: PTZ_Patrol start\n
*					8: PTZ_Patrol stop \n
*					9: PTZ_SETHOME\n
*					10:PTZ_GOHOME\n
*					11:PTZ_GOPRESET\n
*					12:PTZ_RESET-set default home\n
*					13:PTZ_Patorl speed\n
* @return 0 is ok and -1 is error
*
*
*/
int SetPtz(PTZPrm *ptz)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_PTZ;
	ptr = (unsigned char*)&msgbuf.mem_info;//Because the PTZPrm size(3 int) <= meminfo(2 long), so it's ok
	memcpy(ptr, ptz, sizeof(PTZPrm));
	//*ptr = nValue;
	if(hndlApproDrvSem<=0)
		fprintf(stderr, "*****error hndlApproDrvSem = 0\n"); 
	SemWait(hndlApproDrvSem);
	
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int GetPtz_Preset(PresetList *ptz_list)
{
	MSG_BUF msgbuf;
	//unsigned char* ptr;
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PTZ_PRESET;
	//msgbuf.mem_info = prsetlist;
	//ptr = (unsigned char*)&msgbuf.mem_info;
	//memcpy(ptr, ptz, sizeof(PTZPrm));
	//*ptr = nValue;
	if(hndlApproDrvSem<=0)
		fprintf(stderr, "*****error hndlApproDrvSem = 0\n"); 
	SemWait(hndlApproDrvSem);
	
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	memcpy(ptz_list, &msgbuf.prsetlist, sizeof(PresetList));
	SemRelease(hndlApproDrvSem);
	#if 1
	int i;

	for(i=0; i<MAX_PRESET_LIST_SIZE; i++)
	{
		//temp = &list->entry[i];
		fprintf(stderr, "GetPtz_Preset name %016s, enabled %d, x %06d, y %06d, dwell %03d\n",  ptz_list->entry[i].name, ptz_list->entry[i].enabled, ptz_list->entry[i].x, ptz_list->entry[i].y, ptz_list->entry[i].dwell);
	}	
	#endif
	return msgbuf.ret;
}
int GetPtz_Patrol(PatrolList *ptz_list)
{
	MSG_BUF msgbuf;
	int ret;
	//unsigned char* ptr;
	//PatrolList patrollist;
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PTZ_PATROL;
	//msgbuf.mem_info = &patrollist;
	//ptr = (unsigned char*)&msgbuf.mem_info;
	//memcpy(ptr, ptz, sizeof(PTZPrm));
	//*ptr = nValue;
	if(hndlApproDrvSem<=0)
		fprintf(stderr, "*****error hndlApproDrvSem = 0\n"); 
	SemWait(hndlApproDrvSem);
	//fprintf(stderr, "PROC_MSG_ID before:%d qid\n",PROC_MSG_ID); 
	
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	//fprintf(stderr, "PROC_MSG_ID after:%d qid\n",PROC_MSG_ID);
	ret = msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	memcpy( ptz_list, &msgbuf.patrolist, sizeof(PatrolList));
	SemRelease(hndlApproDrvSem);
	//fprintf(stderr, "PROC_MSG_ID:%d qid: %d ret:%d msgbuf.ret:%d\n", PROC_MSG_ID, qid, ret, msgbuf.ret);
#if 1
	int i, j = 0;
				//PatrolEntry *temp = 0;
				//fprintf(stderr, "\n");				
	for(i=0; i<4; i++)
	{
		//temp = &ptz_list.entry[i];
		fprintf(stderr, "GetPtz_Patrol- name %s, \n",  ptz_list->entry[i].name);

		for(j=0; j<8; j++)
			fprintf(stderr, "%d ", ptz_list->entry[i].order[j]);

		fprintf(stderr, ", size = %d\n", ptz_list->entry[i].size);
	}
#endif	
	return msgbuf.ret;
}
int PTGetStatus(PTData *pData)
{
	MSG_BUF msgbuf;
	int ret;
	//unsigned char* ptr;
	//PatrolList patrollist;
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_GET_PTZ_STATUS;
	SemWait(hndlApproDrvSem);	
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid,& msgbuf,sizeof(msgbuf)- sizeof(long),PROC_MSG_ID,0);
	memcpy( pData, &msgbuf.ptdata, sizeof(PTData));
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int PresetUpdate(int index, PresetEntry* entry)
{
	MSG_BUF msgbuf;
	int ret;
	unsigned char* ptr;
	PresetEntry *temp1 = 0;
	//PatrolList patrollist;
	//fprintf(stderr, "ADM:PresetUpdate1 name %016s, enabled %d, x %06d, y %06d, dwell %03d\n",  entry->name, entry->enabled, entry->x, entry->y, entry->dwell);
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PTZ_PRESET;
	memcpy(&msgbuf.prsetlist.entry, entry, sizeof(PresetEntry));
	{
	
			temp1 = &msgbuf.prsetlist.entry;//&list.entry[i];
	fprintf(stderr, "ADM:PresetUpdate2 name %016s, enabled %d, x %06d, y %06d, dwell %03d\n",  temp1->name, temp1->enabled, temp1->x, temp1->y, temp1->dwell);
		}	
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = index;
	SemWait(hndlApproDrvSem);	
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid,& msgbuf,sizeof(msgbuf)- sizeof(long),PROC_MSG_ID,0);

	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;	
}
int PatrolUpdate(int index, PatrolEntry* entry)
{
	MSG_BUF msgbuf;
	int ret;
	unsigned char* ptr;
	//PatrolList patrollist;
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PTZ_PATROL;
	memcpy(&msgbuf.patrolist.entry, entry, sizeof(PatrolEntry));
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = index;
	SemWait(hndlApproDrvSem);	
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid,& msgbuf,sizeof(msgbuf)- sizeof(long),PROC_MSG_ID,0);

	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;	

}


#endif

/**
 * @brief  BinningSkip  setting  for UI
 *
 *
 * @param   nValue    		0: Binning \n
*					1: Skipping \n
*
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
 #if BINNING_FEATURE
int SetBinningSkip(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_BINNING_SKIP;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#endif

/**
 * @brief  time stamp  setting  for UI
 *
 *
 * @param   nValue    		0: timestamp open \n
*					1: timestamp close \n
*
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetTStamp(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_TSTAMP;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetVNFStatus(unsigned char stream, unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	if (stream == 0)
		msgbuf.cmd = MSG_CMD_SET_VNFSTATUS1;
	else if (stream == 1)
		msgbuf.cmd = MSG_CMD_SET_VNFSTATUS2;
	else if (stream == 2)
		msgbuf.cmd = MSG_CMD_SET_VNFSTATUS3;

	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetROIConfig(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_ROICFG;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/* 20140225 */
unsigned int EnMaskGen(unsigned char value)
{
	unsigned int tMask = 0;
	
	if(value == 0) {
		fprintf(stderr, "NONE.\n");
		return 0; }
	if ((value & FFLAG_VS) > 0) {
		fprintf(stderr, "VS.");
		tMask |= (1<<ADV_FEATURE_VS); }
	if ((value & FFLAG_LDC) > 0) {
		fprintf(stderr, "LDC.");
		tMask |= (1<<ADV_FEATURE_LDC); }
	if ((value & FFLAG_SNF) > 0) {
		fprintf(stderr, "SNF.");
		tMask |= (1<<ADV_FEATURE_SNF); }
	if ((value & FFLAG_TNF) > 0) {
		fprintf(stderr, "TNF.");
		tMask |= (1<<ADV_FEATURE_TNF); }
	fprintf(stderr, "\n");
	return tMask;
}

int SetStreamAdvFeature(unsigned char stream, unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned int* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	unsigned int EnMask = EnMaskGen(nValue);

	if (stream == 0)
		msgbuf.cmd = MSG_CMD_SET_M41_ADV_FEATURE;
	else if (stream == 1)
		msgbuf.cmd = MSG_CMD_SET_M42_ADV_FEATURE;
	else
		msgbuf.cmd = MSG_CMD_SET_JPG_ADV_FEATURE;

	ptr = (unsigned int*)&msgbuf.mem_info;
	*ptr = EnMask;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

#if FDETECT_FEATURE
int SetAvFaceDetect(unsigned char nValue)
{
	MSG_BUF msgbuf;
	FACE_PARM* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = MSG_CMD_SET_FDETECT;

	ptr = (FACE_PARM*)&msgbuf.mem_info;
	switch (nValue)
	{
		case 0:
		{
			ptr->type = FACE_NO_DETECT;
			break;
		}
		case 1:
		{
			ptr->type = FACE_DETECT;
			break;
		}
		case 2:
		{
			ptr->type = FACE_MASK;
			break;
		}
		case 3:
		{
			ptr->type = FACE_RECOGIZE;
			break;
		}
	}
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#else

int SetOvDetect(int nValue)  //OV STATUS,RS,200911120
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_FDETECT; //USED FOR OV
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#endif

#if IVS_FEATURE
int SetIVSDetect(int nValue)  //OV STATUS,RS,200911120
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_IVSDETECT; //USED FOR IVS
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int IVSSetParm(int ioption, char* sData, int iMsgCmd)
{
	MSG_BUF msgbuf;
	IVS_PARM* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = iMsgCmd;

	ptr = (IVS_PARM*)&msgbuf.ivs_info;

	ptr->option = ioption;
	//fprintf(stderr,"\n IVSSetParm - ptr->option = %d\n", ptr->option);

	if (sData != NULL) memcpy(ptr->strings, sData, sizeof(ptr->strings));
	//if (sData != NULL) fprintf(stderr,"\n IVSSetParm - ptr->strings = %s, sizeof(ptr->strings) = %d\n", ptr->strings, sizeof(ptr->strings));

	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int IVSGetParm(int ioption, char* sData, int iMsgCmd)
{
	MSG_BUF msgbuf;
	
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = iMsgCmd;
	//msgbuf.extra_data = sData;
	msgbuf.ivs_info.option = ioption;
	memset(msgbuf.ivs_info.strings, 0, sizeof(msgbuf.ivs_info.strings));
	//fprintf(stderr,"\n IVSGetParm - sData = %s - %d\n", sData, sizeof(sData));
	//fprintf(stderr,"\n IVSGetParm - msgbuf.ivs_info.strings = %s - %d\n", msgbuf.ivs_info.strings, sizeof(msgbuf.ivs_info.strings));
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	//fprintf(stderr,"\n IVSGetParm - msgbuf.ivs_info.strings = %s - %d\n", msgbuf.ivs_info.strings, strlen(msgbuf.ivs_info.strings));
	if (sData != NULL) 
	{
		memcpy(sData, msgbuf.ivs_info.strings, strlen(msgbuf.ivs_info.strings));
		//fprintf(stderr,"\n IVSGetParm - sData = %s - %d\n", sData, strlen(sData));
	}
	
	return msgbuf.ret;
}
#endif // IVS_FEATURE
/* 20140225 */

int SetAvOsdTextEnable(int enable)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = MSG_CMD_SET_OSDTEXT_EN;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = enable;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetHistEnable(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_HIST_EN;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetAudio_InVolume(unsigned char nValue) //Add by Joseph 20140516
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AUDIO_INVOLUME;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetAACBitrate(int nValue)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AAC_BITRATE;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/* 20140225 */
#if 0
int SetAvOsdLogoEnable(int enable)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = MSG_CMD_SET_OSDLOGO_EN;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = enable;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#else
int  SetAvOsdLogoEnable(OSDPrm* pOSDPrm)//RS,20091231,MDY
{
	MSG_BUF msgbuf;
	void* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_OSDLOGO_EN;
	ptr = (void*)&msgbuf.mem_info;
	memcpy(ptr, pOSDPrm, sizeof(OSDPrm));
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#endif
/* 20140225 */

int SetAvOsdText(char* strText, int nLength)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = MSG_CMD_SET_OSDTEXT;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = nLength;
	memcpy(++ptr, strText, nLength);
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetClipSnapName(char* strText, int nLength)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;

	msgbuf.cmd = MSG_CMD_SET_CLICKNAME;

	ptr = (int*)&msgbuf.mem_info;
	*ptr = nLength;
	memcpy(++ptr, strText, nLength);
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetClipSnapLocation(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_CLICKSTORAGE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SendForceIFrameMsg(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_IFRAME;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetAudioAlarmValue(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AUDIO_ALARM_LEVEL;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetAudioAlarmFlag(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AUDIO_ALARM_FLAG;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetDynRangePrmMsg(DynRangePrm* dynRangePrm)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DYNRANGE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	memcpy(ptr, dynRangePrm, sizeof(DynRangePrm));
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

#if 0
int ApproPollingMsg()
{
	MSG_BUF msgbuf;
	int ret;
	int timeout = 0;
	
	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_POLLING;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	fprintf(stderr, "ApproPollingMsg:qid:%x\n", qid);
	ret = msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, IPC_NOWAIT);
	if(ret >= 0){
  	while(msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, IPC_NOWAIT) >= 0);
		ret = msgbuf.ret;
	}	
	SemRelease(hndlApproDrvSem);
	return ret;
}
#endif

int SetWDTResetMsg(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WDTRESET;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);/*send msg1*/
	msgrcv( qid, &msgbuf,sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  Iris mode setting for UI
 *
 *
 * @param   nIrisMode    0 : auto ; 1: manual
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetAvIrisMode(unsigned char nIrisMode)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	printf("SetAvIrisMode = %d\n", nIrisMode);
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_IRIS_MODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nIrisMode;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  WDR mode setting for UI
 *
 *
 * @param   nWDRMode    0 : disable ; 1: Enable
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetAvWDRMode(unsigned char nWDRMode)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	printf("SetAvWDRMode = %d\n", nWDRMode);
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_WDR_MODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nWDRMode;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  day / night mode IRCut detecting
 *
 *
 * @param   nDayNight    0 : night mode 	1: day mode
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetAvIRCutStatus(unsigned char nDayNight)
{
#if IPNC_IRCUT_FEATURE || IPNC_IRDETECT_FEATURE
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_DAY_NIGHT; //MSG_CMD_SET_IRCUT_STATUS;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nDayNight;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
#else
	return 0;
#endif
}

int SetExposureMode(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EXPOSUREMODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetExposureValue(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EXPOSUREVALUE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetExposureValue1(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EXPOSUREVALUE1;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  AGC setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetAGC(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AGC;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetAGCMin(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AGC_MIN;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetAGC1(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AGC1;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetAGCMin1(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AGC_MIN1;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  Shutter Speed setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetShutterTime(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_SHUTTER_TIME;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetShutterTimeMin(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_SHUTTER_TIME_MIN;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetShutterTime1(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_SHUTTER_TIME1;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetShutterTimeMin1(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_SHUTTER_TIME_MIN1;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

/**
 * @brief  Flickerless Mode setting for UI
 *
 *
 * @param   nValue    	=128: median \n
*				>128: hight \n
*				<128: low \n
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int SetFlickerlessMode(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_FLICKERLESS_MODE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetColorBar(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_COLORBAR;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
int SetAewbReset(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AEWB_RESET;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetMobileFramerate(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MOBILE_FRAMERATE;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetMobileRateControl(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MOBILE_RATECONTROL;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetMobileQuality(unsigned char nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MOBILE_QUALITY;
	ptr = (unsigned char*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetMobileBitRate(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_MOBILE_BITRATE;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetAutoirisAction(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AUTOIRIS_ACTION;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetTvoutEnable(int nValue)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_TVOUT_ENABLE;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = nValue;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetAudioBroadcastPort(unsigned short port)
{
	MSG_BUF msgbuf;
	int* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_AUDIO_BROADCAST_PORT;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = port;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetEWdrMode(unsigned char value)
{
	MSG_BUF msgbuf;
	unsigned char* ptr;

	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_EWDR_MODE;
	ptr = (int*)&msgbuf.mem_info;
	*ptr = value;
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
/* 20140225 */

#if 0
int SetOverlayImageInfo(OverlayImagePrm* pOverlayImagePrm)
{
	MSG_BUF msgbuf;
	void* ptr;

	DBG("%s\n", __func__);
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_OVERLAY_IMAGE;
	ptr = (void*)&msgbuf.mem_info;
	memcpy(ptr, pOverlayImagePrm, sizeof(OverlayImagePrm));
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}

int SetPrivacyMaskInfo(PrivacyMaskPrm* pPrivacyMaskPrm)
{
	MSG_BUF msgbuf;
	void* ptr;

	DBG("%s\n", __func__);
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_PRIVACY_MASK;
	ptr = (PrivacyMaskPrm*)&msgbuf.pm_info;
	memcpy(ptr, pPrivacyMaskPrm, sizeof(PrivacyMaskPrm));
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#endif

#if TAMPER_FEATURE
int SetTamperInfo(TamperPrm* pTamperPrm)
{
	MSG_BUF msgbuf;
	void* ptr;

	DBG("%s\n", __func__);
	memset(&msgbuf, 0, sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = PROC_MSG_ID;
	msgbuf.cmd = MSG_CMD_SET_TAMPER;
	ptr = (TamperPrm*)&msgbuf.tamper_info;
	memcpy(ptr, pTamperPrm, sizeof(TamperPrm));
	SemWait(hndlApproDrvSem);
	msgsnd(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), 0);/*send msg1*/
	msgrcv(qid, &msgbuf, sizeof(msgbuf)-sizeof(long), PROC_MSG_ID, 0);
	SemRelease(hndlApproDrvSem);
	return msgbuf.ret;
}
#endif

