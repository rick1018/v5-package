/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Description : System Wrapper Layer for Linux using IRQs
--
------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: dwl_x170_linux_irq.c,v $
--  $Revision: 1.15 $
--  $Date: 2010/03/24 06:21:33 $
--
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "dwl.h"
#include "dwl_linux.h"
#include "hx170dec.h"
//#include "memalloc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <poll.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>

#include <errno.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cutils/log.h>

/*#include <items.h>*/

#include "InfotmMedia.h"
/* a mutex protecting the wrapper init */
pthread_mutex_t x170_init_mutex = PTHREAD_MUTEX_INITIALIZER;

/* the decoder device driver nod */
const char *dec_dev = DEC_MODULE_PATH;

/* the memalloc device driver nod */
//const char *mem_dev = MEMALLOC_MODULE_PATH;

/*------------------------------------------------------------------------------
    Function name   : DWLInit
    Description     : Initialize a DWL instance

    Return type     : const void * - pointer to a DWL instance

    Argument        : void * param - not in use, application passes NULL
------------------------------------------------------------------------------*/
const void *DWLInit(DWLInitParam_t * param)
{
    hX170dwl_t *dec_dwl;
    unsigned long base;

    dec_dwl = (hX170dwl_t *) malloc(sizeof(hX170dwl_t));
    memset(dec_dwl, 0, sizeof(hX170dwl_t));

    DWL_DEBUG("DWL: INITIALIZE\n");
    if(dec_dwl == NULL)
    {
        DWL_DEBUG("DWL: failed to alloc hX170dwl_t struct\n");
        return NULL;
    }

	dec_dwl->clientType = param->clientType;

	pthread_mutex_lock(&x170_init_mutex);

    dec_dwl->fd_mem = -1;
    dec_dwl->alc_inst = NULL;
    dec_dwl->pRegBase = MAP_FAILED;
    dec_dwl->mmu_enable = 1;

    /* Linear memories not needed in pp */
    if(dec_dwl->clientType != DWL_CLIENT_TYPE_PP)
    {
        /* open memalloc for linear memory allocation */
        if(alc_open(&dec_dwl->alc_inst, IM_STR("vdec_lib_g1")) != IM_RET_OK) 
        {
            DWL_DEBUG("DWL: failed to open: %s\n");
            goto err;
        }
	}

	dec_dwl->fd_mem = -1;

	/* 
	 * In Android system, /dev/mem functionality has been realized in 
	 * /dev/memalloc, but we use alc_alloc instead of it  
	 */
#if 0
	if(dec_dwl->fd_mem < 0)
	{
		dec_dwl->fd_mem = open("/dev/memalloc", O_RDWR | O_SYNC);
	}

	if(dec_dwl->fd_mem == -1)
	{
		DWL_DEBUG("DWL: failed to open: %s\n", "/dev/memalloc");
		goto err;
	}
#endif 
    switch (dec_dwl->clientType)
    {
    case DWL_CLIENT_TYPE_H264_DEC:
    case DWL_CLIENT_TYPE_MPEG4_DEC:
    case DWL_CLIENT_TYPE_JPEG_DEC:
    case DWL_CLIENT_TYPE_VC1_DEC:
    case DWL_CLIENT_TYPE_MPEG2_DEC:
    case DWL_CLIENT_TYPE_VP6_DEC:
    case DWL_CLIENT_TYPE_VP8_DEC:
    case DWL_CLIENT_TYPE_RV_DEC:
    case DWL_CLIENT_TYPE_AVS_DEC:
    case DWL_CLIENT_TYPE_PP:
        {
            /* open the device */
				dec_dwl->fd = open(dec_dev, O_RDWR | O_SYNC);
				if(dec_dwl->fd == -1)
				{
					DWL_DEBUG("DWL: failed to open '%s'\n", dec_dev);
					goto err;
				}

				/* Notify kernel module that this is a pp instance */
				/* NOTE: Must be called befor registerring for interrupt! */
				if(dec_dwl->clientType == DWL_CLIENT_TYPE_PP){
					DWL_DEBUG("notify kernel this is pp instance \n");
					ioctl(dec_dwl->fd, HX170DEC_PP_INSTANCE);
				}

            break;
        }
    default:
        {
            DWL_DEBUG("DWL: Unknown client type no. %d\n", dec_dwl->clientType);
            goto err;
        }
    }

    if(ioctl(dec_dwl->fd, HX170DEC_IOCGHWOFFSET, &base) == -1)
    {
        DWL_DEBUG("DWL: ioctl failed\n");
        goto err;
    }

    if(ioctl(dec_dwl->fd, HX170DEC_IOCGHWIOSIZE, &dec_dwl->regSize) == -1)
    {
        DWL_DEBUG("DWL: ioctl failed\n");
        goto err;
    }

    /* map the hw registers to user space */
    dec_dwl->pRegBase =
        DWLMapRegisters(dec_dwl->fd, base, dec_dwl->regSize, 1);

    if(dec_dwl->pRegBase == MAP_FAILED)
    {
        DWL_DEBUG("DWL: Failed to mmap regs\n");
        goto err;
    }

    dec_dwl->dvfsEna = 0;
#ifdef DVFS_SUPPORT
    if(ioctl(dec_dwl->fd, HX170DEC_GETDVFS, &dec_dwl->dvfsEna) == -1)
    {
        DWL_DEBUG("DWL: ioctl failed to get dvfs state\n");
    }
#endif 
	
    pthread_mutex_unlock(&x170_init_mutex);
    return dec_dwl;

  err:

    pthread_mutex_unlock(&x170_init_mutex);
    DWLRelease(dec_dwl);

    return NULL;
}

