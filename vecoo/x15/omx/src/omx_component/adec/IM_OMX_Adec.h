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
 * @file        IM_OMX_Adec.h
 * @brief
 * @author      ayakashi (eric.xu@infotmic.com.cn)
 *
 * @version     1.1.0
 * @history
 *   2014.04.15 : Create
 */

#ifndef IM_OMX_AUDIO_DECODE
#define IM_OMX_AUDIO_DECODE

#include "OMX_Component.h"
#include "IM_OMX_Def.h"
#include "IM_OSAL_Queue.h"
#include "IM_OMX_Baseport.h"
#include "IM_OMX_Basecomponent.h"

#define MAX_AUDIO_INPUTBUFFER_NUM    8
#define MAX_AUDIO_OUTPUTBUFFER_NUM   8

#define DEFAULT_AUDIO_INPUT_BUFFER_SIZE     (16 * 1024)
#define DEFAULT_AUDIO_OUTPUT_BUFFER_SIZE    (32 * 1024)

#define DEFAULT_AUDIO_SAMPLING_FREQ  44100
#define DEFAULT_AUDIO_CHANNELS_NUM   2
#define DEFAULT_AUDIO_BIT_PER_SAMPLE 16

#define INPUT_PORT_SUPPORTFORMAT_NUM_MAX    1
#define OUTPUT_PORT_SUPPORTFORMAT_NUM_MAX   1

#define IM_OMX_BUFFER_RETURN    0x00000100
typedef struct _IM_OMX_AUDIODEC_COMPONENT
{
    OMX_HANDLETYPE hCodecHandle;

    OMX_BOOL bFirstFrame;
    OMX_PTR pInputBuffer;
    OMX_U32  indexInputBuffer;

    //int (*IM_checkInputFrame)(OMX_U8 *pInputStream, OMX_U32 buffSize, OMX_U32 flag, OMX_BOOL bPreviousFrameEOF, OMX_BOOL *pbEndOfFrame);
} IM_OMX_AUDIODEC_COMPONENT;


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
    OMX_U32              nPortIndex);
OMX_ERRORTYPE IM_OMX_FreeTunnelBuffer(
    IM_OMX_BASEPORT *pOMXBasePort,
    OMX_U32              nPortIndex);
OMX_ERRORTYPE IM_OMX_ComponentTunnelRequest(
    OMX_IN  OMX_HANDLETYPE hComp,
    OMX_IN OMX_U32         nPort,
    OMX_IN OMX_HANDLETYPE  hTunneledComp,
    OMX_IN OMX_U32         nTunneledPort,
    OMX_INOUT OMX_TUNNELSETUPTYPE *pTunnelSetup);
OMX_ERRORTYPE IM_OMX_BufferProcess(OMX_HANDLETYPE hComponent);
OMX_ERRORTYPE IM_OMX_AudioDecodeGetParameter(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nParamIndex,
    OMX_INOUT OMX_PTR     ComponentParameterStructure);
OMX_ERRORTYPE IM_OMX_AudioDecodeSetParameter(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nIndex,
    OMX_IN OMX_PTR        ComponentParameterStructure);
OMX_ERRORTYPE IM_OMX_AudioDecodeGetConfig(
    OMX_HANDLETYPE hComponent,
    OMX_INDEXTYPE nIndex,
    OMX_PTR pComponentConfigStructure);
OMX_ERRORTYPE IM_OMX_AudioDecodeSetConfig(
    OMX_HANDLETYPE hComponent,
    OMX_INDEXTYPE nIndex,
    OMX_PTR pComponentConfigStructure);
OMX_ERRORTYPE IM_OMX_AudioDecodeGetExtensionIndex(
    OMX_IN OMX_HANDLETYPE  hComponent,
    OMX_IN OMX_STRING      cParameterName,
    OMX_OUT OMX_INDEXTYPE *pIndexType);
OMX_ERRORTYPE IM_OMX_AudioDecodeComponentInit(OMX_IN OMX_HANDLETYPE hComponent);
OMX_ERRORTYPE IM_OMX_AudioDecodeComponentDeinit(OMX_IN OMX_HANDLETYPE hComponent);
OMX_BOOL IM_Check_BufferProcess_State(IM_OMX_BASECOMPONENT *pIMComponent);

#ifdef __cplusplus
}
#endif

#endif /* IM_OMX_AUDIO_DECODE */
