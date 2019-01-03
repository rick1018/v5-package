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
 * @file       IM_OMX_Component_Register.c
 * @brief      IM OpenMAX IL Component Register
 * @author     ayakashi (yanyuan_xu@infotm.com)
 * @version    1.0
 * @history
 *    2012.03.01 : Create
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <assert.h>

#include "OMX_Component.h"
#include "IM_OSAL_Memory.h"
#include "IM_OSAL_ETC.h"
#include "IM_OSAL_Library.h"
#include "IM_OMX_Component_Register.h"
#include "IM_OMX_Macros.h"

#undef  IM_LOG_TAG
#define IM_LOG_TAG    "IM_COMP_REGS"
#define IM_LOG_OFF
#include "IM_OSAL_Trace.h"


#define REGISTRY_FILENAME "Infotmomxregistry"


OMX_ERRORTYPE IM_OMX_Component_Register(IM_OMX_COMPONENT_REGLIST **compList, OMX_U32 *compNum)
{
    OMX_ERRORTYPE  ret = OMX_ErrorNone;
    int            componentNum = 0, roleNum = 0, totalCompNum = 0;
    int            read;
    char          *omxregistryfile = NULL;
    char          *line = NULL;
    char          *libName;
    FILE          *omxregistryfp;
    size_t         len;
    OMX_HANDLETYPE soHandle;
    const char    *errorMsg;
    int (*IM_OMX_COMPONENT_Library_Register)(IMRegisterComponentType **IMComponents);
    IMRegisterComponentType **IMComponentsTemp;
    IM_OMX_COMPONENT_REGLIST *componentList;

    IM_OSAL_Entering();

    omxregistryfile = IM_OSAL_Malloc(strlen("/system/etc/") + strlen(REGISTRY_FILENAME) + 2);
    IM_OSAL_Strcpy(omxregistryfile, "/system/etc/");
    IM_OSAL_Strcat(omxregistryfile, REGISTRY_FILENAME);

    omxregistryfp = fopen(omxregistryfile, "r");
    if (omxregistryfp == NULL) {
        ret = OMX_ErrorUndefined;
        goto EXIT;
    }
    IM_OSAL_Free(omxregistryfile);

    fseek(omxregistryfp, 0, 0);
    componentList = (IM_OMX_COMPONENT_REGLIST *)IM_OSAL_Malloc(sizeof(IM_OMX_COMPONENT_REGLIST) * MAX_OMX_COMPONENT_NUM);
    IM_OSAL_Memset(componentList, 0, sizeof(IM_OMX_COMPONENT_REGLIST) * MAX_OMX_COMPONENT_NUM);
    libName = IM_OSAL_Malloc(MAX_OMX_COMPONENT_LIBNAME_SIZE);

    while ((read = getline(&line, &len, omxregistryfp)) != -1) {
        if ((*line == 'l') && (*(line + 1) == 'i') && (*(line + 2) == 'b') &&
            (*(line + 3) == 'O') && (*(line + 4) == 'M') && (*(line + 5) == 'X')) {
            IM_OSAL_Memset(libName, 0, MAX_OMX_COMPONENT_LIBNAME_SIZE);
            IM_OSAL_Strncpy(libName, line, IM_OSAL_Strlen(line)-1);
            IM_OSAL_Info("libName : %s", libName);


            if ((soHandle = IM_OSAL_dlopen(libName, RTLD_NOW)) != NULL) {
                IM_OSAL_dlerror();    /* clear error*/
                if ((IM_OMX_COMPONENT_Library_Register = IM_OSAL_dlsym(soHandle, "IM_OMX_COMPONENT_Library_Register")) != NULL) {
                    int i = 0, j = 0;

                    componentNum = (*IM_OMX_COMPONENT_Library_Register)(NULL);
                    IMComponentsTemp = (IMRegisterComponentType **)IM_OSAL_Malloc(sizeof(IMRegisterComponentType*) * componentNum);
                    for (i = 0; i < componentNum; i++) {
                        IMComponentsTemp[i] = IM_OSAL_Malloc(sizeof(IMRegisterComponentType));
                        IM_OSAL_Memset(IMComponentsTemp[i], 0, sizeof(IMRegisterComponentType));
                    }
                    (*IM_OMX_COMPONENT_Library_Register)(IMComponentsTemp);

                    for (i = 0; i < componentNum; i++) {
                        IM_OSAL_Strcpy(componentList[totalCompNum].component.componentName, IMComponentsTemp[i]->componentName);
                        for (j = 0; j < IMComponentsTemp[i]->totalRoleNum; j++)
                            IM_OSAL_Strcpy(componentList[totalCompNum].component.roles[j], IMComponentsTemp[i]->roles[j]);
                        componentList[totalCompNum].component.totalRoleNum = IMComponentsTemp[i]->totalRoleNum;

                        IM_OSAL_Strcpy(componentList[totalCompNum].libName, libName);

                        totalCompNum++;
                    }
                    for (i = 0; i < componentNum; i++) {
                        IM_OSAL_Free(IMComponentsTemp[i]);
                    }

                    IM_OSAL_Free(IMComponentsTemp);
                } else {
                    if ((errorMsg = IM_OSAL_dlerror()) != NULL)
                        IM_OSAL_Warning( "dlsym failed: %s", errorMsg);
                }
                IM_OSAL_dlclose(soHandle);
            } else {
                IM_OSAL_Warning("dlopen failed: %s", IM_OSAL_dlerror());
            }
        } else {
            /* not a component name line. skip */
            continue;
        }
    }

    IM_OSAL_Free(libName);
    fclose(omxregistryfp);

    *compList = componentList;
    *compNum = totalCompNum;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_Component_Unregister(IM_OMX_COMPONENT_REGLIST *componentList)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    IM_OSAL_Memset(componentList, 0, sizeof(IM_OMX_COMPONENT_REGLIST) * MAX_OMX_COMPONENT_NUM);
    IM_OSAL_Free(componentList);

EXIT:
    return ret;
}

