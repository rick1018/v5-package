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
 * *  Neither the name of InfoTM Microelectronics Co. nor the names of
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
 * @file        IM_OSAL_Buffer.h
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.03.01 : Create
 */

#ifndef IM_OSAL_BUFFER
#define IM_OSAL_BUFFER

#ifdef __cplusplus
extern "C" {
#endif

#include "OMX_Types.h"
#include "OMX_IVCommon.h"

OMX_ERRORTYPE checkVersionANB(OMX_PTR ComponentParameterStructure);
OMX_U32 checkPortIndexANB(OMX_PTR ComponentParameterStructure);
OMX_U32 getMetadataBufferType(const uint8_t *ptr);
OMX_ERRORTYPE enableAndroidNativeBuffer(OMX_HANDLETYPE hComponent, OMX_PTR ComponentParameterStructure);
OMX_ERRORTYPE getAndroidNativeBuffer(OMX_HANDLETYPE hComponent, OMX_PTR ComponentParameterStructure);
OMX_ERRORTYPE useAndroidNativeBuffer(OMX_HANDLETYPE hComponent, OMX_HANDLETYPE hMemoryHandle, OMX_PTR ComponentParameterStructure);
OMX_U32 getPhyAddrfromANB(OMX_HANDLETYPE hComponent, OMX_HANDLETYPE hMemoryHandle, OMX_U32 nPortIndex, OMX_PTR pUnreadableBuffer);
OMX_PTR getVirtAddrfromANB(OMX_PTR pUnreadableBuffer);
OMX_U32 getPhyAddrfromNB(OMX_PTR pUnreadableBuffer, OMX_BOOL useMMU);
OMX_PTR getVirtAddrfromNB(OMX_PTR pUnreadableBuffer);
OMX_U32 putVirtAddrtoANB(OMX_PTR pUnreadableBuffer);
OMX_COLOR_FORMATTYPE getAndroidNativeBufferFormat(OMX_PTR pUnreadableBuffer);
OMX_BUFFERHEADERTYPE* getbufferHeaderbyPhyAddr(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_U32 nPortIndex, OMX_IN OMX_U32 phyAddr);

void unmapAndroidNativeBuffer(OMX_HANDLETYPE hMemoryHandle, OMX_PTR extendBufferHeader);


OMX_ERRORTYPE enableStoreMetaDataInBuffers(OMX_HANDLETYPE hComponent, OMX_PTR ComponentParameterStructure);
OMX_BOOL isMetadataBufferTypeGrallocSource(OMX_BYTE pInputDataBuffer);
OMX_BOOL isMetadataBufferTypeCameraSource(OMX_BYTE pInputDataBuffer);
OMX_ERRORTYPE preprocessMetaDataInBuffers(OMX_HANDLETYPE hComponent, OMX_HANDLETYPE hMemoryHandle, OMX_PTR pInputDataBuffer);
OMX_ERRORTYPE postprocessMetaDataInBuffers(OMX_HANDLETYPE hComponent, OMX_HANDLETYPE hMemoryHandle, OMX_PTR pInputDataBuffer);

//contiguous buffer allocate
OMX_ERRORTYPE IM_OSAL_ContiguousAllocator_Create(OMX_HANDLETYPE *hMemoryHandle, OMX_BOOL useMMU, OMX_BOOL isEncoder);
OMX_ERRORTYPE IM_OSAL_ContiguousAllocator_Destroy(OMX_HANDLETYPE hMemoryHandle);
OMX_PTR IM_OSAL_ContiguousBuffer_Malloc(OMX_HANDLETYPE hMemoryHandle, OMX_U32 size);
void IM_OSAL_ContiguousBuffer_Free(OMX_HANDLETYPE hMemoryHandle, OMX_PTR contiguousBuffer);

#ifdef __cplusplus
}
#endif

#endif

