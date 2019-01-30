/** ===========================================================================
* @file stream.c
*
* @path $(IPNCPATH)\multimedia\encode_stream\stream
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "display.h"
#include "aewb_xdm.h"
#include "stream.h"
//#include "sample_comm.h"
//#include "IC3240_common.h"
#include "file_msg_drv.h" 

typedef struct _OsdTextPrm{
	int nLegnth;
	char strText[16];
}OsdTextPrm;

//#define __STREAM_DEBUG
#ifdef __STREAM_DEBUG
#define __D(fmt, args...) fprintf(stderr, "Stream Debug: " fmt, ## args)
#else
#define __D(fmt, args...)
#endif

#define __E(fmt, args...) fprintf(stderr, "Stream Error: " fmt, ## args)

static int Init_Msg_Func(STREAM_PARM *pParm);

static STREAM_PARM stream_parm;
//extern SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
//SAMPLE_VENC_GETSTREAM_PARA_S gs_stPara;
void stream_feature_setup( int nFeature, void *pParm );
//extern AEW_EXT_PARAM Aew_ext_parameter;
AEW_EXT_PARAM Aew_ext_parameter;

#if 0
void hexdump(void *mem, unsigned int len)
{
	unsigned int i, j;
	unsigned int HEXDUMP_COLS = 16;

	for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
	{
		/* print offset */
		if(i % HEXDUMP_COLS == 0)
		{
			printf("0x%06x: ", i);
		}

		/* print hex data */
		if(i < len)
		{
			printf("%02x ", 0xFF & ((char*)mem)[i]);
		}
		else /* end of block, just aligning for ASCII dump */
		{
			printf("   ");
		}

		/* print ASCII dump */
		if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
		{
			for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
			{
				if(j >= len) /* end of block, not really printing */
				{
					putchar(' ');
				}
				else if(isprint(((char*)mem)[j])) /* printable char */
				{
					putchar(0xFF & ((char*)mem)[j]);        
				}
				else /* other char */
				{
					putchar('.');
				}
			}
			putchar('\n');
		}
	}
}
#endif

int FindH264FrameType(unsigned char *buffer)
{
	int frameType = EMPTY_FRAME, streamType = -1; //STREAM_MJPG;  //STREAM_MJPG, STREAM_H264_1,STREAM_H264_2;  /*P_FRAME*/,
	unsigned char *nType = buffer+4;  //00 00 01 (x5/x1)
	
//	printf("[%s]%s(%d) %p %p \n",__FILE__,__func__,__LINE__,buffer,nType);			
	while( nType++ < buffer + 300) {
        if (*nType == 0x27 || *nType == 0x28 || *nType == 0x45 || *nType == 0x65 || *nType == 0x67 || *nType == 0x68)			
		{
			frameType = I_FRAME;
			break;
		}
        else if (*nType == 0x21 || *nType == 0x41 || *nType == 0x61)
		{
			frameType = P_FRAME;
			break;
		}        
        else
			frameType = P_FRAME;//  //may be 00 00 00 01 09 10 00 00 00 01 67			
     }      
     printf("[%s]%s(%d) %d\n",__FILE__,__func__,__LINE__,frameType);			
     return frameType;
}

/**
 * @brief	Show info
 * @param	"STREAM_PARM *pParm" : stream parameter
 * @param	"int id"
 * @return	none
 */
void ShowInfo(STREAM_PARM *pParm,int id)
{
    int i;	
	VIDEO_BLK_INFO 	*pVidInfo = NULL;

	__D("ID = %d \n",id);
	__D("pParm = %p \n",pParm);
	__D("pParm->qid = %d \n",pParm->qid);
	__D("pParm->ImageWidth = %d \n",pParm->ImageWidth);
	__D("pParm->ImageHeight = %d \n",pParm->ImageHeight);	
	for(i=0;i<STREAM_EXT_NUM;i++) {
	 __D("pParm->ImageWidth_Ext[%d] = %d \n",i,pParm->ImageWidth_Ext[i]);
 	 __D("pParm->ImageHeight_Ext[%d] = %d \n",i,pParm->ImageHeight_Ext[i]);		
	}

	__D("pParm->JpgQuality = %d \n",pParm->JpgQuality);
	__D("pParm->Mpeg4Quality = %d \n",pParm->Mpeg4Quality);

	for(i=0;i<VIDEO_INFO_END;i++)
	{
 	  pVidInfo = &pParm->MemInfo.video_info[i];
      if (pVidInfo != NULL) __D("pVidInfo[%d]=%p video_type=%d cur_frame=%d cur_serial=%d\n",i,pVidInfo,pVidInfo->video_type,pVidInfo->cur_frame,pVidInfo->cur_serial);	
	}
}

/**
 * @brief	Get stream handle
 * @param	none
 * @return	pointer to stream_parm
 */
STREAM_PARM *stream_get_handle(void)
{
	return &stream_parm;
}

/**
 * @brief	Stream initialization
 * @param	"STREAM_PARM *pParm" : stream parameter
 * @param	"STREAM_SET *pSet"
 * @return	0 : success ;
 */
int stream_init(STREAM_PARM *pParm , STREAM_SET *pSet)
{
	int ret = 0;
	int cnt = 0;

	/* Init memory manager */
	pParm->MemInfo.chn = pSet->chn;
	pParm->MemInfo.mem_layout = pSet->Mem_layout;	
	
	if(MemMng_Init( &(pParm->MemInfo) ) != STREAM_SUCCESS)
	{
		__E("Memory manager init fail\n");
		return STREAM_FAILURE;
	}	

	if(CacheMng_Init( &(pParm->MemInfo) ) != STREAM_SUCCESS)
	{
		__E("Cache manager init fail\n");
		return STREAM_FAILURE;
	}

	GopInit();

    __D("%s[%d]:MemInfo =%x,%x,%x\n",__func__,__LINE__,pParm->MemInfo.start_addr,pParm->MemInfo.start_phyAddr,pParm->MemInfo.totalsize);			
	for( cnt = 0; cnt < STREAM_SEM_NUM; cnt++ )
	{
	  pParm->MemMngSemId[cnt] = Sem_Creat(cnt);
      __D("%s[%d]:MemInfo->video_info[%d]=%x,%x,%x\n",__func__,__LINE__,cnt,pParm->MemInfo.video_info[cnt].start,pParm->MemInfo.video_info[cnt].start_phy,pParm->MemInfo.video_info[cnt].size);			
	}

	for( cnt = 0; cnt < GOP_INDEX_NUM; cnt++ )
	{
		pParm->lockNewFrame[cnt] 	= 0;
		pParm->checkNewFrame[cnt]	= 0;
		Rendezvous_open(&(pParm->objRv[cnt]), 2);
	}
	pParm->IsQuit	= 0;
	pParm->qid 		= Msg_Init(MSG_KEY);

	pParm->ImageWidth = pSet->ImageWidth;
	pParm->ImageHeight = pSet->ImageHeight;

	for( cnt = 0; cnt < STREAM_EXT_NUM ; cnt++ )
	{
		pParm->ImageWidth_Ext[cnt]	= pSet->ImageWidth_Ext[cnt];
		pParm->ImageHeight_Ext[cnt]	= pSet->ImageHeight_Ext[cnt];
	}
	pParm->JpgQuality = pSet->JpgQuality;
	pParm->Mpeg4Quality	= pSet->Mpeg4Quality;

	ret = Init_Msg_Func(pParm);
	if( ret < 0 )
	{
		__E("Init_Msg_Func init fail\n");
		return STREAM_FAILURE;
	}
	
	memset(&Aew_ext_parameter, 0, sizeof(AEW_EXT_PARAM));
	return STREAM_SUCCESS;
}

/**
 * @brief	Stream write
 * @param	"void *pAddr"
 * @param	"int size"
 * @param	"int frame_type"
 * @param	"int stream_type"
 * @param	"STREAM_PARM *pParm" : stream parameter
 * @return	0 : success ; -1 : fial
 */
