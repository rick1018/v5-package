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
 * @file        IM_OMX_Adec.c
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2014.04.15 : Create
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IM_OMX_Macros.h"
#include "IM_OSAL_Event.h"
#include "IM_OMX_Adec.h"
#include "IM_OMX_Basecomponent.h"
#include "IM_OSAL_Thread.h"
#include "IM_OSAL_Semaphore.h"
#include "IM_OSAL_Mutex.h"
#include "IM_OSAL_ETC.h"

#undef  IM_LOG_TAG
#define IM_LOG_TAG    "IM_AUDIO_DEC"
#define IM_LOG_OFF
#include "IM_OSAL_Trace.h"

OMX_ERRORTYPE IM_OMX_UseBuffer(
    OMX_IN OMX_HANDLETYPE            hComponent,
    OMX_INOUT OMX_BUFFERHEADERTYPE **ppBufferHdr,
    OMX_IN OMX_U32                   nPortIndex,
    OMX_IN OMX_PTR                   pAppPrivate,
    OMX_IN OMX_U32                   nSizeBytes,
    OMX_IN OMX_U8                   *pBuffer)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_BUFFERHEADERTYPE     *temp_bufferHeader = NULL;
    OMX_U32                   i = 0;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    pIMPort = &pIMComponent->pIMPort[nPortIndex];
    if (nPortIndex >= pIMComponent->portParam.nPorts) {
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }
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
            temp_bufferHeader->pBuffer        = pBuffer;
            temp_bufferHeader->nAllocLen      = nSizeBytes;
            temp_bufferHeader->pAppPrivate    = pAppPrivate;
            if (nPortIndex == INPUT_PORT_INDEX)
                temp_bufferHeader->nInputPortIndex = INPUT_PORT_INDEX;
            else
                temp_bufferHeader->nOutputPortIndex = OUTPUT_PORT_INDEX;

            pIMPort->assignedBufferNum++;
            if (pIMPort->assignedBufferNum == pIMPort->portDefinition.nBufferCountActual) {
                pIMPort->portDefinition.bPopulated = OMX_TRUE;
                /* IM_OSAL_MutexLock(pIMComponent->compMutex); */
                IM_OMX_CommandQueue(pIMComponent, IM_OMX_CommandCheckTransitions, 0, NULL);
                //pIMComponent->IM_checkTransitions(pOMXComponent);
                /* IM_OSAL_MutexUnlock(pIMComponent->compMutex); */
            }
            *ppBufferHdr = temp_bufferHeader;
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

