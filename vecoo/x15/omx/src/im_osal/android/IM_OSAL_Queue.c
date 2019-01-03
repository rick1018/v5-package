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
 * @file        IM_OSAL_Queue.c
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.03.01 : Create
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IM_OSAL_Memory.h"
#include "IM_OSAL_Mutex.h"
#include "IM_OSAL_Queue.h"

#include "IM_OSAL_Trace.h"
OMX_ERRORTYPE IM_OSAL_QueueCreate(IM_QUEUE *queueHandle)
{
    int i = 0;
    IM_QElem *newqelem = NULL;
    IM_QElem *currentqelem = NULL;
    IM_QUEUE *queue = (IM_QUEUE *)queueHandle;

    OMX_ERRORTYPE ret = OMX_ErrorNone;

    if (!queue)
        return OMX_ErrorBadParameter;

    ret = IM_OSAL_MutexCreate(&queue->qMutex);
    if (ret != OMX_ErrorNone)
        return ret;

    queue->first = (IM_QElem *)IM_OSAL_Malloc(sizeof(IM_QElem));
    if (queue->first == NULL)
        return OMX_ErrorInsufficientResources;

    IM_OSAL_Memset(queue->first, 0, sizeof(IM_QElem));
    currentqelem = queue->last = queue->first;
    queue->numElem = 0;

    for (i = 0; i < (MAX_QUEUE_ELEMENTS - 1); i++) {
        newqelem = (IM_QElem *)IM_OSAL_Malloc(sizeof(IM_QElem));
        if (newqelem == NULL) {
            while (queue->first != NULL) {
                currentqelem = queue->first->qNext;
                IM_OSAL_Free((OMX_PTR)queue->first);
                queue->first = currentqelem;
            }
            return OMX_ErrorInsufficientResources;
        } else {
            IM_OSAL_Memset(newqelem, 0, sizeof(IM_QElem));
            currentqelem->qNext = newqelem;
            currentqelem = newqelem;
        }
    }

    currentqelem->qNext = queue->first;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE IM_OSAL_QueueTerminate(IM_QUEUE *queueHandle)
{
    int i = 0;
    IM_QElem *currentqelem = NULL;
    IM_QUEUE *queue = (IM_QUEUE *)queueHandle;
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    if (!queue)
        return OMX_ErrorBadParameter;

    for ( i = 0; i < (MAX_QUEUE_ELEMENTS - 2); i++) {
        currentqelem = queue->first->qNext;
        IM_OSAL_Free(queue->first);
        queue->first = currentqelem;
    }

    if(queue->first) {
        IM_OSAL_Free(queue->first);
        queue->first = NULL;
    }

    ret = IM_OSAL_MutexTerminate(queue->qMutex);

    return ret;
}

int IM_OSAL_Queue(IM_QUEUE *queueHandle, void *data)
{
    IM_QUEUE *queue = (IM_QUEUE *)queueHandle;
    if (queue == NULL)
        return -1;

    IM_OSAL_MutexLock(queue->qMutex);

    if ((queue->last->data != NULL) || (queue->numElem >= MAX_QUEUE_ELEMENTS)) {
		IM_OSAL_Error("queue->numElem >= MAX_QUEUE_ELEMENTS");
        IM_OSAL_MutexUnlock(queue->qMutex);
        return -1;
    }
    queue->last->data = data;
    queue->last = queue->last->qNext;
    queue->numElem++;

    IM_OSAL_MutexUnlock(queue->qMutex);
    return 0;
}

void *IM_OSAL_Dequeue(IM_QUEUE *queueHandle)
{
    void *data = NULL;
    IM_QUEUE *queue = (IM_QUEUE *)queueHandle;
    if (queue == NULL)
        return NULL;

    IM_OSAL_MutexLock(queue->qMutex);

    if ((queue->first->data == NULL) || (queue->numElem <= 0)) {
        IM_OSAL_MutexUnlock(queue->qMutex);
        return NULL;
    }
    data = queue->first->data;
    queue->first->data = NULL;
    queue->first = queue->first->qNext;
    queue->numElem--;

    IM_OSAL_MutexUnlock(queue->qMutex);
    return data;
}

int IM_OSAL_GetElemNum(IM_QUEUE *queueHandle)
{
    int ElemNum = 0;
    IM_QUEUE *queue = (IM_QUEUE *)queueHandle;
    if (queue == NULL)
        return -1;

    IM_OSAL_MutexLock(queue->qMutex);
    ElemNum = queue->numElem;
    IM_OSAL_MutexUnlock(queue->qMutex);
    return ElemNum;
}

int IM_OSAL_SetElemNum(IM_QUEUE *queueHandle, int ElemNum)
{
    IM_QUEUE *queue = (IM_QUEUE *)queueHandle;
    if (queue == NULL)
        return -1;

    IM_OSAL_MutexLock(queue->qMutex);
    queue->numElem = ElemNum; 
    IM_OSAL_MutexUnlock(queue->qMutex);
    return ElemNum;
}

