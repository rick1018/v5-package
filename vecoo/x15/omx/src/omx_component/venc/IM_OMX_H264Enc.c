/*
 * Copyright (c) 2013, InfoTM Microelectronics Co.,Ltd
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
 * @file        IM_OMX_H2644Enc.c
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.10.08 : Create
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys/time.h"
#include "IM_OMX_Macros.h"
#include "IM_OMX_Basecomponent.h"
#include "IM_OMX_Baseport.h"
#include "IM_OMX_Venc.h"
#include "library_register.h"
#include "IM_OMX_H264Enc.h"
#include "IM_OSAL_Buffer.h"
#include "IM_OSAL_Trace.h"
#include "IM_OSAL_Memory.h"
#include "IM_OSAL_ETC.h"
#include "IM_OSAL_Semaphore.h"
#include "IM_OSAL_Mutex.h"
#include "IM_OSAL_Thread.h"

#undef  IM_LOG_TAG
#define IM_LOG_TAG    "IM_H264_ENC"
#define IM_LOG_OFF
#include "IM_OSAL_Trace.h"

/* H.264 Encoder Supported Levels & profiles */
IM_OMX_VIDEO_PROFILELEVEL supportedAVCProfileLevels[] ={
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel1},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel1b},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel11},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel12},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel13},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel2},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel21},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel22},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel3},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel31},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel32},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel4},
    {OMX_VIDEO_AVCProfileBaseline, OMX_VIDEO_AVCLevel41},

    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel1},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel1b},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel11},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel12},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel13},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel2},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel21},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel22},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel3},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel31},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel32},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel4},
    {OMX_VIDEO_AVCProfileMain, OMX_VIDEO_AVCLevel41},

    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel1},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel1b},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel11},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel12},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel13},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel2},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel21},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel22},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel3},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel31},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel32},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel4},
    {OMX_VIDEO_AVCProfileHigh, OMX_VIDEO_AVCLevel41}};

/*
   typedef struct LevelConversion {
   OMX_U32 omxLevel;
   H264EncLevel avcLevel;
   } LevelConversion;

   static LevelConversion ConversionTable[] = {
   { OMX_VIDEO_AVCLevel1,  H264ENC_LEVEL_1 },
   { OMX_VIDEO_AVCLevel1b, H264ENC_LEVEL_1_b   },   
   { OMX_VIDEO_AVCLevel11, H264ENC_LEVEL_1_1 },
   { OMX_VIDEO_AVCLevel12, H264ENC_LEVEL_1_2 },
   { OMX_VIDEO_AVCLevel13, H264ENC_LEVEL_1_3 },
   { OMX_VIDEO_AVCLevel2,  H264ENC_LEVEL_2 },
   { OMX_VIDEO_AVCLevel21, H264ENC_LEVEL_2_1 },
   { OMX_VIDEO_AVCLevel22, H264ENC_LEVEL_2_2 },
   { OMX_VIDEO_AVCLevel3,  H264ENC_LEVEL_3   },
   { OMX_VIDEO_AVCLevel31, H264ENC_LEVEL_3_1 },
   { OMX_VIDEO_AVCLevel32, H264ENC_LEVEL_3_2 },
   { OMX_VIDEO_AVCLevel4,  H264ENC_LEVEL_4   },
   { OMX_VIDEO_AVCLevel41, H264ENC_LEVEL_4_1 },

//on2 8270 does not support the levels below
//{ OMX_VIDEO_AVCLevel42, H264ENC_LEVEL_4_2 },
//{ OMX_VIDEO_AVCLevel5,  H264ENC_LEVEL_5   },
//{ OMX_VIDEO_AVCLevel51, H264ENC_LEVEL_5_1 }
};
*/

struct formatMap {
    H264EncPictureType   encFormat;   
    OMX_COLOR_FORMATTYPE omxFormat;  
};

static const struct formatMap kFormatMap[] = {
    {H264ENC_YUV420_PLANAR, OMX_COLOR_FormatYUV420Planar},
    {H264ENC_YUV420_SEMIPLANAR, OMX_COLOR_FormatYUV420SemiPlanar},
    {H264ENC_YUV422_INTERLEAVED_YUYV, OMX_COLOR_FormatYCbYCr},
    {H264ENC_YUV422_INTERLEAVED_UYVY, OMX_COLOR_FormatCbYCrY},
    {H264ENC_BGR565, OMX_COLOR_Format16bitRGB565},
    {H264ENC_RGB565, OMX_COLOR_Format16bitBGR565},
    {H264ENC_RGB555, OMX_COLOR_Format16bitARGB1555},
    {H264ENC_RGB444, OMX_COLOR_Format12bitRGB444},
    {H264ENC_BGR888, OMX_COLOR_Format24bitRGB888},
    {H264ENC_RGB888, OMX_COLOR_Format24bitBGR888},
    {H264ENC_BGR888, OMX_COLOR_Format32bitARGB8888},
    {H264ENC_RGB888, OMX_COLOR_Format32bitBGRA8888},
};

static const OMX_U32 kNumEntries = sizeof(kFormatMap) / sizeof(kFormatMap[0]);
static H264EncPictureType getEncFormat(OMX_COLOR_FORMATTYPE format)
{
    OMX_S32 i = 0;
    for(i = 0; i < (OMX_S32)kNumEntries; i++) 
    {
        if(kFormatMap[i].omxFormat == format)
            return kFormatMap[i].encFormat;
    }

    return -1;
}

OMX_U32 OMXAVCProfileToProfileOn2AVC(OMX_VIDEO_AVCPROFILETYPE profile)
{
    OMX_U32 ret = 0; //default OMX_VIDEO_AVCProfileMain

    if (profile == OMX_VIDEO_AVCProfileMain)
        ret = 0;
    else if (profile == OMX_VIDEO_AVCProfileHigh)
        ret = 1;
    else if (profile == OMX_VIDEO_AVCProfileBaseline)
        ret = 2;

    return ret;
}


H264EncLevel OMXAVCLevelToOn2AVCLevel(OMX_VIDEO_AVCLEVELTYPE level)
{
    H264EncLevel ret = H264ENC_LEVEL_4_0; //default OMX_VIDEO_AVCLevel4

    if (level == OMX_VIDEO_AVCLevel1)
        ret = H264ENC_LEVEL_1;
    else if (level == OMX_VIDEO_AVCLevel1b)
        ret = H264ENC_LEVEL_1_b;
    else if (level == OMX_VIDEO_AVCLevel11)
        ret = H264ENC_LEVEL_1_1;
    else if (level == OMX_VIDEO_AVCLevel12)
        ret = H264ENC_LEVEL_1_2;
    else if (level == OMX_VIDEO_AVCLevel13)
        ret = H264ENC_LEVEL_1_3;
    else if (level == OMX_VIDEO_AVCLevel2)
        ret = H264ENC_LEVEL_2;
    else if (level == OMX_VIDEO_AVCLevel21)
        ret = H264ENC_LEVEL_2_1;
    else if (level == OMX_VIDEO_AVCLevel22)
        ret = H264ENC_LEVEL_2_2;
    else if (level == OMX_VIDEO_AVCLevel3)
        ret = H264ENC_LEVEL_3;
    else if (level == OMX_VIDEO_AVCLevel31)
        ret = H264ENC_LEVEL_3_1;
    else if (level == OMX_VIDEO_AVCLevel32)
        ret = H264ENC_LEVEL_3_2;
    else if (level == OMX_VIDEO_AVCLevel4)
        ret = H264ENC_LEVEL_4_0;
    else if (level == OMX_VIDEO_AVCLevel41)
        ret = H264ENC_LEVEL_4_1;


    return ret;
}

