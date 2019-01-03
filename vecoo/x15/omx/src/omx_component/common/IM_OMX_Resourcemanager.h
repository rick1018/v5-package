/*
 * Copyright (c) 2012, InfoTM Microelectronics Co.,Ltd
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
 * @file       IM_OMX_Resourcemanager.h
 * @brief
 * @author     ayakashi (yanyuan_xu@infotm.com)
 * @version    1.0
 * @history
 *    2010.03.01 : Create
 */

#ifndef IM_OMX_RESOURCEMANAGER
#define IM_OMX_RESOURCEMANAGER


#include "IM_OMX_Def.h"
#include "OMX_Component.h"


struct IM_OMX_RM_COMPONENT_LIST;
typedef struct _IM_OMX_RM_COMPONENT_LIST
{
    OMX_COMPONENTTYPE         *pOMXStandComp;
    OMX_U32                    groupPriority;
    struct IM_OMX_RM_COMPONENT_LIST *pNext;
} IM_OMX_RM_COMPONENT_LIST;


#ifdef __cplusplus
extern "C" {
#endif

OMX_ERRORTYPE IM_OMX_ResourceManager_Init();
OMX_ERRORTYPE IM_OMX_ResourceManager_Deinit();
OMX_ERRORTYPE IM_OMX_Get_Resource(OMX_COMPONENTTYPE *pOMXComponent);
OMX_ERRORTYPE IM_OMX_Release_Resource(OMX_COMPONENTTYPE *pOMXComponent);
OMX_ERRORTYPE IM_OMX_In_WaitForResource(OMX_COMPONENTTYPE *pOMXComponent);
OMX_ERRORTYPE IM_OMX_Out_WaitForResource(OMX_COMPONENTTYPE *pOMXComponent);

#ifdef __cplusplus
};
#endif

#endif