OMX_ERRORTYPE IM_OMX_ComponentAPICheck(OMX_COMPONENTTYPE component)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;

    if ((NULL == component.GetComponentVersion)    ||
        (NULL == component.SendCommand)            ||
        (NULL == component.GetParameter)           ||
        (NULL == component.SetParameter)           ||
        (NULL == component.GetConfig)              ||
        (NULL == component.SetConfig)              ||
        (NULL == component.GetExtensionIndex)      ||
        (NULL == component.GetState)               ||
        (NULL == component.ComponentTunnelRequest) ||
        (NULL == component.UseBuffer)              ||
        (NULL == component.AllocateBuffer)         ||
        (NULL == component.FreeBuffer)             ||
        (NULL == component.EmptyThisBuffer)        ||
        (NULL == component.FillThisBuffer)         ||
        (NULL == component.SetCallbacks)           ||
        (NULL == component.ComponentDeInit)        ||
        (NULL == component.UseEGLImage)            ||
        (NULL == component.ComponentRoleEnum))
        ret = OMX_ErrorInvalidComponent;
    else
        ret = OMX_ErrorNone;

    return ret;
}

OMX_ERRORTYPE IM_OMX_ComponentLoad(IM_OMX_COMPONENT *IM_component)
{
    OMX_ERRORTYPE      ret = OMX_ErrorNone;
    OMX_HANDLETYPE     libHandle;
    OMX_COMPONENTTYPE *pOMXComponent;

    IM_OSAL_Entering();

    OMX_ERRORTYPE (*IM_OMX_ComponentInit)(OMX_HANDLETYPE hComponent, OMX_STRING componentName);

    libHandle = IM_OSAL_dlopen(IM_component->libName, RTLD_NOW);
    if (!libHandle) {
        ret = OMX_ErrorInvalidComponentName;
        IM_OSAL_Error("OMX_ErrorInvalidComponentName, Line:%d", __LINE__);
        goto EXIT;
    }

    IM_OMX_ComponentInit = IM_OSAL_dlsym(libHandle, "IM_OMX_ComponentInit");
    if (!IM_OMX_ComponentInit) {
        IM_OSAL_dlclose(libHandle);
        ret = OMX_ErrorInvalidComponent;
        IM_OSAL_Error("OMX_ErrorInvalidComponent, Line:%d", __LINE__);
        goto EXIT;
    }

    pOMXComponent = (OMX_COMPONENTTYPE *)IM_OSAL_Malloc(sizeof(OMX_COMPONENTTYPE));
    INIT_SET_SIZE_VERSION(pOMXComponent, OMX_COMPONENTTYPE);
    ret = (*IM_OMX_ComponentInit)((OMX_HANDLETYPE)pOMXComponent, IM_component->componentName);
    if (ret != OMX_ErrorNone) {
        IM_OSAL_Free(pOMXComponent);
        IM_OSAL_dlclose(libHandle);
        ret = OMX_ErrorInvalidComponent;
        IM_OSAL_Error("OMX_ErrorInvalidComponent, Line:%d", __LINE__);
        goto EXIT;
    } else {
        if (IM_OMX_ComponentAPICheck(*pOMXComponent) != OMX_ErrorNone) {
            IM_OSAL_Free(pOMXComponent);
            IM_OSAL_dlclose(libHandle);
            if (NULL != pOMXComponent->ComponentDeInit)
                pOMXComponent->ComponentDeInit(pOMXComponent);
            ret = OMX_ErrorInvalidComponent;
            IM_OSAL_Error("OMX_ErrorInvalidComponent, Line:%d", __LINE__);
            goto EXIT;
        }
        IM_component->libHandle = libHandle;
        IM_component->pOMXComponent = pOMXComponent;
        ret = OMX_ErrorNone;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_ComponentUnload(IM_OMX_COMPONENT *IM_component)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pOMXComponent = NULL;

    IM_OSAL_Entering();

    if (!IM_component) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pOMXComponent = IM_component->pOMXComponent;
    if (pOMXComponent != NULL) {
        pOMXComponent->ComponentDeInit(pOMXComponent);
        IM_OSAL_Free(pOMXComponent);
        IM_component->pOMXComponent = NULL;
    }

    if (IM_component->libHandle != NULL) {
        IM_OSAL_dlclose(IM_component->libHandle);
        IM_component->libHandle = NULL;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}
