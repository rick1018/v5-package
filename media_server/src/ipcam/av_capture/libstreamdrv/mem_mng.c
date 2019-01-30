/** ===========================================================================
* @file mem_mng.c
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


/****************************
*		Includes		      *
****************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <ctype.h>
#include <unistd.h>
//#include <sys/mman.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/ipc.h>
//#include <sys/msg.h>
#include <sys/shm.h>
#include <limits.h>
//#include <fcntl.h>
#include "avshmem.h"
#include "mem_mng.h"
#include "stream.h"


//#define __MEM_MNG_DEBUG
#ifdef __MEM_MNG_DEBUG
#define __D(fmt, args...) fprintf(stderr, "Mem Debug: " fmt, ## args)
#else
#define __D(fmt, args...)
#endif


#define __E(fmt, args...) fprintf(stderr, "Error: " fmt, ## args)


#define MEMMNG_DMA	(0)
//static DRV_DmaChHndl dmaHndl;
int MemMng_memcpy_open(void);
int MemMng_memcpy_close(void);

//int          shm_id;
//pthread_mutex_t mutex_avframe = PTHREAD_MUTEX_INITIALIZER;
//unsigned char *avframedata;	

MEM_LAYOUT mem_layout_setting[MEM_LAYOUT_NUM] = {
	[0] = {
		.profiles[VIDEO_INFO_MJPG] =
		{
			.cache_size 	= MJPG_CACHE_SIZE,
			.cache_blk_size = MJPG_CACHE_BLK_SIZE,
			.mem_size 		= MJPG_MEM_SIZE,
			.mem_blk_size 	= MJPG_BLK_SIZE,
			.ext_size 		= 0,
		},
		.profiles[VIDEO_INFO_MP4] =
		{
			.cache_size 	= MP4_CACHE_SIZE,
			.cache_blk_size = MP4_CACHE_BLK_SIZE,
			.mem_size 		= MP4_MEM_SIZE,
			.mem_blk_size 	= MP4_BLK_SIZE,
			.ext_size 		= MP4_1_EXTRA_SIZE,
		},
		.profiles[VIDEO_INFO_MP4_EXT] =
		{
			.cache_size 	= MP4_EXT_CACHE_SIZE,
			.cache_blk_size = MP4_EXT_CACHE_BLK_SIZE,
			.mem_size 		= MP4_EXT_MEM_SIZE,
			.mem_blk_size 	= MP4_EXT_BLK_SIZE,
			.ext_size 		= MP4_2_EXTRA_SIZE,
		},
		.profiles[VIDEO_INFO_H264] =
		{
			.cache_size 	= MP4_3_CACHE_SIZE,
			.cache_blk_size = MP4_3_CACHE_BLK_SIZE,
			.mem_size 		= MP4_3_MEM_SIZE,
			.mem_blk_size 	= MP4_3_BLK_SIZE,
			.ext_size 		= MP4_3_EXTRA_SIZE,
		},		
		.profiles[AUDIO_INFO_G711] =
		{
			.cache_size 	= AUDIO_CACHE_SIZE,
			.cache_blk_size = AUDIO_CACHE_BLK_SIZE,
			.mem_size 		= AUDIO_MEM_SIZE,
			.mem_blk_size 	= AUDIO_BLK_SIZE,
			.ext_size 		= 0,
		},
		.profiles[AUDIO_INFO_PCM] =
		{
			.cache_size 	= AUDIO_2_CACHE_SIZE,
			.cache_blk_size = AUDIO_2_CACHE_BLK_SIZE,
			.mem_size 		= AUDIO_2_MEM_SIZE,
			.mem_blk_size 	= AUDIO_2_BLK_SIZE,
			.ext_size 		= 0,
		},
		.totalsizes = 0,
	},
	[1] = {
/*		.profiles[VIDOE_INFO_MJPG] =
		{
			.cache_size 	= 0x100000,
			.cache_blk_size = 50*1024,
			.mem_size 		= 0x100000,
			.mem_blk_size 	= 50*1024,
			.ext_size 		= 0,
		},*/
		.profiles[VIDEO_INFO_MJPG] =
		{
			.cache_size 	= MJPG_CACHE_SIZE,
			.cache_blk_size = MJPG_CACHE_BLK_SIZE,
			.mem_size 		= MJPG_MEM_SIZE,
			.mem_blk_size 	= MJPG_BLK_SIZE,
			.ext_size 		= 0,
		},

		.profiles[VIDEO_INFO_MP4] =
		{
			.cache_size 	= 0x500000,
			.cache_blk_size = 30*1024,
			.mem_size 		= 0x500000,
			.mem_blk_size 	= 30*1024,
			.ext_size 		= MP4_1_EXTRA_SIZE,
		},
		.profiles[VIDEO_INFO_MP4_EXT] =
		{
			.cache_size 	= 0x500000,
			.cache_blk_size = 30*1024,
			.mem_size 		= 0x500000,
			.mem_blk_size 	= 30*1024,
			.ext_size 		= MP4_2_EXTRA_SIZE,
		},
		.profiles[VIDEO_INFO_H264] =
		{
			.cache_size 	= 0x500000,
			.cache_blk_size = 30*1024,
			.mem_size 		= 0x500000,
			.mem_blk_size 	= 30*1024,
			.ext_size 		= MP4_3_EXTRA_SIZE,
		},

		.profiles[AUDIO_INFO_G711] =
		{
			.cache_size 	= AUDIO_CACHE_SIZE,
			.cache_blk_size = AUDIO_CACHE_BLK_SIZE,
			.mem_size 		= AUDIO_MEM_SIZE,
			.mem_blk_size 	= AUDIO_BLK_SIZE,
			.ext_size 		= 0,
		},
		.totalsizes = 0,
	}

};

 
VIDEO_BLK_INFO	Video_info_default[VIDEO_INFO_END] =
{
	{
		.video_type = VIDEO_MJPG,
		.width      = 640,	
		.height     = 480,
		.size		= MJPG_MEM_SIZE,
		.extradata	= NULL,
		.extrasize	= 0,
		.IsCache	= 0,
		.blk_sz		= MJPG_BLK_SIZE,
		.blk_num	= (MJPG_MEM_SIZE/MJPG_BLK_SIZE),
		.blk_free	= (MJPG_MEM_SIZE/MJPG_BLK_SIZE),
		.frame_num	= 0,
		.cur_frame	= -1,
		.cur_serial = -1,
		.cur_blk	= -1,
		.timestamp	= -1,
		.gop		= {
						.last_Start = -1,
						.last_Start_serial = -1,
						.last_End = -1,
						.last_End_serial = -1,
						.lastest_I = -1,
						.lastest_I_serial = -1,
					},
		.frame		= NULL,

	},
	{
		.video_type = VIDEO_MP4,
//		.width = 1280,	
//		.height = 720,			
		.size		= MP4_MEM_SIZE,
		.extradata	= NULL,
		.extrasize	= MP4_1_EXTRA_SIZE,
		.IsCache	= 0,
		.blk_sz		= MP4_BLK_SIZE,
		.blk_num	= (MP4_MEM_SIZE/MP4_BLK_SIZE),
		.blk_free	= (MP4_MEM_SIZE/MP4_BLK_SIZE),
		.frame_num	= 0,
		.cur_frame	= -1,
		.cur_serial = -1,
		.cur_blk	= -1,
		.timestamp	= -1,
		.gop		= {
						.last_Start = -1,
						.last_Start_serial = -1,
						.last_End = -1,
						.last_End_serial = -1,
						.lastest_I = -1,
						.lastest_I_serial = -1,
					},
		.frame		= NULL,

	},
	{
		.video_type = VIDEO_MP4,
//		.width = 320,	
//		.height = 240,			
		.size		= MP4_EXT_MEM_SIZE,
		.extradata	= NULL,
		.extrasize	= MP4_2_EXTRA_SIZE,
		.IsCache	= 0,
		.blk_sz		= MP4_EXT_BLK_SIZE,
		.blk_num	= (MP4_EXT_MEM_SIZE/MP4_EXT_BLK_SIZE),
		.blk_free	= (MP4_EXT_MEM_SIZE/MP4_EXT_BLK_SIZE),
		.frame_num	= 0,
		.cur_frame	= -1,
		.cur_serial = -1,
		.cur_blk	= -1,
		.timestamp	= -1,
		.gop		= {
						.last_Start = -1,
						.last_Start_serial = -1,
						.last_End = -1,
						.last_End_serial = -1,
						.lastest_I = -1,
						.lastest_I_serial = -1,
					},
		.frame		= NULL,

	},
	{
		.video_type = VIDEO_MP4,
//		.width = 320,	
//		.height = 240,			
		.size		= MP4_3_MEM_SIZE,
		.extradata	= NULL,
		.extrasize	= MP4_3_EXTRA_SIZE,
		.IsCache	= 0,
		.blk_sz		= MP4_3_BLK_SIZE,
		.blk_num	= (MP4_3_MEM_SIZE/MP4_3_BLK_SIZE),
		.blk_free	= (MP4_3_MEM_SIZE/MP4_3_BLK_SIZE),
		.frame_num	= 0,
		.cur_frame	= -1,
		.cur_serial = -1,
		.cur_blk	= -1,
		.timestamp	= -1,
		.gop		= {
						.last_Start = -1,
						.last_Start_serial = -1,
						.last_End = -1,
						.last_End_serial = -1,
						.lastest_I = -1,
						.lastest_I_serial = -1,
					},
		.frame		= NULL,

	},	
	{
		.video_type = AUDIO_G711,
//		.width = 320,	
//		.height = 1,						
		.size		= AUDIO_MEM_SIZE,
		.extradata	= NULL,
		.extrasize	= 0,
		.IsCache	= 0,
		.blk_sz		= AUDIO_BLK_SIZE,
		.blk_num	= (AUDIO_MEM_SIZE/AUDIO_BLK_SIZE),
		.blk_free	= (AUDIO_MEM_SIZE/AUDIO_BLK_SIZE),
		.frame_num	= 0,
		.cur_frame	= -1,
		.cur_serial = -1,
		.cur_blk	= -1,
		.timestamp	= -1,
		.gop		= {
						.last_Start = -1,
						.last_Start_serial = -1,
						.last_End = -1,
						.last_End_serial = -1,
						.lastest_I = -1,
						.lastest_I_serial = -1,
					},
		.frame		= NULL,

	},
	{
		.video_type = AUDIO_PCM,
//		.width = 320,	
//		.height = 1,									
		.size		= AUDIO_2_MEM_SIZE,
		.extradata	= NULL,
		.extrasize	= 0,
		.IsCache	= 0,
		.blk_sz		= AUDIO_2_BLK_SIZE,
		.blk_num	= (AUDIO_2_MEM_SIZE/AUDIO_2_BLK_SIZE),
		.blk_free	= (AUDIO_2_MEM_SIZE/AUDIO_2_BLK_SIZE),
		.frame_num	= 0,
		.cur_frame	= -1,
		.cur_serial = -1,
		.cur_blk	= -1,
		.timestamp	= -1,
		.gop		= {
						.last_Start = -1,
						.last_Start_serial = -1,
						.last_End = -1,
						.last_End_serial = -1,
						.lastest_I = -1,
						.lastest_I_serial = -1,
					},
		.frame		= NULL,

	}
};

