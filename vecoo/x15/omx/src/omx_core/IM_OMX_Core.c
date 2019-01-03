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
 * @file        IM_OMX_Core.c
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.03.01 : Create
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IM_OMX_Core.h"
#include "IM_OMX_Component_Register.h"
#include "IM_OSAL_Memory.h"
#include "IM_OSAL_ETC.h"
#include "IM_OSAL_Mutex.h"
#include "IM_OMX_Resourcemanager.h"

#undef  IM_LOG_TAG
#define IM_LOG_TAG    "IM_OMX_CORE"
#define IM_LOG_OFF
#include "IM_OSAL_Trace.h"


static int gInitialized = 0;
static int gComponentNum = 0;

static IM_OMX_COMPONENT_REGLIST *gComponentList = NULL;
static IM_OMX_COMPONENT *gLoadComponentList = NULL;
static OMX_HANDLETYPE ghLoadComponentListMutex = NULL;


OMX_API OMX_ERRORTYPE OMX_APIENTRY IM_OMX_Init(void)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    IM_OSAL_Entering();

    if (gInitialized == 0) {
        if (IM_OMX_Component_Register(&gComponentList, &gComponentNum)) {
            ret = OMX_ErrorInsufficientResources;
            IM_OSAL_Error("IM_OMX_Init : %s", "OMX_ErrorInsufficientResources");
            goto EXIT;
        }

        IM_OMX_ResourceManager_Init();

        IM_OSAL_MutexCreate(&ghLoadComponentListMutex);

        gInitialized = 1;
        IM_OSAL_Info("IM_OMX_Init : %s", "OMX_ErrorNone");
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY IM_OMX_Deinit(void)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    IM_OSAL_Entering();

    IM_OSAL_MutexTerminate(ghLoadComponentListMutex);
    ghLoadComponentListMutex = NULL;

    IM_OMX_ResourceManager_Deinit();
    if(gComponentList != NULL && gInitialized) {
        if (OMX_ErrorNone != IM_OMX_Component_Unregister(gComponentList)) {
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }
    }
    gComponentList = NULL;
    gComponentNum = 0;
    gInitialized = 0;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY IM_OMX_ComponentNameEnum(
        OMX_OUT OMX_STRING cComponentName,
        OMX_IN  OMX_U32 nNameLength,
    OMX_IN  OMX_U32 nIndex)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    IM_OSAL_Entering();

    if (nIndex >= gComponentNum) {
        ret = OMX_ErrorNoMore;
        goto EXIT;
    }

    sprintf(cComponentName, "%s", gComponentList[nIndex].component.componentName);
    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY IM_OMX_GetHandle(
    OMX_OUT OMX_HANDLETYPE *pHandle,
    OMX_IN  OMX_STRING cComponentName,
    OMX_IN  OMX_PTR pAppData,
    OMX_IN  OMX_CALLBACKTYPE *pCallBacks)
{
    OMX_ERRORTYPE      ret = OMX_ErrorNone;
    IM_OMX_COMPONENT *loadComponent;
    IM_OMX_COMPONENT *currentComponent;
    int i = 0;

    IM_OSAL_Entering();

    if (gInitialized != 1) {
        ret = OMX_ErrorNotReady;
        goto EXIT;
    }

    if ((pHandle == NULL) || (cComponentName == NULL) || (pCallBacks == NULL)) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    IM_OSAL_Info("ComponentName : %s", cComponentName);

    IM_OSAL_TraceLevelSet();

    for (i = 0; i < gComponentNum; i++) {
        if (IM_OSAL_Strcmp(cComponentName, gComponentList[i].component.componentName) == 0) {
            loadComponent = IM_OSAL_Malloc(sizeof(IM_OMX_COMPONENT));
            IM_OSAL_Memset(loadComponent, 0, sizeof(IM_OMX_COMPONENT));

            IM_OSAL_Strcpy(loadComponent->libName, gComponentList[i].libName);
            IM_OSAL_Strcpy(loadComponent->componentName, gComponentList[i].component.componentName);
            ret = IM_OMX_ComponentLoad(loadComponent);
            if (ret != OMX_ErrorNone) {
                IM_OSAL_Free(loadComponent);
                IM_OSAL_Error("OMX_Error");
                goto EXIT;
            }

            ret = loadComponent->pOMXComponent->SetCallbacks(loadComponent->pOMXComponent, pCallBacks, pAppData);
            if (ret != OMX_ErrorNone) {
                IM_OMX_ComponentUnload(loadComponent);
                IM_OSAL_Free(loadComponent);
                IM_OSAL_Error("OMX_Error, Line:%d", __LINE__);
                goto EXIT;
            }

            IM_OSAL_MutexLock(ghLoadComponentListMutex);
            if (gLoadComponentList == NULL) {
                gLoadComponentList = loadComponent;
            } else {
                currentComponent = gLoadComponentList;
                while (currentComponent->nextOMXComp != NULL) {
                    currentComponent = currentComponent->nextOMXComp;
                }
                currentComponent->nextOMXComp = loadComponent;
            }
            IM_OSAL_MutexUnlock(ghLoadComponentListMutex);

            *pHandle = loadComponent->pOMXComponent;
            ret = OMX_ErrorNone;
            IM_OSAL_Info("IM_OMX_GetHandle : %s", "OMX_ErrorNone");
            goto EXIT;
        }
    }

    ret = OMX_ErrorComponentNotFound;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY IM_OMX_FreeHandle(OMX_IN OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE      ret = OMX_ErrorNone;
    IM_OMX_COMPONENT *currentComponent;
    IM_OMX_COMPONENT *deleteComponent;

    IM_OSAL_Entering();

    if (gInitialized != 1) {
        ret = OMX_ErrorNotReady;
        goto EXIT;
    }

    if (!hComponent) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    IM_OSAL_MutexLock(ghLoadComponentListMutex);
    currentComponent = gLoadComponentList;
    if (gLoadComponentList->pOMXComponent == hComponent) {
        deleteComponent = gLoadComponentList;
        gLoadComponentList = gLoadComponentList->nextOMXComp;
    } else {
        while ((currentComponent != NULL) && (((IM_OMX_COMPONENT *)(currentComponent->nextOMXComp))->pOMXComponent != hComponent))
            currentComponent = currentComponent->nextOMXComp;

        if (((IM_OMX_COMPONENT *)(currentComponent->nextOMXComp))->pOMXComponent == hComponent) {
            deleteComponent = currentComponent->nextOMXComp;
            currentComponent->nextOMXComp = deleteComponent->nextOMXComp;
        } else if (currentComponent == NULL) {
            ret = OMX_ErrorComponentNotFound;
            IM_OSAL_MutexUnlock(ghLoadComponentListMutex);
            goto EXIT;
        }
    }
    IM_OSAL_MutexUnlock(ghLoadComponentListMutex);

    IM_OMX_ComponentUnload(deleteComponent);
    IM_OSAL_Free(deleteComponent);

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_API OMX_ERRORTYPE OMX_APIENTRY IM_OMX_SetupTunnel(
    OMX_IN OMX_HANDLETYPE hOutput,
    OMX_IN OMX_U32 nPortOutput,
    OMX_IN OMX_HANDLETYPE hInput,
    OMX_IN OMX_U32 nPortInput)
{
    OMX_ERRORTYPE ret = OMX_ErrorNotImplemented;

EXIT:
    return ret;
}

OMX_API OMX_ERRORTYPE IM_OMX_GetContentPipe(
    OMX_OUT OMX_HANDLETYPE *hPipe,
    OMX_IN  OMX_STRING szURI)
{
    OMX_ERRORTYPE ret = OMX_ErrorNotImplemented;

EXIT:
    return ret;
}

OMX_API OMX_ERRORTYPE IM_OMX_GetComponentsOfRole (
    OMX_IN    OMX_STRING role,
    OMX_INOUT OMX_U32 *pNumComps,
    OMX_INOUT OMX_U8  **compNames)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    int           max_role_num = 0;
    OMX_STRING    RoleString[MAX_OMX_COMPONENT_ROLE_SIZE];
    int i = 0, j = 0;

    IM_OSAL_Entering();

    if (gInitialized != 1) {
        ret = OMX_ErrorNotReady;
        goto EXIT;
    }

    *pNumComps = 0;

    for (i = 0; i < MAX_OMX_COMPONENT_NUM; i++) {
        max_role_num = gComponentList[i].component.totalRoleNum;

        for (j = 0; j < max_role_num; j++) {
            if (IM_OSAL_Strcmp(gComponentList[i].component.roles[j], role) == 0) {
                if (compNames != NULL) {
                    IM_OSAL_Strcpy((OMX_STRING)compNames[*pNumComps], gComponentList[i].component.componentName);
                }
                *pNumComps = (*pNumComps + 1);
            }
        }
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_API OMX_ERRORTYPE IM_OMX_GetRolesOfComponent (
    OMX_IN    OMX_STRING compName,
    OMX_INOUT OMX_U32 *pNumRoles,
    OMX_OUT   OMX_U8 **roles)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    OMX_BOOL      detectComp = OMX_FALSE;
    int           compNum = 0, totalRoleNum = 0;
    int i = 0;

    IM_OSAL_Entering();

    if (gInitialized != 1) {
        ret = OMX_ErrorNotReady;
        goto EXIT;
    }

    for (i = 0; i < MAX_OMX_COMPONENT_NUM; i++) {
        if (gComponentList != NULL) {
            if (IM_OSAL_Strcmp(gComponentList[i].component.componentName, compName) == 0) {
                *pNumRoles = totalRoleNum = gComponentList[i].component.totalRoleNum;
                compNum = i;
                detectComp = OMX_TRUE;
                break;
            }
        } else {
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }
    }

    if (detectComp == OMX_FALSE) {
        *pNumRoles = 0;
        ret = OMX_ErrorComponentNotFound;
        goto EXIT;
    }

    if (roles != NULL) {
        for (i = 0; i < totalRoleNum; i++) {
            IM_OSAL_Strcpy(roles[i], gComponentList[compNum].component.roles[i]);
        }
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}