/*------------------------------------------------------------------------------
    Function name   : DWLRelease
    Description     : Release a DWl instance

    Return type     : i32 - 0 for success or a negative error code

    Argument        : const void * instance - instance to be released
------------------------------------------------------------------------------*/
i32 DWLRelease(const void *instance)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

    assert(dec_dwl != NULL);

    pthread_mutex_lock(&x170_init_mutex);
    if(dec_dwl->pRegBase != MAP_FAILED)
        DWLUnmapRegisters((void *) dec_dwl->pRegBase, dec_dwl->regSize);

    if(dec_dwl->fd_mem != -1)
        close(dec_dwl->fd_mem);

    if(dec_dwl->fd != -1)
        close(dec_dwl->fd);

    /* linear memory allocator */
    if(dec_dwl->alc_inst != NULL)
        alc_close(dec_dwl->alc_inst);

	free(dec_dwl);

    pthread_mutex_unlock(&x170_init_mutex);

    return (DWL_OK);
}

/*------------------------------------------------------------------------------
    Function name   : DWLWaitDecHwReady
    Description     : Wait until hardware has stopped running
                        and Decoder interrupt comes.
                      Used for synchronizing software runs with the hardware.
                      The wait could succed, timeout, or fail with an error.

    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT
                                              DWL_HW_WAIT_ERROR

    Argument        : const void * instance - DWL instance
------------------------------------------------------------------------------*/
i32 DWLWaitDecHwReady(const void *instance, u32 timeout)
{
    hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

	int ret;
	fd_set fds;
	struct timeval tv;

	/* Check invalid parameters */
	if(dec_dwl == NULL)
	{
		assert(0);
		return DWL_HW_WAIT_ERROR;
	}

	memset(&tv, 0x00, sizeof(struct timeval));

	{
#ifdef TIME_DEBUG
		struct timeval oldTime;
		struct timeval curTime;
		unsigned int singleDecTime = 0;
		unsigned long long average_time = 0;

		gettimeofday(&oldTime, NULL);
#endif
		FD_ZERO(&fds);
		FD_SET(dec_dwl->fd, &fds);

		tv.tv_sec	= dec_dwl->clientType == DWL_CLIENT_TYPE_JPEG_DEC ? 2 : 0;
		tv.tv_usec	= 200*1000;//60 * 1000;	/* wait 200ms */

		while(!DWL_DECODER_INT)
		{
			ret = select(dec_dwl->fd + 1, &fds, NULL, NULL, &tv);
			if(ret < 0)
			{
				DWL_DEBUG("select decode error\n");
				/* return DWL_HW_WAIT_ERROR; */
				break;
			}
			else if(ret == 0)
			{
				DWL_DEBUG("select decode timeout\n");
				return DWL_HW_WAIT_TIMEOUT;
				/* return DWL_HW_WAIT_ERROR; */
				//break;
			}
		}
#ifdef TIME_DEBUG
		gettimeofday(&curTime, NULL);

		singleDecTime = (curTime.tv_sec - oldTime.tv_sec) * 1000 + \
				(curTime.tv_usec - oldTime.tv_usec) / 1000;
		DWL_DEBUG("[DWL DECODE] decode hard ware time use %dms\n", singleDecTime);
		dec_dwl->total_dec_time += singleDecTime;
		dec_dwl->total_dec_count++;
		average_time = dec_dwl->total_dec_time / dec_dwl->total_dec_count;
		DWL_DEBUG("[DWL DECODE] decode hard ware count = %lld, totaltime = %lld\n", dec_dwl->total_dec_count, dec_dwl->total_dec_time);
		DWL_DEBUG("[DWL DECODE] decode hard ware average time use %lldms\n", average_time);
#endif
	}

	return DWL_HW_WAIT_OK;
}