/**
 * @brief	Reset video frame
 * @param	"VIDEO_FRAME *pframe" : video frame
 * @return	none
 */
void Video_Frame_reset( VIDEO_FRAME *pframe)
{
	int cnt = 0;

	pframe->fram_type	= EMPTY_FRAME;
	pframe->serial 		= -1;
	pframe->blkindex	= -1;
	pframe->blks		= -1;
	pframe->realsize	= -1;
	pframe->flag		= 0;
	pframe->timestamp	= 0;
	for (cnt = 0; cnt < VIDEO_INFO_END; cnt++)
	{
		pframe->ref_serial[cnt] = -1;
	}
}

/**
 * @brief	Video memory allocate
 * @param	"MEM_MNG_INFO *pInfo"
 * @return	0 : success ; -1 : fail
 */
int Video_Mem_Malloc( MEM_MNG_INFO *pInfo )
{
	int	cnt = 0;
	int loops = 0;
	int cnt2 = 0;

	loops = sizeof( Video_info_default )/ sizeof( Video_info_default[0] );

//       __D("%s:sizeof( Video_info_default )=%d,sizeof( Video_info_default[0] )=%d\n",__func__,sizeof( Video_info_default ),sizeof( Video_info_default[0] ));

	pInfo->video_info_nums  = loops;

       /* get the ID of shared memory */         
      fprintf(stderr, "***create SHM_AV chn= %d\n",pInfo->chn);
      pInfo->shm_id = shmget((key_t)(KEY_SHM_AV_CAPTURE + pInfo->chn ), pInfo->totalsize, 0666 | IPC_CREAT);      
      if (pInfo->shm_id < 0 )
	  { 
	  	fprintf(stderr, "****error av_server share memory create\n");
		perror("shmget");
		//exit(EXIT_FAILURE);
		//modify by frank, it's very serious problem, need to reboot camera.
		system("reboot");
	  }            

      /* attach shared memory */    
//	  unsigned char *avframedata;	
//      pInfo->avframedata = (unsigned char *)shmat(pInfo->shm_id, (void *)0, 0);    
//      pInfo->start_phyAddr = (unsigned long)pInfo->avframedata;
        pInfo->start_phyAddr = (unsigned long)shmat(pInfo->shm_id, (void *)0, 0); 
//      fprintf(stderr,"pInfo->start_phyAddr = 0x%x, pInfo->totalsize=%d\n",pInfo->start_phyAddr,pInfo->totalsize);
 	      
//  if (avframedata == (void *)-1)  {   perror("shmget");  exit(EXIT_FAILURE);    }
	
	pInfo->video_info		= calloc( pInfo->video_info_nums, sizeof(VIDEO_BLK_INFO) );
//       __D("%s:sizeof(pInfo->video_info_nums*VIDEO_BLK_INFO)=%d,%d\n",__func__,pInfo->video_info_nums,sizeof(VIDEO_BLK_INFO));	
	memcpy(pInfo->video_info, Video_info_default, sizeof(Video_info_default));

	for (cnt = 0; cnt < loops ; cnt++ )
	{
		pInfo->video_info[cnt].start 	 = pInfo->start_addr + pInfo->offset;
		pInfo->video_info[cnt].start_phy = pInfo->start_phyAddr + pInfo->offset;
		pInfo->video_info[cnt].frame_num = pInfo->video_info[cnt].blk_num;
		pInfo->video_info[cnt].frame	 = calloc(pInfo->video_info[cnt].blk_num, sizeof(VIDEO_FRAME));

		for (cnt2 = 0; cnt2 < pInfo->video_info[cnt].blk_num ; cnt2++)
		{
			Video_Frame_reset(&(pInfo->video_info[cnt].frame[cnt2]));
		}

		pInfo->freesize -= pInfo->video_info[cnt].size;
		pInfo->offset	+= pInfo->video_info[cnt].size;

		if (pInfo->video_info[cnt].extrasize > 0)
		{
			pInfo->video_info[cnt].extradata = (void *)(pInfo->start_addr + pInfo->offset);
			pInfo->freesize -= pInfo->video_info[cnt].extrasize;
			pInfo->offset	+= pInfo->video_info[cnt].extrasize;
		}
		if (pInfo->freesize < 0 || pInfo->video_info[cnt].frame == NULL)
		{
			__E("pInfo->freesize = %d\n",pInfo->freesize);
			__E("Video_Mem_Malloc FAIL\n");
                    return -1;
		}
        __D("[%s:%d]video_info[%d]=%x,%x,%x %d %d\n",__func__,__LINE__,cnt,pInfo->video_info[cnt].size,pInfo->video_info[cnt].start,pInfo->video_info[cnt].start_phy,pInfo->video_info[cnt].width,pInfo->video_info[cnt].height);
	}

	return 0;
}

