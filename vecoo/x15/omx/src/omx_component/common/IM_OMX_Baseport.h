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
 * @file       IM_OMX_Baseport.h
 * @brief
 * @author     ayakashi (yanyuan_xu@infotm.com)
 * @version    1.0
 * @history
 *    2010.03.01 : Create
 */

#ifndef IM_OMX_BASE_PORT
#define IM_OMX_BASE_PORT


#include "OMX_Component.h"
#include "IM_OMX_Def.h"
#include "IM_OSAL_Queue.h"


#define BUFFER_STATE_ALLOCATED  (1 << 0)
#define BUFFER_STATE_ASSIGNED   (1 << 1)
#define HEADER_STATE_ALLOCATED  (1 << 2)
#define BUFFER_STATE_MAPPED     (1 << 3)  //infotm buffer map
#define BUFFER_STATE_ATTACHED   (1 << 4)  //infotm buffer mmu attach
#define BUFFER_OWNED_BY_DECLIB  (1 << 5)  //this flags only be modified by vdec component

#define BUFFER_STATE_FREE        0

#define MAX_BUFFER_NUM          24

#define INPUT_PORT_INDEX    0
#define OUTPUT_PORT_INDEX   1
#define ALL_PORT_INDEX     -1
#define ALL_PORT_NUM        2

typedef struct _IM_OMX_BUFFERHEADERTYPE
{
    OMX_BUFFERHEADERTYPE *OMXBufferHeader;
    OMX_BOOL              bBufferInOMX;
    OMX_HANDLETYPE        ANBHandle;
    OMX_PTR               pPrivate; 
} IM_OMX_BUFFERHEADERTYPE;

typedef enum _IM_PORT_TRANSITIONS{
    PORT_NONE,
    PORT_DISABLING,
    PORT_ENABLING
}IM_PORT_TRANSITIONS;

typedef struct _IM_OMX_BASEPORT
{
    IM_OMX_BUFFERHEADERTYPE       *extendBufferHeader;
    OMX_U32                       *bufferStateAllocate;
    OMX_PARAM_PORTDEFINITIONTYPE   portDefinition;
    OMX_HANDLETYPE                 bufferSemID;
    IM_QUEUE                       bufferQ;
    OMX_U32                        assignedBufferNum;
    OMX_STATETYPE                  portState;
    OMX_HANDLETYPE				   portMutex;
    OMX_HANDLETYPE                 loadedResource;
    OMX_HANDLETYPE                 unloadedResource;

    OMX_BOOL                       bIsPortFlushed;
    OMX_BOOL                       bIsPortDisabled;
    IM_PORT_TRANSITIONS            transitions;
    OMX_MARKTYPE                   markType;

    OMX_CONFIG_RECTTYPE            cropRectangle;

    /* Tunnel Info */
    OMX_HANDLETYPE                 tunneledComponent;
    OMX_U32                        tunneledPort;
    OMX_U32                        tunnelBufferNum;
    OMX_BUFFERSUPPLIERTYPE         bufferSupplier;
    OMX_U32                        tunnelFlags;

    OMX_VIDEO_CONTROLRATETYPE      eControlRate;

    /* For Android Native Buffer */
    OMX_BOOL                       bUseAndroidNativeBuffer;
    /* For Android Store Meta Data inBuffer */
    OMX_BOOL                       bStoreMetaDataInBuffer;
} IM_OMX_BASEPORT;


#ifdef __cplusplus
extern "C" {
#endif

OMX_ERRORTYPE IM_OMX_PortEnableProcess(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 nPortIndex);
OMX_ERRORTYPE IM_OMX_PortDisableProcess(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 nPortIndex);
OMX_ERRORTYPE IM_OMX_BufferFlushProcess(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 nPortIndex);
OMX_ERRORTYPE IM_OMX_BufferFlushProcessNoEvent(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 nPortIndex);
OMX_ERRORTYPE IM_OMX_Port_Constructor(OMX_HANDLETYPE hComponent);
OMX_ERRORTYPE IM_OMX_Port_Destructor(OMX_HANDLETYPE hComponent);


#ifdef __cplusplus
};
#endif


#endif
