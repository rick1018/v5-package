/* ===========================================================================
* @file Appro_interface.c
*
* @path $(IPNCPATH)/util/
*
* @desc Interface for getting Audio/Video data between processes
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stream_msg_drv.h>
#include "avdata_drv.h"
//#include "cmem.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/shm.h>
#include "avshmem.h"
int          shm_id;

//unsigned char *avframedata;

void   *virptr = NULL;
static unsigned char VolInfo[256];
static unsigned char VolInfo_cif[256];
static unsigned char VolInfo_H264[256];

static int gMemSize = 0;

//#define __DEBUG
#ifdef __DEBUG
#define __D(fmt, args...) fprintf(stderr, "[%s]%s(%d) Error: " fmt, __FILE__,__func__,__LINE__,## args)
#else
#define __D(fmt, args...)
#endif
#define __E(fmt, args...) fprintf(stderr, "[%s]%s(%d) Error: " fmt, __FILE__,__func__,__LINE__,## args)


/**
 * @brief Interface for getting Audio/Video data between processes
 *
 *
 * @param   field    The operation field, the value should be AV_OP_XXX_XXX defined at Appro_interface.h
 * @param   serial   frame serial number , used when locking frame data
 * @param   ptr    frame information data structure defined at Appro_interface.h
 *
 * @return the value should be RET_XXXXX defined at Appro_interface.h
 *
 * @pre Must have called ApproDrvInit() and func_get_mem()
 *
 */