/**
 * @brief	Free video memory
 * @param	"MEM_MNG_INFO *pInfo"
 * @return	0 : success ; -1 : fail
 */
int Video_Mem_Free( MEM_MNG_INFO *pInfo )
{
	int	cnt = 0;

	pInfo->video_info_nums = sizeof( Video_info_default )/ sizeof( Video_info_default[0] );

	if (pInfo->video_info == NULL)
	{
		return -1;
	}

	for (cnt = 0; cnt < pInfo->video_info_nums ; cnt++)
	{
		if (pInfo->video_info[cnt].frame)
			free(pInfo->video_info[cnt].frame);
	}
	return 0;
}

int MemMng_Mem_layout(int mem_layout)
{
	extern CACHE_MNG Cache_mng_default[VIDEO_INFO_END];
	int cnt = 0;
	MEM_LAYOUT *pLayout = NULL;
	int totalsize = 0;
	int IsPrint = 0;

	if (mem_layout >= MEM_LAYOUT_NUM || mem_layout < 0)
		mem_layout = MEM_LAYOUT_DEFAULT;

	pLayout = &mem_layout_setting[mem_layout];
	if (IsPrint)
		printf("mem_layout = %d \n", mem_layout);

	for (cnt = 0; cnt < VIDEO_INFO_END; cnt++ )
	{
        Video_info_default[cnt].size		= pLayout->profiles[cnt].mem_size;
        Video_info_default[cnt].extrasize	= pLayout->profiles[cnt].ext_size;
        Video_info_default[cnt].blk_sz		= pLayout->profiles[cnt].mem_blk_size;
        Video_info_default[cnt].blk_num	= pLayout->profiles[cnt].mem_size/pLayout->profiles[cnt].mem_blk_size;
        Video_info_default[cnt].blk_free	= pLayout->profiles[cnt].mem_size/pLayout->profiles[cnt].mem_blk_size;

        Cache_mng_default[cnt].size		  = pLayout->profiles[cnt].cache_size;
        Cache_mng_default[cnt].blk_sz	  = pLayout->profiles[cnt].cache_blk_size;
        Cache_mng_default[cnt].blk_num	  = (pLayout->profiles[cnt].cache_size/pLayout->profiles[cnt].cache_blk_size);
        Cache_mng_default[cnt].cache_num  = (pLayout->profiles[cnt].cache_size/pLayout->profiles[cnt].cache_blk_size);
		

        totalsize += Video_info_default[cnt].size;
        totalsize += Video_info_default[cnt].extrasize;
        totalsize += Cache_mng_default[cnt].size;

        if (IsPrint)
        {
            printf("mem_size[%d]     =0x%X \n",cnt,(unsigned int)Video_info_default[cnt].size);
            printf("mem_blk_size[%d] =0x%X \n",cnt,Video_info_default[cnt].blk_sz);
            printf("mem_blk_free[%d] =0x%X \n",cnt,Video_info_default[cnt].blk_free);			
            printf("cache_size[%d]     =0x%X \n",cnt,Cache_mng_default[cnt].size);
            printf("cache_blk_size[%d] =0x%X \n",cnt,Cache_mng_default[cnt].blk_sz);
        }
	}
	if (IsPrint)
	{
		printf("totalsize          =0x%X \n",totalsize);
		printf("TOTAL_MEM_SIZE     =0x%X \n",TOTAL_MEM_SIZE);
	}
	
	return totalsize;
}

