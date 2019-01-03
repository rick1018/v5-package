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
 * @file        IM_OSAL_Buffer.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.03.01 : Create
 *   2013.10.15 : add enc buffer method
*/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "IM_OMX_Def.h"
#include "IM_OMX_Macros.h"
#include "IM_OSAL_Memory.h"
#include "IM_OSAL_Semaphore.h"
#include "IM_OSAL_Buffer.h"
#include "IM_OMX_Basecomponent.h"

#define IM_LOG_OFF
#include "IM_OSAL_Trace.h"

#ifdef __cplusplus
}
#endif
#ifdef USE_ANDROID_EXTENSION
//#include <ui/android_native_buffer.h>
#include <ui/GraphicBuffer.h>
#include <ui/GraphicBufferMapper.h>
#include <ui/Rect.h>
#include <HardwareAPI.h>
#include <hardware/hardware.h>
#include <MetadataBufferType.h>
#include "gralloc_priv.h"
#endif 
#ifdef VDEC_LIB_G1
#include <InfotmMedia.h>
#include <IM_buffallocapi.h>
#else
#include "mm_dma.h"
#endif

#include "IM_devmmuapi.h"

//using namespace android;

struct AndroidNativeBuffersParams {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
};

typedef struct {
    ALCCTX      alc_inst;
    DMMUINST    dmmu_inst;
}MEMORY_HANDLE;


#ifdef USE_ANDROID_EXTENSION
OMX_ERRORTYPE checkVersionANB(OMX_PTR ComponentParameterStructure)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    OMX_VERSIONTYPE* version = NULL;


    AndroidNativeBuffersParams *pANBP;
    pANBP = (AndroidNativeBuffersParams *)ComponentParameterStructure;

    version = (OMX_VERSIONTYPE*)((char*)pANBP + sizeof(OMX_U32));
    if (*((OMX_U32*)pANBP) <= sizeof(AndroidNativeBuffersParams)) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (version->s.nVersionMajor != VERSIONMAJOR_NUMBER ||
            version->s.nVersionMinor != VERSIONMINOR_NUMBER) {
        ret = OMX_ErrorVersionMismatch;
        goto EXIT;
    }

    ret = OMX_ErrorNone;

EXIT:
    return ret;
}

OMX_U32 checkPortIndexANB(OMX_PTR ComponentParameterStructure)
{
    AndroidNativeBuffersParams *pANBP;
    pANBP = (AndroidNativeBuffersParams *)ComponentParameterStructure;

    return pANBP->nPortIndex;
}

OMX_U32 getMetadataBufferType(const uint8_t *ptr)
{
    OMX_U32 type = *(OMX_U32 *) ptr;
    IM_OSAL_Info("getMetadataBufferType: %ld", type);
    return type;
}

OMX_PTR getVirtAddrfromANB(OMX_PTR pUnreadableBuffer)
{
    OMX_U32 ret = 0;
    void *readableBuffer;
    android_native_buffer_t *buf = (android_native_buffer_t *)pUnreadableBuffer;
    private_handle_t *handle = (private_handle_t *)buf->handle;
    GraphicBufferMapper &mapper = GraphicBufferMapper::get();
    Rect bounds(buf->width, buf->height);

    IM_OSAL_Info("pUnreadableBuffer:0x%x, buf:0x%x, buf->handle:0x%x",
            pUnreadableBuffer, buf, buf->handle);

    ret = mapper.lock(buf->handle, GRALLOC_USAGE_SW_WRITE_OFTEN, bounds, &readableBuffer);
    if (ret != 0) {
        IM_OSAL_Error("mapper.lock Error, Error code:%d", ret);
    }
    IM_OSAL_Info("vir addr = %x", readableBuffer);

    return (OMX_PTR)readableBuffer;
}