/*------------------------------------------------------------------------------
    Function name   : DWLWaitPpHwReady
    Description     : Wait until hardware has stopped running
                      and pp interrupt comes.
                      Used for synchronizing software runs with the hardware.
                      The wait could succed, timeout, or fail with an error.

    Return type     : i32 - one of the values DWL_HW_WAIT_OK
                                              DWL_HW_WAIT_TIMEOUT
                                              DWL_HW_WAIT_ERROR

    Argument        : const void * instance - DWL instance
------------------------------------------------------------------------------*/
i32 DWLWaitPpHwReady(const void *instance, u32 timeout)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *) instance;

	int ret;
	fd_set fds;
	struct timeval tv;

	/* Check invalid parameters */
	if(dec_dwl == NULL)
	{
		assert(0);
		return DWL_HW_WAIT_ERROR;
	}

	memset(&tv, 0x00, sizeof(struct timeval));

	{
#ifdef TIME_DEBUG
		struct timeval oldTime;
		struct timeval curTime;
		unsigned int singlePpTime = 0;
		unsigned long long average_time = 0;

		gettimeofday(&oldTime, NULL);
#endif

		FD_ZERO(&fds);
		FD_SET(dec_dwl->fd, &fds);

		tv.tv_sec	= 0;
		tv.tv_usec	= 50 * 1000;	/* 50ms */

		ret = select(dec_dwl->fd + 1, &fds, NULL, NULL, &tv);
		if(ret < 0)
		{
			DWL_DEBUG("select pp error\n");
			/* return DWL_HW_WAIT_ERROR; */
		}
		else if(ret == 0)
		{
            DWL_DEBUG("select pp timeout\n");
			//return DWL_HW_WAIT_TIMEOUT; 
		}

#ifdef TIME_DEBUG
		gettimeofday(&curTime, NULL);

		singlePpTime = (curTime.tv_sec - oldTime.tv_sec) * 1000 + \
			       (curTime.tv_usec - oldTime.tv_usec) / 1000;
		DWL_DEBUG("[DWL PP] pp hard ware time use %dms\n", singlePpTime);
		dec_dwl->total_pp_time += singlePpTime;
		dec_dwl->total_pp_count++;
		average_time = dec_dwl->total_pp_time / dec_dwl->total_pp_count;
		DWL_DEBUG("[DWL DECODE] pp hw count = %lld, totaltime = %lld\n", dec_dwl->total_pp_count, dec_dwl->total_pp_time);
		DWL_DEBUG("[DWL DECODE] pphw average time %lldms\n", average_time);
#endif
	}

	return DWL_HW_WAIT_OK;

}

/* HW locking */

/*------------------------------------------------------------------------------
    Function name   : DWLReserveHw
    Description     :
    Return type     : i32
    Argument        : const void *instance
------------------------------------------------------------------------------*/
i32 DWLReserveHw(const void *instance)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *)instance;
    
	if(ioctl(dec_dwl->fd, HX170DEC_MUTEX_LOCK, &(dec_dwl->mmu_enable)) == -1){
		return DWL_ERROR; 
	}	

	return DWL_OK;
}

void  DWLLibReset(const void *instance)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *)instance;
    
	if(ioctl(dec_dwl->fd, HX170DEC_RESET) == -1){
		DWL_DEBUG("reseT hw failed");
		return;// DWL_ERROR; 
	}	

	return ;//DWL_OK;
}

void  DWLUpdateClk(const void *instance)
{
	hX170dwl_t *dec_dwl = (hX170dwl_t *)instance;
    
	if(ioctl(dec_dwl->fd, HX170DEC_UPDATE_CLK) == -1){
		return;// DWL_ERROR; 
	}	

	return ;//DWL_OK;
}