int GetAVData( unsigned int field, int serial, AV_DATA * ptr )
{
	int ret=RET_SUCCESS;
	if(virptr == NULL)  return RET_ERROR_OP;
		
	__D("%s\ field=%d serial=%d\n", __func__,field,serial);
#if WEGIN_OEM
	time_t tmp;
	time(&tmp);
	FILE *fp = fopen("/tmp/last_get_data_time","w");
	fprintf(fp, "%d", tmp);
	fclose(fp);
#endif
	switch(field)
	{
		case AV_OP_GET_MJPEG_SERIAL:
			if (serial != -1)
			{
				ret = RET_INVALID_PRM;
			}
			else
			{
				FrameInfo_t curframe = GetCurrentFrame(FMT_MJPEG);
				if (curframe.serial_no < 0)  { ret = RET_NO_VALID_DATA; } 
				else
				{
					int cnt = 0;
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->timestamp = curframe.timestamp;
					for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
					{
						ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
					}
				}
			}
			break;
		case AV_OP_WAIT_NEW_MJPEG_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = WaitNewFrame(FMT_MJPEG);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
				}
			}
			break;
		case AV_OP_LOCK_MJPEG:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					int cnt = 0;
					frame.serial_no = serial;
					frame.format = FMT_MJPEG;
					ret = LockFrame(&frame);
					switch(ret){
						case -98:
							ret = RET_OVERWRITE;
							break;
						case -99:
							ret = RET_NO_MEM;
							break;
						case 0:
							ptr->serial = serial;
							ptr->size = frame.size;
							ptr->width = frame.width;
							ptr->height = frame.height;
							ptr->quality = frame.quality;
							ptr->flags = frame.flags;
							ptr->timestamp = frame.timestamp;
							for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
							{
								ptr->ref_serial[cnt] = frame.ref_serial[cnt];
							}
							ptr->ptr = (unsigned char *)virptr + frame.offset;
							break;
						default:
							ret = RET_UNKNOWN_ERROR;
							break;
					}
				}
			}
			break;
		case AV_OP_UNLOCK_MJPEG:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					frame.serial_no = serial;
					frame.format = FMT_MJPEG;
					UnlockFrame(&frame);
				}
			}
			break;
		case AV_OP_GET_MPEG4_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = GetCurrentFrame(FMT_MPEG4);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					int cnt = 0;
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->flags = curframe.flags;
					ptr->timestamp = curframe.timestamp;
					ptr->temporalId = curframe.temporalId;
					for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
					{
						ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
					}
				}
			}
			break;
		case AV_OP_GET_MPEG4_CIF_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = GetCurrentFrame(FMT_MPEG4_EXT);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					int cnt = 0;
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->flags = curframe.flags;
					ptr->timestamp = curframe.timestamp;
					ptr->temporalId = curframe.temporalId;
					for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
					{
						ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
					}
				}
			}
			break;
		case AV_OP_GET_H264_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = GetCurrentFrame(FMT_H264);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					int cnt = 0;
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->flags = curframe.flags;
					ptr->timestamp = curframe.timestamp;
					ptr->temporalId = curframe.temporalId;
					for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
					{
						ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
					}
				}
			}
			break;
			
		case AV_OP_WAIT_NEW_MPEG4_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = WaitNewFrame(FMT_MPEG4);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->flags = curframe.flags;
					ptr->temporalId = curframe.temporalId;
				}
			}
			break;

		case AV_OP_WAIT_NEW_MPEG4_CIF_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = WaitNewFrame(FMT_MPEG4_EXT);//WaitNewFrame(FMT_MPEG4);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->flags = curframe.flags;
					ptr->temporalId = curframe.temporalId;
				}
			}
			break;

		case AV_OP_WAIT_NEW_H264_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = WaitNewFrame(FMT_H264);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->flags = curframe.flags;
					ptr->temporalId = curframe.temporalId;
				}
			}
			break;


		case AV_OP_LOCK_MP4:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					int cnt = 0;
					frame.serial_no = serial;
					frame.format = FMT_MPEG4;
					ret = LockFrame(&frame);
					switch(ret){
						case -99:
							ret = RET_OVERWRITE;
							fprintf(stderr,"[%s:%d]RET_OVERWRITE\n",__func__,__LINE__);
							break;
						case -100:
							ret = RET_NO_MEM;
							fprintf(stderr,"[%s:%d]RET_NO_MEM\n",__func__,__LINE__);
							break;
						case -101:
							ret = RET_ERROR_OP;
							fprintf("[%s:%d]RET_ERROR_OP: serial: %d\n",__func__,__LINE__,serial);
							break;
						case -98:
							ret = RET_NO_VALID_DATA;
							//printf("RET_NO_VALID_DATA\n");
							break;
						case 0:
							ptr->serial = serial;
							ptr->size = frame.size;
							ptr->width = frame.width;
							ptr->height = frame.height;
							ptr->quality = frame.quality;
							ptr->flags = frame.flags;
							ptr->frameType = frame.frameType;
							ptr->timestamp = frame.timestamp;
							ptr->temporalId = frame.temporalId;
							for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
							{
								ptr->ref_serial[cnt] = frame.ref_serial[cnt];
							}
							ptr->ptr = (unsigned char *)virptr + frame.offset;
							break;
						default:
							ret = RET_UNKNOWN_ERROR;
							fprintf(stderr,"[%s:%d]RET_UNKNOWN_ERROR\n",__func__,__LINE__);
							break;
					}
				}
			}
			break;
			case AV_OP_LOCK_MP4_CIF:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					int cnt = 0;
					frame.serial_no = serial;
					frame.format = FMT_MPEG4_EXT;
					ret = LockFrame(&frame);
					switch(ret){
						case -99:
							ret = RET_OVERWRITE;
							fprintf("[%s:%d]RET_OVERWRTE\n",__func__,__LINE__);
							break;
						case -100:
							ret = RET_NO_MEM;
							fprintf("[%s:%d]RET_NO_MEM\n",__func__,__LINE__);
							break;
						case -101:
							ret = RET_ERROR_OP;
							fprintf("[%s:%d]RET_ERROR_OP: serial: %d\n",__func__,__LINE__,serial);
							break;
						case -98:
							ret = RET_NO_VALID_DATA;
							//printf("RET_NO_VALID_DATA\n");
							break;
						case 0:
							ptr->serial = serial;
							ptr->size = frame.size;
							ptr->width = frame.width;
							ptr->height = frame.height;
							ptr->quality = frame.quality;
							ptr->flags = frame.flags;
							ptr->frameType = frame.frameType;
							ptr->timestamp = frame.timestamp;
							ptr->temporalId = frame.temporalId;
							for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
							{
								ptr->ref_serial[cnt] = frame.ref_serial[cnt];
							}
							ptr->ptr = (unsigned char *)virptr + frame.offset;
							break;
						default:
							ret = RET_UNKNOWN_ERROR;
							fprintf(stderr,"[%s:%d]RET_UNKNOWN_ERROR\n",__func__,__LINE__);
							break;
					}
				}
			}
			break;
			
			case AV_OP_LOCK_H264:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					int cnt = 0;
					frame.serial_no = serial;
					frame.format = FMT_H264;
					ret = LockFrame(&frame);
					switch(ret){
						case -99:
							ret = RET_OVERWRITE;
							fprintf(stderr,"[%s:%d]RET_OVERWRITE\n",__func__,__LINE__);
							break;
						case -100:
							ret = RET_NO_MEM;
							fprintf("[%s:%d]RET_NO_MEM\n",__func__,__LINE__);
							break;
						case -101:
							ret = RET_ERROR_OP;
							fprintf("[%s:%d]RET_ERROR_OP: serial: %d\n",__func__,__LINE__,serial);
							break;
						case -98:
							ret = RET_NO_VALID_DATA;
							//printf("RET_NO_VALID_DATA\n");
							break;
						case 0:
							ptr->serial = serial;
							ptr->size = frame.size;
							ptr->width = frame.width;
							ptr->height = frame.height;
							ptr->quality = frame.quality;
							ptr->flags = frame.flags;
							ptr->frameType = frame.frameType;
							ptr->timestamp = frame.timestamp;
							ptr->temporalId = frame.temporalId;
							for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
							{
								ptr->ref_serial[cnt] = frame.ref_serial[cnt];
							}
							ptr->ptr = (unsigned char *)virptr + frame.offset;
							break;
						default:
							ret = RET_UNKNOWN_ERROR;
							printf("RET_UNKNOWN_ERROR\n");
							break;
					}
				}
			}
			break;

		case AV_OP_UNLOCK_MP4:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					frame.serial_no = serial;
					frame.format = FMT_MPEG4;
					UnlockFrame(&frame);
				}
			}
			break;

		case AV_OP_UNLOCK_MP4_CIF:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					frame.serial_no = serial;
					frame.format = FMT_MPEG4_EXT;
					UnlockFrame(&frame);
				}
			}
			break;

		case AV_OP_UNLOCK_H264:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					frame.serial_no = serial;
					frame.format = FMT_H264;
					UnlockFrame(&frame);
				}
			}
			break;
			
		case AV_OP_LOCK_MP4_VOL:
		{
			FrameInfo_t frame;
			ret = GetVolInfo(&frame,FMT_MPEG4);
			if( ret > 0 )
			{
				memcpy( VolInfo,((unsigned char *)virptr + frame.offset),ret);
			}
			ptr->ptr = VolInfo;
			ptr->size = ret;
			ret = RET_SUCCESS;
			break;
		}
		case AV_OP_GET_MEDIA_INFO:
		{
			FrameInfo_t frame;
			ptr->flags = GetMediaInfo(&frame);
			ret = RET_SUCCESS;
			break;
		}
		case AV_OP_LOCK_MP4_CIF_VOL:
		{
			FrameInfo_t frame;
			ret = GetVolInfo(&frame,FMT_MPEG4_EXT);
			if( ret > 0 )
			{
				memcpy( VolInfo_cif,((unsigned char *)virptr + frame.offset),ret);
			}
			ptr->ptr = VolInfo_cif;
			ptr->size = ret;
			ret = RET_SUCCESS;
			break;
		}
		case AV_OP_LOCK_H264_VOL:
		{
			FrameInfo_t frame;
			ret = GetVolInfo(&frame,FMT_H264);
			if( ret > 0 )
			{
				memcpy( VolInfo_H264,((unsigned char *)virptr + frame.offset),ret);
			}
			ptr->ptr = VolInfo_H264;
			ptr->size = ret;
			ret = RET_SUCCESS;
			break;
		}		
		case AV_OP_UNLOCK_MP4_VOL:
			ret = RET_SUCCESS;
			break;

		case AV_OP_UNLOCK_MP4_CIF_VOL:
			ret = RET_SUCCESS;
			break;
			
		case AV_OP_UNLOCK_H264_VOL:
			ret = RET_SUCCESS;
			break;

		case AV_OP_LOCK_MP4_IFRAME:
		{
			FrameInfo_t curframe;
			LockMpeg4IFrame(&curframe, FMT_MPEG4);
			if(curframe.serial_no == -1){
				ret = RET_NO_VALID_DATA;
			} else if(curframe.serial_no == -2){
				ret = RET_NO_MEM;
			} else {
				int cnt = 0;
				ptr->serial = curframe.serial_no;
				ptr->size = curframe.size;
				ptr->width = curframe.width;
				ptr->height = curframe.height;
				ptr->quality = curframe.quality;
				ptr->flags = curframe.flags;
				ptr->frameType = curframe.frameType;
				ptr->timestamp = curframe.timestamp;
				ptr->temporalId = curframe.temporalId;
				for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
				{
					ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
				}
				ptr->ptr = (unsigned char *)virptr + curframe.offset;
				ret = RET_SUCCESS;
			}
			break;
		}

		case AV_OP_LOCK_MP4_CIF_IFRAME:
		{
			FrameInfo_t curframe;
			LockMpeg4IFrame(&curframe,FMT_MPEG4_EXT);
			if(curframe.serial_no == -1){
				ret = RET_NO_VALID_DATA;
			} else if(curframe.serial_no == -2){
				ret = RET_NO_MEM;
			} else {
				int cnt = 0;
				ptr->serial = curframe.serial_no;
				ptr->size = curframe.size;
				ptr->width = curframe.width;
				ptr->height = curframe.height;
				ptr->quality = curframe.quality;
				ptr->flags = curframe.flags;
				ptr->frameType = curframe.frameType;
				ptr->timestamp = curframe.timestamp;
				ptr->temporalId = curframe.temporalId;
				for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
				{
					ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
				}
				ptr->ptr = (unsigned char *)virptr + curframe.offset;
				ret = RET_SUCCESS;
			}
			break;
		}
		case AV_OP_LOCK_H264_IFRAME:
		{
			FrameInfo_t curframe;
			LockMpeg4IFrame(&curframe,FMT_H264);
			if(curframe.serial_no == -1){
				ret = RET_NO_VALID_DATA;
			} else if(curframe.serial_no == -2){
				ret = RET_NO_MEM;
			} else {
				int cnt = 0;
				ptr->serial = curframe.serial_no;
				ptr->size = curframe.size;
				ptr->width = curframe.width;
				ptr->height = curframe.height;
				ptr->quality = curframe.quality;
				ptr->flags = curframe.flags;
				ptr->frameType = curframe.frameType;
				ptr->timestamp = curframe.timestamp;
				ptr->temporalId = curframe.temporalId;
				for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
				{
					ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
				}
				ptr->ptr = (unsigned char *)virptr + curframe.offset;
				ret = RET_SUCCESS;
			}
			break;
		}
		
		case AV_OP_LOCK_ULAW:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					int cnt = 0;
					frame.serial_no = serial;
					frame.format = FMT_AUDIO;
					ret = LockFrame(&frame);
					switch(ret){
						case -97:
						case -98:
							ret = RET_NO_VALID_DATA;
							break;
						case -99:
							ret = RET_NO_MEM;
							break;
						case 0:
							ptr->serial = serial;
							ptr->size = frame.size;
							ptr->width = frame.width;
							ptr->height = frame.height;
							ptr->quality = frame.quality;
							ptr->flags = frame.flags;
							ptr->timestamp = frame.timestamp;
							ptr->temporalId = frame.temporalId;
							for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
							{
								ptr->ref_serial[cnt] = frame.ref_serial[cnt];
							}
							ptr->ptr = (unsigned char *)virptr + frame.offset;
							break;
						default:
							ret = RET_UNKNOWN_ERROR;
							break;
					}
				}
			}
			break;
		case AV_OP_UNLOCK_ULAW:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					frame.serial_no = serial;
					frame.format = FMT_AUDIO;
					UnlockFrame(&frame);
				}
			}
			break;
		case AV_OP_GET_ULAW_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = GetCurrentFrame(FMT_AUDIO);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					int cnt = 0;
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->timestamp = curframe.timestamp;
					ptr->temporalId = curframe.temporalId;
					for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
					{
						ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
					}
				}
			}
			break;

		case AV_OP_WAIT_NEW_ULAW_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = WaitNewFrame(FMT_AUDIO);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
				}
			}
			break;
		case AV_OP_WAIT_NEW_PCM_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = WaitNewFrame(FMT_AUDIO_PCM);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
				}
			}
			break;			
		case AV_OP_LOCK_PCM:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					int cnt = 0;
					frame.serial_no = serial;
					frame.format = FMT_AUDIO_PCM;
					ret = LockFrame(&frame);
					//fprintf(stderr, "AV_OP_LOCK_PCM:%d\n", ret);
					switch(ret){
						case -97:
						case -98:
							ret = RET_NO_VALID_DATA;
							break;
						case -99:
							ret = RET_NO_MEM;
							break;
						case 0:
							ptr->serial = serial;
							ptr->size = frame.size;
							ptr->width = frame.width;
							ptr->height = frame.height;
							ptr->quality = frame.quality;
							ptr->flags = frame.flags;
							ptr->timestamp = frame.timestamp;
							ptr->temporalId = frame.temporalId;
							for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
							{
								ptr->ref_serial[cnt] = frame.ref_serial[cnt];
							}
							ptr->ptr = (unsigned char *)virptr + frame.offset;
							break;
						default:
							ret = RET_UNKNOWN_ERROR;
							break;
					}
				}
			}
			break;
		case AV_OP_UNLOCK_PCM:
			if(serial == -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t frame;
				if(serial < 0){
					ret = RET_INVALID_PRM;
				} else {
					frame.serial_no = serial;
					frame.format = FMT_AUDIO_PCM;
					UnlockFrame(&frame);
				}
			}
			break;
		case AV_OP_GET_PCM_SERIAL:
			if(serial != -1){
				ret = RET_INVALID_PRM;
			} else {
				FrameInfo_t curframe = GetCurrentFrame(FMT_AUDIO_PCM);
				if(curframe.serial_no < 0){
					ret = RET_NO_VALID_DATA;
				} else {
					int cnt = 0;
					ptr->serial = curframe.serial_no;
					ptr->size = curframe.size;
					ptr->width = curframe.width;
					ptr->height = curframe.height;
					ptr->timestamp = curframe.timestamp;
					ptr->temporalId = curframe.temporalId;
					for (cnt = 0; cnt < FMT_MAX_NUM; cnt++ )
					{
						ptr->ref_serial[cnt] = curframe.ref_serial[cnt];
					}
				}
			}
			break;			
		default:
			ret = RET_INVALID_COMMAND;
			dbg("Command field = %d\n", field);
			break;
	}
	return ret;
}