OMX_ERRORTYPE IM_OMX_AllocateBuffer(
    OMX_IN OMX_HANDLETYPE            hComponent,
    OMX_INOUT OMX_BUFFERHEADERTYPE **ppBuffer,
    OMX_IN OMX_U32                   nPortIndex,
    OMX_IN OMX_PTR                   pAppPrivate,
    OMX_IN OMX_U32                   nSizeBytes)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_BUFFERHEADERTYPE     *temp_bufferHeader = NULL;
    OMX_U8                   *temp_buffer = NULL;
    OMX_U32                   i = 0;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    pIMPort = &pIMComponent->pIMPort[nPortIndex];
    if (nPortIndex >= pIMComponent->portParam.nPorts) {
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }
/*
    if (pIMPort->portState != OMX_StateIdle ) {
        ret = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }
*/
    if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

    temp_buffer = IM_OSAL_Malloc(sizeof(OMX_U8) * nSizeBytes);
    if (temp_buffer == NULL) {
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    temp_bufferHeader = (OMX_BUFFERHEADERTYPE *)IM_OSAL_Malloc(sizeof(OMX_BUFFERHEADERTYPE));
    if (temp_bufferHeader == NULL) {
        IM_OSAL_Free(temp_buffer);
        temp_buffer = NULL;
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    IM_OSAL_Memset(temp_bufferHeader, 0, sizeof(OMX_BUFFERHEADERTYPE));

    for (i = 0; i < pIMPort->portDefinition.nBufferCountActual; i++) {
        if (pIMPort->bufferStateAllocate[i] == BUFFER_STATE_FREE) {
            pIMPort->extendBufferHeader[i].OMXBufferHeader = temp_bufferHeader;
            pIMPort->bufferStateAllocate[i] = (BUFFER_STATE_ALLOCATED | HEADER_STATE_ALLOCATED);
            INIT_SET_SIZE_VERSION(temp_bufferHeader, OMX_BUFFERHEADERTYPE);
            temp_bufferHeader->pBuffer        = temp_buffer;
            temp_bufferHeader->nAllocLen      = nSizeBytes;
            temp_bufferHeader->pAppPrivate    = pAppPrivate;
            if (nPortIndex == INPUT_PORT_INDEX)
                temp_bufferHeader->nInputPortIndex = INPUT_PORT_INDEX;
            else
                temp_bufferHeader->nOutputPortIndex = OUTPUT_PORT_INDEX;
            pIMPort->assignedBufferNum++;
            if (pIMPort->assignedBufferNum == pIMPort->portDefinition.nBufferCountActual) {
                pIMPort->portDefinition.bPopulated = OMX_TRUE;
                /* IM_OSAL_MutexLock(pIMComponent->compMutex); */
                //pIMComponent->IM_checkTransitions(pOMXComponent);
                IM_OMX_CommandQueue(pIMComponent, IM_OMX_CommandCheckTransitions, 0, NULL);
                /* IM_OSAL_MutexUnlock(pIMComponent->compMutex); */
            }
            *ppBuffer = temp_bufferHeader;
            ret = OMX_ErrorNone;
            goto EXIT;
        }
    }

    IM_OSAL_Free(temp_bufferHeader);
    IM_OSAL_Free(temp_buffer);
    ret = OMX_ErrorInsufficientResources;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_FreeBuffer(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_U32        nPortIndex,
    OMX_IN OMX_BUFFERHEADERTYPE *pBufferHdr)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_BUFFERHEADERTYPE     *temp_bufferHeader = NULL;
    OMX_U8                   *temp_buffer = NULL;
    OMX_U32                   i = 0;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    pIMPort = &pIMComponent->pIMPort[nPortIndex];

    if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

    if ((pIMPort->portState != OMX_StateLoaded) && (pIMPort->portState != OMX_StateInvalid)) {
        (*(pIMComponent->pCallbacks->EventHandler)) (pOMXComponent,
                        pIMComponent->callbackData,
                        (OMX_U32)OMX_EventError,
                        (OMX_U32)OMX_ErrorPortUnpopulated,
                        nPortIndex, NULL);
    }

    for (i = 0; i < pIMPort->portDefinition.nBufferCountActual; i++) {
        if (((pIMPort->bufferStateAllocate[i] | BUFFER_STATE_FREE) != 0) && (pIMPort->extendBufferHeader[i].OMXBufferHeader != NULL)) {
            if (pIMPort->extendBufferHeader[i].OMXBufferHeader->pBuffer == pBufferHdr->pBuffer) {
                if (pIMPort->bufferStateAllocate[i] & BUFFER_STATE_ALLOCATED) {
                    IM_OSAL_Free(pIMPort->extendBufferHeader[i].OMXBufferHeader->pBuffer);
                    pIMPort->extendBufferHeader[i].OMXBufferHeader->pBuffer = NULL;
                    pBufferHdr->pBuffer = NULL;
                } else if (pIMPort->bufferStateAllocate[i] & BUFFER_STATE_ASSIGNED) {
                    ; /* None*/
                }
                pIMPort->assignedBufferNum--;
                if (pIMPort->bufferStateAllocate[i] & HEADER_STATE_ALLOCATED) {
                    IM_OSAL_Free(pIMPort->extendBufferHeader[i].OMXBufferHeader);
                    pIMPort->extendBufferHeader[i].OMXBufferHeader = NULL;
                    pBufferHdr = NULL;
                }
                pIMPort->bufferStateAllocate[i] = BUFFER_STATE_FREE;
                ret = OMX_ErrorNone;
                goto EXIT;
            }
        }
    }

EXIT:
    if (ret == OMX_ErrorNone) {
        if (pIMPort->assignedBufferNum == 0) {
            IM_OSAL_Info("pIMPort->unloadedResource signal set");
            /* IM_OSAL_MutexLock(pIMComponent->compMutex); */
            pIMPort->portDefinition.bPopulated = OMX_FALSE;
            //pIMComponent->IM_checkTransitions(pOMXComponent);
            IM_OMX_CommandQueue(pIMComponent, IM_OMX_CommandCheckTransitions, 0, NULL);
            /* IM_OSAL_MutexUnlock(pIMComponent->compMutex); */
        }
    }

    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_AllocateTunnelBuffer(IM_OMX_BASEPORT *pOMXBasePort, OMX_U32 nPortIndex)
{
    OMX_ERRORTYPE                 ret = OMX_ErrorNone;
    IM_OMX_BASEPORT          *pIMPort = NULL;
    OMX_BUFFERHEADERTYPE         *temp_bufferHeader = NULL;
    OMX_U8                       *temp_buffer = NULL;
    OMX_U32                       bufferSize = 0;
    OMX_PARAM_PORTDEFINITIONTYPE  portDefinition;

    ret = OMX_ErrorTunnelingUnsupported;
EXIT:
    return ret;
}

OMX_ERRORTYPE IM_OMX_FreeTunnelBuffer(IM_OMX_BASEPORT *pOMXBasePort, OMX_U32 nPortIndex)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    IM_OMX_BASEPORT* pIMPort = NULL;
    OMX_BUFFERHEADERTYPE* temp_bufferHeader = NULL;
    OMX_U8 *temp_buffer = NULL;
    OMX_U32 bufferSize = 0;

    ret = OMX_ErrorTunnelingUnsupported;
EXIT:
    return ret;
}

OMX_ERRORTYPE IM_OMX_ComponentTunnelRequest(
    OMX_IN OMX_HANDLETYPE hComp,
    OMX_IN OMX_U32        nPort,
    OMX_IN OMX_HANDLETYPE hTunneledComp,
    OMX_IN OMX_U32        nTunneledPort,
    OMX_INOUT OMX_TUNNELSETUPTYPE *pTunnelSetup)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    ret = OMX_ErrorTunnelingUnsupported;
EXIT:
    return ret;
}