OMX_U32 getPhyAddrfromANB(OMX_HANDLETYPE hComponent, OMX_HANDLETYPE hMemoryHandle, OMX_U32 nPortIndex, OMX_PTR pUnreadableBuffer)
{

    private_handle_t *handle = (private_handle_t *)((android_native_buffer_t *)pUnreadableBuffer)->handle;
#ifndef VDEC_LIB_G1
    IM_OSAL_Info("phy addr = %x", buf->offset);
    return (OMX_U32)buf->offset;
#else
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT  *pIMComponent = NULL;
    IM_OMX_BASEPORT       *pIMPort = NULL;
    OMX_U32                phyAddr = 0; 
    OMX_U32                i = 0;
    MEMORY_HANDLE         *mem = (MEMORY_HANDLE *)hMemoryHandle;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    if (pOMXComponent->pComponentPrivate == NULL) {
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    pIMPort = &pIMComponent->pIMPort[nPortIndex];
    if (nPortIndex >= pIMComponent->portParam.nPorts) {
        goto EXIT;
    }

    if(mem->alc_inst == NULL) {
        goto EXIT;
    }

    for (i = 0; i < pIMPort->portDefinition.nBufferCountActual; i++) 
    {
        if(pIMPort->extendBufferHeader[i].OMXBufferHeader->pBuffer == pUnreadableBuffer) {
            if(pIMPort->extendBufferHeader[i].pPrivate != NULL) {
                IM_Buffer *buffer = (IM_Buffer *)pIMPort->extendBufferHeader[i].pPrivate;
                phyAddr = pIMComponent->useMMU ? buffer->dev_addr : buffer->phy_addr;
            } else {
                IM_OSAL_Info("map buffer uid(%d)", handle->buf_uid);
                IM_Buffer *buff = (IM_Buffer *)IM_OSAL_Malloc(sizeof(IM_Buffer));
                alc_map_uid(mem->alc_inst, handle->buf_uid, mem->dmmu_inst ? ALC_FLAG_DEVADDR : ALC_FLAG_PHYADDR, buff);

                pIMPort->extendBufferHeader[i].pPrivate = (OMX_PTR)buff;
                pIMPort->bufferStateAllocate[i] |= BUFFER_STATE_MAPPED;
            
                if(mem->dmmu_inst) {
                    dmmu_attach_buffer(mem->dmmu_inst, handle->buf_uid);
                }
                phyAddr = mem->dmmu_inst ? buff->dev_addr : buff->phy_addr;
            }
            IM_OSAL_Info("====phyAddr = %x, bufferHeader = %p",phyAddr, pIMPort->extendBufferHeader[i].OMXBufferHeader);
            break;
        }
    }

EXIT:
    IM_OSAL_Exiting();

    return phyAddr;
#endif 
}

OMX_COLOR_FORMATTYPE getAndroidNativeBufferFormat(OMX_PTR pUnreadableBuffer) 
{ 
    private_handle_t *handle;
    buffer_handle_t bufferHandle;
    OMX_COLOR_FORMATTYPE colorFormat = OMX_COLOR_FormatUnused;

    memcpy(&bufferHandle, (char*)(pUnreadableBuffer) + 4, sizeof(buffer_handle_t));
    handle = (private_handle_t *)bufferHandle; 
    if(handle == NULL) {
        return OMX_COLOR_FormatUnused;
    }
    IM_OSAL_Error("format = %x", handle->format);
    switch(handle->format)
    {    
        case HAL_PIXEL_FORMAT_RGBA_8888:
            colorFormat = OMX_COLOR_Format32bitARGB8888;
            break;
        case HAL_PIXEL_FORMAT_BGRA_8888:
            colorFormat = OMX_COLOR_Format32bitBGRA8888;
            break;
        case HAL_PIXEL_FORMAT_RGB_565:
            colorFormat = OMX_COLOR_Format16bitRGB565;
            break;
        case HAL_PIXEL_FORMAT_RGB_888:
            colorFormat = OMX_COLOR_Format24bitRGB888;
            break;
        case HAL_PIXEL_FORMAT_YV12: 
            colorFormat = OMX_COLOR_FormatYUV420Planar;
            break;
        case HAL_PIXEL_FORMAT_YCbCr_422_I:
            colorFormat = OMX_COLOR_FormatYUV422Planar;
            break;
        default:
            IM_OSAL_Error("not support format(%x)", handle->format);
    }
    return colorFormat;

}
OMX_U32 putVirtAddrtoANB(OMX_PTR pUnreadableBuffer)
{
    android_native_buffer_t *buf;
    void *readableBuffer;
    int ret = 0;
    GraphicBufferMapper &mapper = GraphicBufferMapper::get();

    buf = (android_native_buffer_t *)pUnreadableBuffer;

    return mapper.unlock(buf->handle);
}

OMX_ERRORTYPE enableAndroidNativeBuffer(OMX_HANDLETYPE hComponent, OMX_PTR ComponentParameterStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;

    EnableAndroidNativeBuffersParams *peanbp;


    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    peanbp = (EnableAndroidNativeBuffersParams *)ComponentParameterStructure;
    pIMPort = &pIMComponent->pIMPort[peanbp->nPortIndex];

    if (peanbp->enable == OMX_FALSE) {
        IM_OSAL_Info("disable AndroidNativeBuffer");
        pIMPort->bUseAndroidNativeBuffer = OMX_FALSE;
    } else {
        IM_OSAL_Info("enable AndroidNativeBuffer");
        pIMPort->bUseAndroidNativeBuffer = OMX_TRUE;
    }

    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE getAndroidNativeBuffer(OMX_HANDLETYPE hComponent, OMX_PTR ComponentParameterStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;

    GetAndroidNativeBufferUsageParams *pganbp;

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    IM_OSAL_Entering();

    pganbp = (GetAndroidNativeBufferUsageParams *)ComponentParameterStructure;
    //imapx800 platform has these feathers : mmu/ids layer
    pganbp->nUsage = GRALLOC_USAGE_HW_COMPOSER | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP | GRALLOC_USAGE_INFOTM_IDS_LAYER ;
    if(pIMComponent->useMMU)
        pganbp->nUsage |= GRALLOC_USAGE_INFOTM_DEVMMU;
    else
        pganbp->nUsage |= GRALLOC_USAGE_INFOTM_LINEAR;
    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}



OMX_ERRORTYPE useAndroidNativeBuffer(OMX_HANDLETYPE hComponent, OMX_HANDLETYPE hMemoryHandle, OMX_PTR ComponentParameterStructure)
{
    OMX_ERRORTYPE                   ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE              *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT           *pIMComponent = NULL;
    IM_OMX_BASEPORT                *pIMPort = NULL;
    OMX_BUFFERHEADERTYPE           *temp_bufferHeader = NULL;
    UseAndroidNativeBufferParams   *puanbp = (UseAndroidNativeBufferParams *)ComponentParameterStructure;
    MEMORY_HANDLE                  *mem = (MEMORY_HANDLE *)hMemoryHandle;

    OMX_U32                    i = 0;

    IM_OSAL_Entering();

    OMX_PTR buffer = (void *)puanbp->nativeBuffer.get();
    android_native_buffer_t *buf = (android_native_buffer_t *)buffer;
    private_handle_t *handle = (private_handle_t *)buf->handle;
    IM_OSAL_Info("buffer:0x%x, buf:0x%x, buf->handle:0x%x", buffer, buf, buf->handle);
    IM_OSAL_Info("buffer size = %d", handle->size);

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    pIMPort = &pIMComponent->pIMPort[puanbp->nPortIndex];
    if (pIMPort->portState != OMX_StateIdle) {
        ret = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }


    temp_bufferHeader = (OMX_BUFFERHEADERTYPE *)IM_OSAL_Malloc(sizeof(OMX_BUFFERHEADERTYPE));
    if (temp_bufferHeader == NULL) {
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    IM_OSAL_Memset(temp_bufferHeader, 0, sizeof(OMX_BUFFERHEADERTYPE));
    for (i = 0; i < pIMPort->portDefinition.nBufferCountActual; i++) {
        if (pIMPort->bufferStateAllocate[i] == BUFFER_STATE_FREE) {
            pIMPort->extendBufferHeader[i].OMXBufferHeader = temp_bufferHeader;
            pIMPort->bufferStateAllocate[i] = (BUFFER_STATE_ASSIGNED | HEADER_STATE_ALLOCATED);
            INIT_SET_SIZE_VERSION(temp_bufferHeader, OMX_BUFFERHEADERTYPE);
            temp_bufferHeader->pBuffer        = (OMX_U8 *)buffer;
            temp_bufferHeader->nAllocLen      = handle->size;
            temp_bufferHeader->pAppPrivate    = puanbp->pAppPrivate;
            if (puanbp->nPortIndex == INPUT_PORT_INDEX)
                temp_bufferHeader->nInputPortIndex = INPUT_PORT_INDEX;
            else
                temp_bufferHeader->nOutputPortIndex = OUTPUT_PORT_INDEX;
             
            pIMPort->assignedBufferNum++;
            if (pIMPort->assignedBufferNum == pIMPort->portDefinition.nBufferCountActual) {
                pIMPort->portDefinition.bPopulated = OMX_TRUE;
                /* IM_OSAL_MutexLock(pIMComponent->compMutex); */
                pIMComponent->IM_checkTransitions(pOMXComponent);
                //IM_OSAL_SemaphorePost(pIMPort->loadedResource);
                /* IM_OSAL_MutexUnlock(pIMComponent->compMutex); */
            }
            *puanbp->bufferHeader = temp_bufferHeader;
            ret = OMX_ErrorNone;
            goto EXIT;
        }
    }

    IM_OSAL_Free(temp_bufferHeader);
    ret = OMX_ErrorInsufficientResources;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}



OMX_ERRORTYPE enableStoreMetaDataInBuffers(OMX_HANDLETYPE hComponent, OMX_PTR ComponentParameterStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;

    StoreMetaDataInBuffersParams *pStoreMetaData;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    pStoreMetaData = (StoreMetaDataInBuffersParams*)ComponentParameterStructure;
    pIMPort = &pIMComponent->pIMPort[pStoreMetaData->nPortIndex];

    if (pStoreMetaData->bStoreMetaData == OMX_FALSE) {
        IM_OSAL_Info("disable StoreMetaDataInBuffers");
        pIMPort->bStoreMetaDataInBuffer = OMX_FALSE;
    } else {
        IM_OSAL_Info("enable StoreMetaDataInBuffers");
        pIMPort->bStoreMetaDataInBuffer = OMX_TRUE;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_BOOL isMetadataBufferTypeCameraSource(OMX_BYTE pInputDataBuffer)
{
    OMX_U32 type = getMetadataBufferType(pInputDataBuffer);

    if (type == kMetadataBufferTypeCameraSource)
        return OMX_TRUE;
    else
        return OMX_FALSE;
}

OMX_BOOL isMetadataBufferTypeGrallocSource(OMX_BYTE pInputDataBuffer)
{
    OMX_U32 type = getMetadataBufferType(pInputDataBuffer);

    if (type == kMetadataBufferTypeGrallocSource)
        return OMX_TRUE;
    else
        return OMX_FALSE;
}


typedef struct _TEMPDATA {
    OMX_PTR *virAddr;
    OMX_U32 phyAddr;
    OMX_U32 dataSize;
}TEMPDATA;

OMX_ERRORTYPE preprocessMetaDataInBuffers(OMX_HANDLETYPE hComponent,
        OMX_HANDLETYPE hMemoryHandle,
        OMX_PTR pInputDataBuffer) 
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT  *pIMComponent = NULL;
    IM_OMX_BASEPORT       *pIMPort = NULL;
    IM_OMX_DATABUFFER     *inputDataBuffer = (IM_OMX_DATABUFFER *)pInputDataBuffer;
    MEMORY_HANDLE         *mem = (MEMORY_HANDLE *)hMemoryHandle;
    TEMPDATA *tmp;
    private_handle_t *handle; 
    IM_OSAL_Entering();

    void *readableBuffer;

    IM_OSAL_Info("preprocessMetaDataInBuffers");
    if(isMetadataBufferTypeGrallocSource(inputDataBuffer->bufferHeader->pBuffer)) {
        buffer_handle_t bufferHandle;
        IM_Buffer buffer;
        memcpy(&bufferHandle, (char*)(inputDataBuffer->bufferHeader->pBuffer) + 4, sizeof(buffer_handle_t));
        handle = (private_handle_t *)bufferHandle; 
        IM_OSAL_Info("gralloc source use physic buffer ");
        //the buffer is physic buffer
        GraphicBufferMapper &mapper = GraphicBufferMapper::get();
        Rect bounds(handle->width, handle->height);

        int ret = mapper.lock(handle, GRALLOC_USAGE_SW_WRITE_OFTEN, bounds, &inputDataBuffer->virAddr);
        if (ret != 0) {
            IM_OSAL_Error("mapper.lock Error, Error code:%d", ret);
        }

        alc_map_uid(mem->alc_inst, handle->buf_uid, mem->dmmu_inst ? ALC_FLAG_DEVADDR : ALC_FLAG_PHYADDR, &buffer);
        if(handle->usage  & (/*GRALLOC_USAGE_HW_VIDEO_ENCODER */ GRALLOC_USAGE_INFOTM_IDS_LAYER | GRALLOC_USAGE_INFOTM_LINEAR) ) {
            inputDataBuffer->phyAddr = buffer.phy_addr;
        } else {
            if(mem->dmmu_inst) {
                inputDataBuffer->phyAddr = buffer.dev_addr;
                dmmu_attach_buffer(mem->dmmu_inst, buffer.uid);
            } else {
                inputDataBuffer->phyAddr = 0;
            }
        } 
        IM_OSAL_Info("vir addr = %x", inputDataBuffer->virAddr);
        IM_OSAL_Info("phy addr = %x", inputDataBuffer->phyAddr);
    } else if(isMetadataBufferTypeCameraSource(inputDataBuffer->bufferHeader->pBuffer)){
        IM_Buffer *buffer;
        IM_INT32 type;
        buffer = (IM_Buffer *)((char*)(inputDataBuffer->bufferHeader->pBuffer) + 4);
        type = buffer->flag;
#define isDevAddr(type) (type & IM_BUFFER_FLAG_DEVADDR)
#define isPhyAddr(type) (type & IM_BUFFER_FLAG_PHYADDR)
#define isVirAddr(type) (!(isDevAddr(type)) && !(isPhyAddr(type)))

        if (mem->dmmu_inst) {
            if (isDevAddr(type)) {
                inputDataBuffer->phyAddr = buffer->dev_addr;
            } else if (isPhyAddr(type)) {
                IM_Buffer tmp;
                if (alc_map_uid(mem->alc_inst, buffer->uid, ALC_FLAG_DEVADDR, &tmp) != IM_RET_OK) {
                    IM_OSAL_Error("Error: alc_map_uid() failed"); 
                    ret = OMX_ErrorUndefined; 
                    goto EXIT;
                }
                inputDataBuffer->phyAddr = tmp.dev_addr;
            } else {
                inputDataBuffer->phyAddr = 0;
                inputDataBuffer->virAddr = buffer->vir_addr;
            }

            dmmu_attach_buffer(mem->dmmu_inst, buffer->uid);
        } else {
            if (isPhyAddr(type)) {
                inputDataBuffer->phyAddr = buffer->phy_addr;
            } else if (isVirAddr(type)) {
                inputDataBuffer->phyAddr = 0;
                inputDataBuffer->virAddr = buffer->vir_addr;
            } else {
                inputDataBuffer->phyAddr = 0;
                inputDataBuffer->virAddr = buffer->vir_addr;
            }
        }

        inputDataBuffer->virAddr = buffer->vir_addr;
    } else {
        inputDataBuffer->phyAddr = 0;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE postprocessMetaDataInBuffers(OMX_HANDLETYPE hComponent, 
        OMX_HANDLETYPE hMemoryHandle,
        OMX_PTR pInputDataBuffer) 
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_DATABUFFER     *inputDataBuffer = (IM_OMX_DATABUFFER *)pInputDataBuffer;
    MEMORY_HANDLE         *mem = (MEMORY_HANDLE *)hMemoryHandle; 

    IM_OSAL_Info("postprocessMetaDataInBuffers");
    if(isMetadataBufferTypeGrallocSource(inputDataBuffer->bufferHeader->pBuffer)) {
        /* to be added*/
        buffer_handle_t bufferHandle;
        private_handle_t *handle; 
        IM_Buffer * buffer;
        memcpy(&bufferHandle, (char*)(inputDataBuffer->bufferHeader->pBuffer) + 4, sizeof(buffer_handle_t));
        handle = (private_handle_t *)bufferHandle; 
  
        if(mem->dmmu_inst) {
            dmmu_detach_buffer(mem->dmmu_inst, handle->buf_uid);
        }

        alc_unmap_uid(mem->alc_inst, handle->buf_uid);
    } else if(isMetadataBufferTypeCameraSource(inputDataBuffer->bufferHeader->pBuffer)){
        IM_Buffer *buffer;
        IM_INT32 type;
        buffer = (IM_Buffer *)((char*)(inputDataBuffer->bufferHeader->pBuffer) + 4);
        type = buffer->flag;

        if (mem->dmmu_inst) {
            if (isDevAddr(type) || isPhyAddr(type)) {
                dmmu_detach_buffer(mem->dmmu_inst, buffer->uid);

                if (isPhyAddr(type)) {
                    if (alc_unmap_uid(mem->alc_inst, buffer->uid) != IM_RET_OK) {
                        IM_OSAL_Error("Error: alc_unmap_uid() failed"); 
                        ret = OMX_ErrorUndefined; 
                    }
                }
            } else {
                IM_OSAL_Error("useMMU. Error camera source buffer type"); 
                ret = OMX_ErrorUndefined; 
            }
        }
    } else {
        /* NOTHING should be done */
    }

    return ret;
}
#endif

OMX_BUFFERHEADERTYPE* getbufferHeaderbyPhyAddr(
        OMX_IN OMX_HANDLETYPE            hComponent,
        OMX_IN OMX_U32                   nPortIndex,
        OMX_IN OMX_U32					 phyAddr)
{
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_BUFFERHEADERTYPE  *temp_bufferHeader = NULL;
    OMX_U32               i = 0;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    if (pOMXComponent->pComponentPrivate == NULL) {
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    pIMPort = &pIMComponent->pIMPort[nPortIndex];
    if (nPortIndex >= pIMComponent->portParam.nPorts) {
        goto EXIT;
    }

    for (i = 0; i < pIMPort->portDefinition.nBufferCountActual; i++) {
        if(pIMPort->extendBufferHeader[i].pPrivate != NULL) {
            IM_Buffer *buffer = (IM_Buffer *)pIMPort->extendBufferHeader[i].pPrivate;
            OMX_U32 temp = pIMComponent->useMMU ? buffer->dev_addr : buffer->phy_addr;
            if(temp == phyAddr) {
               temp_bufferHeader = pIMPort->extendBufferHeader[i].OMXBufferHeader;
               IM_OSAL_Info("phyAddr = %x, tempBuffer = %p", phyAddr, temp_bufferHeader);
               break;
            }
        }
    }

EXIT:
    IM_OSAL_Exiting();

    return temp_bufferHeader;
}

void unmapAndroidNativeBuffer(OMX_HANDLETYPE hMemoryHandle, OMX_PTR pExtendBufferHeader)
{
    MEMORY_HANDLE *mem = (MEMORY_HANDLE *)hMemoryHandle;
    IM_OMX_BUFFERHEADERTYPE *extendBufferHeader = (IM_OMX_BUFFERHEADERTYPE *)pExtendBufferHeader;
  
    IM_Buffer *buffer = (IM_Buffer *)extendBufferHeader->pPrivate;

    if(mem->dmmu_inst) {
        dmmu_detach_buffer(mem->dmmu_inst, buffer->uid);
    }
    alc_unmap_uid(mem->alc_inst, buffer->uid);

    IM_OSAL_Free(buffer);
    extendBufferHeader->pPrivate = NULL;
    
    return;
}

OMX_PTR getVirtAddrfromNB(OMX_PTR pUnreadableBuffer)
{
    OMX_U32 ret = 0;
#ifndef VDEC_LIB_G1
    struct_mm_page *buf = (struct_mm_page *)pUnreadableBuffer;
#else
    IM_Buffer *buf = (IM_Buffer *)pUnreadableBuffer;
#endif

    IM_OSAL_Info("pUnreadableBuffer:0x%x, buf:0x%x ",
            pUnreadableBuffer, buf);


    return (OMX_PTR)buf->vir_addr;
}

OMX_U32 getPhyAddrfromNB(OMX_PTR pUnreadableBuffer, OMX_BOOL useMMU)
{
    OMX_U32 ret = 0;
#ifndef VDEC_LIB_G1
    struct_mm_page *buf = (struct_mm_page *)pUnreadableBuffer;
#else
    IM_Buffer *buf = (IM_Buffer *)pUnreadableBuffer;
#endif
    IM_OSAL_Info("pUnreadableBuffer:0x%x, buf:0x%x ",
            pUnreadableBuffer, buf);

    if (useMMU) {
        if (!(buf->flag & IM_BUFFER_FLAG_DEVADDR))
            IM_OSAL_Error("Buffer type error: required dev addr");
        return (OMX_U32)buf->dev_addr;
    } else {
        if (!(buf->flag & IM_BUFFER_FLAG_PHYADDR))
            IM_OSAL_Error("Buffer type error: required phy addr");
        return (OMX_U32)buf->phy_addr;
    }
}


OMX_ERRORTYPE IM_OSAL_ContiguousAllocator_Create(OMX_HANDLETYPE* hMemoryHandle, OMX_BOOL useMMU, OMX_BOOL isEncoder)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
#ifndef VDEC_LIB_G1
    MMINST *mm_inst = (MMINST *)IM_OSAL_Malloc(sizeof(MMINST));
    *mm_inst = MmOpen();
    if(*mm_inst < 0){
        IM_OSAL_Error("MmOpen() failed\n");
        ret = OMX_ErrorInsufficientResources;
    }

    *hAllocHandle = (OMX_HANDLETYPE)mm_inst;
#else
    MEMORY_HANDLE *mem = (MEMORY_HANDLE *)IM_OSAL_Malloc(sizeof(MEMORY_HANDLE));
    if(mem == NULL) {
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    mem->alc_inst = NULL;
    mem->dmmu_inst = NULL;
    if(alc_open(&mem->alc_inst, IM_STR("im_omx")) != IM_RET_OK) 
    {
        IM_OSAL_Error("failed to alc_open");
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    if(useMMU) {
        if(dmmu_init(&mem->dmmu_inst, isEncoder ? DMMU_DEV_VENC : DMMU_DEV_VDEC) != IM_RET_OK) {
            IM_OSAL_Error("dmmu_init failed");
            ret = OMX_ErrorInsufficientResources;
            goto EXIT;
        }
    }

    *hMemoryHandle = (OMX_HANDLETYPE)mem;
#endif 
EXIT:
    return ret;
}

OMX_ERRORTYPE IM_OSAL_ContiguousAllocator_Destroy(OMX_HANDLETYPE hMemoryHandle)
{
    if(hMemoryHandle != NULL) {
#ifndef VDEC_LIB_G1
        MmClose(*((MMINST *)hAllocHandle));
#else
        MEMORY_HANDLE *mem = (MEMORY_HANDLE *)hMemoryHandle;

        if(mem->alc_inst) {
            alc_close(mem->alc_inst);
        } 

        if(mem->dmmu_inst) {
            dmmu_deinit(mem->dmmu_inst);
        }
#endif
        IM_OSAL_Free(hMemoryHandle);
    }
    return OMX_ErrorNone;
}

OMX_PTR IM_OSAL_ContiguousBuffer_Malloc(OMX_HANDLETYPE hMemoryHandle, 
        OMX_U32 size)
{
    if(hMemoryHandle == NULL) {
        return NULL;
    }
#ifndef VDEC_LIB_G1
    MMINST *mm_inst;
    struct_mm_page *page;
    struct_mm_alloc alloc; 

    mm_inst = (MMINST *)hAllocHandle;

    page = (struct_mm_page *)IM_OSAL_Malloc(sizeof(struct_mm_page));
    memset(page,0,sizeof(struct_mm_page));
    memset(&alloc,0,sizeof(struct_mm_alloc));

    page->size = size;
    alloc.page = page;
    alloc.align = MMALIGN_8B;
    alloc.mpool = false;
#ifdef _SET_CACHED_ENABLE_
    alloc.cached = 0x2b2b2b2b;
#endif
    if(MMRET_OK != MmAlloc(*mm_inst, &alloc)){
        IM_OSAL_Error("MmAlloc failed");
        IM_OSAL_Free(page);
        page = NULL;
    }
    IM_OSAL_Info("page. vir %x, phy = %x, size= %x", page->vir_addr, page->phy_addr, page->size);
    return (OMX_PTR)page;
#else
    MEMORY_HANDLE *mem = (MEMORY_HANDLE *)hMemoryHandle;
    IM_Buffer *buffer;
    OMX_U32 flags;
    IM_RET ret;

    buffer = (IM_Buffer *)IM_OSAL_Malloc(sizeof(IM_Buffer));
    if(buffer == NULL){
        return NULL;
    }
    IM_OSAL_Memset(buffer, 0, sizeof(IM_Buffer));

    buffer->size = size;
    flags = ALC_FLAG_ALIGN_8BYTES;
    flags |= mem->dmmu_inst != NULL ? ALC_FLAG_DEVADDR : ALC_FLAG_PHYADDR;
    ret = alc_alloc(mem->alc_inst, size, flags, buffer);

    if(ret != IM_RET_OK) {
        IM_OSAL_Error("ERROR! No linear buffer available\n");
        return NULL;
    }

    if (mem->dmmu_inst) {
        dmmu_attach_buffer(mem->dmmu_inst, buffer->uid);
    }
    return (OMX_PTR)buffer;
#endif 
}

void IM_OSAL_ContiguousBuffer_Free(OMX_HANDLETYPE hMemoryHandle, 
        OMX_PTR contiguousBuffer) 
{
#ifndef VDEC_LIB_G1
    MMINST *mm_inst = (MMINST *)hAllocHandle;
    struct_mm_page *page = (struct_mm_page *)contiguousBuffer;

    if(page != NULL){
        if(MmFree(*mm_inst, page) != MMRET_OK){
            IM_OSAL_Error("MmFree Failed");
        }

        IM_OSAL_Free(page);
    }
#else
    MEMORY_HANDLE *mem = (MEMORY_HANDLE *)hMemoryHandle;
    IM_Buffer *buffer = (IM_Buffer *)contiguousBuffer;
    if(buffer != NULL) {
        if(mem->dmmu_inst){
            dmmu_detach_buffer(mem->dmmu_inst, buffer->uid);
        }
        if(buffer->vir_addr) {
            alc_free(mem->alc_inst, buffer);
        }

        IM_OSAL_Free(buffer);
    }
#endif 
}
