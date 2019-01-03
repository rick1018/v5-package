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
 * @file       IM_OMX_Component_Register.h
 * @brief      IM OpenMAX IL Component Register
 * @author     SeungBeom Kim (sbcrux.kim@samsung.com)
 * @version    1.0
 * @history
 *    2010.03.01 : Create
 */

#ifndef IM_OMX_COMPONENT_REG
#define IM_OMX_COMPONENT_REG

#include "IM_OMX_Def.h"
#include "OMX_Types.h"
#include "OMX_Core.h"
#include "OMX_Component.h"


typedef struct _IMRegisterComponentType
{
    OMX_U8  componentName[MAX_OMX_COMPONENT_NAME_SIZE];
    OMX_U8  roles[MAX_OMX_COMPONENT_ROLE_NUM][MAX_OMX_COMPONENT_ROLE_SIZE];
    OMX_U32 totalRoleNum;
} IMRegisterComponentType;

typedef struct _IM_OMX_COMPONENT_REGLIST
{
    IMRegisterComponentType component;
    OMX_U8  libName[MAX_OMX_COMPONENT_LIBNAME_SIZE];
} IM_OMX_COMPONENT_REGLIST;

struct IM_OMX_COMPONENT;
typedef struct _IM_OMX_COMPONENT
{
    OMX_U8                    componentName[MAX_OMX_COMPONENT_NAME_SIZE];
    OMX_U8                    libName[MAX_OMX_COMPONENT_LIBNAME_SIZE];
    OMX_HANDLETYPE            libHandle;
    OMX_COMPONENTTYPE        *pOMXComponent;
    struct IM_OMX_COMPONENT *nextOMXComp;
} IM_OMX_COMPONENT;


#ifdef __cplusplus
extern "C" {
#endif


OMX_ERRORTYPE IM_OMX_Component_Register(IM_OMX_COMPONENT_REGLIST **compList, OMX_U32 *compNum);
OMX_ERRORTYPE IM_OMX_Component_Unregister(IM_OMX_COMPONENT_REGLIST *componentList);
OMX_ERRORTYPE IM_OMX_ComponentLoad(IM_OMX_COMPONENT *IM_component);
OMX_ERRORTYPE IM_OMX_ComponentUnload(IM_OMX_COMPONENT *IM_component);


#ifdef __cplusplus
}
#endif

#endif
