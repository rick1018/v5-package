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
 * @file        IM_OMX_Vdec.h
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.03.01 : Create
*/

#ifndef IM_OMX_VIDEO_DECODE
#define IM_OMX_VIDEO_DECODE

#include "OMX_Component.h"
#include "IM_OMX_Def.h"
#include "IM_OSAL_Queue.h"
#include "IM_OMX_Baseport.h"
#include "IM_OMX_Basecomponent.h"

#define MAX_VIDEO_INPUTBUFFER_NUM    10
#define MIN_VIDEO_INPUTBUFFER_NUM    5
#define MAX_VIDEO_OUTPUTBUFFER_NUM   24
#define MIN_VIDEO_OUTPUTBUFFER_NUM   5

#define DEFAULT_FRAME_WIDTH          176
#define DEFAULT_FRAME_HEIGHT         144

#define DEFAULT_VIDEO_INPUT_BUFFER_SIZE    ((DEFAULT_FRAME_WIDTH * DEFAULT_FRAME_HEIGHT) * 2)
#define DEFAULT_VIDEO_OUTPUT_BUFFER_SIZE   ((DEFAULT_FRAME_WIDTH * DEFAULT_FRAME_HEIGHT * 3) / 2)

#define INPUT_BUFFER_NUM_MAX         2
#define DEFAULT_INPUT_BUFFER_SIZE    ((1280 * 720 * 3) / 2)

#define INPUT_PORT_SUPPORTFORMAT_NUM_MAX    1
#define OUTPUT_PORT_SUPPORTFORMAT_NUM_MAX   4

#ifdef USE_ANDROID_EXTENSION
#define ANDROID_MAX_VIDEO_OUTPUTBUFFER_NUM   4
#endif

typedef struct _IM_OMX_VIDEODEC_COMPONENT
{
    OMX_HANDLETYPE     hCodecHandle;

    OMX_BOOL            bInitialized;
    OMX_BOOL            bThumbnailMode;
    OMX_BOOL            bFirstFrame;
    OMX_BOOL            bUseMMU;

    OMX_HANDLETYPE     hMemoryHandle;
}IM_OMX_VIDEODEC_COMPONENT;

#ifdef __cplusplus
extern "C" {
#endif

OMX_ERRORTYPE IM_OMX_UseBuffer(
    OMX_IN OMX_HANDLETYPE            hComponent,
    OMX_INOUT OMX_BUFFERHEADERTYPE **ppBufferHdr,
    OMX_IN OMX_U32                   nPortIndex,
    OMX_IN OMX_PTR                   pAppPrivate,
    OMX_IN OMX_U32                   nSizeBytes,
    OMX_IN OMX_U8                   *pBuffer);
OMX_ERRORTYPE IM_OMX_AllocateBuffer(
    OMX_IN OMX_HANDLETYPE            hComponent,
    OMX_INOUT OMX_BUFFERHEADERTYPE **ppBuffer,
    OMX_IN OMX_U32                   nPortIndex,
    OMX_IN OMX_PTR                   pAppPrivate,
    OMX_IN OMX_U32                   nSizeBytes);
OMX_ERRORTYPE IM_OMX_FreeBuffer(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_U32        nPortIndex,
    OMX_IN OMX_BUFFERHEADERTYPE *pBufferHdr);
OMX_ERRORTYPE IM_OMX_AllocateTunnelBuffer(
    IM_OMX_BASEPORT *pOMXBasePort,
    OMX_U32           nPortIndex);
OMX_ERRORTYPE IM_OMX_FreeTunnelBuffer(
    IM_OMX_BASEPORT *pOMXBasePort,
    OMX_U32           nPortIndex);
OMX_ERRORTYPE IM_OMX_ComponentTunnelRequest(
    OMX_IN  OMX_HANDLETYPE hComp,
    OMX_IN OMX_U32         nPort,
    OMX_IN OMX_HANDLETYPE  hTunneledComp,
    OMX_IN OMX_U32         nTunneledPort,
    OMX_INOUT OMX_TUNNELSETUPTYPE *pTunnelSetup);
OMX_ERRORTYPE IM_OMX_BufferProcess(OMX_HANDLETYPE hComponent);
OMX_ERRORTYPE IM_OMX_VideoDecodeGetParameter(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nParamIndex,
    OMX_INOUT OMX_PTR     ComponentParameterStructure);
OMX_ERRORTYPE IM_OMX_VideoDecodeSetParameter(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nIndex,
    OMX_IN OMX_PTR        ComponentParameterStructure);

OMX_BOOL IM_Check_BufferProcess_State(IM_OMX_BASECOMPONENT *pIMComponent);

OMX_ERRORTYPE IM_OMX_VideoDecodeComponentInit(OMX_IN OMX_HANDLETYPE hComponent);
OMX_ERRORTYPE IM_OMX_VideoDecodeComponentDeinit(OMX_IN OMX_HANDLETYPE hComponent);

#ifdef __cplusplus
}
#endif

#endif
