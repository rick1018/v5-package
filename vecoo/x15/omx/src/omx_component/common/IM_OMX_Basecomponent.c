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
 * @file       IM_OMX_Basecomponent.h
 * @brief
 * @author     ayakashi (yanyuan_xu@infotm.com)
 * @version    1.0
 * @history
 *    2010.03.01 : Create
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IM_OSAL_Event.h"
#include "IM_OSAL_Buffer.h"
#include "IM_OSAL_Trace.h"
#include "IM_OSAL_Memory.h"
#include "IM_OSAL_ETC.h"
#include "IM_OSAL_Semaphore.h"
#include "IM_OSAL_Mutex.h"
#include "IM_OSAL_Thread.h"

#include "IM_OMX_Baseport.h"
#include "IM_OMX_Basecomponent.h"
#include "IM_OMX_Resourcemanager.h"
#include "IM_OMX_Macros.h"

#undef  IM_LOG_TAG
#define IM_LOG_TAG    "IM_BASE_COMP"
#define IM_LOG_OFF
#include "IM_OSAL_Trace.h"


/* Change CHECK_SIZE_VERSION Macro */
OMX_ERRORTYPE IM_OMX_Check_SizeVersion(OMX_PTR header, OMX_U32 size)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    OMX_VERSIONTYPE* version = NULL;
    if (header == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    version = (OMX_VERSIONTYPE*)((char*)header + sizeof(OMX_U32));
    if (*((OMX_U32*)header) != size) {
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

OMX_ERRORTYPE IM_OMX_GetComponentVersion(
    OMX_IN  OMX_HANDLETYPE   hComponent,
    OMX_OUT OMX_STRING       pComponentName,
    OMX_OUT OMX_VERSIONTYPE *pComponentVersion,
    OMX_OUT OMX_VERSIONTYPE *pSpecVersion,
    OMX_OUT OMX_UUIDTYPE    *pComponentUUID)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    OMX_U32                compUUID[3];

    IM_OSAL_Entering();

    /* check parameters */
    if (hComponent     == NULL ||
        pComponentName == NULL || pComponentVersion == NULL ||
        pSpecVersion   == NULL || pComponentUUID    == NULL) {
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

    IM_OSAL_Strcpy(pComponentName, pIMComponent->componentName);
    IM_OSAL_Memcpy(pComponentVersion, &(pIMComponent->componentVersion), sizeof(OMX_VERSIONTYPE));
    IM_OSAL_Memcpy(pSpecVersion, &(pIMComponent->specVersion), sizeof(OMX_VERSIONTYPE));

    /* Fill UUID with handle address, PID and UID.
     * This should guarantee uiniqness */
    compUUID[0] = (OMX_U32)pOMXComponent;
    compUUID[1] = getpid();
    compUUID[2] = getuid();
    IM_OSAL_Memcpy(*pComponentUUID, compUUID, 3 * sizeof(*compUUID));

    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_GetState (
    OMX_IN OMX_HANDLETYPE  hComponent,
    OMX_OUT OMX_STATETYPE *pState)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL || pState == NULL) {
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

    *pState = pIMComponent->currentState;
    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

static OMX_ERRORTYPE IM_OMX_BufferProcessThread(OMX_PTR threadData)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_MESSAGE       *message = NULL;

    IM_OSAL_Entering();

    if (threadData == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)threadData;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    pIMComponent->IM_BufferProcess(pOMXComponent);

    IM_OSAL_ThreadExit(NULL);

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_ComponentStateSet(OMX_COMPONENTTYPE *pOMXComponent, OMX_U32 messageParam)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_MESSAGE       *message;
    OMX_STATETYPE          destState = messageParam;
    OMX_STATETYPE          currentState ;//= pIMComponent->currentState;
    IM_OMX_BASEPORT      *pIMPort = NULL;
    OMX_S32                countValue = 0;
    int                   i = 0, j = 0;

    IM_OSAL_Entering();

    IM_OSAL_MutexLock(pIMComponent->compMutex);
    currentState = pIMComponent->currentState;

    /* check parameters */
    if (currentState == destState) {
         ret = OMX_ErrorSameState;
            goto EXIT;
    }
    if (currentState == OMX_StateInvalid) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    if ((currentState == OMX_StateLoaded) && (destState == OMX_StateIdle)) {
        ret = IM_OMX_Get_Resource(pOMXComponent);
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }
    }
    if (((currentState == OMX_StateIdle) && (destState == OMX_StateLoaded))       ||
        ((currentState == OMX_StateIdle) && (destState == OMX_StateInvalid))      ||
        ((currentState == OMX_StateExecuting) && (destState == OMX_StateInvalid)) ||
        ((currentState == OMX_StatePause) && (destState == OMX_StateInvalid))) {
        IM_OMX_Release_Resource(pOMXComponent);
    }

    IM_OSAL_Info("currentState:%d ---->destState: %d", currentState, destState);


    switch (destState) {
    case OMX_StateInvalid:
        switch (currentState) {
        case OMX_StateIdle:
        case OMX_StateExecuting:
        case OMX_StatePause:
        case OMX_StateLoaded:
        case OMX_StateWaitForResources:
            IM_OSAL_MutexUnlock(pIMComponent->compMutex);
            if (pIMComponent->hBufferProcess) {
                pIMComponent->bExitBufferProcessThread = OMX_TRUE;

                for (i = 0; i < ALL_PORT_NUM; i++) {
                    IM_OSAL_Get_SemaphoreCount(pIMComponent->pIMPort[i].bufferSemID, &countValue);
                    if (countValue == 0)
                        IM_OSAL_SemaphorePost(pIMComponent->pIMPort[i].bufferSemID);
                }

                IM_OSAL_SignalSet(pIMComponent->pauseEvent);
                IM_OSAL_ThreadTerminate(pIMComponent->hBufferProcess);
                pIMComponent->hBufferProcess = NULL;

                for (i = 0; i < ALL_PORT_NUM; i++) {
                    IM_OSAL_MutexTerminate(pIMComponent->IMDataBuffer[i].bufferMutex);
                    pIMComponent->IMDataBuffer[i].bufferMutex = NULL;
                }

                IM_OSAL_SignalTerminate(pIMComponent->pauseEvent);
                for (i = 0; i < ALL_PORT_NUM; i++) {
                    IM_OSAL_SemaphoreTerminate(pIMComponent->pIMPort[i].bufferSemID);
                    pIMComponent->pIMPort[i].bufferSemID = NULL;
                }
            }
            if (pIMComponent->IM_componentTerminate != NULL)
                pIMComponent->IM_componentTerminate(pOMXComponent);

            IM_OSAL_MutexLock(pIMComponent->compMutex);
            pIMComponent->targetState = destState;
            pIMComponent->currentState = OMX_StateInvalid;

            break;
        }
        ret = OMX_ErrorInvalidState;
        break;
    case OMX_StateLoaded:
        switch (currentState) {
        case OMX_StateIdle:
            pIMComponent->bExitBufferProcessThread = OMX_TRUE;
            IM_OSAL_MutexUnlock(pIMComponent->compMutex);

            for (i = 0; i < ALL_PORT_NUM; i++) {
                IM_OSAL_Get_SemaphoreCount(pIMComponent->pIMPort[i].bufferSemID, &countValue);
                if (countValue == 0)
                    IM_OSAL_SemaphorePost(pIMComponent->pIMPort[i].bufferSemID);
            }

            IM_OSAL_SignalSet(pIMComponent->pauseEvent);
            IM_OSAL_ThreadTerminate(pIMComponent->hBufferProcess);
            pIMComponent->hBufferProcess = NULL;

            for (i = 0; i < ALL_PORT_NUM; i++) {
                IM_OSAL_MutexTerminate(pIMComponent->IMDataBuffer[i].bufferMutex);
                pIMComponent->IMDataBuffer[i].bufferMutex = NULL;
            }

            IM_OSAL_SignalTerminate(pIMComponent->pauseEvent);
            for (i = 0; i < ALL_PORT_NUM; i++) {
                IM_OSAL_SemaphoreTerminate(pIMComponent->pIMPort[i].bufferSemID);
                pIMComponent->pIMPort[i].bufferSemID = NULL;
            }

            pIMComponent->IM_componentTerminate(pOMXComponent);

            IM_OSAL_MutexLock(pIMComponent->compMutex);
            pIMComponent->targetState = destState;
            break;
        case OMX_StateWaitForResources:
            ret = IM_OMX_Out_WaitForResource(pOMXComponent);
            pIMComponent->targetState = destState;
            pIMComponent->currentState = destState;
            break;
        case OMX_StateExecuting:
        case OMX_StatePause:
        default:
            ret = OMX_ErrorIncorrectStateTransition;
            break;
        }
        break;
    case OMX_StateIdle:
        switch (currentState) {
        case OMX_StateLoaded:
            pIMComponent->targetState = destState;
            break;
        case OMX_StateExecuting:
        case OMX_StatePause:
			IM_OSAL_MutexUnlock(pIMComponent->compMutex);
            IM_OMX_BufferFlushProcessNoEvent(pOMXComponent, ALL_PORT_INDEX);
			IM_OSAL_MutexLock(pIMComponent->compMutex);
            pIMComponent->targetState = destState;
            pIMComponent->currentState = OMX_StateIdle;
            break;
        case OMX_StateWaitForResources:
            pIMComponent->targetState = destState;
            pIMComponent->currentState = OMX_StateIdle;
            break;
        }
        break;
    case OMX_StateExecuting:
        switch (currentState) {
        case OMX_StateLoaded:
            ret = OMX_ErrorIncorrectStateTransition;
            break;
        case OMX_StateIdle:
            for (i = 0; i < pIMComponent->portParam.nPorts; i++) {
                pIMPort = &pIMComponent->pIMPort[i];
                if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort) && CHECK_PORT_ENABLED(pIMPort)) {
                    for (j = 0; j < pIMPort->tunnelBufferNum; j++) {
                        IM_OSAL_SemaphorePost(pIMComponent->pIMPort[i].bufferSemID);
                    }
                }
            }

            pIMComponent->transientState = IM_OMX_TransStateMax;
            pIMComponent->targetState = destState;
            pIMComponent->currentState = OMX_StateExecuting;
            IM_OSAL_SignalSet(pIMComponent->pauseEvent);
            break;
        case OMX_StatePause:
            for (i = 0; i < pIMComponent->portParam.nPorts; i++) {
                pIMPort = &pIMComponent->pIMPort[i];
                if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort) && CHECK_PORT_ENABLED(pIMPort)) {
                    OMX_U32 semaValue = 0, cnt = 0;
                    IM_OSAL_Get_SemaphoreCount(pIMComponent->pIMPort[i].bufferSemID, &semaValue);
                    if (IM_OSAL_GetElemNum(&pIMPort->bufferQ) > semaValue) {
                        cnt = IM_OSAL_GetElemNum(&pIMPort->bufferQ) - semaValue;
                        for (j = 0; j < cnt; j++) {
                            IM_OSAL_SemaphorePost(pIMComponent->pIMPort[i].bufferSemID);
                        }
                    }
                }
            }

            pIMComponent->targetState = destState;
            pIMComponent->currentState = OMX_StateExecuting;
            IM_OSAL_SignalSet(pIMComponent->pauseEvent);
            break;
        case OMX_StateWaitForResources:
            ret = OMX_ErrorIncorrectStateTransition;
            break;
        }
        break;
    case OMX_StatePause:
        switch (currentState) {
        case OMX_StateLoaded:
            ret = OMX_ErrorIncorrectStateTransition;
            break;
        case OMX_StateIdle:
            pIMComponent->targetState = destState;
            pIMComponent->currentState = OMX_StatePause;
            break;
        case OMX_StateExecuting:
            pIMComponent->targetState = destState;
            pIMComponent->currentState = OMX_StatePause;
            break;
        case OMX_StateWaitForResources:
            ret = OMX_ErrorIncorrectStateTransition;
            break;
        }
        break;
    case OMX_StateWaitForResources:
        switch (currentState) {
        case OMX_StateLoaded:
            ret = IM_OMX_In_WaitForResource(pOMXComponent);
            pIMComponent->targetState = destState;
            pIMComponent->currentState = OMX_StateWaitForResources;
            break;
        case OMX_StateIdle:
        case OMX_StateExecuting:
        case OMX_StatePause:
            ret = OMX_ErrorIncorrectStateTransition;
            break;
        }
        break;
    }

