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
 * @file       IM_OMX_Baseport.c
 * @brief
 * @author     ayakashi (yanyuan_xu@infotm.com)
 * @version    1.0
 * @history
 *    2010.03.01 : Create
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IM_OMX_Macros.h"
#include "IM_OSAL_Event.h"
#include "IM_OSAL_Semaphore.h"
#include "IM_OSAL_Mutex.h"

#include "IM_OMX_Baseport.h"
#include "IM_OMX_Basecomponent.h"

#undef  IM_LOG_TAG
#define IM_LOG_TAG    "IM_BASE_PORT"
#define IM_LOG_OFF
#include "IM_OSAL_Trace.h"

OMX_ERRORTYPE IM_PortReset(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 portIndex)
{
	OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_BUFFERHEADERTYPE  *temp_bufferHeader = NULL;
    int                    i = 0;

    IM_OSAL_Entering();

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    pIMPort = &pIMComponent->pIMPort[portIndex];
    if (portIndex >= pIMComponent->portParam.nPorts) {
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

    IM_OSAL_Memset(pIMPort->extendBufferHeader, 0, sizeof(OMX_BUFFERHEADERTYPE) * MAX_BUFFER_NUM);
    IM_OSAL_Memset(pIMPort->bufferStateAllocate, 0, sizeof(OMX_U32) * MAX_BUFFER_NUM);

EXIT:
	IM_OSAL_Exiting();
	return ret;
}

OMX_ERRORTYPE IM_OMX_FlushPort(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 portIndex)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_BUFFERHEADERTYPE  *bufferHeader = NULL;
    IM_OMX_MESSAGE       *message = NULL;
    OMX_U32                flushNum = 0;
    OMX_S32                semValue = 0;

    IM_OSAL_Entering();

	IM_OSAL_Error("IM_OMX_FlushPorting....");
    pIMPort = &pIMComponent->pIMPort[portIndex];
    while (IM_OSAL_GetElemNum(&pIMPort->bufferQ) > 0) {
        IM_OSAL_Get_SemaphoreCount(pIMComponent->pIMPort[portIndex].bufferSemID, &semValue);
        if (semValue == 0)
            IM_OSAL_SemaphorePost(pIMComponent->pIMPort[portIndex].bufferSemID);
        IM_OSAL_SemaphoretryWait(pIMComponent->pIMPort[portIndex].bufferSemID);

        message = (IM_OMX_MESSAGE *)IM_OSAL_Dequeue(&pIMPort->bufferQ);
        if (message != NULL) {
            bufferHeader = (OMX_BUFFERHEADERTYPE *)message->pCmdData;
            bufferHeader->nFilledLen = 0;

            if (CHECK_PORT_TUNNELED(pIMPort) && !CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                if (portIndex) {
                    OMX_EmptyThisBuffer(pIMPort->tunneledComponent, bufferHeader);
                } else {
                    OMX_FillThisBuffer(pIMPort->tunneledComponent, bufferHeader);
                }
                IM_OSAL_Free(message);
                message = NULL;
            } else if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                IM_OSAL_Error("Tunneled mode is not working, Line:%d", __LINE__);
                ret = OMX_ErrorNotImplemented;
                IM_OSAL_Queue(&pIMPort->bufferQ, pIMPort);
                goto EXIT;
            } else {
                if (portIndex == OUTPUT_PORT_INDEX) {
                    pIMComponent->pCallbacks->FillBufferDone(pOMXComponent, pIMComponent->callbackData, bufferHeader);
                } else {
                    pIMComponent->pCallbacks->EmptyBufferDone(pOMXComponent, pIMComponent->callbackData, bufferHeader);
                }

                IM_OSAL_Free(message);
                message = NULL;
            }
        }
    }

    if (pIMComponent->IMDataBuffer[portIndex].dataValid == OMX_TRUE) {
        if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
            message = IM_OSAL_Malloc(sizeof(IM_OMX_MESSAGE));
            message->pCmdData = pIMComponent->IMDataBuffer[portIndex].bufferHeader;
            message->messageType = 0;
            message->messageParam = -1;
            IM_OSAL_Queue(&pIMPort->bufferQ, message);
            pIMComponent->IM_BufferReset(pOMXComponent, portIndex);
        } else {
            if (portIndex == INPUT_PORT_INDEX)
                pIMComponent->IM_InputBufferReturn(pOMXComponent);
            else if (portIndex == OUTPUT_PORT_INDEX)
                pIMComponent->IM_OutputBufferReturn(pOMXComponent);
        }
    }

    if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
        while (IM_OSAL_GetElemNum(&pIMPort->bufferQ) < pIMPort->assignedBufferNum) {
            IM_OSAL_SemaphoreWait(pIMComponent->pIMPort[portIndex].bufferSemID);
        }
        if (IM_OSAL_GetElemNum(&pIMPort->bufferQ) != pIMPort->assignedBufferNum)
            IM_OSAL_SetElemNum(&pIMPort->bufferQ, pIMPort->assignedBufferNum);
    } else {
        while(1) {
            int cnt = 0;
            IM_OSAL_Get_SemaphoreCount(pIMComponent->pIMPort[portIndex].bufferSemID, &cnt);
            if (cnt == 0)
                break;
            IM_OSAL_SemaphoreWait(pIMComponent->pIMPort[portIndex].bufferSemID);
        }
        IM_OSAL_SetElemNum(&pIMPort->bufferQ, 0);
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_BufferFlushProcess(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 nPortIndex)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_S32                portIndex = 0;
    OMX_U32                i = 0, cnt = 0;
    IM_OMX_DATABUFFER    *flushBuffer = NULL;

    IM_OSAL_Entering();

    if (pOMXComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;


    cnt = (nPortIndex == ALL_PORT_INDEX ) ? ALL_PORT_NUM : 1;

    for (i = 0; i < cnt; i++) {
        if (nPortIndex == ALL_PORT_INDEX)
            portIndex = i;
        else
            portIndex = nPortIndex;

        IM_OSAL_SignalSet(pIMComponent->pauseEvent);

        flushBuffer = &pIMComponent->IMDataBuffer[portIndex];

		IM_OSAL_Error("IM_OMX_FlushPort");
        IM_OSAL_MutexLock(flushBuffer->bufferMutex);
		pIMComponent->IM_DecoderFlush(pOMXComponent,nPortIndex);
        ret = IM_OMX_FlushPort(pOMXComponent, portIndex);
        IM_OSAL_MutexUnlock(flushBuffer->bufferMutex);

		IM_OSAL_MutexLock(pIMComponent->pIMPort[portIndex].portMutex);
        pIMComponent->pIMPort[portIndex].bIsPortFlushed = OMX_FALSE;
		IM_OSAL_MutexUnlock(pIMComponent->pIMPort[portIndex].portMutex);

        if (ret == OMX_ErrorNone) {
            IM_OSAL_Info("OMX_CommandFlush EventCmdComplete");
            pIMComponent->pCallbacks->EventHandler((OMX_HANDLETYPE)pOMXComponent,
                            pIMComponent->callbackData,
                            OMX_EventCmdComplete,
                            OMX_CommandFlush, portIndex, NULL);
        }

        if (portIndex == INPUT_PORT_INDEX) {
            //pIMComponent->checkTimeStamp.needSetStartTimeStamp = OMX_TRUE;
            //pIMComponent->checkTimeStamp.needCheckStartTimeStamp = OMX_FALSE;
            //IM_OSAL_Memset(pIMComponent->timeStamp, -19771003, sizeof(OMX_TICKS) * MAX_TIMESTAMP);
            //IM_OSAL_Memset(pIMComponent->nFlags, 0, sizeof(OMX_U32) * MAX_FLAGS);
            //pIMComponent->getAllDelayBuffer = OMX_FALSE;
            pIMComponent->bSaveFlagEOS = OMX_FALSE;
            pIMComponent->reInputData = OMX_FALSE;
        } else if (portIndex == OUTPUT_PORT_INDEX) {
           // pIMComponent->remainOutputData = OMX_FALSE;
        }
    }

EXIT:
    if (ret != OMX_ErrorNone) {
            pIMComponent->pCallbacks->EventHandler(pOMXComponent,
                            pIMComponent->callbackData,
                            OMX_EventError,
                            ret, 0, NULL);
    }

    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_BufferFlushProcessNoEvent(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 nPortIndex)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_S32                portIndex = 0;
    OMX_U32                i = 0, cnt = 0;
    IM_OMX_DATABUFFER    *flushBuffer = NULL;

    IM_OSAL_Entering();

    if (pOMXComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

	pIMComponent->IM_DecoderFlush(pOMXComponent,nPortIndex);

    cnt = (nPortIndex == ALL_PORT_INDEX ) ? ALL_PORT_NUM : 1;

    for (i = 0; i < cnt; i++) {
        if (nPortIndex == ALL_PORT_INDEX)
            portIndex = i;
        else
            portIndex = nPortIndex;

		IM_OSAL_MutexLock(pIMComponent->pIMPort[portIndex].portMutex);
        pIMComponent->pIMPort[portIndex].bIsPortFlushed = OMX_TRUE;
		IM_OSAL_MutexUnlock(pIMComponent->pIMPort[portIndex].portMutex);

        IM_OSAL_SignalSet(pIMComponent->pauseEvent);

        flushBuffer = &pIMComponent->IMDataBuffer[portIndex];

        IM_OSAL_MutexLock(flushBuffer->bufferMutex);
        ret = IM_OMX_FlushPort(pOMXComponent, portIndex);
        IM_OSAL_MutexUnlock(flushBuffer->bufferMutex);


		IM_OSAL_MutexLock(pIMComponent->pIMPort[portIndex].portMutex);
        pIMComponent->pIMPort[portIndex].bIsPortFlushed = OMX_FALSE;
		IM_OSAL_MutexUnlock(pIMComponent->pIMPort[portIndex].portMutex);

        if (portIndex == INPUT_PORT_INDEX) {
            //pIMComponent->checkTimeStamp.needSetStartTimeStamp = OMX_TRUE;
            //pIMComponent->checkTimeStamp.needCheckStartTimeStamp = OMX_FALSE;
            //IM_OSAL_Memset(pIMComponent->timeStamp, -19771003, sizeof(OMX_TICKS) * MAX_TIMESTAMP);
            //IM_OSAL_Memset(pIMComponent->nFlags, 0, sizeof(OMX_U32) * MAX_FLAGS);
            //pIMComponent->getAllDelayBuffer = OMX_FALSE;
            pIMComponent->bSaveFlagEOS = OMX_FALSE;
            //pIMComponent->remainOutputData = OMX_FALSE;
            //pIMComponent->reInputData = OMX_FALSE;
        }
    }

EXIT:
    if (ret != OMX_ErrorNone) {
        pIMComponent->pCallbacks->EventHandler(pOMXComponent,
                            pIMComponent->callbackData,
                            OMX_EventError,
                            ret, 0, NULL);
    }

    IM_OSAL_Exiting();

    return ret;
}


OMX_ERRORTYPE IM_OMX_EnablePort(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 portIndex)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_U32                i = 0, cnt = 0;

    IM_OSAL_Entering();

    pIMPort = &pIMComponent->pIMPort[portIndex];

    pIMPort->transitions = PORT_ENABLING;

    if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
        ret = pIMComponent->IM_AllocateTunnelBuffer(pIMPort, portIndex);
        if (OMX_ErrorNone != ret) {
            goto EXIT;
        }
        pIMPort->portDefinition.bPopulated = OMX_TRUE;
        if (pIMComponent->currentState == OMX_StateExecuting) {
            for (i=0; i<pIMPort->tunnelBufferNum; i++) {
                IM_OSAL_SemaphorePost(pIMComponent->pIMPort[portIndex].bufferSemID);
            }
        }
    } else if (CHECK_PORT_TUNNELED(pIMPort) && !CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
        if ((pIMComponent->currentState != OMX_StateLoaded) && (pIMComponent->currentState != OMX_StateWaitForResources)) {
            //pIMPort->portDefinition.bPopulated = OMX_TRUE;
        }
    } else {
        if ((pIMComponent->currentState != OMX_StateLoaded) && (pIMComponent->currentState != OMX_StateWaitForResources)) {
            //IM_OSAL_SemaphoreWait(pIMPort->loadedResource);
            //pIMPort->portDefinition.bPopulated = OMX_TRUE;
        }
    }

    pIMComponent->IM_checkTransitions(pOMXComponent);
    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_PortEnableProcess(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 nPortIndex)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    OMX_S32                portIndex = 0;
    OMX_U32                i = 0, cnt = 0;

    IM_OSAL_Entering();

    if (pOMXComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    cnt = (nPortIndex == ALL_PORT_INDEX) ? ALL_PORT_NUM : 1;

    for (i = 0; i < cnt; i++) {
        if (nPortIndex == ALL_PORT_INDEX)
            portIndex = i;
        else
            portIndex = nPortIndex;

        ret = IM_OMX_EnablePort(pOMXComponent, portIndex);
#if 0
        if (ret == OMX_ErrorNone) {
            pIMComponent->pCallbacks->EventHandler(pOMXComponent,
                            pIMComponent->callbackData,
                            OMX_EventCmdComplete,
                            OMX_CommandPortEnable, portIndex, NULL);
        }
#endif 
    }

EXIT:
    if (ret != OMX_ErrorNone) {
            pIMComponent->pCallbacks->EventHandler(pOMXComponent,
                            pIMComponent->callbackData,
                            OMX_EventError,
                            ret, 0, NULL);
        }

    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_DisablePort(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 portIndex)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_U32                i = 0, elemNum = 0;
    IM_OMX_MESSAGE       *message;

    IM_OSAL_Entering();

    pIMPort = &pIMComponent->pIMPort[portIndex];

    if (!CHECK_PORT_ENABLED(pIMPort)) {
        ret = OMX_ErrorNone;
        goto EXIT;
    }
    pIMPort->transitions = PORT_DISABLING;

    if (pIMComponent->currentState!=OMX_StateLoaded) {
        if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
            while (IM_OSAL_GetElemNum(&pIMPort->bufferQ) >0 ) {
                message = (IM_OMX_MESSAGE*)IM_OSAL_Dequeue(&pIMPort->bufferQ);
                IM_OSAL_Free(message);
            }
            ret = pIMComponent->IM_FreeTunnelBuffer(pIMPort, portIndex);
            if (OMX_ErrorNone != ret) {
                goto EXIT;
            }
            pIMPort->portDefinition.bPopulated = OMX_FALSE;
        } else if (CHECK_PORT_TUNNELED(pIMPort) && !CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
            //pIMPort->portDefinition.bPopulated = OMX_FALSE;
            //IM_OSAL_SemaphoreWait(pIMPort->unloadedResource);
        } else {
            if (CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                while (IM_OSAL_GetElemNum(&pIMPort->bufferQ) >0 ) {
                    message = (IM_OMX_MESSAGE*)IM_OSAL_Dequeue(&pIMPort->bufferQ);
                    IM_OSAL_Free(message);
                }
            }
            //pIMPort->portDefinition.bPopulated = OMX_FALSE;
            //IM_OSAL_SemaphoreWait(pIMPort->unloadedResource);
        }
    }

    pIMComponent->IM_checkTransitions(pOMXComponent);
    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_PortDisableProcess(OMX_COMPONENTTYPE *pOMXComponent, OMX_S32 nPortIndex)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    OMX_S32                portIndex = 0;
    OMX_U32                i = 0, cnt = 0;
    IM_OMX_DATABUFFER      *flushBuffer = NULL;

    IM_OSAL_Entering();

    if (pOMXComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    cnt = (nPortIndex == ALL_PORT_INDEX ) ? ALL_PORT_NUM : 1;

    /* port flush*/
    for(i = 0; i < cnt; i++) {
        if (nPortIndex == ALL_PORT_INDEX)
            portIndex = i;
        else
            portIndex = nPortIndex;

        pIMComponent->pIMPort[portIndex].bIsPortFlushed = OMX_TRUE;

        flushBuffer = &pIMComponent->IMDataBuffer[portIndex];

        IM_OSAL_MutexLock(flushBuffer->bufferMutex);
        ret = IM_OMX_FlushPort(pOMXComponent, portIndex);
        IM_OSAL_MutexUnlock(flushBuffer->bufferMutex);

        pIMComponent->pIMPort[portIndex].bIsPortFlushed = OMX_FALSE;

        if (portIndex == INPUT_PORT_INDEX) {
            pIMComponent->checkTimeStamp.needSetStartTimeStamp = OMX_TRUE;
            pIMComponent->checkTimeStamp.needCheckStartTimeStamp = OMX_FALSE;
            IM_OSAL_Memset(pIMComponent->timeStamp, -19771003, sizeof(OMX_TICKS) * MAX_TIMESTAMP);
            IM_OSAL_Memset(pIMComponent->nFlags, 0, sizeof(OMX_U32) * MAX_FLAGS);
            pIMComponent->getAllDelayBuffer = OMX_FALSE;
            pIMComponent->bSaveFlagEOS = OMX_FALSE;
            pIMComponent->reInputData = OMX_FALSE;
        } else if (portIndex == OUTPUT_PORT_INDEX) {
            pIMComponent->remainOutputData = OMX_FALSE;
        }
    }

    for(i = 0; i < cnt; i++) {
        if (nPortIndex == ALL_PORT_INDEX)
            portIndex = i;
        else
            portIndex = nPortIndex;

        ret = IM_OMX_DisablePort(pOMXComponent, portIndex);
        pIMComponent->pIMPort[portIndex].bIsPortDisabled = OMX_FALSE;
#if 0
        if (ret == OMX_ErrorNone) {
            pIMComponent->pCallbacks->EventHandler(pOMXComponent,
                            pIMComponent->callbackData,
                            OMX_EventCmdComplete,
                            OMX_CommandPortDisable, portIndex, NULL);
        }
#endif 
    }

EXIT:
    if (ret != OMX_ErrorNone) {
        pIMComponent->pCallbacks->EventHandler(pOMXComponent,
                        pIMComponent->callbackData,
                        OMX_EventError,
                        ret, 0, NULL);
    }

    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_EmptyThisBuffer(
    OMX_IN OMX_HANDLETYPE        hComponent,
    OMX_IN OMX_BUFFERHEADERTYPE *pBuffer)
{
    OMX_ERRORTYPE           ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_BOOL               findBuffer = OMX_FALSE;
    IM_OMX_MESSAGE       *message;
    OMX_U32                i = 0;

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
    if (pIMComponent->currentState == OMX_StateInvalid) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    if (pBuffer == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (pBuffer->nInputPortIndex != INPUT_PORT_INDEX) {
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

    ret = IM_OMX_Check_SizeVersion(pBuffer, sizeof(OMX_BUFFERHEADERTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if ((pIMComponent->currentState != OMX_StateIdle) &&
        (pIMComponent->currentState != OMX_StateExecuting) &&
        (pIMComponent->currentState != OMX_StatePause)) {
        ret = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    pIMPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    if ((!CHECK_PORT_ENABLED(pIMPort)) ||
        ((CHECK_PORT_BEING_FLUSHED(pIMPort) || CHECK_PORT_BEING_DISABLED(pIMPort)) &&
        (!CHECK_PORT_TUNNELED(pIMPort) || !CHECK_PORT_BUFFER_SUPPLIER(pIMPort))) ||
        ((pIMComponent->transientState == IM_OMX_TransStateExecutingToIdle) &&
        (CHECK_PORT_TUNNELED(pIMPort) && !CHECK_PORT_BUFFER_SUPPLIER(pIMPort)))) {
        ret = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    for (i = 0; i < pIMPort->portDefinition.nBufferCountActual; i++) {
        if (pBuffer == pIMPort->extendBufferHeader[i].OMXBufferHeader) {
            findBuffer = OMX_TRUE;
            break;
        }
    }

    if (findBuffer == OMX_FALSE) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    } else {
        ret = OMX_ErrorNone;
    }

    message = IM_OSAL_Malloc(sizeof(IM_OMX_MESSAGE));
    if (message == NULL) {
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    message->messageType = IM_OMX_CommandEmptyBuffer;
    message->messageParam = (OMX_U32) i;
    message->pCmdData = (OMX_PTR)pBuffer;

    IM_OSAL_Queue(&pIMPort->bufferQ, (void *)message);
    IM_OSAL_SemaphorePost(pIMPort->bufferSemID);

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_FillThisBuffer(
    OMX_IN OMX_HANDLETYPE        hComponent,
    OMX_IN OMX_BUFFERHEADERTYPE *pBuffer)
{
    OMX_ERRORTYPE           ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_BOOL               findBuffer = OMX_FALSE;
    IM_OMX_MESSAGE       *message;
    OMX_U32                i = 0;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
		IM_OSAL_Error("xxxx %d", __LINE__);
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
		IM_OSAL_Error("xxxx %d", __LINE__);
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
		IM_OSAL_Error("xxxx %d", __LINE__);
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    if (pIMComponent->currentState == OMX_StateInvalid) {
		IM_OSAL_Error("xxxx %d", __LINE__);
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    if (pBuffer == NULL) {
		IM_OSAL_Error("xxxx %d", __LINE__);
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (pBuffer->nOutputPortIndex != OUTPUT_PORT_INDEX) {
		IM_OSAL_Error("xxxx %d", __LINE__);
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

    ret = IM_OMX_Check_SizeVersion(pBuffer, sizeof(OMX_BUFFERHEADERTYPE));
    if (ret != OMX_ErrorNone) {
		IM_OSAL_Error("xxxx %d", __LINE__);
        goto EXIT;
    }

    if ((pIMComponent->currentState != OMX_StateIdle) &&
        (pIMComponent->currentState != OMX_StateExecuting) &&
        (pIMComponent->currentState != OMX_StatePause)) {
		IM_OSAL_Error("xxxx %d, currentState = %d", __LINE__, pIMComponent->currentState);
        ret = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    pIMPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    if ((!CHECK_PORT_ENABLED(pIMPort)) ||
        ((CHECK_PORT_BEING_FLUSHED(pIMPort) || CHECK_PORT_BEING_DISABLED(pIMPort)) &&
        (!CHECK_PORT_TUNNELED(pIMPort) || !CHECK_PORT_BUFFER_SUPPLIER(pIMPort))) ||
        ((pIMComponent->transientState == IM_OMX_TransStateExecutingToIdle) &&
        (CHECK_PORT_TUNNELED(pIMPort) && !CHECK_PORT_BUFFER_SUPPLIER(pIMPort)))) {
		IM_OSAL_Error("port enable %d",CHECK_PORT_ENABLED(pIMPort));
		IM_OSAL_Error("port flush %d", CHECK_PORT_BEING_FLUSHED(pIMPort));
		IM_OSAL_Error("port being disable %d",CHECK_PORT_BEING_DISABLED(pIMPort));
		IM_OSAL_Error("transientState = %x",pIMComponent->transientState);
        ret = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    for (i = 0; i < pIMPort->portDefinition.nBufferCountActual; i++) {
        if (pBuffer == pIMPort->extendBufferHeader[i].OMXBufferHeader) {
            findBuffer = OMX_TRUE;
            break;
        }
    }

    if (findBuffer == OMX_FALSE) {
		IM_OSAL_Error("xxxx %d", __LINE__);
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    } else {
        ret = OMX_ErrorNone;
    }

    message = IM_OSAL_Malloc(sizeof(IM_OMX_MESSAGE));
    if (message == NULL) {
		IM_OSAL_Error("xxxx %d", __LINE__);
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    message->messageType = IM_OMX_CommandFillBuffer;
    message->messageParam = (OMX_U32) i;
    message->pCmdData = (OMX_PTR)pBuffer;

    IM_OSAL_Queue(&pIMPort->bufferQ, (void *)message);
    IM_OSAL_SemaphorePost(pIMPort->bufferSemID);

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_Port_Constructor(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    IM_OMX_BASEPORT      *pIMInputPort = NULL;
    IM_OMX_BASEPORT      *pIMOutputPort = NULL;
    int i = 0;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        IM_OSAL_Error("OMX_ErrorBadParameter, Line:%d", __LINE__);
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    if (pOMXComponent->pComponentPrivate == NULL) {
        ret = OMX_ErrorBadParameter;
        IM_OSAL_Error("OMX_ErrorBadParameter, Line:%d", __LINE__);
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    INIT_SET_SIZE_VERSION(&pIMComponent->portParam, OMX_PORT_PARAM_TYPE);
    pIMComponent->portParam.nPorts = ALL_PORT_NUM;
    pIMComponent->portParam.nStartPortNumber = INPUT_PORT_INDEX;

    pIMPort = IM_OSAL_Malloc(sizeof(IM_OMX_BASEPORT) * ALL_PORT_NUM);
    if (pIMPort == NULL) {
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }
    IM_OSAL_Memset(pIMPort, 0, sizeof(IM_OMX_BASEPORT) * ALL_PORT_NUM);
    pIMComponent->pIMPort = pIMPort;

    /* Input Port */
    pIMInputPort = &pIMPort[INPUT_PORT_INDEX];

    IM_OSAL_QueueCreate(&pIMInputPort->bufferQ);
	ret = IM_OSAL_MutexCreate(&pIMInputPort->portMutex);
	if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }

    pIMInputPort->extendBufferHeader = IM_OSAL_Malloc(sizeof(IM_OMX_BUFFERHEADERTYPE) * MAX_BUFFER_NUM);
    if (pIMInputPort->extendBufferHeader == NULL) {
        IM_OSAL_Free(pIMPort);
        pIMPort = NULL;
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }
    IM_OSAL_Memset(pIMInputPort->extendBufferHeader, 0, sizeof(IM_OMX_BUFFERHEADERTYPE) * MAX_BUFFER_NUM);

    pIMInputPort->bufferStateAllocate = IM_OSAL_Malloc(sizeof(OMX_U32) * MAX_BUFFER_NUM);
    if (pIMInputPort->bufferStateAllocate == NULL) {
        IM_OSAL_Free(pIMInputPort->extendBufferHeader);
        pIMInputPort->extendBufferHeader = NULL;
        IM_OSAL_Free(pIMPort);
        pIMPort = NULL;
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }
    IM_OSAL_Memset(pIMInputPort->bufferStateAllocate, 0, sizeof(OMX_U32) * MAX_BUFFER_NUM);

    pIMInputPort->bufferSemID = NULL;
    pIMInputPort->assignedBufferNum = 0;
    pIMInputPort->portState = OMX_StateMax;
    pIMInputPort->bIsPortFlushed = OMX_FALSE;
    pIMInputPort->bIsPortDisabled = OMX_FALSE;
    pIMInputPort->tunneledComponent = NULL;
    pIMInputPort->tunneledPort = 0;
    pIMInputPort->tunnelBufferNum = 0;
    pIMInputPort->bufferSupplier = OMX_BufferSupplyUnspecified;
    pIMInputPort->tunnelFlags = 0;
    pIMInputPort->eControlRate = OMX_Video_ControlRateDisable;
    ret = IM_OSAL_SemaphoreCreate(&pIMInputPort->loadedResource);
    if (ret != OMX_ErrorNone) {
        IM_OSAL_Free(pIMInputPort->bufferStateAllocate);
        pIMInputPort->bufferStateAllocate = NULL;
        IM_OSAL_Free(pIMInputPort->extendBufferHeader);
        pIMInputPort->extendBufferHeader = NULL;
        IM_OSAL_Free(pIMPort);
        pIMPort = NULL;
        goto EXIT;
    }
    ret = IM_OSAL_SemaphoreCreate(&pIMInputPort->unloadedResource);
    if (ret != OMX_ErrorNone) {
        IM_OSAL_SemaphoreTerminate(pIMInputPort->loadedResource);
        pIMInputPort->loadedResource = NULL;
        IM_OSAL_Free(pIMInputPort->bufferStateAllocate);
        pIMInputPort->bufferStateAllocate = NULL;
        IM_OSAL_Free(pIMInputPort->extendBufferHeader);
        pIMInputPort->extendBufferHeader = NULL;
        IM_OSAL_Free(pIMPort);
        pIMPort = NULL;
        goto EXIT;
    }

    INIT_SET_SIZE_VERSION(&pIMInputPort->portDefinition, OMX_PARAM_PORTDEFINITIONTYPE);
    pIMInputPort->portDefinition.nPortIndex = INPUT_PORT_INDEX;
    pIMInputPort->portDefinition.eDir = OMX_DirInput;
    pIMInputPort->portDefinition.nBufferCountActual = 0;
    pIMInputPort->portDefinition.nBufferCountMin = 0;
    pIMInputPort->portDefinition.nBufferSize = 0;
    pIMInputPort->portDefinition.bEnabled = OMX_FALSE;
    pIMInputPort->portDefinition.bPopulated = OMX_FALSE;
    pIMInputPort->portDefinition.eDomain = OMX_PortDomainMax;
    pIMInputPort->portDefinition.bBuffersContiguous = OMX_FALSE;
    pIMInputPort->portDefinition.nBufferAlignment = 0;
    pIMInputPort->markType.hMarkTargetComponent = NULL;
    pIMInputPort->markType.pMarkData = NULL;
    pIMInputPort->bUseAndroidNativeBuffer = OMX_FALSE;
    pIMInputPort->bStoreMetaDataInBuffer = OMX_FALSE;

    /* Output Port */
    pIMOutputPort = &pIMPort[OUTPUT_PORT_INDEX];

    IM_OSAL_QueueCreate(&pIMOutputPort->bufferQ);

	ret = IM_OSAL_MutexCreate(&pIMOutputPort->portMutex);
	if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }

    pIMOutputPort->extendBufferHeader = IM_OSAL_Malloc(sizeof(IM_OMX_BUFFERHEADERTYPE) * MAX_BUFFER_NUM);
    if (pIMOutputPort->extendBufferHeader == NULL) {
        IM_OSAL_SemaphoreTerminate(pIMInputPort->unloadedResource);
        pIMInputPort->unloadedResource = NULL;
        IM_OSAL_SemaphoreTerminate(pIMInputPort->loadedResource);
        pIMInputPort->loadedResource = NULL;
        IM_OSAL_Free(pIMInputPort->bufferStateAllocate);
        pIMInputPort->bufferStateAllocate = NULL;
        IM_OSAL_Free(pIMInputPort->extendBufferHeader);
        pIMInputPort->extendBufferHeader = NULL;
        IM_OSAL_Free(pIMPort);
        pIMPort = NULL;
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    IM_OSAL_Memset(pIMOutputPort->extendBufferHeader, 0, sizeof(IM_OMX_BUFFERHEADERTYPE) * MAX_BUFFER_NUM);

    pIMOutputPort->bufferStateAllocate = IM_OSAL_Malloc(sizeof(OMX_U32) * MAX_BUFFER_NUM);
    if (pIMOutputPort->bufferStateAllocate == NULL) {
        IM_OSAL_Free(pIMOutputPort->extendBufferHeader);
        pIMOutputPort->extendBufferHeader = NULL;

        IM_OSAL_SemaphoreTerminate(pIMInputPort->unloadedResource);
        pIMInputPort->unloadedResource = NULL;
        IM_OSAL_SemaphoreTerminate(pIMInputPort->loadedResource);
        pIMInputPort->loadedResource = NULL;
        IM_OSAL_Free(pIMInputPort->bufferStateAllocate);
        pIMInputPort->bufferStateAllocate = NULL;
        IM_OSAL_Free(pIMInputPort->extendBufferHeader);
        pIMInputPort->extendBufferHeader = NULL;
        IM_OSAL_Free(pIMPort);
        pIMPort = NULL;
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    IM_OSAL_Memset(pIMOutputPort->bufferStateAllocate, 0, sizeof(OMX_U32) * MAX_BUFFER_NUM);

    pIMOutputPort->bufferSemID = NULL;
    pIMOutputPort->assignedBufferNum = 0;
    pIMOutputPort->portState = OMX_StateMax;
    pIMOutputPort->bIsPortFlushed = OMX_FALSE;
    pIMOutputPort->bIsPortDisabled = OMX_FALSE;
    pIMOutputPort->tunneledComponent = NULL;
    pIMOutputPort->tunneledPort = 0;
    pIMOutputPort->tunnelBufferNum = 0;
    pIMOutputPort->bufferSupplier = OMX_BufferSupplyUnspecified;
    pIMOutputPort->tunnelFlags = 0;
    pIMOutputPort->eControlRate = OMX_Video_ControlRateDisable;
    ret = IM_OSAL_SemaphoreCreate(&pIMOutputPort->loadedResource);
    if (ret != OMX_ErrorNone) {
        IM_OSAL_Free(pIMOutputPort->bufferStateAllocate);
        pIMOutputPort->bufferStateAllocate = NULL;
        IM_OSAL_Free(pIMOutputPort->extendBufferHeader);
        pIMOutputPort->extendBufferHeader = NULL;

        IM_OSAL_SemaphoreTerminate(pIMInputPort->unloadedResource);
        pIMInputPort->unloadedResource = NULL;
        IM_OSAL_SemaphoreTerminate(pIMInputPort->loadedResource);
        pIMInputPort->loadedResource = NULL;
        IM_OSAL_Free(pIMInputPort->bufferStateAllocate);
        pIMInputPort->bufferStateAllocate = NULL;
        IM_OSAL_Free(pIMInputPort->extendBufferHeader);
        pIMInputPort->extendBufferHeader = NULL;
        IM_OSAL_Free(pIMPort);
        pIMPort = NULL;
        goto EXIT;
    }
    ret = IM_OSAL_SemaphoreCreate(&pIMOutputPort->unloadedResource);
    if (ret != OMX_ErrorNone) {
        IM_OSAL_SemaphoreTerminate(pIMOutputPort->loadedResource);
        pIMOutputPort->loadedResource = NULL;
        IM_OSAL_Free(pIMOutputPort->bufferStateAllocate);
        pIMOutputPort->bufferStateAllocate = NULL;
        IM_OSAL_Free(pIMOutputPort->extendBufferHeader);
        pIMOutputPort->extendBufferHeader = NULL;

        IM_OSAL_SemaphoreTerminate(pIMInputPort->unloadedResource);
        pIMInputPort->unloadedResource = NULL;
        IM_OSAL_SemaphoreTerminate(pIMInputPort->loadedResource);
        pIMInputPort->loadedResource = NULL;
        IM_OSAL_Free(pIMInputPort->bufferStateAllocate);
        pIMInputPort->bufferStateAllocate = NULL;
        IM_OSAL_Free(pIMInputPort->extendBufferHeader);
        pIMInputPort->extendBufferHeader = NULL;
        IM_OSAL_Free(pIMPort);
        pIMPort = NULL;
        goto EXIT;
    }

    INIT_SET_SIZE_VERSION(&pIMOutputPort->portDefinition, OMX_PARAM_PORTDEFINITIONTYPE);
    pIMOutputPort->portDefinition.nPortIndex = OUTPUT_PORT_INDEX;
    pIMOutputPort->portDefinition.eDir = OMX_DirOutput;
    pIMOutputPort->portDefinition.nBufferCountActual = 0;
    pIMOutputPort->portDefinition.nBufferCountMin = 0;
    pIMOutputPort->portDefinition.nBufferSize = 0;
    pIMOutputPort->portDefinition.bEnabled = OMX_FALSE;
    pIMOutputPort->portDefinition.bPopulated = OMX_FALSE;
    pIMOutputPort->portDefinition.eDomain = OMX_PortDomainMax;
    pIMOutputPort->portDefinition.bBuffersContiguous = OMX_FALSE;
    pIMOutputPort->portDefinition.nBufferAlignment = 0;
    pIMOutputPort->markType.hMarkTargetComponent = NULL;
    pIMOutputPort->markType.pMarkData = NULL;
    pIMOutputPort->bUseAndroidNativeBuffer = OMX_FALSE;
    pIMOutputPort->bStoreMetaDataInBuffer = OMX_FALSE;

    pIMComponent->checkTimeStamp.needSetStartTimeStamp = OMX_FALSE;
    pIMComponent->checkTimeStamp.needCheckStartTimeStamp = OMX_FALSE;
    pIMComponent->checkTimeStamp.startTimeStamp = 0;
    pIMComponent->checkTimeStamp.nStartFlags = 0x0;


    pOMXComponent->EmptyThisBuffer = &IM_OMX_EmptyThisBuffer;
    pOMXComponent->FillThisBuffer  = &IM_OMX_FillThisBuffer;

	pIMComponent->IM_PortReset = &IM_PortReset;

    ret = OMX_ErrorNone;
EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_Port_Destructor(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;

    IM_OSAL_Entering();

    int i = 0;

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
    for (i = 0; i < ALL_PORT_NUM; i++) {
        pIMPort = &pIMComponent->pIMPort[i];

        IM_OSAL_SemaphoreTerminate(pIMPort->loadedResource);
        pIMPort->loadedResource = NULL;
        IM_OSAL_SemaphoreTerminate(pIMPort->unloadedResource);
        pIMPort->unloadedResource = NULL;
        IM_OSAL_Free(pIMPort->bufferStateAllocate);
        pIMPort->bufferStateAllocate = NULL;
        IM_OSAL_Free(pIMPort->extendBufferHeader);
        pIMPort->extendBufferHeader = NULL;

        IM_OSAL_QueueTerminate(&pIMPort->bufferQ);
    }
    IM_OSAL_Free(pIMComponent->pIMPort);
    pIMComponent->pIMPort = NULL;
    ret = OMX_ErrorNone;
EXIT:
    IM_OSAL_Exiting();

    return ret;
}