OMX_BOOL IM_Check_BufferProcess_State(IM_OMX_BASECOMPONENT *pIMComponent)
{
	IM_OSAL_MutexLock(pIMComponent->compMutex);
    if ((pIMComponent->currentState == OMX_StateExecuting) &&
        (pIMComponent->pIMPort[INPUT_PORT_INDEX].portState == OMX_StateIdle) &&
        (pIMComponent->pIMPort[OUTPUT_PORT_INDEX].portState == OMX_StateIdle) &&
        (pIMComponent->transientState != IM_OMX_TransStateExecutingToIdle) &&
        (pIMComponent->transientState != IM_OMX_TransStateIdleToExecuting)) {
		IM_OSAL_MutexUnlock(pIMComponent->compMutex);
        return OMX_TRUE;
    } else {
		IM_OSAL_MutexUnlock(pIMComponent->compMutex);
        return OMX_FALSE;
    }
}

OMX_ERRORTYPE IM_InputBufferReturn(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_BASEPORT      *IMOMXInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    IM_OMX_BASEPORT      *IMOMXOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    IM_OMX_DATABUFFER    *dataBuffer = &pIMComponent->IMDataBuffer[INPUT_PORT_INDEX];
    OMX_BUFFERHEADERTYPE  *bufferHeader = dataBuffer->bufferHeader;

    IM_OSAL_Entering();

    if (bufferHeader != NULL) {
        if (IMOMXInputPort->markType.hMarkTargetComponent != NULL ) {
            bufferHeader->hMarkTargetComponent      = IMOMXInputPort->markType.hMarkTargetComponent;
            bufferHeader->pMarkData                 = IMOMXInputPort->markType.pMarkData;
            IMOMXInputPort->markType.hMarkTargetComponent = NULL;
            IMOMXInputPort->markType.pMarkData = NULL;
        }

        if (bufferHeader->hMarkTargetComponent != NULL) {
            if (bufferHeader->hMarkTargetComponent == pOMXComponent) {
                pIMComponent->pCallbacks->EventHandler(pOMXComponent,
                                pIMComponent->callbackData,
                                OMX_EventMark,
                                0, 0, bufferHeader->pMarkData);
            } else {
                pIMComponent->propagateMarkType.hMarkTargetComponent = bufferHeader->hMarkTargetComponent;
                pIMComponent->propagateMarkType.pMarkData = bufferHeader->pMarkData;
            }
        }

        bufferHeader->nFilledLen = 0;
        pIMComponent->pCallbacks->EmptyBufferDone(pOMXComponent, pIMComponent->callbackData, bufferHeader);
    }

    if ((pIMComponent->currentState == OMX_StatePause) &&
        ((!CHECK_PORT_BEING_FLUSHED(IMOMXInputPort) && !CHECK_PORT_BEING_FLUSHED(IMOMXOutputPort)))) {
        IM_OSAL_SignalWait(pIMComponent->pauseEvent, DEF_MAX_WAIT_TIME);
        IM_OSAL_SignalReset(pIMComponent->pauseEvent);
    }

    dataBuffer->dataValid     = OMX_FALSE;
    dataBuffer->nRemainDataLeft = dataBuffer->bufferHeader->nFilledLen;
    dataBuffer->bufferHeader  = NULL;
	dataBuffer->bDataProcessed = OMX_FALSE;
	dataBuffer->bHardwareUsed = OMX_FALSE;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_InputBufferGetQueue(IM_OMX_BASECOMPONENT *pIMComponent)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASEPORT   *pIMPort = NULL;
    IM_OMX_DATABUFFER *dataBuffer = NULL;
    IM_OMX_MESSAGE    *message = NULL;

    IM_OSAL_Entering();

    pIMPort= &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    dataBuffer = &pIMComponent->IMDataBuffer[INPUT_PORT_INDEX];

    if (pIMComponent->currentState != OMX_StateExecuting) {
        ret = OMX_ErrorUndefined;
        goto EXIT;
    } else {
        IM_OSAL_SemaphoreWait(pIMPort->bufferSemID);
        IM_OSAL_MutexLock(dataBuffer->bufferMutex);
        if (dataBuffer->dataValid != OMX_TRUE) {
            message = (IM_OMX_MESSAGE *)IM_OSAL_Dequeue(&pIMPort->bufferQ);
            if (message == NULL) {
                ret = OMX_ErrorUndefined;
                IM_OSAL_MutexUnlock(dataBuffer->bufferMutex);
                goto EXIT;
            }

            dataBuffer->bufferHeader = (OMX_BUFFERHEADERTYPE *)(message->pCmdData);
            dataBuffer->dataValid = OMX_TRUE;
            dataBuffer->nRemainDataLeft = dataBuffer->bufferHeader->nFilledLen;
			dataBuffer->bDataProcessed = OMX_FALSE;

            IM_OSAL_Free(message);

            if (dataBuffer->bufferHeader->nAllocLen < dataBuffer->bufferHeader->nFilledLen)
                IM_OSAL_Warning("Input Buffer Full, Check input buffer size! allocSize:%d, dataLen:%d", \
						dataBuffer->bufferHeader->nAllocLen, dataBuffer->bufferHeader->nFilledLen);
        }
        IM_OSAL_MutexUnlock(dataBuffer->bufferMutex);
        ret = OMX_ErrorNone;
    }
EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OutputBufferReturn(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_BASEPORT      *IMOMXInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    IM_OMX_BASEPORT      *IMOMXOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    IM_OMX_DATABUFFER    *dataBuffer = &pIMComponent->IMDataBuffer[OUTPUT_PORT_INDEX];
    OMX_BUFFERHEADERTYPE  *bufferHeader = dataBuffer->bufferHeader;

    IM_OSAL_Entering();

    if (bufferHeader != NULL) {
        if (pIMComponent->propagateMarkType.hMarkTargetComponent != NULL) {
            bufferHeader->hMarkTargetComponent = pIMComponent->propagateMarkType.hMarkTargetComponent;
            bufferHeader->pMarkData = pIMComponent->propagateMarkType.pMarkData;
            pIMComponent->propagateMarkType.hMarkTargetComponent = NULL;
            pIMComponent->propagateMarkType.pMarkData = NULL;
        }

        if (bufferHeader->nFlags & OMX_BUFFERFLAG_EOS) {
            pIMComponent->pCallbacks->EventHandler(pOMXComponent,
                            pIMComponent->callbackData,
                            OMX_EventBufferFlag,
                            OUTPUT_PORT_INDEX,
                            bufferHeader->nFlags, NULL);
        }

        pIMComponent->pCallbacks->FillBufferDone(pOMXComponent, pIMComponent->callbackData, bufferHeader);
    }

    if ((pIMComponent->currentState == OMX_StatePause) &&
        ((!CHECK_PORT_BEING_FLUSHED(IMOMXInputPort) && !CHECK_PORT_BEING_FLUSHED(IMOMXOutputPort)))) {
        IM_OSAL_SignalWait(pIMComponent->pauseEvent, DEF_MAX_WAIT_TIME);
        IM_OSAL_SignalReset(pIMComponent->pauseEvent);
    }

    /* reset dataBuffer */
    dataBuffer->dataValid     = OMX_FALSE;
    dataBuffer->bufferHeader  = NULL;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OutputBufferGetQueue(IM_OMX_BASECOMPONENT *pIMComponent)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASEPORT   *pIMPort = NULL;
    IM_OMX_DATABUFFER *dataBuffer = NULL;
    IM_OMX_MESSAGE    *message = NULL;

    IM_OSAL_Entering();

    pIMPort= &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    dataBuffer = &pIMComponent->IMDataBuffer[OUTPUT_PORT_INDEX];

    if (pIMComponent->currentState != OMX_StateExecuting) {
        ret = OMX_ErrorUndefined;
        goto EXIT;
    } else {
        IM_OSAL_SemaphoreWait(pIMPort->bufferSemID);
        IM_OSAL_MutexLock(dataBuffer->bufferMutex);
        if (dataBuffer->dataValid != OMX_TRUE) {
            message = (IM_OMX_MESSAGE *)IM_OSAL_Dequeue(&pIMPort->bufferQ);
            if (message == NULL) {
                ret = OMX_ErrorUndefined;
                IM_OSAL_MutexUnlock(dataBuffer->bufferMutex);
                goto EXIT;
            }

            dataBuffer->bufferHeader = (OMX_BUFFERHEADERTYPE *)(message->pCmdData);
			dataBuffer->bufferHeader->nFilledLen = 0;
            dataBuffer->dataValid = OMX_TRUE;

            IM_OSAL_Free(message);
        }
        IM_OSAL_MutexUnlock(dataBuffer->bufferMutex);
        ret = OMX_ErrorNone;
    }
EXIT:
    IM_OSAL_Exiting();

    return ret;

}

static OMX_ERRORTYPE IM_BufferReset(OMX_COMPONENTTYPE *pOMXComponent, OMX_U32 portIndex)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    /* IM_OMX_BASEPORT      *pIMPort = &pIMComponent->pIMPort[portIndex]; */
    IM_OMX_DATABUFFER    *dataBuffer = &pIMComponent->IMDataBuffer[portIndex];
    /* OMX_BUFFERHEADERTYPE  *bufferHeader = dataBuffer->bufferHeader; */

    dataBuffer->dataValid     = OMX_FALSE;
    dataBuffer->nRemainDataLeft = 0;
    dataBuffer->bufferHeader  = NULL;
	dataBuffer->bDataProcessed = OMX_FALSE;

    return ret;
}


