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
 * @file       IM_OMX_Resourcemanager.c
 * @brief
 * @author     ayakashi (yanyuan_xu@infotm.com)
 * @version    1.0
 * @history
 *    2010.03.01 : Create
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IM_OMX_Resourcemanager.h"
#include "IM_OMX_Basecomponent.h"

#undef  IM_LOG_TAG
#define IM_LOG_TAG    "IM_RM"
#define IM_LOG_OFF
#include "IM_OSAL_Trace.h"


#define MAX_RESOURCE_VIDEO 4

/* Max allowable video scheduler component instance */
static IM_OMX_RM_COMPONENT_LIST *gpVideoRMComponentList = NULL;
static IM_OMX_RM_COMPONENT_LIST *gpVideoRMWaitingList = NULL;
static OMX_HANDLETYPE ghVideoRMComponentListMutex = NULL;


OMX_ERRORTYPE addElementList(IM_OMX_RM_COMPONENT_LIST **ppList, OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE              ret = OMX_ErrorNone;
    IM_OMX_RM_COMPONENT_LIST *pTempComp = NULL;
    IM_OMX_BASECOMPONENT     *pIMComponent = NULL;

    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    if (*ppList != NULL) {
        pTempComp = *ppList;
        while (pTempComp->pNext != NULL) {
            pTempComp = pTempComp->pNext;
        }
        pTempComp->pNext = (IM_OMX_RM_COMPONENT_LIST *)IM_OSAL_Malloc(sizeof(IM_OMX_RM_COMPONENT_LIST));
        if (pTempComp->pNext == NULL) {
            ret = OMX_ErrorInsufficientResources;
            goto EXIT;
        }
        ((IM_OMX_RM_COMPONENT_LIST *)(pTempComp->pNext))->pNext = NULL;
        ((IM_OMX_RM_COMPONENT_LIST *)(pTempComp->pNext))->pOMXStandComp = pOMXComponent;
        ((IM_OMX_RM_COMPONENT_LIST *)(pTempComp->pNext))->groupPriority = pIMComponent->compPriority.nGroupPriority;
        goto EXIT;
    } else {
        *ppList = (IM_OMX_RM_COMPONENT_LIST *)IM_OSAL_Malloc(sizeof(IM_OMX_RM_COMPONENT_LIST));
        if (*ppList == NULL) {
            ret = OMX_ErrorInsufficientResources;
            goto EXIT;
        }
        pTempComp = *ppList;
        pTempComp->pNext = NULL;
        pTempComp->pOMXStandComp = pOMXComponent;
        pTempComp->groupPriority = pIMComponent->compPriority.nGroupPriority;
    }

EXIT:
    return ret;
}

OMX_ERRORTYPE removeElementList(IM_OMX_RM_COMPONENT_LIST **ppList, OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE              ret = OMX_ErrorNone;
    IM_OMX_RM_COMPONENT_LIST *pCurrComp = NULL;
    IM_OMX_RM_COMPONENT_LIST *pPrevComp = NULL;
    OMX_BOOL                   bDetectComp = OMX_FALSE;

    if (*ppList == NULL) {
        ret = OMX_ErrorUndefined;
        goto EXIT;
    }

    pCurrComp = *ppList;
    while (pCurrComp != NULL) {
        if (pCurrComp->pOMXStandComp == pOMXComponent) {
            if (*ppList == pCurrComp) {
                *ppList = pCurrComp->pNext;
                IM_OSAL_Free(pCurrComp);
            } else {
                pPrevComp->pNext = pCurrComp->pNext;
                IM_OSAL_Free(pCurrComp);
            }
            bDetectComp = OMX_TRUE;
            break;
        } else {
            pPrevComp = pCurrComp;
            pCurrComp = pCurrComp->pNext;
        }
    }

    if (bDetectComp == OMX_FALSE)
        ret = OMX_ErrorComponentNotFound;
    else
        ret = OMX_ErrorNone;

EXIT:
    return ret;
}

int searchLowPriority(IM_OMX_RM_COMPONENT_LIST *RMComp_list, int inComp_priority, IM_OMX_RM_COMPONENT_LIST **outLowComp)
{
    int ret = 0;
    IM_OMX_RM_COMPONENT_LIST *pTempComp = NULL;
    IM_OMX_RM_COMPONENT_LIST *pCandidateComp = NULL;

    if (RMComp_list == NULL)
        ret = -1;

    pTempComp = RMComp_list;
    *outLowComp = 0;

    while (pTempComp != NULL) {
        if (pTempComp->groupPriority > inComp_priority) {
            if (pCandidateComp != NULL) {
                if (pCandidateComp->groupPriority < pTempComp->groupPriority)
                    pCandidateComp = pTempComp;
            } else {
                pCandidateComp = pTempComp;
            }
        }

        pTempComp = pTempComp->pNext;
    }

    *outLowComp = pCandidateComp;
    if (pCandidateComp == NULL)
        ret = 0;
    else
        ret = 1;

EXIT:
    return ret;
}

OMX_ERRORTYPE removeComponent(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    if (pIMComponent->currentState == OMX_StateIdle) {
        (*(pIMComponent->pCallbacks->EventHandler))
            (pOMXComponent, pIMComponent->callbackData,
            OMX_EventError, OMX_ErrorResourcesLost, 0, NULL);
        ret = OMX_SendCommand(pOMXComponent, OMX_CommandStateSet, OMX_StateLoaded, NULL);
        if (ret != OMX_ErrorNone) {
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }
    } else if ((pIMComponent->currentState == OMX_StateExecuting) || (pIMComponent->currentState == OMX_StatePause)) {
        /* Todo */
    }

    ret = OMX_ErrorNone;

EXIT:
    return ret;
}