EXIT:
    if (ret == OMX_ErrorNone) {
        if ( pIMComponent->currentState == destState && pIMComponent->pCallbacks != NULL) {
            IM_OSAL_Info("%d  ---> %d OK", currentState, destState);
            pIMComponent->pCallbacks->EventHandler((OMX_HANDLETYPE)pOMXComponent,
            pIMComponent->callbackData,
            OMX_EventCmdComplete, OMX_CommandStateSet,
            destState, NULL);
        }

		IM_OSAL_MutexUnlock(pIMComponent->compMutex);
        IM_checkTransitions(pOMXComponent);
    } else {
        if (pIMComponent->pCallbacks != NULL) {
            pIMComponent->pCallbacks->EventHandler((OMX_HANDLETYPE)pOMXComponent,
            pIMComponent->callbackData,
            OMX_EventError, ret, 0, NULL);
        }
	    IM_OSAL_MutexUnlock(pIMComponent->compMutex);
    }
    IM_OSAL_Exiting();

    return ret;
}


OMX_ERRORTYPE IM_checkTransitions(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE         ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    OMX_STATETYPE         destState = OMX_StateMax;pIMComponent->targetState;
    OMX_STATETYPE         currentState = OMX_StateMax;pIMComponent->currentState;
    IM_OMX_MESSAGE       *message;
    IM_OMX_BASEPORT      *pIMPort = NULL;
 
    OMX_S32               countValue = 0;
    OMX_S32               i = 0;

    IM_OSAL_Entering();
	IM_OSAL_MutexLock(pIMComponent->compMutex);
    destState = pIMComponent->targetState;
    currentState = pIMComponent->currentState;
    IM_OSAL_Info("destState:%d, currentState:%d", destState, currentState);
    if(destState != currentState) {
        OMX_BOOL transitionComplete = OMX_TRUE;
        if(currentState == OMX_StateLoaded)  {
            //assert(destState == OMX_StateIdle);    
            for (i = 0; i < pIMComponent->portParam.nPorts; i++) {
                pIMPort = (pIMComponent->pIMPort + i);
                if (pIMPort == NULL) {
                    ret = OMX_ErrorBadParameter;
                    goto EXIT;
                }
                if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                    if (CHECK_PORT_ENABLED(pIMPort)) {
                        ret = pIMComponent->IM_AllocateTunnelBuffer(pIMPort, i);
                        if (ret != OMX_ErrorNone)
                            goto EXIT;
                    }
                } else {
                    if (pIMPort->portDefinition.bPopulated != OMX_TRUE) {
                        transitionComplete = OMX_FALSE;
                    }
                }
            }

            if(transitionComplete == OMX_TRUE) { 
                ret = pIMComponent->IM_componentInit(pOMXComponent);
                if (ret != OMX_ErrorNone) {
                    /*
                     * if (CHECK_PORT_TUNNELED == OMX_TRUE) thenTunnel Buffer Free
                     */
                    goto EXIT;
                }
                pIMComponent->bExitBufferProcessThread = OMX_FALSE;
                IM_OSAL_SignalCreate(&pIMComponent->pauseEvent);
                for (i = 0; i < ALL_PORT_NUM; i++) {
                    IM_OSAL_SemaphoreCreate(&pIMComponent->pIMPort[i].bufferSemID);
                }
                for (i = 0; i < ALL_PORT_NUM; i++) {
                    IM_OSAL_MutexCreate(&pIMComponent->IMDataBuffer[i].bufferMutex);
                }
                ret = IM_OSAL_ThreadCreate(&pIMComponent->hBufferProcess,
                        IM_OMX_BufferProcessThread,
                        pOMXComponent);
                if (ret != OMX_ErrorNone) {
                    /*
                     * if (CHECK_PORT_TUNNELED == OMX_TRUE) thenTunnel Buffer Free
                     */
                    IM_OSAL_SignalTerminate(pIMComponent->pauseEvent);
                    for (i = 0; i < ALL_PORT_NUM; i++) {
                        IM_OSAL_MutexTerminate(pIMComponent->IMDataBuffer[i].bufferMutex);
                        pIMComponent->IMDataBuffer[i].bufferMutex = NULL;
                    }
                    for (i = 0; i < ALL_PORT_NUM; i++) {
                        IM_OSAL_SemaphoreTerminate(pIMComponent->pIMPort[i].bufferSemID);
                        pIMComponent->pIMPort[i].bufferSemID = NULL;
                    }

                    ret = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
            }
        } else if (currentState == OMX_StateIdle) {
            for (i = 0; i < pIMComponent->portParam.nPorts; i++) {
                pIMPort = (pIMComponent->pIMPort + i);
                if(pIMPort == NULL) {
                    IM_OSAL_Error("port is destroyed");
                    continue;
                }
                if (CHECK_PORT_TUNNELED(pIMPort) && CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                    while (IM_OSAL_GetElemNum(&pIMPort->bufferQ) > 0) {
                        message = (IM_OMX_MESSAGE*)IM_OSAL_Dequeue(&pIMPort->bufferQ);
                        if (message != NULL)
                            IM_OSAL_Free(message);
                    }
                    ret = pIMComponent->IM_FreeTunnelBuffer(pIMPort, i);
                    if (OMX_ErrorNone != ret) {
                        goto EXIT;
                    }
                } else {
                    if (pIMPort->assignedBufferNum != 0) {
                        transitionComplete = OMX_FALSE;
                    }
                }
            }

        }
        if(transitionComplete == OMX_TRUE) {
            pIMComponent->currentState = destState;
                IM_OSAL_Info("state %d-->%d OK", currentState, destState);
            if (pIMComponent->pCallbacks != NULL) {
                pIMComponent->pCallbacks->EventHandler((OMX_HANDLETYPE)pOMXComponent,
                        pIMComponent->callbackData,
                        OMX_EventCmdComplete, OMX_CommandStateSet,
                        destState, NULL);
            }

        }
    }

#if 1
    for (i = 0; i < pIMComponent->portParam.nPorts; i++) {
        pIMPort = (pIMComponent->pIMPort + i);

        if(pIMPort == NULL) {
            IM_OSAL_Error("port is destroyed");
            continue;
        }
        IM_OSAL_Info("Port[%d]: transitions:%d, bPopulated:%d", i, pIMPort->transitions, pIMPort->portDefinition.bPopulated);
        if (pIMPort->transitions == PORT_DISABLING) {
            if(pIMPort->portDefinition.bPopulated == OMX_FALSE)  {
                pIMPort->transitions = PORT_NONE;
                pIMPort->portDefinition.bEnabled = OMX_FALSE;
                IM_OSAL_Info("port disable complete");
                pIMComponent->pCallbacks->EventHandler(pOMXComponent,
                        pIMComponent->callbackData,
                        OMX_EventCmdComplete,
                        OMX_CommandPortDisable, i, NULL);

            }
        } else if (pIMPort->transitions == PORT_ENABLING) {
            if(pIMPort->portDefinition.bPopulated == OMX_TRUE)  {
                pIMPort->transitions = PORT_NONE;
                pIMPort->portDefinition.bEnabled = OMX_TRUE;
                IM_OSAL_Info("port enable complete");
                pIMComponent->pCallbacks->EventHandler(pOMXComponent,
                        pIMComponent->callbackData,
                        OMX_EventCmdComplete,
                        OMX_CommandPortEnable, i, NULL);
            }
        }
    }
#endif 


EXIT:
    IM_OSAL_MutexUnlock(pIMComponent->compMutex);
    if (ret != OMX_ErrorNone) {
        if (pIMComponent->pCallbacks != NULL) {
            pIMComponent->pCallbacks->EventHandler((OMX_HANDLETYPE)pOMXComponent,
                    pIMComponent->callbackData,
                    OMX_EventError, ret, 0, NULL);
        }
    }
    IM_OSAL_Exiting();

    return ret;

}


