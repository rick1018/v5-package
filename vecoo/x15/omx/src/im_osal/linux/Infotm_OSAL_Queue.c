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
 * AND ANY EXPRESS OR IMPLIED WARRANInfotmES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANInfotmES OF MERCHANTABILITY AND FITNESS FOR A PARInfotmCULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENInfotmAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSInfotmTUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPInfotmON) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * @file        Infotm_OSAL_Queue.c
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.03.01 : Create
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Infotm_OSAL_Memory.h"
#include "Infotm_OSAL_Mutex.h"
#include "Infotm_OSAL_Queue.h"


OMX_ERRORTYPE Infotm_OSAL_QueueCreate(Infotm_QUEUE *queueHandle)
{
    int i = 0;
    Infotm_QElem *newqelem = NULL;
    Infotm_QElem *currentqelem = NULL;
    Infotm_QUEUE *queue = (Infotm_QUEUE *)queueHandle;

    OMX_ERRORTYPE ret = OMX_ErrorNone;

    if (!queue)
        return OMX_ErrorBadParameter;

    ret = Infotm_OSAL_MutexCreate(&queue->qMutex);
    if (ret != OMX_ErrorNone)
        return ret;

    queue->first = (Infotm_QElem *)Infotm_OSAL_Malloc(sizeof(Infotm_QElem));
    if (queue->first == NULL)
        return OMX_ErrorInsufficientResources;

    Infotm_OSAL_Memset(queue->first, 0, sizeof(Infotm_QElem));
    currentqelem = queue->last = queue->first;
    queue->numElem = 0;

    for (i = 0; i < (MAX_QUEUE_ELEMENTS - 2); i++) {
        newqelem = (Infotm_QElem *)Infotm_OSAL_Malloc(sizeof(Infotm_QElem));
        if (newqelem == NULL) {
            while (queue->first != NULL) {
                currentqelem = queue->first->qNext;
                Infotm_OSAL_Free((OMX_PTR)queue->first);
                queue->first = currentqelem;
            }
            return OMX_ErrorInsufficientResources;
        } else {
            Infotm_OSAL_Memset(newqelem, 0, sizeof(Infotm_QElem));
            currentqelem->qNext = newqelem;
            currentqelem = newqelem;
        }
    }

    currentqelem->qNext = queue->first;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE Infotm_OSAL_QueueTerminate(Infotm_QUEUE *queueHandle)
{
    int i = 0;
    Infotm_QElem *currentqelem = NULL;
    Infotm_QUEUE *queue = (Infotm_QUEUE *)queueHandle;
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    if (!queue)
        return OMX_ErrorBadParameter;

    for ( i = 0; i < (MAX_QUEUE_ELEMENTS - 2); i++) {
        currentqelem = queue->first->qNext;
        Infotm_OSAL_Free(queue->first);
        queue->first = currentqelem;
    }

    if(queue->first) {
        Infotm_OSAL_Free(queue->first);
        queue->first = NULL;
    }

    ret = Infotm_OSAL_MutexTerminate(queue->qMutex);

    return ret;
}

int Infotm_OSAL_Queue(Infotm_QUEUE *queueHandle, void *data)
{
    Infotm_QUEUE *queue = (Infotm_QUEUE *)queueHandle;
    if (queue == NULL)
        return -1;

    Infotm_OSAL_MutexLock(queue->qMutex);

    if ((queue->last->data != NULL) || (queue->numElem >= MAX_QUEUE_ELEMENTS)) {
        Infotm_OSAL_MutexUnlock(queue->qMutex);
        return -1;
    }
    queue->last->data = data;
    queue->last = queue->last->qNext;
    queue->numElem++;

    Infotm_OSAL_MutexUnlock(queue->qMutex);
    return 0;
}

void *Infotm_OSAL_Dequeue(Infotm_QUEUE *queueHandle)
{
    void *data = NULL;
    Infotm_QUEUE *queue = (Infotm_QUEUE *)queueHandle;
    if (queue == NULL)
        return NULL;

    Infotm_OSAL_MutexLock(queue->qMutex);

    if ((queue->first->data == NULL) || (queue->numElem <= 0)) {
        Infotm_OSAL_MutexUnlock(queue->qMutex);
        return NULL;
    }
    data = queue->first->data;
    queue->first->data = NULL;
    queue->first = queue->first->qNext;
    queue->numElem--;

    Infotm_OSAL_MutexUnlock(queue->qMutex);
    return data;
}

int Infotm_OSAL_GetElemNum(Infotm_QUEUE *queueHandle)
{
    int ElemNum = 0;
    Infotm_QUEUE *queue = (Infotm_QUEUE *)queueHandle;
    if (queue == NULL)
        return -1;

    Infotm_OSAL_MutexLock(queue->qMutex);
    ElemNum = queue->numElem;
    Infotm_OSAL_MutexUnlock(queue->qMutex);
    return ElemNum;
}

int Infotm_OSAL_SetElemNum(Infotm_QUEUE *queueHandle, int ElemNum)
{
    Infotm_QUEUE *queue = (Infotm_QUEUE *)queueHandle;
    if (queue == NULL)
        return -1;

    Infotm_OSAL_MutexLock(queue->qMutex);
    queue->numElem = ElemNum; 
    Infotm_OSAL_MutexUnlock(queue->qMutex);
    return ElemNum;
}