int stream_write(void *pAddr, int size, int frame_type ,int stream_type ,unsigned int timestamp ,unsigned int temporalId ,STREAM_PARM *pParm)
{
	VIDEO_BLK_INFO 	*pVidInfo = NULL;

	int ret = -1;
	int sem_id = 0;
	int	cnt = 0;
	if (pParm->IsPause == 1) return STREAM_SUCCESS;

	if(pParm->IsQuit == 1)	return STREAM_FAILURE;

	switch( stream_type )
	{
		case STREAM_H264_1:
		case STREAM_MP4:
			pVidInfo = &(pParm->MemInfo.video_info[VIDEO_INFO_MP4]);
			sem_id	 = pParm->MemMngSemId[STREAM_SEM_MPEG4];
		break;

		case STREAM_MJPG:
			pVidInfo = &(pParm->MemInfo.video_info[VIDEO_INFO_MJPG]);
			sem_id	 = pParm->MemMngSemId[STREAM_SEM_JPEG];
		break;
		
		case STREAM_H264_2:
		case STREAM_MP4_EXT:
			pVidInfo = &(pParm->MemInfo.video_info[VIDEO_INFO_MP4_EXT]);
			sem_id	 = pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
		break;
		case STREAM_H264_3:
			pVidInfo = &(pParm->MemInfo.video_info[VIDEO_INFO_H264]);
			sem_id	 = pParm->MemMngSemId[STREAM_SEM_H264];
			break; 
		case STREAM_AUDIO:
			pVidInfo = &(pParm->MemInfo.video_info[AUDIO_INFO_G711]);
			sem_id	 = pParm->MemMngSemId[STREAM_SEM_AUDIO];
		break;
		case STREAM_AUDIO_PCM:
			pVidInfo = &(pParm->MemInfo.video_info[AUDIO_INFO_PCM]);
			sem_id	 = pParm->MemMngSemId[STREAM_SEM_AUDIO_PCM];
			break; 

		default:
		break;
	}

//	char *paddr = (char *)pAddr; 	printf( "Video_type = %d , pAddr[0] : %x, pAddr[1]: %x \n",pVidInfo->video_type,*paddr,*(paddr+1));
//	if(stream_type == STREAM_MJPG) fprintf(stderr,"stream_type : %d, frame_type : %d, size : %d,  timestamp : %u \n",stream_type, frame_type, size, timestamp);
//	__D("\n[%d] pVidInfo=%p width=%d height=%d cur_frame=%d cur_serial=%d\n",__LINE__,pVidInfo,pVidInfo->width,pVidInfo->height,pVidInfo->cur_frame,pVidInfo->cur_serial);

	if( stream_type == STREAM_H264_1 || stream_type == STREAM_H264_2 )
	{
		//pVidInfo->extrasize = 14;
	}
	else if( stream_type == STREAM_MP4 || stream_type == STREAM_MP4_EXT)
	{
		//pVidInfo->extrasize = 18;
	}else{
		pVidInfo->extrasize = 0;
	}

	if (pVidInfo->cur_serial == -1 && frame_type!= I_FRAME&& frame_type != AUDIO_FRAME)
		return -1;//H264 first frame must be I-Frame (sps+pps) for storing into extradata 

	Sem_lock(sem_id);

	pVidInfo->timestamp = timestamp;
	pVidInfo->temporalId = temporalId;

	ret = MemMng_Video_Write( pAddr, size, frame_type, pVidInfo);
	
	if( ret < 0 )
	{
		__E("Error code: %d at %d\n", ret,__LINE__);
	} 

//	pVidInfo->width++;
//	pVidInfo->height++;
//    __D("[%d] pVidInfo=%p video_type=%d size=%d %d %d %d\n", __LINE__,pVidInfo, pVidInfo->video_type,size,pVidInfo->cur_frame,pVidInfo->width,pVidInfo->height);						   

	for( cnt = 0; cnt < VIDEO_INFO_END; cnt++ )
	{
		pVidInfo->frame[pVidInfo->cur_frame].ref_serial[cnt] =	pParm->MemInfo.video_info[cnt].cur_serial;
	}

	/* post process */
	switch( stream_type )
	{
		case STREAM_H264_1:
		case STREAM_MP4:

			if(pParm->lockNewFrame[GOP_INDEX_MP4] && ret == 0)
			{
				if(frame_type == P_FRAME)
				{
					ret = LockCurrentGopP(pVidInfo,GOP_INDEX_MP4);
					if(ret)
					{
						__D("Error code: %d at %d\n", ret,__LINE__);
						pParm->lockNewFrame[GOP_INDEX_MP4] = 0;
					}
				}
				else if(frame_type == I_FRAME)
				{
				//	__D(": Line %d\n", __LINE__);
					pParm->lockNewFrame[GOP_INDEX_MP4] = 0;
				}
			}
			if(pParm->checkNewFrame[GOP_INDEX_MP4])
			{
					Rendezvous_meet(&(pParm->objRv[GOP_INDEX_MP4]));
					pParm->checkNewFrame[GOP_INDEX_MP4] = 0;
					Rendezvous_reset(&(pParm->objRv[GOP_INDEX_MP4]));
			}

		break;

		case STREAM_AUDIO:

			if(pParm->checkNewFrame[GOP_INDEX_AUDIO])
			{
					Rendezvous_meet(&(pParm->objRv[GOP_INDEX_AUDIO]));
					pParm->checkNewFrame[GOP_INDEX_AUDIO] = 0;
					Rendezvous_reset(&(pParm->objRv[GOP_INDEX_AUDIO]));
			}

		break;

		case STREAM_H264_2:
		case STREAM_MP4_EXT:
			if(pParm->lockNewFrame[GOP_INDEX_MP4_EXT] && ret == 0)
			{
				if(frame_type == P_FRAME)
				{
					ret = LockCurrentGopP(pVidInfo,GOP_INDEX_MP4_EXT);
					if(ret)
					{
						ERR("Error code: %d at %d\n", ret,__LINE__);
						pParm->lockNewFrame[GOP_INDEX_MP4_EXT] = 0;
					}
				}
				else if(frame_type == I_FRAME)
				{
				//	__D(": Line %d\n", __LINE__);
					pParm->lockNewFrame[GOP_INDEX_MP4_EXT] = 0;
				}
			}
			if(pParm->checkNewFrame[GOP_INDEX_MP4_EXT])
			{
					Rendezvous_meet(&(pParm->objRv[GOP_INDEX_MP4_EXT]));
					pParm->checkNewFrame[GOP_INDEX_MP4_EXT] = 0;
					Rendezvous_reset(&(pParm->objRv[GOP_INDEX_MP4_EXT]));
			}
		break;

		case STREAM_MJPG:
			if(pParm->checkNewFrame[GOP_INDEX_JPEG])
			{
					Rendezvous_meet(&(pParm->objRv[GOP_INDEX_JPEG]));
					pParm->checkNewFrame[GOP_INDEX_JPEG] = 0;
					Rendezvous_reset(&(pParm->objRv[GOP_INDEX_JPEG]));
			}
		break;

		default:
		break;
	}

	Sem_unlock(sem_id);

	if( ret == GOP_ERROR_OP ) ret = 0;

    ShowInfo(pParm, 0);

	/*if( ret < 0 )
	{
		return STREAM_FAILURE;
	}
	else
	{
		return STREAM_SUCCESS;
	}*/

	
	return STREAM_SUCCESS;
}

/**
 * @brief	Send quit command
 * @param	"STREAM_PARM *pParm" : stream parameter
 * @return	none
 */
static void SendQuitCmd( STREAM_PARM *pParm )
{
	MSG_BUF msgbuf;

	memset(&msgbuf,0,sizeof(msgbuf));
	msgbuf.Des = MSG_TYPE_MSG1;
	msgbuf.Src = 0;
	msgbuf.cmd = MSG_CMD_QUIT;
	Msg_Send( pParm->qid , &msgbuf , sizeof(msgbuf) );

	pParm->IsQuit = 1;
}

/**
 * @brief	Stream end
 * @param	"STREAM_PARM *pParm" : stream parameter
 * @return	none
 */
int stream_end(STREAM_PARM *pParm)
{
	int cnt = 0;

	SendQuitCmd(pParm);

	pthread_join(pParm->threadControl, (void **) 0);

	fprintf(stderr,"Msg_ThrFxn closing...\n");

	for( cnt = 0; cnt < GOP_INDEX_NUM; cnt++ )
	{
		Rendezvous_force(&(pParm->objRv[cnt]));
		Rendezvous_close(&(pParm->objRv[cnt]));
	}

	for( cnt = 0; cnt < STREAM_SEM_NUM; cnt++ )
	{
		Sem_kill(pParm->MemMngSemId[cnt]);
	}

	GopCleanup(&pParm->MemInfo.video_info[VIDEO_INFO_MP4],GOP_INDEX_MP4);
	GopCleanup(&pParm->MemInfo.video_info[VIDEO_INFO_MP4],GOP_INDEX_MP4_EXT);

	CacheMng_Release(&(pParm->MemInfo));
	MemMng_release( &(pParm->MemInfo) );

	Msg_Kill( pParm->qid);
	memset(pParm,0,sizeof(STREAM_PARM));

	return STREAM_SUCCESS;
}

/**
 * @brief	Message control thread
 * @param	"void* args" : arguments
 * @return	none
 */