int cmem_fd;
static int ref_count = 0;

static int validate_init()
{
    switch (cmem_fd) {
      case -3:
        __E("CMEM_exit() already called, check stderr output for earlier "
            "CMEM failure messages (possibly version mismatch).\n");

        return 0;

      case -2:
        __E("CMEM_init() not called, you must initialize CMEM before "
            "making API calls.\n");

        return 0;

      case -1:
        __E("CMEM file descriptor -1 (failed 'open()'), ensure CMEMK "
            "kernel module cmemk.ko has been installed with 'insmod'");

        return 0;

      default:
        return 1;
    }
}

int CMEM_exit(void)
{
    int result = 0;

    __D("exit: entered - ref_count %d, cmem_fd %d\n", ref_count, cmem_fd);
	
#if 0
    if (!validate_init()) {
        return -1;
    }

    __D("exit: decrementing ref_count\n");

    ref_count--;
    if (ref_count == 0) {
        result = close(cmem_fd);

        __D("exit: ref_count == 0, closed /dev/cmem (%s)\n",
            result == -1 ? strerror(errno) : "succeeded");

        /* setting -3 allows to distinguish CMEM exit from CMEM failed */
        cmem_fd = -3;
    }
#else
    /* detach shared memory */    
//    if (shmdt(avframedata) == -1) { perror("shmdt"); exit(1);  }       
	 
    /* destroy shared memory */    
//    if (shmctl(shm_id, IPC_RMID, 0) == -1) { perror("shmctl"); exit(1); }      
//    else printf("destroy shared memory\n");
#endif
	if (shmdt(virptr) == -1) { perror("Appro_interface: shmdt"); exit(1);  }	

    __D("exit: exiting, returning %d\n", result);

    return result;
}