OMX_ERRORTYPE IM_OMX_ResourceManager_Init()
{
    IM_OSAL_Entering();
    IM_OSAL_MutexCreate(&ghVideoRMComponentListMutex);
    IM_OSAL_Exiting();
    return OMX_ErrorNone;
}

OMX_ERRORTYPE IM_OMX_ResourceManager_Deinit()
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    IM_OMX_RM_COMPONENT_LIST *pCurrComponent;
    IM_OMX_RM_COMPONENT_LIST *pNextComponent;

    IM_OSAL_Entering();

    IM_OSAL_MutexLock(ghVideoRMComponentListMutex);

    if (gpVideoRMComponentList) {
        pCurrComponent = gpVideoRMComponentList;
        while (pCurrComponent != NULL) {
            pNextComponent = pCurrComponent->pNext;
            IM_OSAL_Free(pCurrComponent);
            pCurrComponent = pNextComponent;
        }
        gpVideoRMComponentList = NULL;
    }

    if (gpVideoRMWaitingList) {
        pCurrComponent = gpVideoRMWaitingList;
        while (pCurrComponent != NULL) {
            pNextComponent = pCurrComponent->pNext;
            IM_OSAL_Free(pCurrComponent);
            pCurrComponent = pNextComponent;
        }
        gpVideoRMWaitingList = NULL;
    }
    IM_OSAL_MutexUnlock(ghVideoRMComponentListMutex);

    IM_OSAL_MutexTerminate(ghVideoRMComponentListMutex);
    ghVideoRMComponentListMutex = NULL;

    ret = OMX_ErrorNone;
EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_Get_Resource(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE              ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT     *pIMComponent = NULL;
    IM_OMX_RM_COMPONENT_LIST *pComponentTemp = NULL;
    IM_OMX_RM_COMPONENT_LIST *pComponentCandidate = NULL;
    int numElem = 0;
    int lowCompDetect = 0;

    IM_OSAL_Entering();

    IM_OSAL_MutexLock(ghVideoRMComponentListMutex);

    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    pComponentTemp = gpVideoRMComponentList;
    if (pIMComponent->codecType == HW_VIDEO_CODEC) {
        if (pComponentTemp != NULL) {
            while (pComponentTemp) {
                numElem++;
                pComponentTemp = pComponentTemp->pNext;
            }
        } else {
            numElem = 0;
        }
        if (numElem >= MAX_RESOURCE_VIDEO) {
            lowCompDetect = searchLowPriority(gpVideoRMComponentList, pIMComponent->compPriority.nGroupPriority, &pComponentCandidate);
            if (lowCompDetect <= 0) {
                ret = OMX_ErrorInsufficientResources;
                goto EXIT;
            } else {
                ret = removeComponent(pComponentCandidate->pOMXStandComp);
                if (ret != OMX_ErrorNone) {
                    ret = OMX_ErrorInsufficientResources;
                    goto EXIT;
                } else {
                    ret = removeElementList(&gpVideoRMComponentList, pComponentCandidate->pOMXStandComp);
                    ret = addElementList(&gpVideoRMComponentList, pOMXComponent);
                    if (ret != OMX_ErrorNone) {
                        ret = OMX_ErrorInsufficientResources;
                        goto EXIT;
                    }
                }
            }
        } else {
            ret = addElementList(&gpVideoRMComponentList, pOMXComponent);
            if (ret != OMX_ErrorNone) {
                ret = OMX_ErrorInsufficientResources;
                goto EXIT;
            }
        }
    }
    ret = OMX_ErrorNone;

EXIT:

    IM_OSAL_MutexUnlock(ghVideoRMComponentListMutex);

    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_Release_Resource(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE              ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT     *pIMComponent = NULL;
    IM_OMX_RM_COMPONENT_LIST *pComponentTemp = NULL;
    OMX_COMPONENTTYPE         *pOMXWaitComponent = NULL;
    int numElem = 0;

    IM_OSAL_Entering();

    IM_OSAL_MutexLock(ghVideoRMComponentListMutex);

    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    pComponentTemp = gpVideoRMWaitingList;
    if (pIMComponent->codecType == HW_VIDEO_CODEC) {
        if (gpVideoRMComponentList == NULL) {
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }

        ret = removeElementList(&gpVideoRMComponentList, pOMXComponent);
        if (ret != OMX_ErrorNone) {
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }
        while (pComponentTemp) {
            numElem++;
            pComponentTemp = pComponentTemp->pNext;
        }
        if (numElem > 0) {
            pOMXWaitComponent = gpVideoRMWaitingList->pOMXStandComp;
            removeElementList(&gpVideoRMWaitingList, pOMXWaitComponent);
            ret = OMX_SendCommand(pOMXWaitComponent, OMX_CommandStateSet, OMX_StateIdle, NULL);
            if (ret != OMX_ErrorNone) {
                goto EXIT;
            }
        }
    }

EXIT:

    IM_OSAL_MutexUnlock(ghVideoRMComponentListMutex);

    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_In_WaitForResource(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    IM_OSAL_Entering();

    IM_OSAL_MutexLock(ghVideoRMComponentListMutex);

    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    if (pIMComponent->codecType == HW_VIDEO_CODEC)
        ret = addElementList(&gpVideoRMWaitingList, pOMXComponent);

    IM_OSAL_MutexUnlock(ghVideoRMComponentListMutex);

    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_Out_WaitForResource(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    IM_OSAL_Entering();

    IM_OSAL_MutexLock(ghVideoRMComponentListMutex);

    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    if (pIMComponent->codecType == HW_VIDEO_CODEC)
        ret = removeElementList(&gpVideoRMWaitingList, pOMXComponent);

    IM_OSAL_MutexUnlock(ghVideoRMComponentListMutex);

    IM_OSAL_Exiting();

    return ret;
}