/**
 * @brief	Memory manager initialization
 * @param	"MEM_MNG_INFO *pInfo"
 * @return	0 : success ; -1 : fail
 */
int MemMng_Init( MEM_MNG_INFO *pInfo )
{
//	int mem_layout = pInfo->mem_layout;
//	int chn = pInfo->chn;
//	memset(pInfo, 0, sizeof(MEM_MNG_INFO));	
//    __D("%s:sizeof(MEM_MNG_INFO)=%d\n",__func__,sizeof(MEM_MNG_INFO));

//	pInfo->chn = chn;
	pInfo->totalsize  = MemMng_Mem_layout(pInfo->mem_layout);
//       pInfo->start_addr = (unsigned long)OSA_cmemAlloc(pInfo->totalsize, 32);
//	if (pInfo->start_addr == 0) goto MEM_INIT_FAIL;

//	pInfo->start_phyAddr	= CMEM_getPhys((void *)pInfo->start_addr);
//	if (pInfo->start_phyAddr == 0)	fprintf(stderr, "Failed to get physical address of %#x\n", (unsigned int) pInfo->start_phyAddr);
	pInfo->freesize 	    = pInfo->totalsize;
	pInfo->offset			= 0;
	pInfo->video_info_nums	= 0;

	if (Video_Mem_Malloc( pInfo ) == 0)
           __D(" MEM_INIT Success \n");
	else
          return -1;

  	MemMng_memcpy_open();

	return 0;

MEM_INIT_FAIL:
	__E(" MEM_INIT_FAIL \n");
	return -1;
}

int MemMng_release_ch(MEM_MNG_INFO *pInfo, int cnt)
{

	fprintf(stderr,"MemMng_release_ch1...\n");

	
	if (pInfo->video_info[cnt].gop.last_Start > 0)
	{
		//if (pInfo->video_info[cnt].gop.last_Start == frame_id)
		{
			pInfo->video_info[cnt].gop.last_Start		= -1;
			pInfo->video_info[cnt].gop.last_Start_serial	= -1;
			pInfo->video_info[cnt].gop.last_End		= -1;
			pInfo->video_info[cnt].gop.last_End_serial	= -1;
		}
	}
	fprintf(stderr,"MemMng_release_ch2...\n");

	if (pInfo->video_info[cnt].gop.lastest_I > 0)
	{
		//if (pInfo->video_info[cnt].gop.lastest_I == frame_id)
		{
			pInfo->video_info[cnt].gop.lastest_I = -1;
			pInfo->video_info[cnt].gop.last_End_serial = -1;
		}
	}
	fprintf(stderr,"MemMng_release_ch3...\n");

	pInfo->video_info[cnt].cur_serial = -1;

	pInfo->video_info[cnt].cur_frame = -1;
	pInfo->video_info[cnt].cur_blk = -1;
//	pInfo->video_info[cnt].frame_num 
	
	/*pInfo->video_info[cnt].frame[0].fram_type	= EMPTY_FRAME;
	pInfo->video_info[cnt].frame[0].serial 		= -1;
	pInfo->video_info[cnt].frame[0].blkindex	= -1;
	pInfo->video_info[cnt].frame[0].blks		= -1;
	pInfo->video_info[cnt].frame[0].realsize	= -1;
	pInfo->video_info[cnt].frame[0].flag		= 0;
	pInfo->video_info[cnt].frame[0].timestamp	= 0;*/
	int cnt2 ;
	//for (cnt2 = 0; cnt2 < VIDEO_INFO_END; cnt2++)
	{
		;//pInfo->video_info[cnt].frame[0].ref_serial[cnt2] = -1;
	}	
	 //pInfo->video_info[cnt].blk_num = 0;

//	for (cnt2 = 0; cnt2 < pInfo->video_info[cnt].blk_num ; cnt2++)
	{
	//	Video_Frame_reset(&(pInfo->video_info[cnt].frame[cnt2]));
	}	
  fprintf(stderr,"MemMng_release_ch4...\n");
}

/**
 * @brief	Release memory manager
 * @param	"MEM_MNG_INFO *pInfo"
 * @return	0 : success ; -1 : fail
 */
int MemMng_release(MEM_MNG_INFO *pInfo)
{
	Video_Mem_Free(pInfo);
	if (pInfo->video_info)
	{
		free(pInfo->video_info);
		pInfo->video_info = NULL;
	}
	if (pInfo->start_addr)
	{
//	     OSA_cmemFree((void *)pInfo->start_addr);

            /* detach shared memory */    
           if (shmdt(pInfo->start_phyAddr) == -1) { perror("shmdt"); exit(EXIT_FAILURE);  }       
	 
           /* destroy shared memory */    
           if (shmctl(pInfo->shm_id, IPC_RMID, 0) == -1) { perror("shmctl"); exit(EXIT_FAILURE); }      
           else printf("destroy shared memory\n");
		   
	    pInfo->start_addr = 0;
	}

	MemMng_memcpy_close();

	return 0;
}

