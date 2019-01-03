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
--  Abstract : Hantro 6280/7280/8270 Encoder Wrapper Layer for OS services
--
--------------------------------------------------------------------------------
--
--  Version control information, please leave untouched.
--
--  $RCSfile: ewl_x280_common.h,v $
--  $Revision: 1.1 $
--
------------------------------------------------------------------------------*/
#ifndef __EWL_X280_COMMON_H__
#define __EWL_X280_COMMON_H__

#include <stdio.h>
#include <signal.h>
#include "ewl_lock.h"

#ifdef IM_DEBUG
#define PTRACE(trace, ...) printf(trace, ##__VA_ARGS__)
#else
#define PTRACE(trace, ...)
#endif

#define ASIC_STATUS_IRQ_INTERVAL        0x100
#define ASIC_STATUS_TEST_COPY_RDY       0x80
#define ASIC_STATUS_TEST_IRQ            0x40
#define ASIC_STATUS_BUFF_FULL           0x20
#define ASIC_STATUS_RESET               0x10
#define ASIC_STATUS_ERROR               0x08
#define ASIC_STATUS_FRAME_READY         0x04
#define ASIC_IRQ_LINE                   0x01

#define ASIC_STATUS_ALL     (ASIC_STATUS_IRQ_INTERVAL |\
                             ASIC_STATUS_TEST_IRQ |\
                             ASIC_STATUS_TEST_COPY_RDY |\
                             ASIC_STATUS_FRAME_READY | \
                             ASIC_STATUS_BUFF_FULL | \
                             ASIC_STATUS_RESET | \
                             ASIC_STATUS_ERROR)

// IOCTL code, must keep it same with kernel driver.
#define HX280ENC_IOCGHWOFFSET       0x1000
#define HX280ENC_IOCGHWIOSIZE       0x1001
#define HX280ENC_IOC_RESERVE_HW     0x1002
#define HX280ENC_IOC_RELEASE_HW     0x1003

#define ENC_MODULE_PATH             "/dev/venc"


#include <InfotmMedia.h>
#include <IM_buffallocapi.h>

typedef struct
{
    u32 clientType;
    int fd_enc;
    u32 regSize;
    u32 regBase;
    volatile u32 *pRegBase;
    ALCCTX	alc;
} hx280ewl_t;

#endif /* __EWLX280_COMMON_H__ */
