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
  --  Description : Encoder Wrapper Layer (user space module)
  --
  --------------------------------------------------------------------------------
  --
  --  Version control information, please leave untouched.
  --
  --  $RCSfile: ewl_x280_irq.c,v $
  --  $Revision: 1.1 $
  --
  ------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  1. Include headers
  ------------------------------------------------------------------------------*/

#include "basetype.h"
#include "ewl.h"
//#include "hx280enc.h"   /* This EWL uses the kernel module */
#include "ewl_x280_common.h"
//#include "ewl_lock.h"

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include <semaphore.h>
#include <poll.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

/*------------------------------------------------------------------------------
  2. External compiler flags
  --------------------------------------------------------------------------------

  ENC_MODULE_PATH     defines the path for encoder device driver nod.
  e.g. "/dev/imapx200-venc"
  MEMALLOC_MODULE_PATH defines the path for memalloc device driver nod.
  e.g. "/dev/memalloc"
  ENC_IO_BASE         defines the IO base address for encoder HW registers
  e.g. 0xC0000000
  SDRAM_LM_BASE       defines the base address for SDRAM as seen by HW
  e.g. 0x80000000

  --------------------------------------------------------------------------------
  3. Module defines
  ------------------------------------------------------------------------------*/

extern volatile u32 asic_status;

/*******************************************************************************
  Function name   : EWLReserveHw
Description     : Reserve HW resource for currently running codec
 *******************************************************************************/
i32 EWLReserveHw(const void *inst)
{
	hx280ewl_t *enc = (hx280ewl_t *)inst;

    PTRACE("%s()\n", __FUNCTION__);

	if(inst == NULL)
	{
		return EWL_ERROR;
	}

	if(ioctl(enc->fd_enc, HX280ENC_IOC_RESERVE_HW, NULL))
	{
		PTRACE("EWLReserveHw() failed\n");
		return -1;
	}

	EWLWriteReg(inst, 0x38, 0);

	return EWL_OK;
}

/*******************************************************************************
  Function name   : EWLWaitHwRdy
Description     : Wait for the encoder semaphore
Return type     : i32 
Argument        : void
 *******************************************************************************/
i32 EWLWaitHwRdy(const void *inst)
{
	hx280ewl_t *enc = (hx280ewl_t *)inst;

	int ret;
	fd_set fds;
	struct timeval tv;

	struct timeval old;
	struct timeval cur;
	unsigned int timeuse = 0;
    
    PTRACE("%s()\n", __FUNCTION__);

	memset(&old, 0x00, sizeof(struct timeval));
	memset(&cur, 0x00, sizeof(struct timeval));

	/* Check invalid parameters */
	if(enc == NULL)
	{
		return EWL_HW_WAIT_ERROR;
	}

	FD_ZERO(&fds);
	FD_SET(enc->fd_enc, &fds);

	tv.tv_sec	= 100;
	tv.tv_usec	= 0;
	//tv.tv_sec	= 0;
	//tv.tv_usec	= 60 * 1000;

	gettimeofday(&old, NULL);
	ret = select(enc->fd_enc + 1, &fds, NULL, NULL, &tv);
	gettimeofday(&cur, NULL);
	if(ret < 0)
	{
        PTRACE("select() failed");
		return EWL_HW_WAIT_ERROR;
	}
	if(ret == 0)
	{
        PTRACE("select() timeout");
		return EWL_HW_WAIT_ERROR;
	}

	timeuse = (cur.tv_sec - old.tv_sec) * 1000 + (cur.tv_usec - old.tv_usec) / 1000;
    PTRACE("[EWL] timeuse %dms\n", timeuse);

	asic_status = enc->pRegBase[1]; /* update the buffered asic status */

	return EWL_OK;
}