/**
 * @brief	Open memory copy
 * @param	none
 * @return	0 : success ; -1 : fail
 */
int MemMng_memcpy_open(void)
{
	
//#if	MEMMNG_DMA
//	DRV_dmaOpen(&dmaHndl, DRV_DMA_MODE_NORMAL, 1);
//#endif

	return 0;
}


/**
 * @brief	Do memory copy
 * @param	"void *pDes" : destination address
 * @param	"void *pSrc" : source address
 * @param	"int size" : size
 * @return	0 : success ; others : fail
 */
int MemMng_memcpy(void *pDes, void *pSrc, int size )
{
#if	MEMMNG_DMA
	DRV_DmaCopy1D copy1D;

	Sem_lock( STREAM_SEM_MEMCPY);

	copy1D.srcPhysAddr 	= (unsigned long)DRV_dmaGetPhysAddr((unsigned long)pSrc);
	copy1D.dstPhysAddr 	= (unsigned long)DRV_dmaGetPhysAddr((unsigned long)pDes);
	copy1D.size 		=	size ;
	DRV_dmaCopy1D(&dmaHndl, &copy1D, 1);

	Sem_unlock( STREAM_SEM_MEMCPY);
#else
	STREAM_PARM *pParm=stream_get_handle();	
	unsigned char *avframedata = pParm->MemInfo.start_phyAddr; //avframedata;
	__D("MemMng_memcpy successed(%x,%x,%d) !!\n",pDes,pSrc,size);
	//	memcpy( pDes, pSrc, size );
	memcpy( (void*)avframedata + (unsigned long)pDes, pSrc, size );

//       MemMng_printheader(avframedata + (unsigned long)pSrc,5); 
//       MemMng_printheader(avframedata + (unsigned long)pDes,5); 
#endif

	return 0;
}

/**
 * @brief	Close memory copy
 * @param	none
 * @return	0 : success
 */
int MemMng_memcpy_close(void)
{
#if	MEMMNG_DMA
	DRV_dmaClose(&dmaHndl);
#endif
	return 0;

}

/**
 * @brief	Free video frame
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @param	"int frame_id" : frame ID
 * @return	0 : success ; -1 : fail
 */
int MemMng_VidFrame_Free( VIDEO_BLK_INFO *pVidInfo, int frame_id)
{
	int blkStart = -1;

	if (frame_id >= pVidInfo->frame_num || frame_id < 0)
	{
		__E("invalidate frame_id\n");
		return -1;
	}

	if (pVidInfo->frame[frame_id].fram_type == EMPTY_FRAME)
	{
//		__D("free frame already %d\n",frame_id);
		return 0;
	}

//	__E("pVidInfo->frame[frame_id].blks = %d\n",pVidInfo->frame[frame_id].blks);
	if (pVidInfo->frame[frame_id].blks > 0)
	{
		pVidInfo->blk_free += pVidInfo->frame[frame_id].blks;
		blkStart = pVidInfo->frame[frame_id].blkindex;
	}

	if (pVidInfo->gop.last_Start > 0)
	{
		if (pVidInfo->gop.last_Start == frame_id)
		{
			pVidInfo->gop.last_Start		= -1;
			pVidInfo->gop.last_Start_serial	= -1;
			pVidInfo->gop.last_End		= -1;
			pVidInfo->gop.last_End_serial	= -1;
		}
	}

	if (pVidInfo->gop.lastest_I > 0)
	{
		if (pVidInfo->gop.lastest_I == frame_id)
		{
			pVidInfo->gop.lastest_I = -1;
			pVidInfo->gop.last_End_serial = -1;
		}
	}

	Video_Frame_reset(&(pVidInfo->frame[frame_id]));

	return blkStart;
}

/**
 * @brief	Set flag to video frame
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @return	none
 */
void MemMng_VidFrame_SetFlag(VIDEO_BLK_INFO *pVidInfo)
{
	int last_frame = 0;
	int frame_type = pVidInfo->frame[pVidInfo->cur_frame].fram_type;

	if (frame_type == I_FRAME && pVidInfo->video_type == VIDEO_MP4)
	{
		pVidInfo->frame[pVidInfo->cur_frame].flag = 0x0001;

		/* check if the first frame */
		if ((pVidInfo->cur_frame - 1) > 0)
			last_frame = pVidInfo->cur_frame - 1;
        else
			last_frame = pVidInfo->frame_num - 1;

		if (pVidInfo->frame[last_frame].fram_type == P_FRAME ||
			pVidInfo->frame[last_frame].fram_type == B_FRAME)
			pVidInfo->frame[last_frame].flag = 0x0002;
	}
	else
	{
		pVidInfo->frame[pVidInfo->cur_frame].flag = 0;
	}
}

