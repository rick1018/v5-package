/*
 * Copyright (c) 2014, InfoTM Microelectronics Co.,Ltd
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of  InfoTM Microelectronics Co. nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANIMES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANIMES OF MERCHANTABILITY AND FITNESS FOR A PARIMCULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENIMAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSIMTUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPIMON) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * @file      library_register.c
 * @brief
 * @author    ayakashi (eric.xu@infotmic.com.cn)
 * @version   1.1.0
 * @history
 *   2012.04.15 : Create
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "IM_OSAL_Memory.h"
#include "IM_OSAL_ETC.h"
#include "library_register.h"

#undef  IM_LOG_TAG
#define IM_LOG_TAG    "IM_AUDIO_DEC"
#define IM_LOG_OFF
#include "IM_OSAL_Trace.h"


OSCL_EXPORT_REF int IM_OMX_COMPONENT_Library_Register(IMRegisterComponentType **ppIMComponent)
{
    IM_OSAL_Entering();

    if (ppIMComponent == NULL)
        goto EXIT;

    /* component 1 - audio decoder MP2 */
    IM_OSAL_Strcpy(ppIMComponent[0]->componentName, IM_OMX_COMPONENT_AUDIO_DEC);
    IM_OSAL_Strcpy(ppIMComponent[0]->roles[0], IM_OMX_COMPONENT_MP2_DEC_ROLE);
    IM_OSAL_Strcpy(ppIMComponent[0]->roles[1], IM_OMX_COMPONENT_AAC_DEC_ROLE);
    ppIMComponent[0]->totalRoleNum = MAX_COMPONENT_ROLE_NUM;

EXIT:
    IM_OSAL_Exiting();
    return MAX_COMPONENT_NUM;
}