OMX_ERRORTYPE IM_H264Enc_GetParameter(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_INDEXTYPE  nParamIndex,
        OMX_INOUT OMX_PTR     pComponentParameterStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL || pComponentParameterStructure == NULL) {
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
        ret = OMX_StateInvalid;
        goto EXIT;
    }

    switch (nParamIndex) {
        case OMX_IndexParamVideoAvc:
            {
                OMX_VIDEO_PARAM_AVCTYPE *pDstAVCComponent = (OMX_VIDEO_PARAM_AVCTYPE *)pComponentParameterStructure;
                OMX_VIDEO_PARAM_AVCTYPE *pSrcAVCComponent = NULL;
                IM_H264ENC_HANDLE      *pH264Enc = NULL;

                ret = IM_OMX_Check_SizeVersion(pDstAVCComponent, sizeof(OMX_VIDEO_PARAM_AVCTYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                if (pDstAVCComponent->nPortIndex >= ALL_PORT_NUM) {
                    ret = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }

                pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
                pSrcAVCComponent = &pH264Enc->AVCComponent[pDstAVCComponent->nPortIndex];

                IM_OSAL_Memcpy(pDstAVCComponent, pSrcAVCComponent, sizeof(OMX_VIDEO_PARAM_AVCTYPE));
            }
            break;
        case OMX_IndexParamStandardComponentRole:
            {
                OMX_PARAM_COMPONENTROLETYPE *pComponentRole = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;
                ret = IM_OMX_Check_SizeVersion(pComponentRole, sizeof(OMX_PARAM_COMPONENTROLETYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                IM_OSAL_Strcpy((char *)pComponentRole->cRole, IM_OMX_COMPONENT_AVC_ENC_ROLE);
            }
            break;
        case OMX_IndexParamVideoProfileLevelQuerySupported:
            {
                OMX_VIDEO_PARAM_PROFILELEVELTYPE *pDstProfileLevel = (OMX_VIDEO_PARAM_PROFILELEVELTYPE*)pComponentParameterStructure;
                IM_OMX_VIDEO_PROFILELEVEL *pProfileLevel = NULL;
                OMX_U32 maxProfileLevelNum = 0;

                ret = IM_OMX_Check_SizeVersion(pDstProfileLevel, sizeof(OMX_VIDEO_PARAM_PROFILELEVELTYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                if (pDstProfileLevel->nPortIndex >= ALL_PORT_NUM) {
                    ret = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }

                pProfileLevel = supportedAVCProfileLevels;
                maxProfileLevelNum = sizeof(supportedAVCProfileLevels) / sizeof(IM_OMX_VIDEO_PROFILELEVEL);

                if (pDstProfileLevel->nProfileIndex >= maxProfileLevelNum) {
                    ret = OMX_ErrorNoMore;
                    goto EXIT;
                }

                pProfileLevel += pDstProfileLevel->nProfileIndex;
                pDstProfileLevel->eProfile = pProfileLevel->profile;
                pDstProfileLevel->eLevel = pProfileLevel->level;
            }
            break;
        case OMX_IndexParamVideoProfileLevelCurrent:
            {
                OMX_VIDEO_PARAM_PROFILELEVELTYPE *pDstProfileLevel = (OMX_VIDEO_PARAM_PROFILELEVELTYPE*)pComponentParameterStructure;
                OMX_VIDEO_PARAM_AVCTYPE *pSrcAVCComponent = NULL;
                IM_H264ENC_HANDLE        *pH264Enc = NULL;

                ret = IM_OMX_Check_SizeVersion(pDstProfileLevel, sizeof(OMX_VIDEO_PARAM_PROFILELEVELTYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                if (pDstProfileLevel->nPortIndex >= ALL_PORT_NUM) {
                    ret = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }

                pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
                pSrcAVCComponent = &pH264Enc->AVCComponent[pDstProfileLevel->nPortIndex];

                pDstProfileLevel->eProfile = pSrcAVCComponent->eProfile;
                pDstProfileLevel->eLevel = pSrcAVCComponent->eLevel;
            }
            break;
        case OMX_IndexParamVideoErrorCorrection:
            {
                OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *pDstErrorCorrectionType = (OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *)pComponentParameterStructure;
                OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *pSrcErrorCorrectionType = NULL;
                IM_H264ENC_HANDLE        *pH264Enc = NULL;

                ret = IM_OMX_Check_SizeVersion(pDstErrorCorrectionType, sizeof(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                if (pDstErrorCorrectionType->nPortIndex != OUTPUT_PORT_INDEX) {
                    ret = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }

                pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
                pSrcErrorCorrectionType = &pH264Enc->errorCorrectionType[OUTPUT_PORT_INDEX];

                pDstErrorCorrectionType->bEnableHEC = pSrcErrorCorrectionType->bEnableHEC;
                pDstErrorCorrectionType->bEnableResync = pSrcErrorCorrectionType->bEnableResync;
                pDstErrorCorrectionType->nResynchMarkerSpacing = pSrcErrorCorrectionType->nResynchMarkerSpacing;
                pDstErrorCorrectionType->bEnableDataPartitioning = pSrcErrorCorrectionType->bEnableDataPartitioning;
                pDstErrorCorrectionType->bEnableRVLC = pSrcErrorCorrectionType->bEnableRVLC;
            }
            break;
        default:
            ret = IM_OMX_VideoEncodeGetParameter(hComponent, nParamIndex, pComponentParameterStructure);
            break;
    }
EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_H264Enc_SetParameter(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_INDEXTYPE  nIndex,
        OMX_IN OMX_PTR        pComponentParameterStructure)
{
    OMX_ERRORTYPE           ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL || pComponentParameterStructure == NULL) {
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
        ret = OMX_StateInvalid;
        goto EXIT;
    }

    switch (nIndex) {
        case OMX_IndexParamVideoAvc:
            {
                OMX_VIDEO_PARAM_AVCTYPE *pDstAVCComponent = NULL;
                OMX_VIDEO_PARAM_AVCTYPE *pSrcAVCComponent = (OMX_VIDEO_PARAM_AVCTYPE *)pComponentParameterStructure;
                IM_H264ENC_HANDLE      *pH264Enc = NULL;

                ret = IM_OMX_Check_SizeVersion(pSrcAVCComponent, sizeof(OMX_VIDEO_PARAM_AVCTYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                if (pSrcAVCComponent->nPortIndex >= ALL_PORT_NUM) {
                    ret = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }

                pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
                pDstAVCComponent = &pH264Enc->AVCComponent[pSrcAVCComponent->nPortIndex];

                IM_OSAL_Memcpy(pDstAVCComponent, pSrcAVCComponent, sizeof(OMX_VIDEO_PARAM_AVCTYPE));
            }
            break;
        case OMX_IndexParamStandardComponentRole:
            {
                OMX_PARAM_COMPONENTROLETYPE *pComponentRole = (OMX_PARAM_COMPONENTROLETYPE*)pComponentParameterStructure;

                ret = IM_OMX_Check_SizeVersion(pComponentRole, sizeof(OMX_PARAM_COMPONENTROLETYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                if ((pIMComponent->currentState != OMX_StateLoaded) && (pIMComponent->currentState != OMX_StateWaitForResources)) {
                    ret = OMX_ErrorIncorrectStateOperation;
                    goto EXIT;
                }

                if (!IM_OSAL_Strcmp((char*)pComponentRole->cRole, IM_OMX_COMPONENT_AVC_ENC_ROLE)) {
                    pIMComponent->pIMPort[OUTPUT_PORT_INDEX].portDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
                } else {
                    ret = OMX_ErrorBadParameter;
                    goto EXIT;
                }
            }
            break;
        case OMX_IndexParamVideoProfileLevelCurrent:
            {
                OMX_VIDEO_PARAM_PROFILELEVELTYPE *pSrcProfileLevel = (OMX_VIDEO_PARAM_PROFILELEVELTYPE *)pComponentParameterStructure;
                OMX_VIDEO_PARAM_AVCTYPE *pDstAVCComponent = NULL;
                IM_H264ENC_HANDLE        *pH264Enc = NULL;

                ret = IM_OMX_Check_SizeVersion(pSrcProfileLevel, sizeof(OMX_VIDEO_PARAM_PROFILELEVELTYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                if (pSrcProfileLevel->nPortIndex >= ALL_PORT_NUM) {
                    ret = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }

                pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;

                pDstAVCComponent = &pH264Enc->AVCComponent[pSrcProfileLevel->nPortIndex];
                pDstAVCComponent->eProfile = pSrcProfileLevel->eProfile;
                pDstAVCComponent->eLevel = pSrcProfileLevel->eLevel;
            }
            break;
        case OMX_IndexParamVideoErrorCorrection:
            {
                OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *pSrcErrorCorrectionType = (OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *)pComponentParameterStructure;
                OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE *pDstErrorCorrectionType = NULL;
                IM_H264ENC_HANDLE        *pH264Enc = NULL;

                ret = IM_OMX_Check_SizeVersion(pSrcErrorCorrectionType, sizeof(OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE));
                if (ret != OMX_ErrorNone) {
                    goto EXIT;
                }

                if (pSrcErrorCorrectionType->nPortIndex != OUTPUT_PORT_INDEX) {
                    ret = OMX_ErrorBadPortIndex;
                    goto EXIT;
                }

                pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
                pDstErrorCorrectionType = &pH264Enc->errorCorrectionType[OUTPUT_PORT_INDEX];

                pDstErrorCorrectionType->bEnableHEC = pSrcErrorCorrectionType->bEnableHEC;
                pDstErrorCorrectionType->bEnableResync = pSrcErrorCorrectionType->bEnableResync;
                pDstErrorCorrectionType->nResynchMarkerSpacing = pSrcErrorCorrectionType->nResynchMarkerSpacing;
                pDstErrorCorrectionType->bEnableDataPartitioning = pSrcErrorCorrectionType->bEnableDataPartitioning;
                pDstErrorCorrectionType->bEnableRVLC = pSrcErrorCorrectionType->bEnableRVLC;
            }
            break;

#ifdef USE_ANDROID_EXTENSION
        case OMX_IndexParmaPrependSpsPpsIdrFrame:
            ret = OMX_ErrorBadParameter;//OMX_ErrorNone;
            break;
        case OMX_IndexParamUseMMU:
            {
                IM_OSAL_Info("set OMX_IndexParamUseMMU");
                IM_H264ENC_HANDLE   *pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;

#ifdef VENC_MMU
                pH264Enc->bUseMMU = *(OMX_BOOL *)pComponentParameterStructure;
                pIMComponent->useMMU = *(OMX_BOOL *)pComponentParameterStructure;
#else
                pH264Enc->bUseMMU = OMX_FALSE;
                pIMComponent->useMMU = OMX_FALSE;
#endif
                ret = OMX_ErrorNone;
                break;
            }
#endif 
        default:
            ret = IM_OMX_VideoEncodeSetParameter(hComponent, nIndex, pComponentParameterStructure);
            break;
    }
EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_H264Enc_GetConfig(
        OMX_HANDLETYPE hComponent,
        OMX_INDEXTYPE nIndex,
        OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE           ret = OMX_ErrorNone;
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

    switch (nIndex) {
        default:
            ret = IM_OMX_GetConfig(hComponent, nIndex, pComponentConfigStructure);
            break;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_H264Enc_SetConfig(
        OMX_HANDLETYPE hComponent,
        OMX_INDEXTYPE nIndex,
        OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE     *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL || pComponentConfigStructure == NULL) {
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

    switch (nIndex) {
        case OMX_IndexConfigVideoIntraVOPRefresh:
            {
                IM_H264ENC_HANDLE        *pH264Enc = NULL;
                OMX_CONFIG_INTRAREFRESHVOPTYPE *params =
                    (OMX_CONFIG_INTRAREFRESHVOPTYPE *)pComponentConfigStructure;

                if (params->nPortIndex != OUTPUT_PORT_INDEX) {
                    return OMX_ErrorBadPortIndex;
                }

                IM_OSAL_Info("OMX_IndexConfigVideoIntraVOPRefresh");
                pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
                pH264Enc->bKeyFrameRequested = params->IntraRefreshVOP;
                pH264Enc->bSpsPpsRequested = params->IntraRefreshVOP;
                return OMX_ErrorNone;
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

OMX_ERRORTYPE IM_H264Enc_GetExtensionIndex(
        OMX_IN OMX_HANDLETYPE  hComponent,
        OMX_IN OMX_STRING      cParameterName,
        OMX_OUT OMX_INDEXTYPE *pIndexType)
{
    OMX_ERRORTYPE           ret = OMX_ErrorNone;
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

#ifdef USE_ANDROID_EXTENSION
    if (IM_OSAL_Strcmp(cParameterName, IM_INDEX_PARAM_STORE_METADATA_BUFFER) == 0) {
        *pIndexType = OMX_IndexParamStoreMetaDataBuffer;
        ret = OMX_ErrorNone;
    } else if (IM_OSAL_Strcmp(cParameterName, IM_INDEX_PARAM_PREPEND_SPSPPS_IDR_FRAME) == 0) { 
        *pIndexType = OMX_IndexParmaPrependSpsPpsIdrFrame;
        ret = OMX_ErrorNone;
    } else if (IM_OSAL_Strcmp(cParameterName, IM_INDEX_PARAM_USE_MMU) == 0) {
        *pIndexType = OMX_IndexParamUseMMU;
        ret = OMX_ErrorNone;
    } else {
        ret = IM_OMX_GetExtensionIndex(hComponent, cParameterName, pIndexType);
    }
#else
    ret = IM_OMX_GetExtensionIndex(hComponent, cParameterName, pIndexType);
#endif

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_H264Enc_ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex)
{
    OMX_ERRORTYPE            ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE       *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT   *pIMComponent = NULL;

    IM_OSAL_Entering();

    if ((hComponent == NULL) || (cRole == NULL)) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (nIndex == (MAX_COMPONENT_ROLE_NUM-1)) {
        IM_OSAL_Strcpy((char *)cRole, IM_OMX_COMPONENT_AVC_ENC_ROLE);
        ret = OMX_ErrorNone;
    } else {
        ret = OMX_ErrorNoMore;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_H264_VENC_Flush(OMX_COMPONENTTYPE *pOMXComponent, OMX_U32 nPortIndex)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    return ret;
}

typedef struct MEMORY_HANDLE {
    OMX_HANDLETYPE alc_inst;
    OMX_HANDLETYPE dmmu_inst;
} MEMORY_HANDLE;
/* On2 H264Enc Init */
OMX_ERRORTYPE IM_H264Enc_Init(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE              ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT     *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_BASEPORT          *pIMInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    IM_OMX_BASEPORT          *pIMOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    OMX_VIDEO_PORTDEFINITIONTYPE *video_def = &pIMInputPort->portDefinition.format.video;
    IM_H264ENC_HANDLE        *pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
    OMX_VIDEO_PARAM_AVCTYPE  *avcType;

    OMX_PTR                    hOn2EncHandle = NULL;
    OMX_S32                    returnCodec = 0;
    H264EncConfig               *pEncConfig = &pH264Enc->hOn2EncHandle.h264EncConfig;
    H264EncInst               pEncInst;
    H264EncRet                    encRet;


    IM_OSAL_Entering();

    IM_OSAL_ContiguousAllocator_Create(&pH264Enc->hMemoryHandle, pH264Enc->bUseMMU, OMX_TRUE);
    
    pH264Enc->bFirstFrame = OMX_TRUE; 

    pIMComponent->bUseFlagEOF = OMX_FALSE;
    pIMComponent->bSaveFlagEOS = OMX_FALSE;

    //TODO: init On2 enc lib
    pEncConfig->streamType = H264ENC_BYTE_STREAM;
    pEncConfig->level = OMXAVCLevelToOn2AVCLevel(pH264Enc->AVCComponent[OUTPUT_PORT_INDEX].eLevel); 
    pEncConfig->width = video_def->nFrameWidth;
    pEncConfig->height = video_def->nFrameHeight;
    pEncConfig->complexityLevel = H264ENC_COMPLEXITY_1;
    pEncConfig->frameRateNum  = video_def->xFramerate >> 16; 
    pEncConfig->frameRateDenom = 1;
    pEncConfig->useMMU = pH264Enc->bUseMMU;      /* use MMU or not */
    pEncConfig->dmmuInst = ((MEMORY_HANDLE *)pH264Enc->hMemoryHandle)->dmmu_inst;

    IM_OSAL_Info("EncConfig::level = %d", pEncConfig->level);
    IM_OSAL_Info("EncConfig::width = %d", pEncConfig->width);
    IM_OSAL_Info("EncConfig::height = %d", pEncConfig->height);
    IM_OSAL_Info("EncConfig::frameRateNum = %d", pEncConfig->frameRateNum);
    IM_OSAL_Info("EncConfig::useMMU = %d", pEncConfig->useMMU);

    encRet = H264EncInit(pEncConfig, &pEncInst);
    if (encRet != H264ENC_OK  || pEncInst == NULL) {
        IM_OSAL_Error("H264EncInit Error, ret 0x%x, pEncInst 0x%x", encRet, pEncInst);
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    pH264Enc->hOn2EncHandle.h264EncInst = pEncInst;
    IM_OSAL_Memset(pIMComponent->timeStamp, -19771003, sizeof(OMX_TICKS) * MAX_TIMESTAMP);
    IM_OSAL_Memset(pIMComponent->nFlags, 0, sizeof(OMX_U32) * MAX_FLAGS);

    if(pIMOutputPort->portDefinition.bBuffersContiguous == OMX_FALSE){
        pH264Enc->pSelfOutBuffer = IM_OSAL_ContiguousBuffer_Malloc(pH264Enc->hMemoryHandle, \
                video_def->nFrameWidth * video_def->nFrameHeight *3 /2);
        if(pH264Enc->pSelfOutBuffer == NULL){
            ret = OMX_ErrorInsufficientResources;
            IM_OSAL_Error("IM_OSAL_Malloc  Malloc  sel out buffer failed");
            goto EXIT;
        }    
    } 

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

/* On2 Terminate */
OMX_ERRORTYPE IM_H264Enc_Terminate(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE          ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_H264ENC_HANDLE    *pH264Enc = NULL;
    OMX_PTR                hOn2EncHandle = NULL;

    IM_OSAL_Entering();

    IM_OSAL_Error("IM_H264Enc_Terminate");
    pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
    if (pH264Enc != NULL) {
        if(pH264Enc->hOn2EncHandle.h264EncInst != NULL) {
            H264EncRelease(pH264Enc->hOn2EncHandle.h264EncInst);
        }

        if(pH264Enc->hMemoryHandle != NULL) {
            if(pH264Enc->pSelfOutBuffer != NULL) {
                IM_OSAL_ContiguousBuffer_Free(pH264Enc->hMemoryHandle, pH264Enc->pSelfOutBuffer);
            }
            if(pH264Enc->pSelfInBuffer != NULL) {
                IM_OSAL_ContiguousBuffer_Free(pH264Enc->hMemoryHandle, pH264Enc->pSelfInBuffer);
            }
            IM_OSAL_ContiguousAllocator_Destroy(pH264Enc->hMemoryHandle);
        }

        if(pH264Enc->pSpsPps != NULL) {
            IM_OSAL_Free(pH264Enc->pSpsPps);
            pH264Enc->pSpsPps = NULL;
        }

    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_U32 getRealInputBufferSize(OMX_VIDEO_PORTDEFINITIONTYPE *video_def, OMX_COLOR_FORMATTYPE colorFormat) 
{   
    OMX_U32 bufferSize = video_def->nFrameWidth * video_def->nFrameHeight;
    switch (colorFormat) 
    {
        case OMX_COLOR_FormatYUV420Planar:
        case OMX_COLOR_FormatYUV420SemiPlanar:
            bufferSize = (bufferSize * 3) / 2;
            break;
        case OMX_COLOR_FormatYCbYCr:
        case OMX_COLOR_FormatCbYCrY:
        case OMX_COLOR_Format16bitRGB565:
        case OMX_COLOR_Format16bitBGR565:
        case OMX_COLOR_Format16bitARGB1555:
        case OMX_COLOR_Format12bitRGB444:
            bufferSize *= 2;
            break;
        case OMX_COLOR_Format24bitRGB888:
        case OMX_COLOR_Format24bitBGR888:
        case OMX_COLOR_Format32bitARGB8888:
        case OMX_COLOR_Format32bitBGRA8888:
            bufferSize *= 4;
            break;
        default:
            bufferSize *= 2;
    }

    return bufferSize;
}

OMX_ERRORTYPE IM_H264_Encode(OMX_COMPONENTTYPE *pOMXComponent, IM_OMX_DATABUFFER *pInputData, IM_OMX_DATABUFFER  *pOutputData)
{
    OMX_ERRORTYPE              ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT     *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_H264ENC_HANDLE        *pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
    IM_OMX_BASEPORT          *pIMInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    IM_OMX_BASEPORT          *pIMOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    OMX_VIDEO_PORTDEFINITIONTYPE *video_def = &pIMInputPort->portDefinition.format.video;
    OMX_VIDEO_PARAM_AVCTYPE  *avcType = &pH264Enc->AVCComponent[OUTPUT_PORT_INDEX];
    H264EncRet                encRet;
    static struct timeval   sLastGopEncodeTime = {0, 0};
    /*
    struct timeval          start = {0, 0}, stop = {0, 0};
    gettimeofday(&start, 0);
    */
    IM_OSAL_Entering();
    //operation out buffer 
    if(pIMOutputPort->portDefinition.bBuffersContiguous == OMX_TRUE) {
        pH264Enc->hOn2EncHandle.h264EncIn.busOutBuf = getPhyAddrfromNB(pOutputData->bufferHeader->pBuffer, pH264Enc->bUseMMU);
        pH264Enc->hOn2EncHandle.h264EncIn.pOutBuf = getVirtAddrfromNB(pOutputData->bufferHeader->pBuffer);
        pH264Enc->hOn2EncHandle.h264EncIn.outBufSize = pOutputData->bufferHeader->nAllocLen;
    } else {
        pH264Enc->hOn2EncHandle.h264EncIn.busOutBuf = getPhyAddrfromNB(pH264Enc->pSelfOutBuffer, pH264Enc->bUseMMU);
        pH264Enc->hOn2EncHandle.h264EncIn.pOutBuf = getVirtAddrfromNB(pH264Enc->pSelfOutBuffer);
        pH264Enc->hOn2EncHandle.h264EncIn.outBufSize = video_def->nFrameWidth * video_def->nFrameHeight * 3 /2;
        IM_OSAL_Info("out vir addr = %p, phy addr = 0x%x, size = %d",  pH264Enc->hOn2EncHandle.h264EncIn.pOutBuf, pH264Enc->hOn2EncHandle.h264EncIn.busOutBuf,pH264Enc->hOn2EncHandle.h264EncIn.outBufSize);
    }

    if (pH264Enc->bFirstFrame == OMX_TRUE) {
        OMX_COLOR_FORMATTYPE colorFormat = video_def->eColorFormat;
        H264EncPictureType encFormat;
        H264EncRet encRet;

        if(colorFormat == OMX_COLOR_FormatAndroidOpaque) {
            if (pInputData->bufferHeader->nFilledLen == 0)
                goto EXIT;

            colorFormat = getAndroidNativeBufferFormat(pInputData->bufferHeader->pBuffer);
            IM_OSAL_Info("android native buffer format = %x", colorFormat);
            if(OMX_COLOR_FormatUnused == colorFormat) {
                pOutputData->bufferHeader->nFilledLen = 0;//pH264Enc->hOn2EncHandle.h264EncOut.streamSize;
                pOutputData->bufferHeader->nFlags = pInputData->bufferHeader->nFlags;
                pOutputData->bufferHeader->nFlags |= OMX_BUFFERFLAG_EOS;
                goto EXIT;
            }
        }

        encFormat = getEncFormat(colorFormat);
        IM_OSAL_Info("android color format = %x, encFormat = %x", colorFormat, encFormat);

        pH264Enc->inSize = getRealInputBufferSize(video_def, colorFormat);

        encRet = H264EncGetCodingCtrl(pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncCodingCtrl);
        if(encRet != H264ENC_OK) {
            IM_OSAL_Error("H264EncGetRateCtrl failed, encRet = %d, h264EncInst = %x, h264EncRateCtrl = %x", encRet, pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncRateCtrl);
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }
        pH264Enc->hOn2EncHandle.h264EncCodingCtrl.videoFullRange = 1;
        encRet = H264EncSetCodingCtrl(pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncCodingCtrl);
        if(encRet != H264ENC_OK) {
            IM_OSAL_Error("H264EncGetRateCtrl failed, encRet = %d, h264EncInst = %x, h264EncRateCtrl = %x", encRet, pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncRateCtrl);
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }
        encRet = H264EncGetRateCtrl(pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncRateCtrl);
        if(encRet != H264ENC_OK) {
            IM_OSAL_Error("H264EncGetRateCtrl failed, encRet = %d, h264EncInst = %x, h264EncRateCtrl = %x", encRet, pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncRateCtrl);
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }

        IM_OSAL_Info("rate ctrl:pictureRc = %d, mbRc = %d, qpHdr = %d, qpMax = %d, qpMin = %d, hrd = %d", 
                pH264Enc->hOn2EncHandle.h264EncRateCtrl.pictureRc, 
                pH264Enc->hOn2EncHandle.h264EncRateCtrl.mbRc,
                pH264Enc->hOn2EncHandle.h264EncRateCtrl.qpHdr,
                pH264Enc->hOn2EncHandle.h264EncRateCtrl.qpMax,
                pH264Enc->hOn2EncHandle.h264EncRateCtrl.qpMin,
                pH264Enc->hOn2EncHandle.h264EncRateCtrl.hrd);
        if(pIMOutputPort->eControlRate == OMX_Video_ControlRateConstant) {
            IM_OSAL_Error("config constant bit rate");
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.pictureRc = 1;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.mbRc = 1;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.pictureSkip = 0;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.qpHdr = -1;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.qpMax = 51;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.qpMin = 10;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.hrd = 1;
        } else {
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.pictureRc = 1;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.mbRc = 0;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.pictureSkip = 0;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.qpHdr = 25;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.qpMax = 45;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.qpMin = 25;
            pH264Enc->hOn2EncHandle.h264EncRateCtrl.hrd = 0;
        }
        IM_OSAL_Info("pFrames = %d, videoFullRange = %d", avcType->nPFrames, pH264Enc->hOn2EncHandle.h264EncCodingCtrl.videoFullRange);
        pH264Enc->hOn2EncHandle.h264EncRateCtrl.gopLen = avcType->nPFrames > 149 ? 150 :avcType->nPFrames + 1;
        pH264Enc->hOn2EncHandle.h264EncRateCtrl.bitPerSecond = pIMOutputPort->portDefinition.format.video.nBitrate;
        IM_OSAL_Info("bit rate = %d", pH264Enc->hOn2EncHandle.h264EncRateCtrl.bitPerSecond);
        encRet = H264EncSetRateCtrl(pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncRateCtrl);

        if(encRet != H264ENC_OK) {
            IM_OSAL_Error("H264EncSetRateCtrl failed, encRet = %d", encRet);
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }

        encRet = H264EncGetPreProcessing(pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncPreProc);
        if(encRet != H264ENC_OK) {
            IM_OSAL_Error("H264EncGetPreProcessing failed, encRet = %d", encRet);
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }

        pH264Enc->hOn2EncHandle.h264EncPreProc.origWidth = video_def->nFrameWidth;
        pH264Enc->hOn2EncHandle.h264EncPreProc.origHeight = video_def->nFrameHeight;
        pH264Enc->hOn2EncHandle.h264EncPreProc.inputType = encFormat;
        encRet = H264EncSetPreProcessing(pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncPreProc);
        if(encRet != H264ENC_OK) {
            IM_OSAL_Error("H264EncSetPreProcessing failed, encRet = %d", encRet);
            ret = OMX_ErrorUndefined;
            goto EXIT;
        }

        //TODO:
        //deal H264ENC_NAL_UNIT CASE

        encRet = H264EncStrmStart(pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncIn, &pH264Enc->hOn2EncHandle.h264EncOut);
        if(encRet != H264ENC_OK) {
            IM_OSAL_Error("H264EncStrmStart failed, encRet = %d", encRet);
            ret = OMX_ErrorInsufficientResources;
            goto EXIT;
        }


        pOutputData->bufferHeader->nFilledLen = pH264Enc->hOn2EncHandle.h264EncOut.streamSize;
        pOutputData->bufferHeader->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;
        IM_OSAL_Info("sps pps Size = %d", pH264Enc->hOn2EncHandle.h264EncOut.streamSize);

        //save spspps data
        pH264Enc->pSpsPps = IM_OSAL_Malloc(pH264Enc->hOn2EncHandle.h264EncOut.streamSize);
        pH264Enc->spsPpsSize = pH264Enc->hOn2EncHandle.h264EncOut.streamSize;
        IM_OSAL_Memcpy(pH264Enc->pSpsPps, pH264Enc->hOn2EncHandle.h264EncIn.pOutBuf, pH264Enc->hOn2EncHandle.h264EncOut.streamSize);


        if(pIMOutputPort->portDefinition.bBuffersContiguous == OMX_FALSE) {
            IM_OSAL_Memcpy(pOutputData->bufferHeader->pBuffer, pH264Enc->hOn2EncHandle.h264EncIn.pOutBuf, pH264Enc->hOn2EncHandle.h264EncOut.streamSize);
        }

        pInputData->nRemainDataLeft = pInputData->bufferHeader->nFilledLen;
        pH264Enc->bFirstFrame = OMX_FALSE;
        goto EXIT;
    } 

    if(pH264Enc->bSpsPpsRequested == OMX_TRUE) {
        IM_OSAL_Warning("sps pps requested");
        pOutputData->bufferHeader->nFilledLen = pH264Enc->spsPpsSize;
        pOutputData->bufferHeader->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;
        if(pIMOutputPort->portDefinition.bBuffersContiguous == OMX_FALSE) {
            IM_OSAL_Memcpy(pOutputData->bufferHeader->pBuffer, pH264Enc->pSpsPps, pH264Enc->spsPpsSize);
        } else {
            IM_OSAL_Memcpy(pH264Enc->hOn2EncHandle.h264EncIn.pOutBuf, pH264Enc->pSpsPps, pH264Enc->spsPpsSize);
        }
        pInputData->nRemainDataLeft = pInputData->bufferHeader->nFilledLen;
        pH264Enc->bSpsPpsRequested = OMX_FALSE;
        goto EXIT;
    }

    if ((pInputData->bufferHeader->nFlags & OMX_BUFFERFLAG_ENDOFFRAME) &&
            (pIMComponent->bUseFlagEOF == OMX_FALSE)) {
        pIMComponent->bUseFlagEOF = OMX_TRUE;
        //goto EXIT;
    } 

    if ((pInputData->bufferHeader->nFlags & OMX_BUFFERFLAG_EOS) == OMX_BUFFERFLAG_EOS){
        IM_OSAL_Info("OMX_BUFFERFLAG_EOS");
        encRet = H264EncStrmEnd(pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncIn, &pH264Enc->hOn2EncHandle.h264EncOut);
        if(encRet != H264ENC_OK) {
            ret = OMX_ErrorInsufficientResources;
            goto EXIT;
        }

        pOutputData->bufferHeader->nFilledLen = 0;//pH264Enc->hOn2EncHandle.h264EncOut.streamSize;
        pOutputData->bufferHeader->nFlags = pInputData->bufferHeader->nFlags;
        pOutputData->bufferHeader->nFlags |= OMX_BUFFERFLAG_EOS;
        //pOutputData->bufferHeader->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;
        pOutputData->bufferHeader->nTimeStamp = pH264Enc->timeInterval + pH264Enc->lastTimeStamp;

        if(pIMOutputPort->portDefinition.bBuffersContiguous == OMX_FALSE) {
            IM_OSAL_Memcpy(pOutputData->bufferHeader->pBuffer, pH264Enc->hOn2EncHandle.h264EncIn.pOutBuf, pH264Enc->hOn2EncHandle.h264EncOut.streamSize);
        }
        goto EXIT;
#ifdef USE_ANDROID_EXTENSION
    } else if (pIMInputPort->bStoreMetaDataInBuffer != OMX_FALSE) {
        ret = preprocessMetaDataInBuffers(pOMXComponent, pH264Enc->hMemoryHandle, (OMX_PTR)pInputData);
        if (ret != OMX_ErrorNone) { 
            IM_OSAL_Error("preprocessMetaDataInBuffers failed, err 0x%x", ret);
            goto EXIT;
        }
#endif
    }

    if (pInputData->phyAddr == 0) {
        OMX_PTR  tmpVir;
        OMX_U32  tmpPhy;
        if(pH264Enc->hMemoryHandle == NULL) {
            IM_OSAL_ContiguousAllocator_Create(&pH264Enc->hMemoryHandle, pH264Enc->bUseMMU, OMX_TRUE);
        }
        if(pH264Enc->pSelfInBuffer == NULL) {
            pH264Enc->pSelfInBuffer = IM_OSAL_ContiguousBuffer_Malloc(pH264Enc->hMemoryHandle, pH264Enc->inSize);
            IM_OSAL_Info("in Size = %d", pH264Enc->inSize);
            if(pH264Enc->pSelfInBuffer == NULL){
                ret = OMX_ErrorInsufficientResources;
                IM_OSAL_Error("IM_OSAL_Malloc  Malloc  sel out buffer failed");
                goto EXIT;
            } 
        }

        tmpPhy = getPhyAddrfromNB(pH264Enc->pSelfInBuffer, pH264Enc->bUseMMU);
        tmpVir= getVirtAddrfromNB(pH264Enc->pSelfInBuffer);
        pInputData->dataSize = pInputData->bufferHeader->nFilledLen;
        IM_OSAL_Info("in data size = %d ", pInputData->dataSize);
        if (pInputData->virAddr == NULL)
            IM_OSAL_Memcpy(tmpVir, pInputData->bufferHeader->pBuffer, pInputData->dataSize);
        else 
            IM_OSAL_Memcpy(tmpVir, pInputData->virAddr, pInputData->dataSize);
        pInputData->virAddr = tmpVir;
        pInputData->phyAddr = tmpPhy;
    }

    switch(pH264Enc->hOn2EncHandle.h264EncPreProc.inputType)
    {
        case H264ENC_YUV420_PLANAR:
            pH264Enc->hOn2EncHandle.h264EncIn.busLuma = pInputData->phyAddr;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaU = pInputData->phyAddr + video_def->nFrameWidth * video_def->nFrameHeight;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaV = pH264Enc->hOn2EncHandle.h264EncIn.busChromaU + (video_def->nFrameWidth / 2) * (video_def->nFrameHeight / 2);
            break;
        case H264ENC_YUV420_SEMIPLANAR:
            pH264Enc->hOn2EncHandle.h264EncIn.busLuma = pInputData->phyAddr;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaU = pInputData->phyAddr + video_def->nFrameWidth * video_def->nFrameHeight;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaV = pInputData->phyAddr + video_def->nFrameWidth * video_def->nFrameHeight;
            break;
        case H264ENC_YUV422_INTERLEAVED_YUYV:
            //TODO::
            //
            pH264Enc->hOn2EncHandle.h264EncIn.busLuma = pInputData->phyAddr;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaU = pInputData->phyAddr + video_def->nFrameWidth * video_def->nFrameHeight;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaV = 0;
            break;
        case H264ENC_RGB565:
            pH264Enc->hOn2EncHandle.h264EncIn.busLuma = pInputData->phyAddr;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaU = 0;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaV = 0;
            break;
        case H264ENC_RGB888:
            pH264Enc->hOn2EncHandle.h264EncIn.busLuma = pInputData->phyAddr;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaU = 0;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaV = 0;
            break;
        case H264ENC_BGR888:
            pH264Enc->hOn2EncHandle.h264EncIn.busLuma = pInputData->phyAddr;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaU = 0;
            pH264Enc->hOn2EncHandle.h264EncIn.busChromaV = 0;
            break;
        default:
            ret = OMX_ErrorBadParameter;
            IM_OSAL_Error("does not support the format %x", pH264Enc->hOn2EncHandle.h264EncPreProc.inputType);
    }

    pH264Enc->hOn2EncHandle.h264EncIn.timeIncrement = pH264Enc->frameCount == 0 ? 0 : 1;
    if(pH264Enc->frameCount % pH264Enc->hOn2EncHandle.h264EncRateCtrl.gopLen == 0) {
        /* tune bus frequency */
        H264EncConfig *pEncConfig = &pH264Enc->hOn2EncHandle.h264EncConfig;

        struct timeval          gopEncodeTime = {0, 0};
        gettimeofday(&gopEncodeTime, 0);
        if (pH264Enc->frameCount != 0) {
            OMX_U32 deltaMs = (gopEncodeTime.tv_sec - sLastGopEncodeTime.tv_sec) * 1000
                            + (gopEncodeTime.tv_usec - sLastGopEncodeTime.tv_usec) / 1000;
            OMX_U32 bestDeltaMs = 1000 * pH264Enc->hOn2EncHandle.h264EncRateCtrl.gopLen * pEncConfig->frameRateDenom / pEncConfig->frameRateNum;

            H264EncTuneFreq(pH264Enc->hOn2EncHandle.h264EncInst, (int)((deltaMs << 10) / bestDeltaMs));

            IM_OSAL_Error("H264EncStrmEncode gop deltaMs = %u, bestDeltaMs %u, ratio %d",
                    deltaMs, bestDeltaMs, (int)(deltaMs << 10) / bestDeltaMs);
        }
        sLastGopEncodeTime = gopEncodeTime;

        //IM_OSAL_Error("request encode sync frame, gopLen = %d", pH264Enc->hOn2EncHandle.h264EncRateCtrl.gopLen);
        pH264Enc->hOn2EncHandle.h264EncIn.codingType = H264ENC_INTRA_FRAME;
    } else {
        pH264Enc->hOn2EncHandle.h264EncIn.codingType = H264ENC_PREDICTED_FRAME;
    } 

    if(pH264Enc->bKeyFrameRequested) {
        IM_OSAL_Info("idr frame requested");
        pH264Enc->hOn2EncHandle.h264EncIn.codingType = H264ENC_INTRA_FRAME;
        pH264Enc->bKeyFrameRequested = OMX_FALSE;
    }
    pH264Enc->frameCount++;
    encRet = H264EncStrmEncode(pH264Enc->hOn2EncHandle.h264EncInst, &pH264Enc->hOn2EncHandle.h264EncIn, &pH264Enc->hOn2EncHandle.h264EncOut);

#ifdef USE_ANDROID_EXTENSION
    if (pIMInputPort->bStoreMetaDataInBuffer != OMX_FALSE) {
        postprocessMetaDataInBuffers(pOMXComponent,  pH264Enc->hMemoryHandle, (OMX_PTR)pInputData);
    }
#endif

    pInputData->bHardwareUsed = OMX_TRUE;
    pInputData->bDataProcessed = OMX_TRUE;
    pInputData->nRemainDataLeft = 0;
    pOutputData->bufferHeader->nFlags = pInputData->bufferHeader->nFlags;
    if(encRet == H264ENC_FRAME_READY) {
        //IM_OSAL_Error("encode Stream Size = %d", pH264Enc->hOn2EncHandle.h264EncOut.streamSize);
        pOutputData->bufferHeader->nFilledLen = pH264Enc->hOn2EncHandle.h264EncOut.streamSize;
        if(pH264Enc->hOn2EncHandle.h264EncOut.codingType == H264ENC_INTRA_FRAME) {
            //IM_OSAL_Error("encode II Stream Size");
            pOutputData->bufferHeader->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
        }
        pOutputData->bufferHeader->nTimeStamp= pInputData->bufferHeader->nTimeStamp;
        IM_OSAL_Info("encode out timestamp %lld", pOutputData->bufferHeader->nTimeStamp);
        pH264Enc->lastTimeStamp = pOutputData->bufferHeader->nTimeStamp;
        if(pH264Enc->frameCount > 0)
            pH264Enc->timeInterval = pOutputData->bufferHeader->nTimeStamp - pH264Enc->lastTimeStamp;
        if(pIMOutputPort->portDefinition.bBuffersContiguous == OMX_FALSE) {
            IM_OSAL_Memcpy(pOutputData->bufferHeader->pBuffer, pH264Enc->hOn2EncHandle.h264EncIn.pOutBuf, pH264Enc->hOn2EncHandle.h264EncOut.streamSize);
        }
    } else {
        IM_OSAL_Error("H264EncStrmEncode failed, encRet = %d", encRet);
        ret = OMX_ErrorUndefined;
    }
    /*
    gettimeofday(&stop, 0);
    OMX_U32 singleFrameTimeMs = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000;
    IM_OSAL_Error("H264EncStrmEncode single frame %u", singleFrameTimeMs);
    */
EXIT:
    IM_OSAL_Exiting();

    return ret;
}

/* MFC Encode */
OMX_ERRORTYPE IM_H264Enc_bufferProcess(OMX_COMPONENTTYPE *pOMXComponent, IM_OMX_DATABUFFER *pInputData, IM_OMX_DATABUFFER *pOutputData)
{
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT   *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_H264ENC_HANDLE      *pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
    IM_OMX_BASEPORT        *pIMInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    IM_OMX_BASEPORT        *pIMOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    OMX_BOOL                 endOfFrame = OMX_FALSE;
    OMX_BOOL                 flagEOS = OMX_FALSE;

    IM_OSAL_Entering();

    if ((!CHECK_PORT_ENABLED(pIMInputPort)) || (!CHECK_PORT_ENABLED(pIMOutputPort)) ||
            (!CHECK_PORT_POPULATED(pIMInputPort)) || (!CHECK_PORT_POPULATED(pIMOutputPort))) {
        ret = OMX_ErrorNone;
        goto EXIT;
    }
    if (OMX_FALSE == IM_Check_BufferProcess_State(pIMComponent)) {
        ret = OMX_ErrorNone;
        goto EXIT;
    }


    ret = IM_H264_Encode(pOMXComponent, pInputData, pOutputData);
    if (ret != OMX_ErrorNone) {
        pIMComponent->pCallbacks->EventHandler((OMX_HANDLETYPE)pOMXComponent,
                pIMComponent->callbackData,
                OMX_EventError, ret, 0, NULL);
    } else {

        /* pOutputData->usedDataLen = 0; */
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OSCL_EXPORT_REF OMX_ERRORTYPE IM_OMX_ComponentInit(OMX_HANDLETYPE hComponent, OMX_STRING componentName)
{
    OMX_ERRORTYPE            ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE       *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT   *pIMComponent = NULL;
    IM_OMX_BASEPORT        *pIMPort = NULL;
    IM_H264ENC_HANDLE      *pH264Enc = NULL;
    int i = 0;

    IM_OSAL_Entering();

    if ((hComponent == NULL) || (componentName == NULL)) {
        ret = OMX_ErrorBadParameter;
        IM_OSAL_Error("OMX_ErrorBadParameter, Line:%d", __LINE__);
        goto EXIT;
    }
    if (IM_OSAL_Strcmp(IM_OMX_COMPONENT_INFOTM_VENC, componentName) != 0) {
        ret = OMX_ErrorBadParameter;
        IM_OSAL_Error("OMX_ErrorBadParameter, componentName:%s, Line:%d", componentName, __LINE__);
        goto EXIT;
    }

    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_VideoEncodeComponentInit(pOMXComponent);
    if (ret != OMX_ErrorNone) {
        IM_OSAL_Error("OMX_Error, Line:%d", __LINE__);
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    pIMComponent->codecType = HW_VIDEO_CODEC;

    pIMComponent->componentName = (OMX_STRING)IM_OSAL_Malloc(MAX_OMX_COMPONENT_NAME_SIZE);
    if (pIMComponent->componentName == NULL) {
        IM_OMX_VideoEncodeComponentDeinit(pOMXComponent);
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }
    IM_OSAL_Memset(pIMComponent->componentName, 0, MAX_OMX_COMPONENT_NAME_SIZE);

    pH264Enc = IM_OSAL_Malloc(sizeof(IM_H264ENC_HANDLE));
    if (pH264Enc == NULL) {
        IM_OMX_VideoEncodeComponentDeinit(pOMXComponent);
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }
    IM_OSAL_Memset(pH264Enc, 0, sizeof(IM_H264ENC_HANDLE));
    pIMComponent->hCodecHandle = (OMX_HANDLETYPE)pH264Enc;

    IM_OSAL_Strcpy(pIMComponent->componentName, IM_OMX_COMPONENT_INFOTM_VENC);
    /* Set componentVersion */
    pIMComponent->componentVersion.s.nVersionMajor = VERSIONMAJOR_NUMBER;
    pIMComponent->componentVersion.s.nVersionMinor = VERSIONMINOR_NUMBER;
    pIMComponent->componentVersion.s.nRevision     = REVISION_NUMBER;
    pIMComponent->componentVersion.s.nStep         = STEP_NUMBER;
    /* Set specVersion */
    pIMComponent->specVersion.s.nVersionMajor = VERSIONMAJOR_NUMBER;
    pIMComponent->specVersion.s.nVersionMinor = VERSIONMINOR_NUMBER;
    pIMComponent->specVersion.s.nRevision     = REVISION_NUMBER;
    pIMComponent->specVersion.s.nStep         = STEP_NUMBER;

    /* Android CapabilityFlags */
    pIMComponent->capabilityFlags.iIsOMXComponentMultiThreaded                   = OMX_TRUE;
    pIMComponent->capabilityFlags.iOMXComponentSupportsExternalInputBufferAlloc  = OMX_TRUE;
    pIMComponent->capabilityFlags.iOMXComponentSupportsExternalOutputBufferAlloc = OMX_TRUE;
    pIMComponent->capabilityFlags.iOMXComponentSupportsMovableInputBuffers       = OMX_FALSE;
    pIMComponent->capabilityFlags.iOMXComponentSupportsPartialFrames             = OMX_FALSE;
    pIMComponent->capabilityFlags.iOMXComponentUsesNALStartCodes                 = OMX_TRUE;
    pIMComponent->capabilityFlags.iOMXComponentCanHandleIncompleteFrames         = OMX_TRUE;
    pIMComponent->capabilityFlags.iOMXComponentUsesFullAVCFrames                 = OMX_TRUE;

    /* Input port */
    pIMPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    pIMPort->portDefinition.format.video.nFrameWidth = DEFAULT_FRAME_WIDTH;
    pIMPort->portDefinition.format.video.nFrameHeight= DEFAULT_FRAME_HEIGHT;
    pIMPort->portDefinition.format.video.nStride = 0; /*DEFAULT_FRAME_WIDTH;*/
    pIMPort->portDefinition.nBufferSize = DEFAULT_VIDEO_INPUT_BUFFER_SIZE;
    pIMPort->portDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
    IM_OSAL_Memset(pIMPort->portDefinition.format.video.cMIMEType, 0, MAX_OMX_MIMETYPE_SIZE);
    IM_OSAL_Strcpy(pIMPort->portDefinition.format.video.cMIMEType, "raw/video");
    pIMPort->portDefinition.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
    pIMPort->portDefinition.bEnabled = OMX_TRUE;

    /* Output port */
    pIMPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    pIMPort->portDefinition.format.video.nFrameWidth = DEFAULT_FRAME_WIDTH;
    pIMPort->portDefinition.format.video.nFrameHeight= DEFAULT_FRAME_HEIGHT;
    pIMPort->portDefinition.format.video.nStride = 0; /*DEFAULT_FRAME_WIDTH;*/
    pIMPort->portDefinition.nBufferSize = DEFAULT_VIDEO_OUTPUT_BUFFER_SIZE;
    pIMPort->portDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
    IM_OSAL_Memset(pIMPort->portDefinition.format.video.cMIMEType, 0, MAX_OMX_MIMETYPE_SIZE);
    IM_OSAL_Strcpy(pIMPort->portDefinition.format.video.cMIMEType, "video/avc");
    pIMPort->portDefinition.format.video.eColorFormat = OMX_COLOR_FormatUnused;
    pIMPort->portDefinition.bEnabled = OMX_TRUE;

    for(i = 0; i < ALL_PORT_NUM; i++) {
        INIT_SET_SIZE_VERSION(&pH264Enc->AVCComponent[i], OMX_VIDEO_PARAM_AVCTYPE);
        pH264Enc->AVCComponent[i].nPortIndex = i;
        pH264Enc->AVCComponent[i].eProfile   = OMX_VIDEO_AVCProfileBaseline;
        pH264Enc->AVCComponent[i].eLevel     = OMX_VIDEO_AVCLevel41;
    }

    pOMXComponent->GetParameter      = &IM_H264Enc_GetParameter;
    pOMXComponent->SetParameter      = &IM_H264Enc_SetParameter;
    pOMXComponent->GetConfig         = &IM_H264Enc_GetConfig;
    pOMXComponent->SetConfig         = &IM_H264Enc_SetConfig;
    pOMXComponent->GetExtensionIndex = &IM_H264Enc_GetExtensionIndex;
    pOMXComponent->ComponentRoleEnum = &IM_H264Enc_ComponentRoleEnum;
    pOMXComponent->ComponentDeInit   = &IM_OMX_ComponentDeinit;

    pIMComponent->IM_componentInit      = &IM_H264Enc_Init;
    pIMComponent->IM_componentTerminate = &IM_H264Enc_Terminate;
    pIMComponent->IM_bufferProcess      = &IM_H264Enc_bufferProcess;
    pIMComponent->IM_DecoderFlush       = &IM_H264_VENC_Flush;;


    pIMComponent->currentState = OMX_StateLoaded;
    pIMComponent->targetState = OMX_StateLoaded;

    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_ComponentDeinit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE            ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE       *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT   *pIMComponent = NULL;
    IM_H264ENC_HANDLE      *pH264Enc = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    pH264Enc = (IM_H264ENC_HANDLE *)pIMComponent->hCodecHandle;
    if(pH264Enc != NULL) {
        IM_OSAL_Free(pH264Enc);
        pH264Enc = pIMComponent->hCodecHandle = NULL;
    }

    IM_OSAL_Free(pIMComponent->componentName);
    pIMComponent->componentName = NULL;

    ret = IM_OMX_VideoEncodeComponentDeinit(pOMXComponent);
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}