static OMX_ERRORTYPE IM_OMX_MessageHandlerThread(OMX_PTR threadData)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_MESSAGE       *message = NULL;
    OMX_U32                messageType = 0, portIndex = 0;

    IM_OSAL_Entering();

    if (threadData == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pOMXComponent = (OMX_COMPONENTTYPE *)threadData;
    ret = IM_OMX_Check_SizeVersion(pOMXComponent, sizeof(OMX_COMPONENTTYPE));
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    while (pIMComponent->bExitMessageHandlerThread != OMX_TRUE) {
        IM_OSAL_SemaphoreWait(pIMComponent->msgSemaphoreHandle);
        message = (IM_OMX_MESSAGE *)IM_OSAL_Dequeue(&pIMComponent->messageQ);
        if (message != NULL) {
            messageType = message->messageType;
            switch (messageType) {
                case OMX_CommandStateSet:
                    ret = IM_OMX_ComponentStateSet(pOMXComponent, message->messageParam);
                    break;
                case OMX_CommandFlush:
                    ret = IM_OMX_BufferFlushProcess(pOMXComponent, message->messageParam);
                    break;
                case OMX_CommandPortDisable:
                    ret = IM_OMX_PortDisableProcess(pOMXComponent, message->messageParam);
                    break;
                case OMX_CommandPortEnable:
                    ret = IM_OMX_PortEnableProcess(pOMXComponent, message->messageParam);
                    break;
                case OMX_CommandMarkBuffer:
                    portIndex = message->messageParam;
                    pIMComponent->pIMPort[portIndex].markType.hMarkTargetComponent = ((OMX_MARKTYPE *)message->pCmdData)->hMarkTargetComponent;
                    pIMComponent->pIMPort[portIndex].markType.pMarkData            = ((OMX_MARKTYPE *)message->pCmdData)->pMarkData;
                    break;
                case (OMX_COMMANDTYPE)IM_OMX_CommandComponentDeInit:
                    pIMComponent->bExitMessageHandlerThread = OMX_TRUE;
                    break;
                case (OMX_COMMANDTYPE)IM_OMX_CommandCheckTransitions:
                    IM_checkTransitions(pOMXComponent);
                    break;
                default:
                    break;
            }
            IM_OSAL_Free(message);
            message = NULL;
        }
    }

    IM_OSAL_ThreadExit(NULL);

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

static OMX_ERRORTYPE IM_StateSet(IM_OMX_BASECOMPONENT *pIMComponent, OMX_U32 nParam)
{
    OMX_U32 destState = nParam;
    OMX_U32 i = 0;

    IM_OSAL_MutexLock(pIMComponent->compMutex);
    if ((destState == OMX_StateIdle) && (pIMComponent->currentState == OMX_StateLoaded)) {
        pIMComponent->transientState = IM_OMX_TransStateLoadedToIdle;
        for(i = 0; i < pIMComponent->portParam.nPorts; i++) {
            pIMComponent->pIMPort[i].portState = OMX_StateIdle;
        }
        IM_OSAL_Info("to OMX_StateIdle");
    } else if ((destState == OMX_StateLoaded) && (pIMComponent->currentState == OMX_StateIdle)) {
        pIMComponent->transientState = IM_OMX_TransStateIdleToLoaded;
        for (i = 0; i < pIMComponent->portParam.nPorts; i++) {
            pIMComponent->pIMPort[i].portState = OMX_StateLoaded;
        }
        IM_OSAL_Info("to OMX_StateLoaded");
    } else if ((destState == OMX_StateIdle) && (pIMComponent->currentState == OMX_StateExecuting)) {
        pIMComponent->transientState = IM_OMX_TransStateExecutingToIdle;
        IM_OSAL_Info("to OMX_StateIdle");
    } else if ((destState == OMX_StateExecuting) && (pIMComponent->currentState == OMX_StateIdle)) {
        pIMComponent->transientState = IM_OMX_TransStateIdleToExecuting;
        IM_OSAL_Info("to OMX_StateExecuting");
    } else if (destState == OMX_StateInvalid) {
        for (i = 0; i < pIMComponent->portParam.nPorts; i++) {
            pIMComponent->pIMPort[i].portState = OMX_StateInvalid;
        }
    }
    IM_OSAL_MutexUnlock(pIMComponent->compMutex);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE IM_SetPortFlush(IM_OMX_BASECOMPONENT *pIMComponent, OMX_U32 nParam)
{
    OMX_ERRORTYPE     ret = OMX_ErrorNone;
    IM_OMX_BASEPORT *pIMPort = NULL;
    OMX_U32           portIndex = nParam;
    OMX_U16           i = 0, cnt = 0, index = 0;


    if ((pIMComponent->currentState == OMX_StateExecuting) ||
        (pIMComponent->currentState == OMX_StatePause)) {
        if ((portIndex != ALL_PORT_INDEX) &&
           ((OMX_S32)portIndex >= (OMX_S32)pIMComponent->portParam.nPorts)) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }
        /*********************
        *    need flush event set ?????
        **********************/
        cnt = (portIndex == ALL_PORT_INDEX ) ? ALL_PORT_NUM : 1;
        for (i = 0; i < cnt; i++) {
            if (portIndex == ALL_PORT_INDEX)
                index = i;
            else
                index = portIndex;
			IM_OSAL_MutexLock(pIMComponent->pIMPort[index].portMutex);
            pIMComponent->pIMPort[index].bIsPortFlushed = OMX_TRUE;
			IM_OSAL_MutexUnlock(pIMComponent->pIMPort[index].portMutex);
        }
    } else {
        ret = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }
    ret = OMX_ErrorNone;

EXIT:
    return ret;
}

OMX_ERRORTYPE IM_SetPortEnable(IM_OMX_BASECOMPONENT *pIMComponent, OMX_U32 nParam)
{
    OMX_ERRORTYPE     ret = OMX_ErrorNone;
    IM_OMX_BASEPORT *pIMPort = NULL;
    OMX_U32           portIndex = nParam;
    OMX_U16           i = 0, cnt = 0;

    IM_OSAL_Entering();

    if ((portIndex != ALL_PORT_INDEX) &&
        ((OMX_S32)portIndex >= (OMX_S32)pIMComponent->portParam.nPorts)) {
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

    if (portIndex == ALL_PORT_INDEX) {
        for (i = 0; i < pIMComponent->portParam.nPorts; i++) {
            pIMPort = &pIMComponent->pIMPort[i];

            if (CHECK_PORT_ENABLED(pIMPort)) {
                ret = OMX_ErrorIncorrectStateOperation;
                goto EXIT;
            } else {
                pIMPort->portState = OMX_StateIdle;
            }
        }
    } else {
        pIMPort = &pIMComponent->pIMPort[portIndex];
        if (CHECK_PORT_ENABLED(pIMPort)) {
            ret = OMX_ErrorIncorrectStateOperation;
            goto EXIT;
        } else {
            pIMPort->portState = OMX_StateIdle;
        }
    }
    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;

}

OMX_ERRORTYPE IM_SetPortDisable(IM_OMX_BASECOMPONENT *pIMComponent, OMX_U32 nParam)
{
    OMX_ERRORTYPE     ret = OMX_ErrorNone;
    IM_OMX_BASEPORT *pIMPort = NULL;
    OMX_U32           portIndex = nParam;
    OMX_U16           i = 0, cnt = 0;

    IM_OSAL_Entering();

    if ((portIndex != ALL_PORT_INDEX) &&
        ((OMX_S32)portIndex >= (OMX_S32)pIMComponent->portParam.nPorts)) {
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

    if (portIndex == ALL_PORT_INDEX) {
        for (i = 0; i < pIMComponent->portParam.nPorts; i++) {
            pIMPort = &pIMComponent->pIMPort[i];
            if (!CHECK_PORT_ENABLED(pIMPort)) {
                ret = OMX_ErrorIncorrectStateOperation;
                goto EXIT;
            }
            pIMPort->portState = OMX_StateLoaded;
            pIMPort->bIsPortDisabled = OMX_TRUE;
        }
    } else {
        pIMPort = &pIMComponent->pIMPort[portIndex];
        pIMPort->portState = OMX_StateLoaded;
        pIMPort->bIsPortDisabled = OMX_TRUE;
    }
    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_SetMarkBuffer(IM_OMX_BASECOMPONENT *pIMComponent, OMX_U32 nParam)
{
    OMX_ERRORTYPE     ret = OMX_ErrorNone;
    IM_OMX_BASEPORT *pIMPort = NULL;
    OMX_U32           portIndex = nParam;
    OMX_U16           i = 0, cnt = 0;


    if (nParam >= pIMComponent->portParam.nPorts) {
        ret = OMX_ErrorBadPortIndex;
        goto EXIT;
    }

    if ((pIMComponent->currentState == OMX_StateExecuting) ||
        (pIMComponent->currentState == OMX_StatePause)) {
        ret = OMX_ErrorNone;
    } else {
        ret = OMX_ErrorIncorrectStateOperation;
    }

EXIT:
    return ret;
}

 OMX_ERRORTYPE IM_OMX_CommandQueue(
    IM_OMX_BASECOMPONENT *pIMComponent,
    OMX_COMMANDTYPE        Cmd,
    OMX_U32                nParam,
    OMX_PTR                pCmdData)
{
    OMX_ERRORTYPE    ret = OMX_ErrorNone;
    IM_OMX_MESSAGE *command = (IM_OMX_MESSAGE *)IM_OSAL_Malloc(sizeof(IM_OMX_MESSAGE));

    if (command == NULL) {
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    command->messageType  = (OMX_U32)Cmd;
    command->messageParam = nParam;
    command->pCmdData     = pCmdData;

    ret = IM_OSAL_Queue(&pIMComponent->messageQ, (void *)command);
    if (ret != 0) {
        ret = OMX_ErrorUndefined;
        goto EXIT;
    }
    ret = IM_OSAL_SemaphorePost(pIMComponent->msgSemaphoreHandle);

EXIT:
    return ret;
}

OMX_ERRORTYPE IM_OMX_SendCommand(
    OMX_IN OMX_HANDLETYPE  hComponent,
    OMX_IN OMX_COMMANDTYPE Cmd,
    OMX_IN OMX_U32         nParam,
    OMX_IN OMX_PTR         pCmdData)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_MESSAGE       *message = NULL;

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
#if 1
    switch (Cmd) {
    case OMX_CommandStateSet :
        IM_OSAL_Info("Command: OMX_CommandStateSet");
        IM_StateSet(pIMComponent, nParam);
        break;
    case OMX_CommandFlush :
        IM_OSAL_Info("Command: OMX_CommandFlush");
        ret = IM_SetPortFlush(pIMComponent, nParam);
        if (ret != OMX_ErrorNone)
            goto EXIT;
        break;
    case OMX_CommandPortDisable :
        IM_OSAL_Info("Command: OMX_CommandPortDisable");
        ret = IM_SetPortDisable(pIMComponent, nParam);
        if (ret != OMX_ErrorNone)
            goto EXIT;
        break;
    case OMX_CommandPortEnable :
        IM_OSAL_Info("Command: OMX_CommandPortEnable");
        ret = IM_SetPortEnable(pIMComponent, nParam);
        if (ret != OMX_ErrorNone)
            goto EXIT;
        break;
    case OMX_CommandMarkBuffer :
        IM_OSAL_Info("Command: OMX_CommandMarkBuffer");
        ret = IM_SetMarkBuffer(pIMComponent, nParam);
        if (ret != OMX_ErrorNone)
            goto EXIT;
        break;
/*
    case IM_CommandFillBuffer :
    case IM_CommandEmptyBuffer :
    case IM_CommandDeInit :
*/
    default:
        break;
    }
#endif 
    ret = IM_OMX_CommandQueue(pIMComponent, Cmd, nParam, pCmdData);

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_GetParameter(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nParamIndex,
    OMX_INOUT OMX_PTR     ComponentParameterStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
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

    if (ComponentParameterStructure == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (pIMComponent->currentState == OMX_StateInvalid) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    switch (nParamIndex) {
    case (OMX_INDEXTYPE)OMX_COMPONENT_CAPABILITY_TYPE_INDEX:
    {
        /* For Android PV OpenCORE */
        OMXComponentCapabilityFlagsType *capabilityFlags = (OMXComponentCapabilityFlagsType *)ComponentParameterStructure;
        IM_OSAL_Memcpy(capabilityFlags, &pIMComponent->capabilityFlags, sizeof(OMXComponentCapabilityFlagsType));
    }
        break;
    case OMX_IndexParamAudioInit:
    case OMX_IndexParamVideoInit:
    case OMX_IndexParamImageInit:
    case OMX_IndexParamOtherInit:
    {
        OMX_PORT_PARAM_TYPE *portParam = (OMX_PORT_PARAM_TYPE *)ComponentParameterStructure;
        ret = IM_OMX_Check_SizeVersion(portParam, sizeof(OMX_PORT_PARAM_TYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }
        portParam->nPorts         = 0;
        portParam->nStartPortNumber     = 0;
    }
        break;
    case OMX_IndexParamPortDefinition:
    {
        OMX_PARAM_PORTDEFINITIONTYPE *portDefinition = (OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure;
        OMX_U32                       portIndex = portDefinition->nPortIndex;
        IM_OMX_BASEPORT             *pIMPort;

        if (portIndex >= pIMComponent->portParam.nPorts) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }
        ret = IM_OMX_Check_SizeVersion(portDefinition, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        pIMPort = &pIMComponent->pIMPort[portIndex];
		IM_OSAL_Info("port %d def colorformat = %08x",portIndex,pIMPort->portDefinition.format.video.eColorFormat);
        IM_OSAL_Memcpy(portDefinition, &pIMPort->portDefinition, portDefinition->nSize);
    }
        break;
    case OMX_IndexParamPriorityMgmt:
    {
        OMX_PRIORITYMGMTTYPE *compPriority = (OMX_PRIORITYMGMTTYPE *)ComponentParameterStructure;

        ret = IM_OMX_Check_SizeVersion(compPriority, sizeof(OMX_PRIORITYMGMTTYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        compPriority->nGroupID       = pIMComponent->compPriority.nGroupID;
        compPriority->nGroupPriority = pIMComponent->compPriority.nGroupPriority;
    }
        break;

    case OMX_IndexParamCompBufferSupplier:
    {
        OMX_PARAM_BUFFERSUPPLIERTYPE *bufferSupplier = (OMX_PARAM_BUFFERSUPPLIERTYPE *)ComponentParameterStructure;
        OMX_U32                       portIndex = bufferSupplier->nPortIndex;
        IM_OMX_BASEPORT             *pIMPort;

        if ((pIMComponent->currentState == OMX_StateLoaded) ||
            (pIMComponent->currentState == OMX_StateWaitForResources)) {
            if (portIndex >= pIMComponent->portParam.nPorts) {
                ret = OMX_ErrorBadPortIndex;
                goto EXIT;
            }
            ret = IM_OMX_Check_SizeVersion(bufferSupplier, sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE));
            if (ret != OMX_ErrorNone) {
                goto EXIT;
            }

            pIMPort = &pIMComponent->pIMPort[portIndex];


            if (pIMPort->portDefinition.eDir == OMX_DirInput) {
                if (CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                    bufferSupplier->eBufferSupplier = OMX_BufferSupplyInput;
                } else if (CHECK_PORT_TUNNELED(pIMPort)) {
                    bufferSupplier->eBufferSupplier = OMX_BufferSupplyOutput;
                } else {
                    bufferSupplier->eBufferSupplier = OMX_BufferSupplyUnspecified;
                }
            } else {
                if (CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                    bufferSupplier->eBufferSupplier = OMX_BufferSupplyOutput;
                } else if (CHECK_PORT_TUNNELED(pIMPort)) {
                    bufferSupplier->eBufferSupplier = OMX_BufferSupplyInput;
                } else {
                    bufferSupplier->eBufferSupplier = OMX_BufferSupplyUnspecified;
                }
            }
        }
        else
        {
            ret = OMX_ErrorIncorrectStateOperation;
            goto EXIT;
        }
    }
        break;
    default:
    {
        ret = OMX_ErrorUnsupportedIndex;
        goto EXIT;
    }
        break;
    }

    ret = OMX_ErrorNone;

EXIT:

    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_SetParameter(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nIndex,
    OMX_IN OMX_PTR        ComponentParameterStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
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

    if (ComponentParameterStructure == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (pIMComponent->currentState == OMX_StateInvalid) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    switch (nIndex) {
    case OMX_IndexParamAudioInit:
    case OMX_IndexParamVideoInit:
    case OMX_IndexParamImageInit:
    case OMX_IndexParamOtherInit:
    {
        OMX_PORT_PARAM_TYPE *portParam = (OMX_PORT_PARAM_TYPE *)ComponentParameterStructure;
        ret = IM_OMX_Check_SizeVersion(portParam, sizeof(OMX_PORT_PARAM_TYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        if ((pIMComponent->currentState != OMX_StateLoaded) &&
            (pIMComponent->currentState != OMX_StateWaitForResources)) {
            ret = OMX_ErrorIncorrectStateOperation;
            goto EXIT;
        }
        ret = OMX_ErrorUndefined;
        /* IM_OSAL_Memcpy(&pIMComponent->portParam, portParam, sizeof(OMX_PORT_PARAM_TYPE)); */
    }
        break;
    case OMX_IndexParamPortDefinition:
    {
        OMX_PARAM_PORTDEFINITIONTYPE *portDefinition = (OMX_PARAM_PORTDEFINITIONTYPE *)ComponentParameterStructure;
        OMX_U32               portIndex = portDefinition->nPortIndex;
        IM_OMX_BASEPORT         *pIMPort;

        if (portIndex >= pIMComponent->portParam.nPorts) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }
        ret = IM_OMX_Check_SizeVersion(portDefinition, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        pIMPort = &pIMComponent->pIMPort[portIndex];

        if ((pIMComponent->currentState != OMX_StateLoaded) && (pIMComponent->currentState != OMX_StateWaitForResources)) {
            if (pIMPort->portDefinition.bEnabled == OMX_TRUE) {
                ret = OMX_ErrorIncorrectStateOperation;
                goto EXIT;
            }
        }
        if (portDefinition->nBufferCountActual < pIMPort->portDefinition.nBufferCountMin) {
            ret = OMX_ErrorBadParameter;
            goto EXIT;
        }

        IM_OSAL_Memcpy(&pIMPort->portDefinition, portDefinition, portDefinition->nSize);
    }
        break;
    case OMX_IndexParamPriorityMgmt:
    {
        OMX_PRIORITYMGMTTYPE *compPriority = (OMX_PRIORITYMGMTTYPE *)ComponentParameterStructure;

        if ((pIMComponent->currentState != OMX_StateLoaded) &&
            (pIMComponent->currentState != OMX_StateWaitForResources)) {
            ret = OMX_ErrorIncorrectStateOperation;
            goto EXIT;
        }

        ret = IM_OMX_Check_SizeVersion(compPriority, sizeof(OMX_PRIORITYMGMTTYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        pIMComponent->compPriority.nGroupID = compPriority->nGroupID;
        pIMComponent->compPriority.nGroupPriority = compPriority->nGroupPriority;
    }
        break;
    case OMX_IndexParamCompBufferSupplier:
    {
        OMX_PARAM_BUFFERSUPPLIERTYPE *bufferSupplier = (OMX_PARAM_BUFFERSUPPLIERTYPE *)ComponentParameterStructure;
        OMX_U32               portIndex = bufferSupplier->nPortIndex;
        IM_OMX_BASEPORT      *pIMPort = &pIMComponent->pIMPort[portIndex];

        if ((pIMComponent->currentState != OMX_StateLoaded) && (pIMComponent->currentState != OMX_StateWaitForResources)) {
            if (pIMPort->portDefinition.bEnabled == OMX_TRUE) {
                ret = OMX_ErrorIncorrectStateOperation;
                goto EXIT;
            }
        }

        if (portIndex >= pIMComponent->portParam.nPorts) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }
        ret = IM_OMX_Check_SizeVersion(bufferSupplier, sizeof(OMX_PARAM_BUFFERSUPPLIERTYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        if (bufferSupplier->eBufferSupplier == OMX_BufferSupplyUnspecified) {
            ret = OMX_ErrorNone;
            goto EXIT;
        }
        if (CHECK_PORT_TUNNELED(pIMPort) == 0) {
            ret = OMX_ErrorNone; /*OMX_ErrorNone ?????*/
            goto EXIT;
        }

        if (pIMPort->portDefinition.eDir == OMX_DirInput) {
            if (bufferSupplier->eBufferSupplier == OMX_BufferSupplyInput) {
                /*
                if (CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                    ret = OMX_ErrorNone;
                }
                */
                pIMPort->tunnelFlags |= IM_TUNNEL_IS_SUPPLIER;
                bufferSupplier->nPortIndex = pIMPort->tunneledPort;
                ret = OMX_SetParameter(pIMPort->tunneledComponent, OMX_IndexParamCompBufferSupplier, bufferSupplier);
                goto EXIT;
            } else if (bufferSupplier->eBufferSupplier == OMX_BufferSupplyOutput) {
                ret = OMX_ErrorNone;
                if (CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                    pIMPort->tunnelFlags &= ~IM_TUNNEL_IS_SUPPLIER;
                    bufferSupplier->nPortIndex = pIMPort->tunneledPort;
                    ret = OMX_SetParameter(pIMPort->tunneledComponent, OMX_IndexParamCompBufferSupplier, bufferSupplier);
                }
                goto EXIT;
            }
        } else if (pIMPort->portDefinition.eDir == OMX_DirOutput) {
            if (bufferSupplier->eBufferSupplier == OMX_BufferSupplyInput) {
                ret = OMX_ErrorNone;
                if (CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                    pIMPort->tunnelFlags &= ~IM_TUNNEL_IS_SUPPLIER;
                    ret = OMX_ErrorNone;
                }
                goto EXIT;
            } else if (bufferSupplier->eBufferSupplier == OMX_BufferSupplyOutput) {
                /*
                if (CHECK_PORT_BUFFER_SUPPLIER(pIMPort)) {
                    ret = OMX_ErrorNone;
                }
                */
                pIMPort->tunnelFlags |= IM_TUNNEL_IS_SUPPLIER;
                ret = OMX_ErrorNone;
                goto EXIT;
            }
        }
    }
        break;
    default:
    {
        ret = OMX_ErrorUnsupportedIndex;
        goto EXIT;
    }
        break;
    }

    ret = OMX_ErrorNone;

EXIT:

    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_GetConfig(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nIndex,
    OMX_INOUT OMX_PTR     pComponentConfigStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
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
    ret = OMX_ErrorUnsupportedIndex;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_SetConfig(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nIndex,
    OMX_IN OMX_PTR        pComponentConfigStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
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
    ret = OMX_ErrorUnsupportedIndex;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_GetExtensionIndex(
    OMX_IN OMX_HANDLETYPE  hComponent,
    OMX_IN OMX_STRING      cParameterName,
    OMX_OUT OMX_INDEXTYPE *pIndexType)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
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

    ret = OMX_ErrorBadParameter;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_SetCallbacks (
    OMX_IN OMX_HANDLETYPE    hComponent,
    OMX_IN OMX_CALLBACKTYPE* pCallbacks,
    OMX_IN OMX_PTR           pAppData)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
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

    if (pCallbacks == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (pIMComponent->currentState == OMX_StateInvalid) {
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }
    if (pIMComponent->currentState != OMX_StateLoaded) {
        ret = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    pIMComponent->pCallbacks = pCallbacks;
    pIMComponent->callbackData = pAppData;

    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_UseEGLImage(
    OMX_IN OMX_HANDLETYPE            hComponent,
    OMX_INOUT OMX_BUFFERHEADERTYPE **ppBufferHdr,
    OMX_IN OMX_U32                   nPortIndex,
    OMX_IN OMX_PTR                   pAppPrivate,
    OMX_IN void                     *eglImage)
{
    return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE IM_OMX_BaseComponent_Constructor(
    OMX_IN OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        IM_OSAL_Error("OMX_ErrorBadParameter, Line:%d", __LINE__);
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    pIMComponent = IM_OSAL_Malloc(sizeof(IM_OMX_BASECOMPONENT));
    if (pIMComponent == NULL) {
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }

    IM_OSAL_Memset(pIMComponent, 0, sizeof(IM_OMX_BASECOMPONENT));

    pIMComponent->IM_checkTransitions = &IM_checkTransitions;
    pOMXComponent->pComponentPrivate = (OMX_PTR)pIMComponent;

    ret = IM_OSAL_SemaphoreCreate(&pIMComponent->msgSemaphoreHandle);
    if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }
    ret = IM_OSAL_MutexCreate(&pIMComponent->compMutex);
    if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }

    pIMComponent->bExitMessageHandlerThread = OMX_FALSE;
    IM_OSAL_QueueCreate(&pIMComponent->messageQ);
    ret = IM_OSAL_ThreadCreate(&pIMComponent->hMessageHandler, IM_OMX_MessageHandlerThread, pOMXComponent);
    if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }

    pOMXComponent->GetComponentVersion = &IM_OMX_GetComponentVersion;
    pOMXComponent->SendCommand         = &IM_OMX_SendCommand;
    pOMXComponent->GetExtensionIndex   = &IM_OMX_GetExtensionIndex;
    pOMXComponent->GetState            = &IM_OMX_GetState;
    pOMXComponent->SetCallbacks        = &IM_OMX_SetCallbacks;
    pOMXComponent->UseEGLImage         = &IM_OMX_UseEGLImage;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_BaseComponent_Destructor(
    OMX_IN OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    OMX_U32                semaValue = 0;

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

    IM_OMX_CommandQueue(pIMComponent, IM_OMX_CommandComponentDeInit, 0, NULL);
    IM_OSAL_SleepMillisec(0);
    IM_OSAL_Get_SemaphoreCount(pIMComponent->msgSemaphoreHandle, &semaValue);
    if (semaValue == 0)
        IM_OSAL_SemaphorePost(pIMComponent->msgSemaphoreHandle);
    IM_OSAL_SemaphorePost(pIMComponent->msgSemaphoreHandle);

    IM_OSAL_ThreadTerminate(pIMComponent->hMessageHandler);
    pIMComponent->hMessageHandler = NULL;

    //check checkTransition finished
    IM_OSAL_MutexLock(pIMComponent->compMutex);
    IM_OSAL_MutexTerminate(pIMComponent->compMutex);
    pIMComponent->compMutex = NULL;
    IM_OSAL_SemaphoreTerminate(pIMComponent->msgSemaphoreHandle);
    pIMComponent->msgSemaphoreHandle = NULL;
    IM_OSAL_QueueTerminate(&pIMComponent->messageQ);

    IM_OSAL_Free(pIMComponent);
    pIMComponent = NULL;

    ret = OMX_ErrorNone;
EXIT:
    IM_OSAL_Exiting();

    return ret;
}
