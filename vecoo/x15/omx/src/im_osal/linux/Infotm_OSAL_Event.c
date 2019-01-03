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
 * @file        Infotm_OSAL_Event.c
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.03.01 : Create
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "Infotm_OSAL_Memory.h"
#include "Infotm_OSAL_Mutex.h"
#include "Infotm_OSAL_Event.h"

#undef  Infotm_LOG_TAG
#define Infotm_LOG_TAG    "Infotm_OSAL_EVENT"
#define Infotm_LOG_OFF
#include "Infotm_OSAL_Trace.h"


OMX_ERRORTYPE Infotm_OSAL_SignalCreate(OMX_HANDLETYPE *eventHandle)
{
    Infotm_OSAL_THREADEVENT *event;
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    event = (Infotm_OSAL_THREADEVENT *)Infotm_OSAL_Malloc(sizeof(Infotm_OSAL_THREADEVENT));
    if (!event) {
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    Infotm_OSAL_Memset(event, 0, sizeof(Infotm_OSAL_THREADEVENT));
    event->signal = OMX_FALSE;

    ret = Infotm_OSAL_MutexCreate(&event->mutex);
    if (ret != OMX_ErrorNone) {
        Infotm_OSAL_Free(event);
        goto EXIT;
    }

    if (pthread_cond_init(&event->condition, NULL)) {
        Infotm_OSAL_MutexTerminate(event->mutex);
        Infotm_OSAL_Free(event);
        ret = OMX_ErrorUndefined;
        goto EXIT;
    }

    *eventHandle = (OMX_HANDLETYPE)event;
    ret = OMX_ErrorNone;

EXIT:
    return ret;
}

OMX_ERRORTYPE Infotm_OSAL_SignalTerminate(OMX_HANDLETYPE eventHandle)
{
    Infotm_OSAL_THREADEVENT *event = (Infotm_OSAL_THREADEVENT *)eventHandle;
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    if (!event) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    ret = Infotm_OSAL_MutexLock(event->mutex);
    if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    if (pthread_cond_destroy(&event->condition)) {
        ret = OMX_ErrorUndefined;
        goto EXIT;
    }

    ret = Infotm_OSAL_MutexUnlock(event->mutex);
    if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorUndefined;
        goto EXIT;
    }

    ret = Infotm_OSAL_MutexTerminate(event->mutex);
    if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorUndefined;
        goto EXIT;
    }

    Infotm_OSAL_Free(event);

EXIT:
    return ret;
}

OMX_ERRORTYPE Infotm_OSAL_SignalReset(OMX_HANDLETYPE eventHandle)
{
    Infotm_OSAL_THREADEVENT *event = (Infotm_OSAL_THREADEVENT *)eventHandle;
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    if (!event) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    ret = Infotm_OSAL_MutexLock(event->mutex);
    if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    event->signal = OMX_FALSE;

    Infotm_OSAL_MutexUnlock(event->mutex);

EXIT:
    return ret;
}

OMX_ERRORTYPE Infotm_OSAL_SignalSet(OMX_HANDLETYPE eventHandle)
{
    Infotm_OSAL_THREADEVENT *event = (Infotm_OSAL_THREADEVENT *)eventHandle;
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    if (!event) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    ret = Infotm_OSAL_MutexLock(event->mutex);
    if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    event->signal = OMX_TRUE;
    pthread_cond_signal(&event->condition);

    Infotm_OSAL_MutexUnlock(event->mutex);

EXIT:
    return ret;
}

OMX_ERRORTYPE Infotm_OSAL_SignalWait(OMX_HANDLETYPE eventHandle, OMX_U32 ms)
{
    Infotm_OSAL_THREADEVENT *event = (Infotm_OSAL_THREADEVENT *)eventHandle;
    OMX_ERRORTYPE         ret = OMX_ErrorNone;
    struct timespec       timeout;
    struct timeval        now;
    int                   funcret = 0;
    OMX_U32               tv_us;

    FunctionIn();

    if (!event) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    gettimeofday(&now, NULL);

    tv_us = now.tv_usec + ms * 1000;
    timeout.tv_sec = now.tv_sec + tv_us / 1000000;
    timeout.tv_nsec = (tv_us % 1000000) * 1000;

    ret = Infotm_OSAL_MutexLock(event->mutex);
    if (ret != OMX_ErrorNone) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    if (ms == 0) {
        if (!event->signal)
            ret = OMX_ErrorTimeout;
    } else if (ms == DEF_MAX_WAIT_TIME) {
        while (!event->signal)
            pthread_cond_wait(&event->condition, (pthread_mutex_t *)(event->mutex));
        ret = OMX_ErrorNone;
    } else {
        while (!event->signal) {
            funcret = pthread_cond_timedwait(&event->condition, (pthread_mutex_t *)(event->mutex), &timeout);
            if ((!event->signal) && (funcret == ETIMEDOUT)) {
                ret = OMX_ErrorTimeout;
                break;
            }
        }
    }

    Infotm_OSAL_MutexUnlock(event->mutex);

EXIT:
    FunctionOut();

    return ret;
}