OMX_BOOL IM_Preprocessor_InputData(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_BOOL               ret = OMX_FALSE;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_DATABUFFER    *inputUseBuffer = &pIMComponent->IMDataBuffer[INPUT_PORT_INDEX];
    OMX_U32                copySize = 0;
    OMX_BYTE               checkInputStream = NULL;
    OMX_U32                checkInputStreamLen = 0;
    OMX_U32                checkedSize = 0;
    OMX_BOOL               flagEOF = OMX_FALSE;
    OMX_BOOL               previousFrameEOF = OMX_FALSE;

    IM_OSAL_Entering();

    if (inputUseBuffer->dataValid == OMX_TRUE) {
        if (inputUseBuffer->bDataProcessed == OMX_TRUE && inputUseBuffer->nRemainDataLeft <= 0){
            IM_InputBufferReturn(pOMXComponent);
		}else{
			ret = OMX_TRUE;
            inputUseBuffer->dataValid = OMX_TRUE;
		}
    }

    IM_OSAL_Exiting();

    return ret;
}

OMX_BOOL IM_Postprocess_OutputData(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_BOOL               ret = OMX_FALSE;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_DATABUFFER    *outputUseBuffer = &pIMComponent->IMDataBuffer[OUTPUT_PORT_INDEX];
    OMX_U32                copySize = 0;

    IM_OSAL_Entering();

    if (outputUseBuffer->dataValid == OMX_TRUE){
		if(outputUseBuffer->bufferHeader->nFilledLen > 0 
			|| (outputUseBuffer->bufferHeader->nFlags & OMX_BUFFERFLAG_EOS) 
            || (outputUseBuffer->bufferHeader->nFlags & IM_OMX_BUFFER_RETURN)) {
            ret = OMX_TRUE;
        	if (outputUseBuffer->bufferHeader->nAllocLen < outputUseBuffer->bufferHeader->nFilledLen){
            	IM_OSAL_Warning("output buffer is smaller than decoded data size Out Length");
				outputUseBuffer->bufferHeader->nFilledLen = outputUseBuffer->bufferHeader->nAllocLen;
        	}
            IM_OutputBufferReturn(pOMXComponent);
        } else {
        	ret = OMX_FALSE;
		}
    } else {
		//we need more buffers.
        ret = OMX_TRUE;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_BufferProcess(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_AUDIODEC_COMPONENT *pAudioDec = (IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle;
    IM_OMX_BASEPORT      *IMInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    IM_OMX_BASEPORT      *IMOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    IM_OMX_DATABUFFER    *inputUseBuffer = &pIMComponent->IMDataBuffer[INPUT_PORT_INDEX];
    IM_OMX_DATABUFFER    *outputUseBuffer = &pIMComponent->IMDataBuffer[OUTPUT_PORT_INDEX];
    OMX_U32                copySize = 0;

    pIMComponent->remainOutputData = OMX_FALSE;
    pIMComponent->reInputData = OMX_FALSE;

    IM_OSAL_Entering();

    while (1) {
        IM_OSAL_MutexLock(pIMComponent->compMutex);
        if(pIMComponent->bExitBufferProcessThread) {
            IM_OSAL_MutexUnlock(pIMComponent->compMutex);
            IM_OSAL_Error("ExitBufferProcessThread");
            break;
        }

        if (((pIMComponent->currentState == OMX_StatePause) ||
                    (pIMComponent->currentState == OMX_StateIdle) ||
                    (pIMComponent->transientState == IM_OMX_TransStateLoadedToIdle) ||
                    (pIMComponent->transientState == IM_OMX_TransStateExecutingToIdle)) &&
                (pIMComponent->transientState != IM_OMX_TransStateIdleToLoaded)&&
                ((!CHECK_PORT_BEING_FLUSHED(IMInputPort) && !CHECK_PORT_BEING_FLUSHED(IMOutputPort)))) {
            IM_OSAL_MutexUnlock(pIMComponent->compMutex);
            IM_OSAL_SignalWait(pIMComponent->pauseEvent, DEF_MAX_WAIT_TIME);
            IM_OSAL_SignalReset(pIMComponent->pauseEvent);
        }else{
            IM_OSAL_MutexUnlock(pIMComponent->compMutex);
        }

        while (IM_Check_BufferProcess_State(pIMComponent) && !pIMComponent->bExitBufferProcessThread) {

            IM_OSAL_MutexLock(outputUseBuffer->bufferMutex);
            if ((outputUseBuffer->dataValid != OMX_TRUE) &&
                    (!CHECK_PORT_BEING_FLUSHED(IMOutputPort))) {
                IM_OSAL_MutexUnlock(outputUseBuffer->bufferMutex);
                ret = IM_OutputBufferGetQueue(pIMComponent);
                if ((ret == OMX_ErrorUndefined) ||
                        (IMInputPort->portState != OMX_StateIdle) ||
                        (IMOutputPort->portState != OMX_StateIdle)) {
                    break;
                }
                IM_OSAL_MutexLock(outputUseBuffer->bufferMutex);
            } 

            if(outputUseBuffer->dataValid == OMX_TRUE){
                IM_OSAL_MutexLock(inputUseBuffer->bufferMutex);
                if ((IM_Preprocessor_InputData(pOMXComponent) == OMX_FALSE)) {
                    IM_OSAL_MutexUnlock(inputUseBuffer->bufferMutex);
                    IM_OSAL_MutexUnlock(outputUseBuffer->bufferMutex);
                    if(!CHECK_PORT_BEING_FLUSHED(IMInputPort)){
                        ret = IM_InputBufferGetQueue(pIMComponent);
                    }
                    break;
                }

                ret = pIMComponent->IM_bufferProcess(pOMXComponent, inputUseBuffer, outputUseBuffer);
                IM_OSAL_MutexUnlock(inputUseBuffer->bufferMutex);
            }
           
            if (IM_Postprocess_OutputData(pOMXComponent) == OMX_FALSE)
                pIMComponent->remainOutputData = OMX_TRUE;
            else
                pIMComponent->remainOutputData = OMX_FALSE;

            IM_OSAL_MutexUnlock(outputUseBuffer->bufferMutex);
			if(ret == IM_OMX_WAIT_PORT_RECONFIG){
				IM_OSAL_SleepMillisec(5);
			}
        }
    }

EXIT:

    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_AudioDecodeGetParameter(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_INDEXTYPE  nParamIndex,
        OMX_INOUT OMX_PTR     ComponentParameterStructure)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    if (pIMComponent->currentState == OMX_StateInvalid ) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    if (ComponentParameterStructure == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    switch (nParamIndex) {
        case OMX_IndexParamAudioInit:
            {
                OMX_PORT_PARAM_TYPE *portParam = (OMX_PORT_PARAM_TYPE *)ComponentParameterStructure;
                ret = IM_OMX_Check_SizeVersion(portParam, sizeof(OMX_PORT_PARAM_TYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                portParam->nPorts           = pIMComponent->portParam.nPorts;
                portParam->nStartPortNumber = pIMComponent->portParam.nStartPortNumber;
                ret = OMX_ErrorNone;
            }
            break;
        case OMX_IndexParamAudioPortFormat:
            {
                OMX_AUDIO_PARAM_PORTFORMATTYPE *portFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *)ComponentParameterStructure;
                OMX_U32                         portIndex = portFormat->nPortIndex;
                OMX_U32                         index    = portFormat->nIndex;
                IM_OMX_BASEPORT            *pIMPort = NULL;
                OMX_PARAM_PORTDEFINITIONTYPE   *portDefinition = NULL;
                OMX_U32                         supportFormatNum = 0; /* supportFormatNum = N-1 */

                ret = IM_OMX_Check_SizeVersion(portFormat, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                if ((portIndex >= pIMComponent->portParam.nPorts)) {
                    ret = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }


                if (portIndex == INPUT_PORT_INDEX) {
                    supportFormatNum = INPUT_PORT_SUPPORTFORMAT_NUM_MAX - 1;
                    if (index > supportFormatNum) {
                        ret = OMX_ErrorNoMore;
                        goto EXIT;
                    }

                    pIMPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
                    portDefinition = &pIMPort->portDefinition;

                    portFormat->eEncoding = portDefinition->format.audio.eEncoding;
                } else if (portIndex == OUTPUT_PORT_INDEX) {
                    supportFormatNum = OUTPUT_PORT_SUPPORTFORMAT_NUM_MAX - 1;
                    if (index > supportFormatNum) {
                        ret = OMX_ErrorNoMore;
                        goto EXIT;
                    }

                    pIMPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
                    portDefinition = &pIMPort->portDefinition;

                    portFormat->eEncoding = portDefinition->format.audio.eEncoding;
                }
                ret = OMX_ErrorNone;
            }
            break;
        default:
            {
                ret = IM_OMX_GetParameter(hComponent, nParamIndex, ComponentParameterStructure);
            }
            break;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}
OMX_ERRORTYPE IM_OMX_AudioDecodeSetParameter(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_INDEXTYPE  nIndex,
        OMX_IN OMX_PTR        ComponentParameterStructure)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    if (pIMComponent->currentState == OMX_StateInvalid ) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    if (ComponentParameterStructure == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    switch (nIndex) {
        case OMX_IndexParamAudioPortFormat:
            {
                OMX_AUDIO_PARAM_PORTFORMATTYPE *portFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *)ComponentParameterStructure;
                OMX_U32                         portIndex = portFormat->nPortIndex;
                OMX_U32                         index    = portFormat->nIndex;
                IM_OMX_BASEPORT            *pIMPort = NULL;
                OMX_PARAM_PORTDEFINITIONTYPE   *portDefinition = NULL;
                OMX_U32                         supportFormatNum = 0; /* supportFormatNum = N-1 */

                ret = IM_OMX_Check_SizeVersion(portFormat, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                if ((portIndex >= pIMComponent->portParam.nPorts)) {
                    ret = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }

                pIMPort = &pIMComponent->pIMPort[portIndex];
                portDefinition = &pIMPort->portDefinition;

                portDefinition->format.audio.eEncoding = portFormat->eEncoding;
                ret = OMX_ErrorNone;
            }
            break;
        default:
            {
                ret = IM_OMX_SetParameter(hComponent, nIndex, ComponentParameterStructure);
            }
            break;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_AudioDecodeGetConfig(
        OMX_HANDLETYPE hComponent,
        OMX_INDEXTYPE nIndex,
        OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    if (pComponentConfigStructure == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (pIMComponent->currentState == OMX_StateInvalid) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    switch (nIndex) {
        default:
            ret = IM_OMX_GetConfig(hComponent, nIndex, pComponentConfigStructure);
            break;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_AudioDecodeSetConfig(
        OMX_HANDLETYPE hComponent,
        OMX_INDEXTYPE nIndex,
        OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    if (pComponentConfigStructure == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (pIMComponent->currentState == OMX_StateInvalid) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    switch (nIndex) {
        case OMX_IndexConfigAudioMute:
            {
                IM_OSAL_Info("OMX_IndexConfigAudioMute");
                ret = OMX_ErrorUnsupportedIndex;
            }
            break;
        case OMX_IndexConfigAudioVolume:
            {
                IM_OSAL_Info("OMX_IndexConfigAudioVolume");
                ret = OMX_ErrorUnsupportedIndex;
            }
            break;
        default:
            ret = IM_OMX_SetConfig(hComponent, nIndex, pComponentConfigStructure);
            break;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_AudioDecodeGetExtensionIndex(
        OMX_IN OMX_HANDLETYPE  hComponent,
        OMX_IN OMX_STRING      cParameterName,
        OMX_OUT OMX_INDEXTYPE *pIndexType)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    if ((cParameterName == NULL) || (pIndexType == NULL)) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (pIMComponent->currentState == OMX_StateInvalid) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    ret = IM_OMX_GetExtensionIndex(hComponent, cParameterName, pIndexType);

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_AudioDecodeComponentInit(OMX_IN OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE                  ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE             *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT      *pIMComponent = NULL;
    IM_OMX_BASEPORT           *pIMPort = NULL;
    IM_OMX_AUDIODEC_COMPONENT *pAudioDec = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        IM_OSAL_Error("OMX_Error, Line:%d", __LINE__);
        goto EXIT;
    }

    ret = IM_OMX_BaseComponent_Constructor(pOMXComponent);
    if (ret != OMX_ErrorNone) {
        IM_OSAL_Error("OMX_Error, Line:%d", __LINE__);
        goto EXIT;
    }

    ret = IM_OMX_Port_Constructor(pOMXComponent);
    if (ret != OMX_ErrorNone) {
        IM_OMX_BaseComponent_Destructor(pOMXComponent);
        IM_OSAL_Error("OMX_Error, Line:%d", __LINE__);
        goto EXIT;
    }

    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    pAudioDec = IM_OSAL_Malloc(sizeof(IM_OMX_AUDIODEC_COMPONENT));
    if (pAudioDec == NULL) {
        IM_OMX_BaseComponent_Destructor(pOMXComponent);
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }

    IM_OSAL_Memset(pAudioDec, 0, sizeof(IM_OMX_AUDIODEC_COMPONENT));
    pIMComponent->hComponentHandle = (OMX_HANDLETYPE)pAudioDec;
    pIMComponent->bSaveFlagEOS = OMX_FALSE;

    /* Input port */
    pIMPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    pIMPort->portDefinition.nBufferCountActual = MAX_AUDIO_INPUTBUFFER_NUM;
    pIMPort->portDefinition.nBufferCountMin = MAX_AUDIO_INPUTBUFFER_NUM;
    pIMPort->portDefinition.nBufferSize = DEFAULT_AUDIO_INPUT_BUFFER_SIZE;
    pIMPort->portDefinition.eDomain = OMX_PortDomainAudio;

    pIMPort->portDefinition.format.audio.cMIMEType = IM_OSAL_Malloc(MAX_OMX_MIMETYPE_SIZE);
    IM_OSAL_Strcpy(pIMPort->portDefinition.format.audio.cMIMEType, "audio/raw");
    pIMPort->portDefinition.format.audio.pNativeRender = 0;
    pIMPort->portDefinition.format.audio.bFlagErrorConcealment = OMX_FALSE;
    pIMPort->portDefinition.format.audio.eEncoding = OMX_AUDIO_CodingUnused;

    /* Output port */
    pIMPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    pIMPort->portDefinition.nBufferCountActual = MAX_AUDIO_OUTPUTBUFFER_NUM;
    pIMPort->portDefinition.nBufferCountMin = MAX_AUDIO_OUTPUTBUFFER_NUM;
    pIMPort->portDefinition.nBufferSize = DEFAULT_AUDIO_OUTPUT_BUFFER_SIZE;
    pIMPort->portDefinition.eDomain = OMX_PortDomainAudio;

    pIMPort->portDefinition.format.audio.cMIMEType = IM_OSAL_Malloc(MAX_OMX_MIMETYPE_SIZE);
    IM_OSAL_Strcpy(pIMPort->portDefinition.format.audio.cMIMEType, "audio/raw");
    pIMPort->portDefinition.format.audio.pNativeRender = 0;
    pIMPort->portDefinition.format.audio.bFlagErrorConcealment = OMX_FALSE;
    pIMPort->portDefinition.format.audio.eEncoding = OMX_AUDIO_CodingUnused;


    pOMXComponent->UseBuffer              = &IM_OMX_UseBuffer;
    pOMXComponent->AllocateBuffer         = &IM_OMX_AllocateBuffer;
    pOMXComponent->FreeBuffer             = &IM_OMX_FreeBuffer;
    pOMXComponent->ComponentTunnelRequest = &IM_OMX_ComponentTunnelRequest;

    pIMComponent->IM_BufferProcess        = &IM_OMX_BufferProcess;
    pIMComponent->IM_BufferReset          = &IM_BufferReset;
    pIMComponent->IM_InputBufferReturn    = &IM_InputBufferReturn;
    pIMComponent->IM_OutputBufferReturn   = &IM_OutputBufferReturn;
    pIMComponent->IM_AllocateTunnelBuffer = &IM_OMX_AllocateTunnelBuffer;
    pIMComponent->IM_FreeTunnelBuffer     = &IM_OMX_FreeTunnelBuffer;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_AudioDecodeComponentDeinit(OMX_IN OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE                  ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE             *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT      *pIMComponent = NULL;
    IM_OMX_BASEPORT           *pIMPort = NULL;
    IM_OMX_AUDIODEC_COMPONENT *pAudioDec = NULL;
    int                            i = 0;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    pAudioDec = (IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle;
    IM_OSAL_Free(pAudioDec);
    pIMComponent->hComponentHandle = pAudioDec = NULL;

    for(i = 0; i < ALL_PORT_NUM; i++) {
        pIMPort = &pIMComponent->pIMPort[i];
        IM_OSAL_Free(pIMPort->portDefinition.format.audio.cMIMEType);
        pIMPort->portDefinition.format.audio.cMIMEType = NULL;
    }

    ret = IM_OMX_Port_Destructor(pOMXComponent);

    ret = IM_OMX_BaseComponent_Destructor(hComponent);

EXIT:
    IM_OSAL_Exiting();

    return ret;
}