int CMEM_init(int chn)
{
    int flags;
    unsigned int version;

    __D("init: entered - ref_count %d, cmem_fd %d\n", ref_count, cmem_fd);

#if 0
    if (cmem_fd >= 0) {
        ref_count++;
        __D("init: /dev/cmem already opened, incremented ref_count %d\n",
            ref_count);
        return 0;
    }

     cmem_fd = open("/dev/mem", O_RDWR);
//     cmem_fd = open("/tmp/cmem", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (cmem_fd == -1) {
        __E("init: Failed to open /dev/cmem: '%s'\n", strerror(errno));
        return -1;
    }

    ref_count++;

    __D("init: successfully opened /dev/cmem, matching driver version...\n");	
	
#if 0
    version = CMEM_getVersion();
    if ((version & 0xffff0000) != (CMEM_VERSION & 0xffff0000)) {
        __E("init: major version mismatch between interface and driver.\n");
        __E("    needs driver version %#x, got %#x\n", CMEM_VERSION, version);
        CMEM_exit();
        return -1;
    }
    else if ((version & 0x0000ffff) < (CMEM_VERSION & 0x0000ffff)) {
        __E("init: minor version mismatch between interface and driver.\n");
        __E("    needs driver minor version %#x or greater.\n"
            "    got minor version %#x (full version %#x)\n",
            CMEM_VERSION & 0x0000ffff, version & 0x0000ffff, version);
        CMEM_exit();
        return -1;
    }