#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 16
#endif 
void MemMng_printheader(void *mem, unsigned int len)
{
        unsigned int i, j;
        
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

static int FindH264SPSPPSFrameType(unsigned char *buffer)
{
   int type = 0;
   int sps = 0;
   int pps = 0;
   unsigned char *nType=buffer;
//   nType =(unsigned char *)buffer;
//   printf("NALU ");

//   MemMng_printheader(buffer, 16);

     while( nType < buffer + 300) {      	
    	 nType = FindNextH264StartCode(nType,nType+300);
		//if(*nType == 0x67 || *nType == 0x68) fprintf(stderr,"CH3 %3x \n",*nType);   	 		

     	 if(*nType == 0x67 || *nType == 0x27) {  sps = 1; }  //(src[i+4]&0x1f)==7)
  	   else if(*nType == 0x68 || *nType == 0x28) { pps = 1; }
       else if(*nType == 0x41 || *nType == 0x21) { type = -1; break; }        
       else type = -1;//  //may be 00 00 00 01 09 10 00 00 00 01 67			
     }      
//      printf("\n");
	 if(sps == 1 && pps == 1)
	 	type = 1;
   return type;
}

/**
 * @brief	Insert video frame
 * @param	"void *pData"
 * @param	"int size"
 * @param	"int blks"
 * @param	"int frame_type"
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @return	0 : success ; -1 : fail
 */
MemMng_VidFrame_Insert( void *pData, int size, int blks, int frame_type, VIDEO_BLK_INFO *pVidInfo)
{
//VENC_STREAM_S *pstStream = (VENC_STREAM_S *)pData;
//HI_U32 size_v=0;	
//int i;

//  struct v4l2_buffer *pstStream = ( *)pData;
//HI_U32 size_v=0;	
//int i;

  unsigned long WriteAddr = 0;

	if (pVidInfo == NULL || frame_type >= END_FRAME_TYPE || blks <= 0)
	{
		return -1;
	}
	if (frame_type != DUMMY_FRAME)
	{	
	   if(pVidInfo->cur_serial == -1 && frame_type != I_FRAME && frame_type != AUDIO_FRAME)
			return -1;//first frame must be I-Frame
	   if( pVidInfo->cur_serial < INT_MAX ) pVidInfo->cur_serial++;
	   else	pVidInfo->cur_serial = 0; //avoid integer overflow
	}
	pVidInfo->cur_blk	= ( pVidInfo->cur_blk+1 ) % pVidInfo->blk_num;
	pVidInfo->cur_frame = ( pVidInfo->cur_frame+1 )% pVidInfo->frame_num;

	pVidInfo->frame[pVidInfo->cur_frame].serial		= pVidInfo->cur_serial;
	pVidInfo->frame[pVidInfo->cur_frame].fram_type	= frame_type;
	pVidInfo->frame[pVidInfo->cur_frame].blkindex	= pVidInfo->cur_blk;
	pVidInfo->frame[pVidInfo->cur_frame].blks		= blks;
	pVidInfo->frame[pVidInfo->cur_frame].realsize	= size;
	pVidInfo->frame[pVidInfo->cur_frame].timestamp	= pVidInfo->timestamp;

	if (frame_type != DUMMY_FRAME)	
	{
		 WriteAddr = pVidInfo->start + pVidInfo->cur_blk*pVidInfo->blk_sz;
		
//		if (!WriteAddr || pData == NULL)
    if( pData == NULL)
		{
			__E("MemMng_memcpy fail!!\n");
			return -1;
		}
#if 0			  
		if(frame_type!=0)//only for video, audio's frame_type is 0
		{
			for (i = 0; i < pstStream->u32PackCount; i++)  
			{		
				memcpy( (void*)avframedata + (unsigned long)WriteAddr+size_v, pstStream->pstPack[i].pu8Addr[0], pstStream->pstPack[i].u32Len[0]);	

				size_v += pstStream->pstPack[i].u32Len[0]; 
			   if (pstStream->pstPack[i].u32Len[1] > 0)
			   {
					memcpy( (void*)avframedata + (unsigned long)WriteAddr+size_v, pstStream->pstPack[i].pu8Addr[1], pstStream->pstPack[i].u32Len[1]);
				 size_v += pstStream->pstPack[i].u32Len[1];   	
			   }
			 }
		}
		else
		
		{//for audio
			MemMng_memcpy( (void *)WriteAddr, pData , size );
		}
#else
	  MemMng_memcpy( (void *)WriteAddr, pData , size );
#endif

	//memcpy( (void*)avframedata + (unsigned long)WriteAddr, pstStream, size );
		//MemMng_memcpy( (void *)WriteAddr, pData , size );
//		MemMng_memcpy( (void *)avframedata + WriteAddr, pData , size );
//              __D("%s:%d %p %d %d\n",__func__, __LINE__ , avframedata + WriteAddr, size,pVidInfo->frame[pVidInfo->cur_frame].serial);		
//              if(pVidInfo->video_type == VIDEO_MJPG) { MemMng_printheader(pData,5); MemMng_printheader(pData+size - 5,5); }			  			  
	}


//       __D("%s:%d\n",__func__, __LINE__);						   
	if (frame_type == I_FRAME && pVidInfo->video_type == VIDEO_MP4)
	{
		if (pVidInfo->gop.last_Start < 0)
		{
			pVidInfo->gop.last_Start = pVidInfo->cur_frame;
			pVidInfo->gop.lastest_I  = pVidInfo->cur_frame;

			pVidInfo->gop.last_Start_serial = pVidInfo->cur_serial;
			pVidInfo->gop.lastest_I_serial = pVidInfo->cur_serial;

			pVidInfo->gop.last_End = -1;
			pVidInfo->gop.last_End_serial = -1;
		}
        else
        {
			pVidInfo->gop.last_Start = pVidInfo->gop.lastest_I;
			pVidInfo->gop.last_Start_serial = pVidInfo->gop.lastest_I_serial;
			if ((pVidInfo->cur_frame - 1) > 0)
				pVidInfo->gop.last_End = pVidInfo->cur_frame - 1;
            else
				pVidInfo->gop.last_End = pVidInfo->frame_num - 1;

			pVidInfo->gop.last_End_serial = pVidInfo->cur_serial - 1;

			pVidInfo->gop.lastest_I = pVidInfo->cur_frame;
			pVidInfo->gop.lastest_I_serial = pVidInfo->cur_serial;
		}
	}
	//fprintf(stderr, "###1\n");
//	MemMng_printheader(pData, 16);
	if (pVidInfo->cur_serial == 0 && frame_type != DUMMY_FRAME )
	{
//	    fprintf(stderr, "###2 frame_type=%d,video_type=%d\n",frame_type,pVidInfo->video_type);
		if (pVidInfo->extradata != NULL && pVidInfo->extrasize != 0)
		{
//		    fprintf(stderr, "#####%s:%d %p %p,%d, frametype:%d size:%d\n",__func__, __LINE__,pData,pVidInfo->extradata,pVidInfo->extrasize, frame_type,size);						   
//			if(frame_type!=0)
//				memcpy( (void*)avframedata + (unsigned long)pVidInfo->extradata, (void*)avframedata + (unsigned long)WriteAddr, pVidInfo->extrasize);
//			else
//      MemMng_printheader(pData, 16);
			if(frame_type == I_FRAME && size>=64)//Frank, Geo 's bitstream sometimes show  size=5????
			{
				if(FindH264SPSPPSFrameType(pData)==1)
					MemMng_memcpy(pVidInfo->extradata,pData,pVidInfo->extrasize);
				else
				{
					fprintf(stderr, "##### error for first frame \n\n\n");
					pVidInfo->cur_serial = -1;
					return 0;
				}
			}
			else
			{ 
				fprintf(stderr, "##### error for frame_type == I_FRAME && size>=64 \n\n\n");
				pVidInfo->cur_serial = -1;
				return 0;
			}
//                  MemMng_printheader(pData,pVidInfo->extrasize);
		}
	}
//	fprintf(stderr, "###3 pVidInfo=%p video_type=%d cur_frame=%d frame_type=%d cur_serial=%d\n",pVidInfo,pVidInfo->video_type,pVidInfo->cur_frame,pVidInfo->frame[pVidInfo->cur_frame].fram_type,pVidInfo->cur_serial);

	MemMng_VidFrame_SetFlag(pVidInfo);

	/* Update  global info */
	pVidInfo->cur_blk = (pVidInfo->cur_blk + blks - 1) % pVidInfo->blk_num;
	pVidInfo->blk_free -= blks;
	
//	__D("[%d]MemMng_memcpy OK !! %d\n",__LINE__,pVidInfo->blk_free); 
	return 0;
}

/**
 * @brief	Video write
 * @param	"void *pData"
 * @param	"int size"
 * @param	"int frame_type"
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @return	0 : success ; -1 : fail
 */
int MemMng_Video_Write( void *pData, int size, int frame_type, VIDEO_BLK_INFO *pVidInfo )
{
	int blk_use = (size + pVidInfo->blk_sz-1) / pVidInfo->blk_sz;
	int IsContiguousBlk = 0;
	int	free_cnt = 0;

	if (blk_use > pVidInfo->blk_num || size == 0)
		goto MEM_MNG_WRITE_FAIL;

	do
      {
//          __D("%s: %d %d %d %d\n",__func__, __LINE__, IsContiguousBlk, blk_use,pVidInfo->blk_free );
	  if (blk_use > pVidInfo->blk_free)
	  {
		int nextframe = pVidInfo->cur_frame + 1;
		int freeframe = (nextframe + free_cnt) % pVidInfo->frame_num;
		/* Not enough free blk */
		if (MemMng_VidFrame_Free(pVidInfo, freeframe) < 0)
		{
			goto MEM_MNG_WRITE_FAIL;
		}
        else
        {
			free_cnt++;
//            __D("MemMng_VidFrame_Free[%d]\n",__LINE__);			
			continue;
		}
	  }
      else
      {
//        __D("[%d]\n", __LINE__);						   
		int nextblk = (pVidInfo->cur_blk + 1)%pVidInfo->blk_num;
		if ((nextblk + blk_use ) > pVidInfo->blk_num)
		{   /* No Contiguous Blk */
		   int dummy_blk_cnt = pVidInfo->blk_num - nextblk;
		   if (MemMng_VidFrame_Insert(NULL, 0, dummy_blk_cnt, DUMMY_FRAME, pVidInfo) < 0)
	 	   {
			   goto MEM_MNG_WRITE_FAIL;
	 	   }
           else
           {
			  free_cnt = 0;
              __D("MemMng_VidFrame_Insert[%d]\n", __LINE__);						   
			  continue;
		   }					 
	    }
        else
        {
	  	  IsContiguousBlk = 1;
	    }
	  }
	} while (!IsContiguousBlk);

	if (MemMng_VidFrame_Insert(pData, size, blk_use, frame_type, pVidInfo) < 0)
	{
		goto MEM_MNG_WRITE_FAIL;
	}
//    __D("[%d] pVidInfo=%p video_type=%d size=%d %d %d\n", __LINE__,pVidInfo, pVidInfo->video_type,size,pVidInfo->cur_frame,pVidInfo->cur_serial);						   
	return 0;

MEM_MNG_WRITE_FAIL:
	__E("MEM_MNG_WRITE_FAIL \n");
	return -1;
}


/**
 * @brief	Video read frame
 * @param	"void *pDest" : destination address
 * @param	"int *pSize" : size
 * @param	"int *pFrm_type" : frame type
 * @param	"int bufflimit" : buffer limit
 * @param	"VIDEO_FRAME *pFrame"
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @return	0 : success ; -1 : fail
 */
int MemMng_Video_ReadFrame(void *pDest, int *pSize, int *pFrm_type, int bufflimit, VIDEO_FRAME *pFrame, VIDEO_BLK_INFO *pVidInfo)
{
	if (pDest == NULL || pVidInfo == NULL || pFrame == NULL)
	{
		__E("MemMng_Video_ReadFrame: invalidate pointer\n");
		return -1;
	}

	if (pFrame->fram_type == DUMMY_FRAME || pFrame->fram_type == EMPTY_FRAME)
	{
		__E("MemMng_Video_ReadFrame: invalidate frame_type\n");
		return -1;
	}

	if (pFrame->realsize > bufflimit)
	{
		__E("MemMng_Video_ReadFrame: Over bufflimit\n");
		return -1;
	}

	void *pSrc = (void *)(pVidInfo->start + pFrame->blkindex*pVidInfo->blk_sz);

	*pSize 		= pFrame->realsize;
	*pFrm_type 	= pFrame->fram_type;

	MemMng_memcpy( pDest, pSrc, pFrame->realsize );

	return 0;
}

/**
 * @brief	Get video frame by serial
 * @param	"int serial" : serial no.
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @return	frame address ; NULL
 */
VIDEO_FRAME * MemMng_GetFrameBySerial( int serial , VIDEO_BLK_INFO *pVidInfo)
{
	if (serial < 0 || pVidInfo == NULL || serial > pVidInfo->cur_serial)
	{
		//__E("MemMng_GetFrameBySerial: Fail \n");
		return NULL;
	}

	int cnt = 0;
	if (serial == pVidInfo->frame[pVidInfo->cur_frame].serial)
	{
		return &(pVidInfo->frame[pVidInfo->cur_frame]);
	}

#if 0
       int i=0;
 	 __E("START\n");			
	for (cnt = 0; cnt < pVidInfo->frame_num ; cnt++)
	{		
 	  if(pVidInfo->video_type == 1)
 	  __E("Mem video(%d) = %d frame = %d flag = %d serial = %d %d %d %d\n",i++,pVidInfo->video_type,pVidInfo->frame[cnt].fram_type, pVidInfo->frame[cnt].flag,serial, pVidInfo->frame[cnt].serial,pVidInfo->cur_frame,pVidInfo->frame_num);		
	}
 	 __E("END\n");			
#endif 	 

	for (cnt = 0; cnt < pVidInfo->frame_num ; cnt++)
	{	
		if (serial == pVidInfo->frame[cnt].serial )
		{
			if (pVidInfo->frame[cnt].fram_type == DUMMY_FRAME ||
				pVidInfo->frame[cnt].fram_type == EMPTY_FRAME)
			{
				//__E("MemMng_GetFrameBySerial: invalidate frame_type\n");
				//return NULL;
				continue;
			}
			return &(pVidInfo->frame[cnt]);
		}
	}
	__D("MemMng_GetFrameBySerial: Search no frame\n");
	return NULL;
}

/**
 * @brief	Get physical base address
 * @param	"MEM_MNG_INFO *pInfo"
 * @return	physical base address
 */
unsigned long GetMemMngPhyBaseAddr(MEM_MNG_INFO *pInfo)
{
	if (pInfo == NULL)
		return 0;
	return pInfo->start_phyAddr;
}

/**
 * @brief	Get total memory size
 * @param	"MEM_MNG_INFO *pInfo"
 * @return	totalsize
 */
unsigned long GetMemMngTotalMemSize(MEM_MNG_INFO *pInfo)
{
	if (pInfo == NULL)
		return 0;
	return pInfo->totalsize;
}

/**
 * @brief	Get current serial number
 * @param	"VIDEO_BLK_INFO *video_info"
 * @return	current serial number
 */
int GetCurrentSerialNo(VIDEO_BLK_INFO *video_info)
{
	return video_info->cur_serial;
}

/**
 * @brief	Get current frame
 * @param	"VIDEO_BLK_INFO *video_info"
 * @return	current frame
 */
VIDEO_FRAME * GrabCurrentFrame(VIDEO_BLK_INFO *video_info)
{					
//	__D("[%s:%d] video_info=%p %d %d %d\n",__func__, __LINE__, video_info,video_info->cur_frame, video_info->frame[video_info->cur_frame].serial, video_info->frame[video_info->cur_frame].timestamp);
	if (video_info == NULL)	{ __D("[%s:%d]\n",__func__,__LINE__); return NULL; }	
	if (video_info->cur_frame < 0){	__D("[%s:%d]\n",__func__,__LINE__); return NULL; }
	
	__D("[%s:%d] video_info=%p %d %d %d\n",__func__, __LINE__, video_info,video_info->cur_frame, video_info->frame[video_info->cur_frame].serial, video_info->frame[video_info->cur_frame].timestamp);	
	return &(video_info->frame[video_info->cur_frame]);
}

/**
 * @brief	Get current frame
 * @param	"VIDEO_BLK_INFO *video_info"
 * @return	current frame
 */
VIDEO_FRAME * GetLastI_Frame(VIDEO_BLK_INFO *video_info)
{
	if (video_info == NULL)
		return NULL;
	if (video_info->gop.lastest_I < 0)
		return NULL;
	return &(video_info->frame[video_info->gop.lastest_I]);
}

/**
 * @brief	Get current offset
 * @param	"VIDEO_BLK_INFO *video_info"
 * @return	current offset
 */
int GetCurrentOffset(VIDEO_BLK_INFO *video_info)
{
	return video_info->cur_blk*video_info->blk_sz;
}

typedef struct _TEST_TBL{
	int size;
	int video_flag;
}TEST_TBL;


TEST_TBL testTbl[] = {

    { 90*1024, I_FRAME },
    { 100*1024, P_FRAME },
    { 120*1024, P_FRAME },
    { 100*1024, P_FRAME },
    { 300*1024, I_FRAME },
    { 100*1024, P_FRAME },
    { 100*1024, P_FRAME },
    { 100*1024, I_FRAME },
    { 100*1024, P_FRAME },
    { 120*1024, P_FRAME },
    { 100*1024, P_FRAME },
    { 300*1024, I_FRAME },
    { 100*1024, P_FRAME },
    { 500*1024, P_FRAME },
    { 300*1024, I_FRAME },
    { 100*1024, P_FRAME },
    { 100*1024, P_FRAME },
    { 100*1024, P_FRAME },
    { 100*1024, I_FRAME },
    { 100*1024, I_FRAME },
    { 300*1024, P_FRAME },
    { 100*1024, P_FRAME },
    { 100*1024, P_FRAME },
    { 100*1024, I_FRAME },

};

char TestData[1000*1024];

/**
 * @brief	Test memory manager
 * @param	none
 * @return	none
 */
void TestMemMng(void)
{
	VIDEO_BLK_INFO *pVidInfo;
	MEM_MNG_INFO MemInfo;

	int tblnum = sizeof(testTbl)/sizeof(testTbl[0]);
	int cnt = 0;

	MemMng_Init( &MemInfo );

	pVidInfo = &MemInfo.video_info[0];

	for (cnt = 0; cnt < tblnum; cnt++)
	{
		__D("cnt  %d start\n",cnt);
		MemMng_Video_Write(TestData, testTbl[cnt].size, testTbl[cnt].video_flag, pVidInfo);
		__D("cnt  %d end\n",cnt);
	}

	__D("MemMng_release \n");
	MemMng_release( &MemInfo );
}