void *Msg_CTRL(void* args)
{
	STREAM_PARM *pParm = (STREAM_PARM *)args;
	int qid;
	int msg_size = 0;
	MSG_BUF msgbuf;

//    ShowInfo(pParm, 1);

	qid 					= (int )pParm->qid;
	Rendezvous_Handle	hRv = &pParm->objRv[0];

	while((!gblGetQuit())&&(pParm->IsQuit == 0))
	{	
		msg_size = msgrcv( qid, &msgbuf, sizeof(msgbuf)-sizeof(long), MSG_TYPE_MSG1, 0);
//		__D("qid=%d, Received Des=%d, Src=%d, cmd=%d\n",qid,msgbuf.Des,msgbuf.Src,msgbuf.cmd);		
		if( msg_size < 0 )
		{
			__E("Receive msg fail \n");
			break;
		}
		else
		{
// 		    pParm = stream_get_handle();
			switch( msgbuf.cmd )
			{
				case MSG_CMD_GET_MEM: //0x01
//					__D("MSG_CMD_GET_MEM case \n");
					msgbuf.mem_info.addr = GetMemMngPhyBaseAddr(&(pParm->MemInfo));
					msgbuf.mem_info.size = GetMemMngTotalMemSize(&(pParm->MemInfo));
//					fprintf(stderr,"[%s:%d] msgbuf.mem_info.addr = 0x%x, msgbuf.mem_info.size=%d\n",__func__,__LINE__,msgbuf.mem_info.addr,msgbuf.mem_info.size);
					if(msgbuf.mem_info.addr != 0 && msgbuf.mem_info.size > 0)
						msgbuf.ret = 0;
					else
						msgbuf.ret = -1;
					break;
				case MSG_CMD_SET_MEM:
					__D("MSG_CMD_SET_MEM case \n");
					break;
				case MSG_CMD_PRINT_MEM:
					__D("MSG_CMD_PRINT_MEM case \n");
					break;
				case MSG_CMD_QUIT:
					pParm->IsQuit = 1;
					break;
				case MSG_CMD_GET_NEW_FRAME:
//					__D("MSG_CMD_GET_NEW_FRAME case \n");
					switch(msgbuf.frame_info.format)
					{
						case FMT_MJPEG:
							pParm->checkNewFrame[GOP_INDEX_JPEG] = 1;
							Rendezvous_meet(&hRv[GOP_INDEX_JPEG]);
						break;

						case FMT_MPEG4:
							pParm->checkNewFrame[GOP_INDEX_MP4] = 1;
							Rendezvous_meet(&hRv[GOP_INDEX_MP4]);
						break;

						case FMT_MPEG4_EXT:
							pParm->checkNewFrame[GOP_INDEX_MP4_EXT] = 1;
							Rendezvous_meet(&hRv[GOP_INDEX_MP4_EXT]);
						break;

						case FMT_AUDIO:
							pParm->checkNewFrame[GOP_INDEX_AUDIO] = 1;
							Rendezvous_meet(&hRv[GOP_INDEX_AUDIO]);
						break;
					}
				case MSG_CMD_GET_CUR_FRAME: //0x05
				{
					VIDEO_FRAME		*pFrame = NULL;
					VIDEO_BLK_INFO 	*pVidInfo = NULL;
					int				sem_id = 0;  

					__D("MSG_CMD_GET_CUR_FRAME case \n");
					switch(msgbuf.frame_info.format)
					{
						case FMT_MJPEG:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_JPEG];
						break;

						case FMT_MPEG4:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_MPEG4];
						break;

						case FMT_MPEG4_EXT:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
						break;

						case FMT_AUDIO:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_AUDIO];
						break;
					}

			 		Sem_lock(sem_id);
			 //		__D("[%d]msgbuf.frame_info.format : %d %d %d\n",__LINE__,msgbuf.frame_info.format,msgbuf.frame_info.width,msgbuf.frame_info.height);
					switch(msgbuf.frame_info.format){
						case FMT_MJPEG:
/*							for(i=0;i<VIDEO_INFO_END;i++)
							{
							 pVidInfo = &pParm->MemInfo.video_info[i];
						     if (pVidInfo != NULL) __D("[%d] pVidInfo=%p, video_type=%d, cur_frame=%d, cur_serial=%d\n",__LINE__,pVidInfo,pVidInfo->video_type,pVidInfo->cur_frame,pVidInfo->cur_serial);	
							} */

							pVidInfo = &(pParm->MemInfo.video_info[VIDEO_INFO_MJPG]);
//						  if (pVidInfo != NULL) __D("[%d]pVidInfo=%p,video_type=%d, cur_frame=%d, cur_serial=%d %d\n",__LINE__,pVidInfo,pVidInfo->video_type,pVidInfo->cur_frame,pVidInfo->width,pVidInfo->height);	
							pFrame = GrabCurrentFrame(pVidInfo);
							break;
						case FMT_MPEG4:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MP4];
							pFrame = GetLastI_Frame(pVidInfo);
							break;
						case FMT_MPEG4_EXT:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MP4_EXT];
							pFrame = GetLastI_Frame(pVidInfo);
							break;
						case FMT_AUDIO:
							pVidInfo = &pParm->MemInfo.video_info[AUDIO_INFO_G711];
							pFrame = GrabCurrentFrame(pVidInfo);
							break;
					}
					
				//    __D("[%d]msgbuf.frame_info.format : %d %d %d\n",__LINE__,msgbuf.frame_info.format,msgbuf.frame_info.width,msgbuf.frame_info.height);					

					if(pFrame == NULL)
					{
						__D("[%d]MSG_CMD_GET_CUR_FRAME : pFrame = NULL \n",__LINE__);
						msgbuf.ret = -1;
					}
					else {
						int cnt = 0;
						msgbuf.frame_info.serial_no = pFrame->serial;
						msgbuf.frame_info.size = pFrame->realsize;
						msgbuf.frame_info.offset = GetCurrentOffset(pVidInfo);
						switch(msgbuf.frame_info.format)
						{
							case FMT_MJPEG:
								msgbuf.frame_info.width = pParm->ImageWidth_Ext[STREAM_EXT_JPG];
								msgbuf.frame_info.height = pParm->ImageHeight_Ext[STREAM_EXT_JPG];
							break;
							case FMT_MPEG4:
								msgbuf.frame_info.width = pParm->ImageWidth;
								msgbuf.frame_info.height = pParm->ImageHeight;
							break;
							case FMT_MPEG4_EXT:
								msgbuf.frame_info.width = pParm->ImageWidth_Ext[STREAM_EXT_MP4_CIF];
								msgbuf.frame_info.height = pParm->ImageHeight_Ext[STREAM_EXT_MP4_CIF];
							break;
						}
						msgbuf.frame_info.flags = pFrame->flag;
						msgbuf.frame_info.frameType = pFrame->fram_type;
						msgbuf.frame_info.timestamp = pFrame->timestamp;
						msgbuf.frame_info.temporalId = pFrame->temporalId;
						for (cnt = 0; cnt < VIDEO_INFO_END; cnt++ )
						{
							msgbuf.frame_info.ref_serial[cnt] = pFrame->ref_serial[cnt];
						}

						msgbuf.ret = 0;
					}
					Sem_unlock(sem_id);
					break;
				}
				case MSG_CMD_LOCK_FRAME:
				{
					VIDEO_BLK_INFO *pVidInfo = NULL;
					int				sem_id = 0; 

//					__D("MSG_CMD_LOCK_FRAME case(%d,%d)\n",msgbuf.frame_info.width,msgbuf.frame_info.height);
					switch(msgbuf.frame_info.format)
					{
						case FMT_MJPEG:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_JPEG];
						break;

						case FMT_MPEG4:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_MPEG4];
						break;

						case FMT_MPEG4_EXT:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
						break;

						case FMT_AUDIO:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_AUDIO];
                            msgbuf.frame_info.quality = pParm->Audio_Codec;//RS,MDY
						break;
					}
					Sem_lock(sem_id);

					switch(msgbuf.frame_info.format){
						case FMT_MJPEG:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MJPG];
							msgbuf.frame_info.quality = pParm->JpgQuality;
							msgbuf.ret = CacheMng_Video_CacheLock(msgbuf.frame_info.serial_no, pVidInfo);
							break;
						case FMT_AUDIO:
							pVidInfo = &pParm->MemInfo.video_info[AUDIO_INFO_G711];
							msgbuf.frame_info.quality = pParm->Audio_Codec;
							msgbuf.ret = CacheMng_Video_CacheLock(msgbuf.frame_info.serial_no, pVidInfo);
							break;
						case FMT_MPEG4:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MP4];
							msgbuf.frame_info.quality = pParm->Mpeg4Quality;
							msgbuf.ret = LockGopBySerial(msgbuf.frame_info.serial_no, pVidInfo,GOP_INDEX_MP4);

							if(msgbuf.ret == GOP_INCOMPLETE){
								msgbuf.ret = 0;
								pParm->lockNewFrame[GOP_INDEX_MP4] = 1;
							}
							break;
						case FMT_MPEG4_EXT:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MP4_EXT];
							msgbuf.frame_info.quality = pParm->Mpeg4Quality;
							msgbuf.ret = LockGopBySerial(msgbuf.frame_info.serial_no, pVidInfo,GOP_INDEX_MP4_EXT);

							if(msgbuf.ret == GOP_INCOMPLETE){
								msgbuf.ret = 0;
								pParm->lockNewFrame[GOP_INDEX_MP4_EXT] = 1;
							}
							break;
					}
					if(msgbuf.ret == 0){
						CACHE_DATA_INFO Cache_info;
						int	cnt =0;
						CacheMng_GetCacheInfoBySerial(msgbuf.frame_info.serial_no, &Cache_info, pVidInfo);
						msgbuf.frame_info.offset = Cache_info.start_phy - GetMemMngPhyBaseAddr(&pParm->MemInfo);
						msgbuf.frame_info.size = Cache_info.realsize;

						switch(msgbuf.frame_info.format)
						{
							case FMT_MJPEG:
							msgbuf.frame_info.width = pParm->ImageWidth_Ext[STREAM_EXT_JPG];
							msgbuf.frame_info.height = pParm->ImageHeight_Ext[STREAM_EXT_JPG];
							break;
							case FMT_MPEG4:
							msgbuf.frame_info.width = pParm->ImageWidth;
							msgbuf.frame_info.height = pParm->ImageHeight;
							break;
							case FMT_MPEG4_EXT:
							msgbuf.frame_info.width = pParm->ImageWidth_Ext[STREAM_EXT_MP4_CIF];
							msgbuf.frame_info.height = pParm->ImageHeight_Ext[STREAM_EXT_MP4_CIF];
							break;
						}
						msgbuf.frame_info.flags 	= Cache_info.flag;
						msgbuf.frame_info.frameType = Cache_info.fram_type;
						msgbuf.frame_info.timestamp = Cache_info.timestamp;
						msgbuf.frame_info.temporalId = Cache_info.temporalId;
						for (cnt = 0; cnt < VIDEO_INFO_END; cnt++ )
						{
							msgbuf.frame_info.ref_serial[cnt] = Cache_info.ref_serial[cnt];
						}
					}
					Sem_unlock(sem_id);
					break;
				}
				case MSG_CMD_UNLOCK_FRAME:
				{
					VIDEO_BLK_INFO *pVidInfo = NULL;
					int				sem_id = 0,ret;

//					__D("MSG_CMD_UnLOCK_FRAME case(%d,%d)\n",msgbuf.frame_info.width,msgbuf.frame_info.height);
					switch(msgbuf.frame_info.format)
					{
						case FMT_MJPEG:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_JPEG];
						break;

						case FMT_MPEG4:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_MPEG4];
						break;

						case FMT_MPEG4_EXT:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
						break;

						case FMT_AUDIO:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_AUDIO];
                            msgbuf.frame_info.quality = pParm->Audio_Codec;
						break;
					}
					Sem_lock(sem_id);


					switch(msgbuf.frame_info.format){
						case FMT_MJPEG:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MJPG];
							if(CacheMng_Video_CacheUnlock(msgbuf.frame_info.
								serial_no, pVidInfo)== 0){
								/* No return */
							}
							break;
						case FMT_AUDIO:
							pVidInfo = &pParm->MemInfo.video_info[AUDIO_INFO_G711];
 						    msgbuf.frame_info.quality = pParm->Audio_Codec;
							if(CacheMng_Video_CacheUnlock(msgbuf.frame_info.
								serial_no, pVidInfo)== 0){
								/* No return */
							}
							break;
						case FMT_MPEG4:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MP4];
							ret = UnlockGopBySerial(msgbuf.frame_info.serial_no,
								pVidInfo,GOP_INDEX_MP4);
							if(ret == GOP_INCOMPLETE){
								pParm->lockNewFrame[GOP_INDEX_MP4] = 0;
								ret = 0;
							}
							if(ret == 0){
								/* No return */
							}
							break;
						case FMT_MPEG4_EXT:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MP4_EXT];
							ret = UnlockGopBySerial(msgbuf.frame_info.serial_no,
								pVidInfo,GOP_INDEX_MP4_EXT);
							if(ret == GOP_INCOMPLETE){
								pParm->lockNewFrame[GOP_INDEX_MP4_EXT] = 0;
								ret = 0;
							}
							if(ret == 0){
								/* No return */
							}
							break;
					}
					Sem_unlock(sem_id);
					break;
				}
				case MSG_CMD_GET_VOL:
				{
					VIDEO_BLK_INFO *pVidInfo = NULL;
					__D("MSG_CMD_GET_VOL case \n");
					switch(msgbuf.frame_info.format)
					{
						case FMT_MPEG4:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MP4];
						break;

						case FMT_MPEG4_EXT:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MP4_EXT];
						break;

					}
					if(pVidInfo == NULL) break;
					msgbuf.frame_info.offset = (unsigned long)(pVidInfo->extradata) - pParm->MemInfo.start_addr;
					msgbuf.frame_info.size	  = pVidInfo->extrasize;
					break;
				}
				case MSG_CMD_GET_MEDIA_INFO:
				{
					msgbuf.frame_info.format = 1; //VIDEO_streamGetMediaInfo();

					break;
				}
				case MSG_CMD_GET_AND_LOCK_IFRAME:
				{
					VIDEO_FRAME* pFrame = NULL;
					VIDEO_BLK_INFO *pVidInfo = NULL;
					int				sem_id = 0;
					switch(msgbuf.frame_info.format)
					{
						case FMT_MJPEG:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_JPEG];
						break;

						case FMT_MPEG4:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_MPEG4];
						break;

						case FMT_MPEG4_EXT:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_MPEG4_EXT];
						break;

						case FMT_AUDIO:
							sem_id	= pParm->MemMngSemId[STREAM_SEM_AUDIO];
	                        msgbuf.frame_info.quality = pParm->Audio_Codec;//RS,MDY
						break;
					}
					Sem_lock(sem_id);


					switch(msgbuf.frame_info.format)
					{
						case FMT_MJPEG:
							break;
						case FMT_MPEG4:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MP4];
							pFrame = GetLastI_Frame(pVidInfo);
							break;

						case FMT_MPEG4_EXT:
							pVidInfo = &pParm->MemInfo.video_info[VIDEO_INFO_MP4_EXT];
							pFrame = GetLastI_Frame(pVidInfo);
							break;
					}
					if(pFrame == NULL)
					{
						msgbuf.ret = -1;
					}
					else
					{
						switch(msgbuf.frame_info.format)
						{
							case FMT_MPEG4:
								msgbuf.frame_info.serial_no = pFrame->serial;
								msgbuf.frame_info.quality = pParm->Mpeg4Quality;
								msgbuf.ret = LockGopBySerial(msgbuf.frame_info.serial_no, pVidInfo,GOP_INDEX_MP4);

								if(msgbuf.ret == GOP_INCOMPLETE){
									msgbuf.ret = GOP_COMPLETE;
									pParm->lockNewFrame[GOP_INDEX_MP4] = 1;
								}
								break;
							case FMT_MPEG4_EXT:
								msgbuf.frame_info.serial_no = pFrame->serial;
								msgbuf.frame_info.quality = pParm->Mpeg4Quality;
								msgbuf.ret = LockGopBySerial(msgbuf.frame_info.serial_no, pVidInfo,GOP_INDEX_MP4_EXT);

								if(msgbuf.ret == GOP_INCOMPLETE){
									msgbuf.ret = GOP_COMPLETE;
									pParm->lockNewFrame[GOP_INDEX_MP4_EXT] = 1;
								}
								break;
						}


						if(msgbuf.ret == GOP_COMPLETE){
							CACHE_DATA_INFO Cache_info;
							int cnt = 0;
							CacheMng_GetCacheInfoBySerial(pFrame->serial,
								&Cache_info, pVidInfo);
							msgbuf.frame_info.offset = Cache_info.start_phy -
								GetMemMngPhyBaseAddr(&pParm->MemInfo);
							msgbuf.frame_info.size = Cache_info.realsize;
							switch(msgbuf.frame_info.format)
							{
								case FMT_MPEG4:
									msgbuf.frame_info.width = pParm->ImageWidth;
									msgbuf.frame_info.height = pParm->ImageHeight;
								break;
								case FMT_MPEG4_EXT:
									msgbuf.frame_info.width = pParm->ImageWidth_Ext[STREAM_EXT_MP4_CIF];
									msgbuf.frame_info.height = pParm->ImageHeight_Ext[STREAM_EXT_MP4_CIF];
								break;
							}
							msgbuf.frame_info.flags = Cache_info.flag;
							msgbuf.frame_info.frameType = Cache_info.fram_type;
							msgbuf.frame_info.timestamp = Cache_info.timestamp;
							msgbuf.frame_info.temporalId = Cache_info.temporalId;
							for (cnt = 0; cnt < VIDEO_INFO_END; cnt++ )
							{
								msgbuf.frame_info.ref_serial[cnt] = Cache_info.ref_serial[cnt];
							}
						}
					}
					Sem_unlock(sem_id);
					break;
				}
				case MSG_CMD_SET_DAY_NIGHT:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_DAY_NIGHT, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_BACKLIGHT:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_BLC, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_BRIGHTNESS:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = (temp==0) ? 1: temp;
					stream_feature_setup(STREAM_FEATURE_BRIGHTNESS, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_CONTRAST:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = (temp==0) ? 1: temp;
					stream_feature_setup(STREAM_FEATURE_CONTRAST, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_SATURATION:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = (temp==0) ? 1: temp;
					stream_feature_setup(STREAM_FEATURE_SATURATION, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_SHARPNESS:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = (temp==0) ? 1: temp;
					stream_feature_setup(STREAM_FEATURE_SHARPNESS, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_EXPOSUREMODE:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
			        stream_feature_setup(STREAM_FEATURE_EXPOSUREMODE, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_EXPOSUREVALUE:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_EXPOSUREVALUE, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_EXPOSUREVALUE1:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_EXPOSUREVALUE1, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_AGC:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_AGC, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_AGC_MIN:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_AGC_MIN, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_AGC1:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_AGC1, &value);
					msgbuf.ret = 0;
					break;
				}				
				case MSG_CMD_SET_AGC_MIN1:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_AGC_MIN1, &value);
					msgbuf.ret = 0;
					break;
				}				
				case MSG_CMD_SET_SHUTTER_TIME:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_SHUTTER_TIME, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_SHUTTER_TIME_MIN:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_SHUTTER_TIME_MIN, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_SHUTTER_TIME1:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_SHUTTER_TIME1, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_SHUTTER_TIME_MIN1:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_SHUTTER_TIME_MIN1, &value);
					msgbuf.ret = 0;
					break;
				}
				
				case MSG_CMD_SET_FLICKERLESS_MODE:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_FLICKERLESS_MODE, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_COLORBAR:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_COLORBAR, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_WHITE_BALANCE:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_AWB_MODE, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_AUDIO_INVOLUME:  //Add by Joseph 20140516
				{
					unsigned char value = *(unsigned char *)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_AUDIO_INVOLUME, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_AAC_BITRATE:
				{
					unsigned int value;
					value = *(unsigned int*)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_AAC_BITRATE, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_BITRATE1:
				{
					unsigned int value;
					value = *(unsigned int*)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_BIT_RATE1, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_BITRATE2:
				{
					unsigned int value;
					value = *(unsigned int*)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_BIT_RATE2, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_JPEG_QUALITY:
				{
					unsigned int value;
					value = *(unsigned int*)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_JPG_QUALITY, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_FRAMERATE1:
				{
					unsigned int value;
					value = *(unsigned int*)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_FRAMERATE1, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_FRAMERATE2:
				{
					unsigned int value;
					value = *(unsigned int*)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_FRAMERATE2, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_FRAMERATE3:
				{
					unsigned int value;
					value = *(unsigned int*)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_FRAMERATE3, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_TYPE_2A:	/*img2a*/
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = (int)temp;
					stream_feature_setup(STREAM_FEATURE_AEW_TYPE, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_2A_PRIORITY:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = (int)temp;
					stream_feature_setup(STREAM_FEATURE_AEW_PRIORITY, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_TV_SYSTEM:	/*ntsc/pal*/
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = (int)temp;
					stream_feature_setup(STREAM_FEATURE_ENV_50_60HZ, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_M41_GOP: //Add by Joseph 20131118
				{
					int value = *(int*)(&msgbuf.mem_info);
                    fprintf(stderr, "MSG_CMD_SET_M41_GOP value: %d\n " ,value);
					stream_feature_setup(STREAM_FEATURE_M41_GOP, &value);
					msgbuf.ret = 0;
					break;
				}				
#if 0				
				case MSG_CMD_SET_BINNING_SKIP:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = (int)temp;
					stream_feature_setup(STREAM_FEATURE_BINNING_SKIP, &value);
					msgbuf.ret = 0;
					break;
				}
#endif				
				case MSG_CMD_SET_LOCALDISPLAY:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = (int)temp;
					stream_feature_setup(STREAM_FEATURE_LOCALDISPLAY, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_OSDENABLE:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = (int)temp;
					stream_feature_setup(STREAM_FEATURE_OSDENABLE, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_TSTAMP:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_TSTAMPENABLE, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_PTZ:
				{
					int value = *(int*)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_PTZ, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_MOTION:
				{
					ApproMotionPrm tMotionPrm;
					memcpy(&tMotionPrm, &msgbuf.mem_info, sizeof(ApproMotionPrm));
					stream_feature_setup(STREAM_FEATURE_MOTION, &tMotionPrm);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_VNFSTATUS1:
				{
					unsigned int  value = *(unsigned int *)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_VNFSTATUS1, &value);
					msgbuf.ret = 0;
					break;
				}

				case MSG_CMD_SET_VNFSTATUS2:
				{
					unsigned int  value = *(unsigned int *)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_VNFSTATUS2, &value);
					msgbuf.ret = 0;
					break;
				}

				case MSG_CMD_SET_VNFSTATUS3:
				{
					unsigned int value = *(unsigned int *)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_VNFSTATUS3, &value);
					msgbuf.ret = 0;
					break;
				}

				case MSG_CMD_SET_ROICFG:
				{
					unsigned int value = *(unsigned int *)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_ROICFG, &value);
					msgbuf.ret = 0;
					break;
				}

				case MSG_CMD_SET_OSDTEXT_EN:
				{
					unsigned int value = *(unsigned int *)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_OSDTEXT_EN, &value);
					msgbuf.ret = 0;
					break;
				}

				case MSG_CMD_SET_HIST_EN:
				{
					unsigned int value = *(unsigned int *)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_HIST_EN, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_OSDLOGO_EN:
				{
					unsigned int value = *(unsigned int *)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_OSDLOGO_EN, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_OSDTEXT:
				{
					OsdTextPrm* pPrm = (OsdTextPrm*)&msgbuf.mem_info;
					stream_feature_setup(STREAM_FEATURE_OSDTEXT, pPrm);
					msgbuf.ret = pPrm->nLegnth;
					break;
				}
				case MSG_CMD_SET_FDETECT:
				{
					FaceDetectParam faceParam;
					memcpy(&faceParam, &msgbuf.mem_info, sizeof(FaceDetectParam));
					stream_feature_setup(STREAM_FEATURE_FACE_SETUP, &faceParam);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_DATETIMEPRM:
				{
					DateTimePrm datetimeParam;
					memcpy(&datetimeParam, &msgbuf.mem_info, sizeof(DateTimePrm));
					stream_feature_setup(STREAM_FEATURE_DATETIMEPRM, &datetimeParam);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_OSDPRM1:
				{
					OSDPrm osdPrm;
					memcpy(&osdPrm, &msgbuf.mem_info, sizeof(OSDPrm));
					stream_feature_setup(STREAM_FEATURE_OSDPRM1, &osdPrm);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_OSDPRM2:
				{
					OSDPrm osdPrm;
					memcpy(&osdPrm, &msgbuf.mem_info, sizeof(OSDPrm));
					stream_feature_setup(STREAM_FEATURE_OSDPRM2, &osdPrm);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_OSDPRM3:
				{
					OSDPrm osdPrm;
					memcpy(&osdPrm, &msgbuf.mem_info, sizeof(OSDPrm));
					stream_feature_setup(STREAM_FEATURE_OSDPRM3, &osdPrm);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_CLICKNAME:
				{
					OsdTextPrm* pPrm = (OsdTextPrm*)&msgbuf.mem_info;
					stream_feature_setup(STREAM_FEATURE_CLICKNAME, pPrm);
					msgbuf.ret = pPrm->nLegnth;
					break;
				}
				case MSG_CMD_SET_CLICKSTORAGE:
				{
					unsigned int value = *(unsigned int *)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_CLICKSTORAGE, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_IFRAME:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_IFRAME, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_AUDIO_ALARM_LEVEL:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_AUDIO_ALARM_LEVEL, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_AUDIO_ALARM_FLAG:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_AUDIO_ALARM_FLAG, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_MIRROR:
				{
					unsigned int value = *(unsigned int *)(&msgbuf.mem_info);
					stream_feature_setup(STREAM_FEATURE_MIRROR, &value);
					msgbuf.ret = 0;
					break;
				}

				case MSG_CMD_SET_ROIPRM1:
				{
					CodecROIPrm codecROIPrm;
					memcpy(&codecROIPrm, &msgbuf.mem_info, sizeof(CodecROIPrm));
					stream_feature_setup(STREAM_FEATURE_ROIPRM1, &codecROIPrm);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_ROIPRM2:
				{
					CodecROIPrm codecROIPrm;
					memcpy(&codecROIPrm, &msgbuf.mem_info, sizeof(CodecROIPrm));
					stream_feature_setup(STREAM_FEATURE_ROIPRM2, &codecROIPrm);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_ROIPRM3:
				{
					CodecROIPrm codecROIPrm;
					memcpy(&codecROIPrm, &msgbuf.mem_info, sizeof(CodecROIPrm));
					stream_feature_setup(STREAM_FEATURE_ROIPRM3, &codecROIPrm);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_ADVCODECPRM1:
				{
					CodecAdvPrm codecAdvPrm;
					memcpy(&codecAdvPrm, &msgbuf.mem_info, sizeof(CodecAdvPrm));
					stream_feature_setup(STREAM_FEATURE_ADVCODECPRM1, &codecAdvPrm);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_ADVCODECPRM2:
				{
					CodecAdvPrm codecAdvPrm;
					memcpy(&codecAdvPrm, &msgbuf.mem_info, sizeof(CodecAdvPrm));
					stream_feature_setup(STREAM_FEATURE_ADVCODECPRM2, &codecAdvPrm);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_ADVCODECPRM3:
				{
					CodecAdvPrm codecAdvPrm;
					memcpy(&codecAdvPrm, &msgbuf.mem_info, sizeof(CodecAdvPrm));
					stream_feature_setup(STREAM_FEATURE_ADVCODECPRM3, &codecAdvPrm);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_SET_DYNRANGE:
				{
					DynRangePrm dynRangePrm;
					memcpy(&dynRangePrm, &msgbuf.mem_info, sizeof(DynRangePrm));
					stream_feature_setup(STREAM_FEATURE_DYNRANGE, &dynRangePrm);
					msgbuf.ret = 0;
					break;
				}
                case MSG_CMD_SET_WDTRESET:
				{
					int value;
					unsigned char temp;
					temp = *(unsigned char*)(&msgbuf.mem_info);
					value = temp;
					stream_feature_setup(STREAM_FEATURE_WDTRESET, &value);
					msgbuf.ret = 0;
					break;
				}
				case MSG_CMD_POLLING:
				{
					msgbuf.ret = 0;
					break;
				}
			default:
					__E("stream default case \n");
					break;
			}
			if(msgbuf.Src != 0)
			{
				/* response */
				msgbuf.Des = msgbuf.Src;
				msgbuf.Src = MSG_TYPE_MSG1;
				msgsnd( qid,&msgbuf,sizeof(msgbuf)-sizeof(long),0);
//				__D("Stream send Des=%d, Src=%d, cmd=%d,addr=%d,size=%d\n",msgbuf.Des,msgbuf.Src,msgbuf.cmd,msgbuf.mem_info.addr,msgbuf.mem_info.size);
			}
		}
	}

    pthread_exit(NULL);
}




/**
 * @brief	Initialize message function
 * @param	"STREAM_PARM *pParm" : stream param
 * @return	none
 */
static int Init_Msg_Func(STREAM_PARM *pParm)
{

	pthread_attr_t     attr;
	struct sched_param schedParam;

	/* Initialize the thread attributes */
    if (pthread_attr_init(&attr)) {
        ERR("Failed to initialize thread attrs\n");
        return STREAM_FAILURE;
    }

	/* Set the capture thread priority */
//	schedParam.sched_priority = sched_get_priority_max(SCHED_FIFO) - 1;
//	if (pthread_attr_setschedparam(&attr, &schedParam))
//	{
//		return STREAM_FAILURE;
//	}
	__D(" Init_Msg_Func \n" );
	ShowInfo(pParm, 0);
	pthread_create(&(pParm->threadControl),&attr,Msg_CTRL,(void *)pParm);

	return STREAM_SUCCESS;
}

/**
 * @brief	Setup stream feature
 * @param	"int nFeature" : feature select
 * @param	"STREAM_PARM *pParm" : input value
 * @return	none
 */
void stream_feature_setup( int nFeature, void *pParm )
{
	if( nFeature < 0|| nFeature >= STREAM_FEATURE_NUM )
		return;

	switch(nFeature)
	{
		case STREAM_FEATURE_BIT_RATE1:
		{
			int input_val = *(int *)pParm;
//			VIDEO_bitRate_setparm( 0, input_val );

			break;
		}
		case STREAM_FEATURE_BIT_RATE2:
		{
			int input_val = *(int *)pParm;
//			VIDEO_bitRate_setparm( 1, input_val );

			break;
		}
		case STREAM_FEATURE_JPG_QUALITY:
		{
			int input_val = *(int *)pParm;
//			VIDEO_jpeg_quality_setparm(input_val);

			break;
		}
		case STREAM_FEATURE_AUDIO_INVOLUME:
		{
			unsigned char input_val = *(unsigned char *)pParm;
//			SAMPLE_Acodec_SetGain(input_val);//(input_val);

			break;
		}
		case STREAM_FEATURE_AAC_BITRATE:
		{
			int input_val = *(int *)pParm;
//			AUDIO_aac_bitRate_setparm(input_val);

			break;
		}
		case STREAM_FEATURE_FRAMERATE1:
		{
			unsigned int input_val = *(unsigned int *)pParm;
			int set_val = 0;

//			VIDEO_frameRate_setparm(0, input_val);
			if( Aew_ext_parameter.day_night != AE_DAY)
			{
				if(input_val>15000)
					set_val = 0;
				else if(input_val>5000)
					set_val = 1;
				else
					set_val = 2;

				if( Aew_ext_parameter.SENSOR_FRM_RATE_SETUP )
					Aew_ext_parameter.SENSOR_FRM_RATE_SETUP(set_val);

				Aew_ext_parameter.sensor_frame_rate = set_val;
			}
			break;
		}
		case STREAM_FEATURE_FRAMERATE2:
		{
			unsigned int input_val = *(unsigned int *)pParm;
//			VIDEO_frameRate_setparm(1, input_val);
			break;
		}
		case STREAM_FEATURE_FRAMERATE3:
		{
			unsigned int input_val = *(unsigned int *)pParm;
//			VIDEO_frameRate_setparm(2, input_val);
			break;
		}
		case STREAM_FEATURE_SHARPNESS:
		{
			int input_val = *(int *)pParm;

			Aew_ext_parameter.sharpness = input_val;			
			
//			send_cmd_snx(CMD_SHARPNESS, Aew_ext_parameter.sharpness);

//			SAMPLE_COMM_ISP_Sharpness(Aew_ext_parameter.sharpness);

			break;
		}

		case STREAM_FEATURE_CONTRAST:
		{
			int input_val = *(int *)pParm;

			Aew_ext_parameter.contrast = input_val;
			
 //   	send_cmd_snx(CMD_CONTRAST, Aew_ext_parameter.contrast);

//			SAMPLE_COMM_ISP_Contrast(Aew_ext_parameter.contrast);

			break;
		}
		case STREAM_FEATURE_BRIGHTNESS:
		{
			int input_val = *(int *)pParm;

			Aew_ext_parameter.brightness = input_val;

//			SAMPLE_COMM_ISP_Brightness(Aew_ext_parameter.brightness);
 //   	send_cmd_snx(CMD_BRIGHTNESS, Aew_ext_parameter.brightness);

			break;
		}
		case STREAM_FEATURE_EXPOSUREMODE:
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.exposuremode = input_val;
	//		SysInfo* pSysInfo = GetSysInfo();
			char szMsg[100];
			sprintf(szMsg, "exposuremode:%d\n", input_val);
			fprintf(stderr, szMsg);
			if(Aew_ext_parameter.exposuremode == 0)//auto
			{
				//Aew_ext_parameter.shutter_time_min = 12;
//				SAMPLE_COMM_ISP_Exposure_Time_Min(12);//to 1/10000 sec
				//Aew_ext_parameter.shutter_time = 2; 
//				SAMPLE_COMM_ISP_Exposure_Time(2);//from 1/30 sec by auto mode, but AirLive need change to 1/60
				Aew_ext_parameter.agc = 1;//DAY gain max
				Aew_ext_parameter.agc_min = 0;//DAY gain min
				Aew_ext_parameter.agc1 = 1; //NIGHT gain max
				Aew_ext_parameter.agc_min1 = 0; //NIGHT gain min
			}
			else //manual
			{
#if 0				
				if(Aew_ext_parameter.day_night == AE_DAY)
				{
					if(pSysInfo->lan_config.shuttertime_min == pSysInfo->lan_config.shuttertime)
//						SAMPLE_COMM_ISP_ME_Exposure_Time(pSysInfo->lan_config.shuttertime);
					else
					{
//						SAMPLE_COMM_ISP_Exposure_Time_Min(pSysInfo->lan_config.shuttertime_min);
//						SAMPLE_COMM_ISP_Exposure_Time(pSysInfo->lan_config.shuttertime);
					}
					Aew_ext_parameter.agc = pSysInfo->lan_config.agc;
					Aew_ext_parameter.agc_min = pSysInfo->lan_config.agc_min;
				}
				else
				{					
					if(pSysInfo->lan_config.shuttertime_min == pSysInfo->lan_config.shuttertime)
						SAMPLE_COMM_ISP_ME_Exposure_Time(pSysInfo->lan_config.shuttertime);
					else
					{
						SAMPLE_COMM_ISP_Exposure_Time_Min(pSysInfo->lan_config.shuttertime_min1);
						SAMPLE_COMM_ISP_Exposure_Time(pSysInfo->lan_config.shuttertime1);	
					}
					Aew_ext_parameter.agc1 = pSysInfo->lan_config.agc1;
					Aew_ext_parameter.agc_min1 = pSysInfo->lan_config.agc_min1;
				}
#endif				
			}
  		    //ALG_aewbMode(Aew_ext_parameter.awb_mode); 
			break;
		}		
		case STREAM_FEATURE_EXPOSUREVALUE:
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.exposurevalue = input_val;
   		    //ALG_aewbMode(Aew_ext_parameter.awb_mode); 
// 	        fprintf(stderr,"Aew_ext_parameter.exposurevalue  = %d\n", Aew_ext_parameter.exposurevalue);
			break;
		}
		case STREAM_FEATURE_EXPOSUREVALUE1:
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.exposurevalue1= input_val;
  		    //ALG_aewbMode(Aew_ext_parameter.awb_mode); 
  //	    fprintf(stderr,"Aew_ext_parameter.exposurevalue1  = %d\n", Aew_ext_parameter.exposurevalue1 );
			break;
		}

		case STREAM_FEATURE_AGC:
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.agc = input_val;
 		    //ALG_aewbMode(Aew_ext_parameter.awb_mode); //Cato@20120823
//	        fprintf(stderr,"Aew_ext_parameter.agc  = %d\n", Aew_ext_parameter.agc );
			break;
		}
		case STREAM_FEATURE_AGC_MIN:
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.agc_min = input_val;
   		    //ALG_aewbMode(Aew_ext_parameter.awb_mode); //Cato@20120823
// 	        fprintf(stderr,"Aew_ext_parameter.agc_min  = %d\n", Aew_ext_parameter.agc_min );
			break;
		}

		case STREAM_FEATURE_AGC1:
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.agc1 = input_val;
 		    //ALG_aewbMode(Aew_ext_parameter.awb_mode); //Cato@20120823
//	       fprintf(stderr,"Aew_ext_parameter.agc1  = %d\n", Aew_ext_parameter.agc1 );
			break;
		}
		case STREAM_FEATURE_AGC_MIN1:
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.agc_min1= input_val;
 		    //ALG_aewbMode(Aew_ext_parameter.awb_mode); //Cato@20120823
//	        fprintf(stderr,"Aew_ext_parameter.agc_min1  = %d\n", Aew_ext_parameter.agc_min1 );
			break;
		}		
		case STREAM_FEATURE_SHUTTER_TIME: //max shutter time
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.shutter_time= input_val;
#ifdef IMGS_ITN	
			DRV_imgsSetAEModeNormalAE();
			DRV_imgsSetShutterTime(Aew_ext_parameter.shutter_time, 1);
#else			
//			if(Aew_ext_parameter.day_night == AE_DAY) SAMPLE_COMM_ISP_Exposure_Time(Aew_ext_parameter.shutter_time);
#endif 		       
 	        fprintf(stderr,"Msg_ThrFxn :Shutter_Time = %d\n", input_val);
			break;
		}
		case STREAM_FEATURE_SHUTTER_TIME_MIN://min shutter time
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.shutter_time_min= input_val;
#ifdef IMGS_ITN	
			DRV_imgsSetAEModeNormalAE();
			DRV_imgsSetShutterTime(Aew_ext_parameter.shutter_time, 1);
#else			
//			if(Aew_ext_parameter.day_night == AE_DAY) SAMPLE_COMM_ISP_Exposure_Time_Min(Aew_ext_parameter.shutter_time_min);
#endif 		       
 	        fprintf(stderr,"Msg_ThrFxn :Shutter_Time_min = %d\n", input_val);
			break;
		}
		case STREAM_FEATURE_SHUTTER_TIME1:
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.shutter_time1= input_val;
#ifdef IMGS_ITN	
			DRV_imgsSetAEModeNormalAE();
			DRV_imgsSetShutterTime(Aew_ext_parameter.shutter_time1, 1);
#else			
//			if(Aew_ext_parameter.day_night == AE_NIGHT) SAMPLE_COMM_ISP_Exposure_Time(Aew_ext_parameter.shutter_time1);
#endif 		       
 	        fprintf(stderr,"Msg_ThrFxn :Shutter_Time1 = %d\n", input_val);
			break;
		}
		case STREAM_FEATURE_SHUTTER_TIME_MIN1:
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.shutter_time_min1= input_val;
#ifdef IMGS_ITN	
			DRV_imgsSetAEModeNormalAE();
			DRV_imgsSetShutterTime(Aew_ext_parameter.shutter_min1, 1);
#else			
//			if(Aew_ext_parameter.day_night == AE_NIGHT) SAMPLE_COMM_ISP_Exposure_Time_Min(Aew_ext_parameter.shutter_time_min1);
#endif 		       
 	        fprintf(stderr,"Msg_ThrFxn :Shutter_Time_min1 = %d\n", input_val);
			break;
		}
		//modify by Joseph 20140604
		case STREAM_FEATURE_FLICKERLESS_MODE:
		{
			int input_val = *(int *)pParm;			
			int exposuremode = 0 ;
			
			if (input_val == 1){
                if(Aew_ext_parameter.exposuremode != 0){
                   stream_feature_setup(STREAM_FEATURE_EXPOSUREMODE, &exposuremode);
                }                
            }else{
                stream_feature_setup(STREAM_FEATURE_ENV_50_60HZ, &exposuremode);
            }

 	        fprintf(stderr,"Msg_ThrFxn :flikcerless_mode = %d\n", input_val);
			break;
		}
		
		case STREAM_FEATURE_BLC:
		{
			int input_val = *(int *)pParm;
			if( input_val == 0 )
			{
				Aew_ext_parameter.blc = BACKLIGHT_LOW;
			}
			else if( input_val == 1 )
			{
				Aew_ext_parameter.blc = BACKLIGHT_NORMAL;
			}
			else if( input_val == 2)
			{
				Aew_ext_parameter.blc = BACKLIGHT_HIGH;
			}
			else
			{
				Aew_ext_parameter.blc = BACKLIGHT_NORMAL;
			}

			break;
		}

		case STREAM_FEATURE_SATURATION:
		{
			int input_val = *(int *)pParm;

			Aew_ext_parameter.saturation = input_val;			

 //   	send_cmd_snx(CMD_SATURATION, Aew_ext_parameter.saturation);
			//SAMPLE_COMM_ISP_Saturation(Aew_ext_parameter.saturation);
//            if(Aew_ext_parameter.awb_mode == BLACKWHITE) SAMPLE_COMM_ISP_Saturation(0);
//            else SAMPLE_COMM_ISP_Saturation(Aew_ext_parameter.saturation);

			break;
		}

		case STREAM_FEATURE_AWB_MODE:
		{
			int input_val = *(int *)pParm;
	//		SysInfo* pSysInfo = GetSysInfo();
			
 	  //      fprintf(stderr,"Msg_ThrFxn :AWB_mode = %d\n", input_val);
			if( input_val == 0 )
			{
				Aew_ext_parameter.awb_mode = OUTDOOR;
			}
			else if( input_val == 2 )
			{
				Aew_ext_parameter.awb_mode = INDOOR;
			}
			else if( input_val == 3 )
			{
				Aew_ext_parameter.awb_mode = BLACKWHITE;
			}
			else if( input_val == 4 )
			{
				Aew_ext_parameter.awb_mode = TUNGSTEN;
			}			
			else
			{
				Aew_ext_parameter.awb_mode = AWB_AUTO;
			}
#if 0				
      if(pSysInfo->lan_config.nDayNight == AE_NIGHT){
         Aew_ext_parameter.awb_mode = BLACKWHITE;
//         SAMPLE_COMM_ISP_AWB(Aew_ext_parameter.awb_mode);
      }else{
//         SAMPLE_COMM_ISP_AWB(Aew_ext_parameter.awb_mode);
      }
      if(Aew_ext_parameter.awb_mode == BLACKWHITE){
          SAMPLE_COMM_ISP_Saturation(0);
      }else if(pSysInfo->lan_config.nDayNight == AE_NIGHT){
          SAMPLE_COMM_ISP_Saturation(0);
      }else{
          SAMPLE_COMM_ISP_Saturation(Aew_ext_parameter.saturation);
      }			
#endif      
			break;
		}

		case STREAM_FEATURE_DAY_NIGHT:
		{
			int input_val = *(int *)pParm;

			if( Aew_ext_parameter.SENSOR_FRM_RATE_SETUP )
				Aew_ext_parameter.SENSOR_FRM_RATE_SETUP(0);

			Aew_ext_parameter.sensor_frame_rate = 0;

  //          fprintf(stderr,"Msg_ThrFxn[%d]:DayNight_mode = %d,saturation = %d\n",__LINE__, input_val,Aew_ext_parameter.saturation);
			if( input_val == 0 )
			{
				Aew_ext_parameter.day_night = AE_DAY;
//				SAMPLE_COMM_ISP_Saturation(Aew_ext_parameter.saturation);
//				printf("STREAM_FEATURE_DAY_NIGHT = %d\n",Aew_ext_parameter.day_night);
			}
//			else if( input_val == 1 )
            else 
			{
				Aew_ext_parameter.day_night = AE_NIGHT;
//				SAMPLE_COMM_ISP_Saturation(0); //Aew_ext_parameter.saturation);
	//			printf("STREAM_FEATURE_DAY_NIGHT = %d\n",Aew_ext_parameter.day_night);
			}
//			else
//			{
//				Aew_ext_parameter.day_night = AE_NIGHT;
//			}

			break;
		}

		case STREAM_FEATURE_AEW_TYPE:
		{
			int input_val = *(int *)pParm;
			Aew_ext_parameter.aew_enable= (input_val == 0) ? AEW_DISABLE:AEW_ENABLE;
//			VIDEO_aewbSetType(input_val);

			break;
		}
		case STREAM_FEATURE_AEW_PRIORITY:
		{
			int input_val = *(int *)pParm;
//			VIDEO_aewbPriority(input_val);

			break;
		}
			
		case STREAM_FEATURE_ENV_50_60HZ:
		{
			int input_val = *(int *)pParm;

			/*if( input_val == 0 )
			{
				Aew_ext_parameter.env_50_60Hz= VIDEO_NTSC;
			}
			else if( input_val == 1 )
			{
				Aew_ext_parameter.env_50_60Hz = VIDEO_PAL;
			}
			else
			{
				Aew_ext_parameter.env_50_60Hz = VIDEO_NONE;
			}*/
			Aew_ext_parameter.env_50_60Hz = input_val;
//			SAMPLE_COMM_ISP_AntiFlicker(Aew_ext_parameter.env_50_60Hz);

			break;
		}
		case STREAM_FEATURE_BINNING_SKIP:
		{
			int input_val = *(int *)pParm;
			if( input_val == 0 )
			{
				Aew_ext_parameter.binning_mode= SENSOR_BINNING;
			}
			else if( input_val == 1 )
			{
				Aew_ext_parameter.binning_mode = SENSOR_SKIP;
			}
			else
			{
				Aew_ext_parameter.binning_mode = SENSOR_BINNING;
			}

			break;
		}
		case STREAM_FEATURE_LOCALDISPLAY:
		{
			int input_val = *(int *)pParm;
//			SetDispInterface(input_val);
			break;
		}
		case STREAM_FEATURE_OSDENABLE:
		{
			int input_val = *(int *)pParm;
//			VIDEO_streamSetOSDEnable(input_val);
			break;
		}

		case STREAM_FEATURE_TSTAMPENABLE:
		{
			int input_val = *(int *)pParm;
//			SetDrawDateTimeStatus(input_val);
			break;
		}
		case STREAM_FEATURE_PTZ:
		{
			int input_val = *(int *)pParm;
//			SetPtzCmd(input_val);
			break;
		}
		case STREAM_FEATURE_MOTION:
		{
//			VIDEO_motion_setparm((ApproMotionPrm*) pParm);
			break;
		}
		case STREAM_FEATURE_VNFSTATUS1:
		{
			unsigned int input_val = *(unsigned int *)pParm;
//			VIDEO_streamSetVNFStatus( 0, input_val );
			break;
		}
		case STREAM_FEATURE_VNFSTATUS2:
		{
			unsigned int input_val = *(unsigned int *)pParm;
//			VIDEO_streamSetVNFStatus( 1, input_val );
			break;
		}
		case STREAM_FEATURE_VNFSTATUS3:
		{
			unsigned int input_val = *(unsigned int *)pParm;
//			VIDEO_streamSetVNFStatus( 2, input_val );
			break;
		}
		case STREAM_FEATURE_ROICFG:
		{
			unsigned int input_val = *(unsigned int *)pParm;
//			SetROICfgEnable(input_val);
			break;
		}
		case STREAM_FEATURE_OSDTEXT_EN:
		{
			int input_val = *(int *)pParm;
//			SetOsdTextEnable(input_val);
			break;
		}
		case STREAM_FEATURE_HIST_EN:
		{
			int input_val = *(int *)pParm;
//			SetHistogramEnable(input_val);
			break;
		}
		case STREAM_FEATURE_OSDLOGO_EN:
		{
			int input_val = *(int *)pParm;
//			SetOsdLogoEnable(input_val);
			break;
		}
		case STREAM_FEATURE_OSDTEXT:
		{
			OsdTextPrm* pPrm = (OsdTextPrm*)pParm;
//			SetOsdText(pPrm->strText, pPrm->nLegnth);
			break;
		}
		case STREAM_FEATURE_FACE_SETUP:
		{
//			VIDEO_streamSetFace((FaceDetectParam*)pParm);
			break;
		}
		case STREAM_FEATURE_DATETIMEPRM:
		{
//			VIDEO_streamSetDateTimePrm((DateTimePrm*)pParm);
			break;
		}
		case STREAM_FEATURE_OSDPRM1:
		{
//			VIDEO_streamOsdPrm((OSDPrm*)pParm, 0);
			break;
		}
		case STREAM_FEATURE_OSDPRM2:
		{
//			VIDEO_streamOsdPrm((OSDPrm*)pParm, 1);
			break;
		}
		case STREAM_FEATURE_OSDPRM3:
		{
//			VIDEO_streamOsdPrm((OSDPrm*)pParm, 2);
			break;
		}
		case STREAM_FEATURE_CLICKNAME:
		{
			OsdTextPrm* pPrm = (OsdTextPrm*)pParm;
//			SetSnapName(pPrm->strText, pPrm->nLegnth);
			break;
		}
		case STREAM_FEATURE_CLICKSTORAGE:
		{
			int input_val = *(int *)pParm;
//			SetSnapLocation(input_val);
			break;
		}
		case STREAM_FEATURE_IFRAME:
		{
			int input_val = *(int *)pParm;
//			VIDEO_codecReset(input_val);
			break;
		}
		case STREAM_FEATURE_AUDIO_ALARM_LEVEL:
		{
			int input_val = *(int *)pParm;
//			AUDIO_alarmLevel(input_val);
			break;
		}
		case STREAM_FEATURE_AUDIO_ALARM_FLAG:
		{
			int input_val = *(int *)pParm;
//			AUDIO_alarmFlag(input_val);
			break;
		}
		case STREAM_FEATURE_MIRROR:
		{
			int input_val = *(int *)pParm;
//			VIDEO_mirrorPrm(input_val);
//            SAMPLE_COMM_ISP_Mirror(input_val);   
			break;
		}
		case STREAM_FEATURE_ROIPRM1:
		{
//			VIDEO_streamROIPrm((CodecROIPrm*)pParm, 0);
			break;
		}
		case STREAM_FEATURE_ROIPRM2:
		{
//			VIDEO_streamROIPrm((CodecROIPrm*)pParm, 1);
			break;
		}
		case STREAM_FEATURE_ROIPRM3:
		{
//			VIDEO_streamROIPrm((CodecROIPrm*)pParm, 2);
			break;
		}
  	    case STREAM_FEATURE_TVOUT_ENABLE:
	    {
		 int input_val = *(int *)pParm;
		 fprintf(stderr, "STREAM_FEATURE_TVOUT_ENABLE %d\n", input_val);
//		 VIDEO_displaySetTvOut(input_val);
		 break;
	    }
		case STREAM_FEATURE_ADVCODECPRM1:
		{
//			VIDEO_codecAdvPrm((CodecAdvPrm*)pParm, 0);
			break;
		}
		case STREAM_FEATURE_ADVCODECPRM2:
		{
//			VIDEO_codecAdvPrm((CodecAdvPrm*)pParm, 1);
			break;
		}
		case STREAM_FEATURE_ADVCODECPRM3:
		{
//			VIDEO_codecAdvPrm((CodecAdvPrm*)pParm, 2);
			break;
		}
		case STREAM_FEATURE_DYNRANGE:
		{
//			VIDEO_dynRangePrm((DynRangePrm*)pParm);
			break;
		}
		case STREAM_FEATURE_WDTRESET:
		{
			int input_val = *(int *)pParm;
//			if(input_val) AVSERVER_WDT_RESET();
			break;
		}
  	    case STREAM_FEATURE_SET_MIRROR:
	    {
		 int input_val = *(int *)pParm;
		 fprintf(stderr, "STREAM_FEATURE_SET_MIRROR %d\n", input_val);
//		 video_SetMirror(input_val);
		 break;
	    }		
	   case STREAM_FEATURE_SET_AUDIO_BROADCAST_PORT:
  	   {
		int input_val = *(int *)pParm;
		fprintf(stderr, "STREAM_FEATURE_SET_AUDIO_BROADCAST_PORT %d\n", input_val);
//		AVSERVER_SetAudioBroadcastPort((unsigned short)input_val);
		break;
	   }
	   case STREAM_FEATURE_EWDR_MODE:
	   {
		int input_val = *(int *)pParm;
		fprintf(stderr, "STREAM_FEATURE_EWDR_MODE %d\n", input_val);
		//AVSERVER_SetAudioBroadcastPort((unsigned short)input_val);
		break;
	   }
	   default:
			__E("unknow feature setup num = %d\n", nFeature);
		break;
	}
}