#endif
    __D("init: ... match good (%#x)\n", version);

    flags = fcntl(cmem_fd, F_GETFD);
    if (flags != -1) {
        fcntl(cmem_fd, F_SETFD, flags | FD_CLOEXEC);
    }
    else {
        __E("init: fcntl(F_GETFD) failed: '%s'\n", strerror(errno));
    }
#else
//       shm_id = shmget((key_t)KEY_SHM_AV_CAPTURE, pInfo->totalsize, 0666 | IPC_CREAT);      
      shm_id = shmget((key_t)( KEY_SHM_AV_CAPTURE + chn ), 0, 0);
      if (shm_id < 0 ) {  perror("shmget"); exit(-1);  }
      else	 __D("shared memory id:%d\n", shm_id );        	
	  
#endif
    __D("init: exiting, returning success\n");

    return 0;
}


void *CMEM_MMAP( unsigned long physp,  int size )
{
	void *userp;

	__D("%s\n",__func__);
#if 0	
	if (cmem_fd == -1)
	{
        printf("allocPool: You must initialize CMEM before making API calls.\n");
        return NULL;
    }
	/* Map the physical address to user space */
    userp = mmap(0,                       // Preferred start address
                 size,                    // Length to be mapped
                 PROT_WRITE | PROT_READ,  // Read and write access
                 MAP_SHARED,              // Shared memory
                 cmem_fd,                 // File descriptor
                 physp);                  // The byte offset from fd
#else
   /* attach shared memory */    
    userp = (void *)shmat(shm_id, (void *)0, 0);    
    if (userp == (void *)-1)  {   perror("shmget");  exit(1);    }
#endif

    if (userp == MAP_FAILED)
	{
        printf("allocPool: Failed to mmap buffer at physical address %#lx\n", physp);
//        ioctl(cmem_fd, CMEM_IOCFREE, &physp); //cato to dm368
        return NULL;
    }

	return userp;

}
/**
 * @brief Setup the memory for getting audio / video data
 *
 *
 * @param   pdata    could be NULL, not use now
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int func_get_mem( void *pdata )
{
	MSG_MEM mem_info = GetPhyMem();

	fprintf(stderr, "[%s]%s(%d) addr= 0x%x, size= %d\n",__FILE__,__func__,__LINE__,mem_info.addr,mem_info.size);
//	__D("%s\n",__func__);
//	if (mem_info.addr == 0)
	if (mem_info.size == 0)
	{
		__E(" %s: Memory return Invalid mem_info.size= %d\n",mem_info.size);
		//return -1;
	}
	if (gMemSize == 0 && virptr == NULL) 
	       virptr = CMEM_MMAP( mem_info.addr, (int)mem_info.size );
	else
	{
		fprintf(stderr, "Warning: Memory map have been done before\n");
		return 0;
	}
	
	if(virptr == NULL)  return -1;
	
	gMemSize = mem_info.size;
	return 0;
}
/**
 * @brief Release the resource of the appro interface
 *
 *
 *
 * @return 0 is ok and -1 is error
 *
 *
 */
int ApproInterfaceExit()
{
	__D("%s\n",__func__);
	if (gMemSize)
		munmap(virptr, gMemSize);
	else
		fprintf(stderr, "Error: Maping size is zero!!\n");
	virptr = NULL;
	gMemSize = 0;
	return ApproDrvExit();
}

