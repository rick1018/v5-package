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
 * @file      IM_AudioDec.c
 * @brief
 * @author    ayakashi (eric.xu@infotmic.com.cn)
 * @version   1.1.0
 * @history
 *   2012.04.15 : Create
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IM_OMX_Macros.h"
#include "IM_OMX_Basecomponent.h"
#include "IM_OMX_Baseport.h"
#include "IM_OMX_Adec.h"
#include "IM_OSAL_ETC.h"
#include "IM_OSAL_Semaphore.h"
#include "IM_OSAL_Thread.h"
#include "library_register.h"
#include "IM_AudioDec.h"

#undef  IM_LOG_TAG
#define IM_LOG_TAG    "IM_AUDIO_INTER_DEC"
#define IM_LOG_OFF
#include "IM_OSAL_Trace.h"

//#define SRP_DUMP_TO_FILE
#ifdef SRP_DUMP_TO_FILE
#include "stdio.h"

FILE *inFile;
FILE *outFile;
#endif

OMX_ERRORTYPE IM_AudioDec_GetParameter(
    OMX_IN    OMX_HANDLETYPE hComponent,
    OMX_IN    OMX_INDEXTYPE  nParamIndex,
    OMX_INOUT OMX_PTR        pComponentParameterStructure)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
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
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    switch (nParamIndex) {
    case OMX_IndexParamAudioAac:
    {
        OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams =
            (OMX_AUDIO_PARAM_AACPROFILETYPE *)pComponentParameterStructure;

        if (aacParams->nPortIndex != INPUT_PORT_INDEX) {
            return OMX_ErrorUndefined;
        }   

        aacParams->nBitRate = 0;
        aacParams->nAudioBandWidth = 0;
        aacParams->nAACtools = 0;
        aacParams->nAACERtools = 0;
        aacParams->eAACProfile = OMX_AUDIO_AACObjectMain;

        aacParams->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP4ADTS;

        aacParams->eChannelMode = OMX_AUDIO_ChannelModeStereo;
/*
            aacParams->nChannels = mStreamInfo->numChannels;
            aacParams->nSampleRate = mStreamInfo->sampleRate;
            aacParams->nFrameLength = mStreamInfo->frameSize;
*/
    }
    break;
#if 0
    case OMX_IndexParamAudioMp3:
    {
        OMX_AUDIO_PARAM_MP3TYPE *pDstMp3Param = (OMX_AUDIO_PARAM_MP3TYPE *)pComponentParameterStructure;
        OMX_AUDIO_PARAM_MP3TYPE *pSrcMp3Param = NULL;
        IM_AUDIODEC_CODEC       *pAudioDecCodec = NULL;

        ret = IM_OMX_Check_SizeVersion(pDstMp3Param, sizeof(OMX_AUDIO_PARAM_MP3TYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        if (pDstMp3Param->nPortIndex >= ALL_PORT_NUM) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        pAudioDecCodec = (IM_AUDIODEC_CODEC *)((IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle)->hCodecHandle;
        pSrcMp3Param = &pAudioDecCodec->mp3Param;

        IM_OSAL_Memcpy(pDstMp3Param, pSrcMp3Param, sizeof(OMX_AUDIO_PARAM_MP3TYPE));
    }
        break;
#endif 
    case OMX_IndexParamAudioPcm:
    {
        OMX_AUDIO_PARAM_PCMMODETYPE *pDstPcmParam = (OMX_AUDIO_PARAM_PCMMODETYPE *)pComponentParameterStructure;
        OMX_AUDIO_PARAM_PCMMODETYPE *pSrcPcmParam = NULL;
        IM_AUDIODEC_CODEC           *pAudioDecCodec = NULL;

        ret = IM_OMX_Check_SizeVersion(pDstPcmParam, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        if (pDstPcmParam->nPortIndex >= ALL_PORT_NUM) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        pAudioDecCodec = (IM_AUDIODEC_CODEC *)((IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle)->hCodecHandle;
        pSrcPcmParam = &pAudioDecCodec->pcmParam;

        IM_OSAL_Memcpy(pDstPcmParam, pSrcPcmParam, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
    }
        break;
    case OMX_IndexParamStandardComponentRole:
    {
        OMX_S32 codecType;
        OMX_PARAM_COMPONENTROLETYPE *pComponentRole = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;

        ret = IM_OMX_Check_SizeVersion(pComponentRole, sizeof(OMX_PARAM_COMPONENTROLETYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }
        
        if(pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.audio.eEncoding == OMX_AUDIO_CodingAAC) {
            IM_OSAL_Strcpy((char *)pComponentRole->cRole, IM_OMX_COMPONENT_AAC_DEC_ROLE);
        } else if (pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.audio.eEncoding == OMX_AUDIO_CodingAC3) {
            IM_OSAL_Strcpy((char *)pComponentRole->cRole, IM_OMX_COMPONENT_AC3_DEC_ROLE);
        } else {
            IM_OSAL_Strcpy((char *)pComponentRole->cRole, IM_OMX_COMPONENT_MP2_DEC_ROLE);
        }
    }
        break;
    default:
        ret = IM_OMX_AudioDecodeGetParameter(hComponent, nParamIndex, pComponentParameterStructure);
        break;
    }
EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_AudioDec_SetParameter(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nIndex,
    OMX_IN OMX_PTR        pComponentParameterStructure)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
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
        ret = OMX_ErrorInvalidState;
        goto EXIT;
    }

    switch (nIndex) {
    case OMX_IndexParamAudioAac:
    {
        IM_AUDIODEC_CODEC       *pAudioDecCodec = NULL;
        OMX_AUDIO_PARAM_AACPROFILETYPE *pDstAacParam;

        OMX_AUDIO_PARAM_AACPROFILETYPE *aacParam =
            (OMX_AUDIO_PARAM_AACPROFILETYPE *)pComponentParameterStructure;

        if (aacParam->nPortIndex != INPUT_PORT_INDEX) {
            return OMX_ErrorUndefined;
        }   

        ret = IM_OMX_Check_SizeVersion(aacParam, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        if (aacParam->nPortIndex >= ALL_PORT_NUM) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        pAudioDecCodec = (IM_AUDIODEC_CODEC *)((IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle)->hCodecHandle;
        pDstAacParam = &pAudioDecCodec->aacParam;

        IM_OSAL_Memcpy(pDstAacParam, aacParam, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
        IM_OSAL_Error("aacParam:channel:%d, sampleRate = %d, ", pDstAacParam->nChannels, pDstAacParam->nSampleRate);

    }
        break;
#if 0
    case OMX_IndexParamAudioMp3:
    {
        OMX_AUDIO_PARAM_MP3TYPE *pDstMp3Param = NULL;
        OMX_AUDIO_PARAM_MP3TYPE *pSrcMp3Param = (OMX_AUDIO_PARAM_MP3TYPE *)pComponentParameterStructure;
        IM_AUDIODEC_CODEC       *pAudioDecCodec = NULL;

        ret = IM_OMX_Check_SizeVersion(pSrcMp3Param, sizeof(OMX_AUDIO_PARAM_MP3TYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        if (pSrcMp3Param->nPortIndex >= ALL_PORT_NUM) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        pAudioDecCodec = (IM_AUDIODEC_CODEC *)((IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle)->hCodecHandle;
        pDstMp3Param = &pAudioDecCodec->mp3Param;

        IM_OSAL_Memcpy(pDstMp3Param, pSrcMp3Param, sizeof(OMX_AUDIO_PARAM_MP3TYPE));
    }
        break;
#endif 
    case OMX_IndexParamAudioPcm:
    {
        OMX_AUDIO_PARAM_PCMMODETYPE *pDstPcmParam = NULL;
        OMX_AUDIO_PARAM_PCMMODETYPE *pSrcPcmParam = (OMX_AUDIO_PARAM_PCMMODETYPE *)pComponentParameterStructure;
        IM_AUDIODEC_CODEC           *pAudioDecCodec = NULL;

        ret = IM_OMX_Check_SizeVersion(pSrcPcmParam, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
        if (ret != OMX_ErrorNone) {
            goto EXIT;
        }

        if (pSrcPcmParam->nPortIndex >= ALL_PORT_NUM) {
            ret = OMX_ErrorBadPortIndex;
            goto EXIT;
        }

        pAudioDecCodec = (IM_AUDIODEC_CODEC *)((IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle)->hCodecHandle;
        pDstPcmParam = &pAudioDecCodec->pcmParam;

        IM_OSAL_Memcpy(pDstPcmParam, pSrcPcmParam, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));

        IM_OSAL_Error("pcmParam:channel:%d, sampleRate = %d, nBitPerSample = %d", pDstPcmParam->nChannels, pDstPcmParam->nSamplingRate, pDstPcmParam->nBitPerSample);
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

        if (!IM_OSAL_Strcmp((char*)pComponentRole->cRole, IM_OMX_COMPONENT_MP2_DEC_ROLE)) {
            pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.audio.eEncoding = OMX_AUDIO_CodingMP3;
        } else if(!IM_OSAL_Strcmp((char*)pComponentRole->cRole, IM_OMX_COMPONENT_AAC_DEC_ROLE)) {
            pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.audio.eEncoding = OMX_AUDIO_CodingAAC;
        } else if(!IM_OSAL_Strcmp((char*)pComponentRole->cRole, IM_OMX_COMPONENT_AC3_DEC_ROLE)) {
            pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.audio.eEncoding = OMX_AUDIO_CodingAC3;
        } else {
            ret = OMX_ErrorBadParameter;
            goto EXIT;
        }
    }
        break;
    default:
        ret = IM_OMX_AudioDecodeSetParameter(hComponent, nIndex, pComponentParameterStructure);
        break;
    }
EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_AudioDec_GetConfig(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nIndex,
    OMX_IN OMX_PTR        pComponentConfigStructure)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
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
    default:
        ret = IM_OMX_AudioDecodeGetConfig(hComponent, nIndex, pComponentConfigStructure);
        break;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_AudioDec_SetConfig(
    OMX_IN OMX_HANDLETYPE hComponent,
    OMX_IN OMX_INDEXTYPE  nIndex,
    OMX_IN OMX_PTR        pComponentConfigStructure)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
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
    default:
        ret = IM_OMX_AudioDecodeSetConfig(hComponent, nIndex, pComponentConfigStructure);
        break;
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_AudioDec_GetExtensionIndex(
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

    ret = IM_OMX_AudioDecodeGetExtensionIndex(hComponent, cParameterName, pIndexType);

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_AudioDec_ComponentRoleEnum(
    OMX_IN  OMX_HANDLETYPE hComponent,
    OMX_OUT OMX_U8        *cRole,
    OMX_IN  OMX_U32        nIndex)
{
    OMX_ERRORTYPE               ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE          *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT   *pIMComponent = NULL;
    OMX_S32                     codecType;

    IM_OSAL_Entering();

    if ((hComponent == NULL) || (cRole == NULL)) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (nIndex > (MAX_COMPONENT_ROLE_NUM - 1)) {
        ret = OMX_ErrorNoMore;
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

    if(nIndex == 0) {
        IM_OSAL_Strcpy((char *)cRole, IM_OMX_COMPONENT_MP2_DEC_ROLE);
    } else if (nIndex == 1) {
        IM_OSAL_Strcpy((char *)cRole, IM_OMX_COMPONENT_AAC_DEC_ROLE);
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_AudioDec_Init(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE                  ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT      *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_AUDIODEC_COMPONENT *pAudioDec = (IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle;
    IM_AUDIODEC_CODEC             *pAudioDecCodec = (IM_AUDIODEC_CODEC *)pAudioDec->hCodecHandle;

    IM_OSAL_Entering();

    IM_OSAL_Memset(pIMComponent->timeStamp, -19771003, sizeof(OMX_TICKS) * MAX_TIMESTAMP);
    IM_OSAL_Memset(pIMComponent->nFlags, 0, sizeof(OMX_U32) * MAX_FLAGS);
    pIMComponent->bUseFlagEOF = OMX_TRUE; 
    pIMComponent->bSaveFlagEOS = OMX_FALSE;
	
	/* Init Audio Decoder */
	IM_OSAL_Memset(&pAudioDecCodec->inType, 0, sizeof(ADEC_INPUT_TYPE));	
	IM_OSAL_Memset(&pAudioDecCodec->outType, 0, sizeof(ADEC_OUTPUT_TYPE));	
	
    if(pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.audio.eEncoding == OMX_AUDIO_CodingMP3) {
        pAudioDecCodec->inType.type = IM_STRM_AUDIO_MP2;
    } else if (pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.audio.eEncoding == OMX_AUDIO_CodingAAC) {
        pAudioDecCodec->inType.type = IM_STRM_AUDIO_AAC;
    } else if (pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.audio.eEncoding == OMX_AUDIO_CodingAC3) {
        pAudioDecCodec->inType.type = IM_STRM_AUDIO_EAC3;
    } else {
        ret = OMX_ErrorUnsupportedSetting;
        goto EXIT;
    }
    pAudioDecCodec->outType.channels = pAudioDecCodec->pcmParam.nChannels;
    pAudioDecCodec->outType.bitspersample = pAudioDecCodec->pcmParam.nBitPerSample;
    pAudioDecCodec->outType.samplerate = pAudioDecCodec->pcmParam.nSamplingRate;

    if(IM_RET_ADEC_OK != adec_init(&pAudioDecCodec->adecCtx, &pAudioDecCodec->inType, &pAudioDecCodec->outType)) {
        IM_OSAL_Error("adec_init  failed ");
        ret = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /* init timestamp calc function */
    if(adec_timestamp_init(&pAudioDecCodec->timestampCalc) < 0) {
        IM_OSAL_Error("timestamp calc func init failed");
        goto EXIT;
    }

    pAudioDecCodec->timestampCalc->setFromInputTimestamp(pAudioDecCodec->timestampCalc, 0);

    pAudioDecCodec->isFirstFrame = OMX_TRUE;


#ifdef SRP_DUMP_TO_FILE
    inFile = fopen("/data/InFile.mp2", "w+");
    outFile = fopen("/data/OutFile.pcm", "w+");
#endif

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_AudioDec_Terminate(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE                  ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT      *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_AUDIODEC_COMPONENT *pAudioDec = (IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle;
    IM_AUDIODEC_CODEC         *pAudioDecCodec = (IM_AUDIODEC_CODEC *)pAudioDec->hCodecHandle;

    IM_OSAL_Entering();
    /* deinit audio decoder */
    if(adec_deinit(pAudioDecCodec->adecCtx) != IM_RET_ADEC_OK) {
		IM_OSAL_Error("adec_deinit failed");
	}
	
	pAudioDecCodec->adecCtx = NULL;
	adec_timestamp_deinit(pAudioDecCodec->timestampCalc);
	pAudioDecCodec->timestampCalc = NULL;

#ifdef SRP_DUMP_TO_FILE
    fclose(inFile);
    fclose(outFile);
#endif

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_AudioDec_Flush(OMX_COMPONENTTYPE *pOMXComponent)
{
    OMX_ERRORTYPE                  ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT      *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_AUDIODEC_COMPONENT *pAudioDec = (IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle;
    IM_AUDIODEC_CODEC         *pAudioDecCodec = (IM_AUDIODEC_CODEC *)pAudioDec->hCodecHandle;

    IM_OSAL_Entering();
	
	if(pAudioDecCodec->adecCtx != NULL)
		adec_flush(pAudioDecCodec->adecCtx);

    IM_OSAL_Exiting();

	return ret;
}

OMX_ERRORTYPE IM_Audio_Decode_Block(OMX_COMPONENTTYPE *pOMXComponent, IM_OMX_DATABUFFER *pInputDataBuffer, IM_OMX_DATABUFFER *pOutputDataBuffer)
{
    OMX_ERRORTYPE                  ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT      *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_AUDIODEC_COMPONENT *pAudioDec = (IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle;
    IM_AUDIODEC_CODEC         *pAudioDecCodec = (IM_AUDIODEC_CODEC *)pAudioDec->hCodecHandle;
    OMX_S32 				   samplesPerFrame;

    IM_RET 					decodeRet;		
    IM_OSAL_Entering();

    /* set input timestamp */
    if((pInputDataBuffer->bufferHeader->nFlags & OMX_BUFFERFLAG_CODECCONFIG) == 0 && 
			pInputDataBuffer->bufferHeader->nOffset == 0) {
    	IM_OSAL_Info("in timestamp :%lld", pInputDataBuffer->bufferHeader->nTimeStamp);
    	pAudioDecCodec->timestampCalc->setFromInputTimestamp(pAudioDecCodec->timestampCalc, pInputDataBuffer->bufferHeader->nTimeStamp);
    }
	
    pAudioDecCodec->adecIn.input_buffer.vir_addr = pInputDataBuffer->bufferHeader->pBuffer + pInputDataBuffer->bufferHeader->nOffset;	
    pAudioDecCodec->adecIn.input_buffer.size = pInputDataBuffer->bufferHeader->nFilledLen;
    pAudioDecCodec->adecIn.output_buffer.vir_addr = pOutputDataBuffer->bufferHeader->pBuffer + pOutputDataBuffer->bufferHeader->nOffset;
    pAudioDecCodec->adecIn.output_buffer.size = pOutputDataBuffer->bufferHeader->nAllocLen;

    if(pInputDataBuffer->bufferHeader->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
        IM_OSAL_Error("OMX_BUFFERFLAG_CODECCONFIG");
        pAudioDecCodec->adecIn.flag = 8;//IM_ADEC_EXTRADATA;
    } else {
        pAudioDecCodec->adecIn.flag = 0;
    }

    decodeRet = adec_decode(pAudioDecCodec->adecCtx, &pAudioDecCodec->adecIn, &pAudioDecCodec->adecOut);

    if(decodeRet < 0) {
        IM_OSAL_Error("adec_decode error");
        pAudioDecCodec->adecOut.bytes_consumed = pInputDataBuffer->bufferHeader->nFilledLen;
        pAudioDecCodec->errorTolerance++;
        if(pAudioDecCodec->errorTolerance > MAX_FAILED_TIMES) {
            ret = OMX_ErrorStreamCorrupt;
			goto EXIT;
		}
	}
	
	if(pAudioDecCodec->adecOut.bytes_consumed < 0 || pAudioDecCodec->adecIn.input_buffer.size < pAudioDecCodec->adecOut.bytes_consumed) {
		IM_OSAL_Error("adec_decode byte consumed error");
		pAudioDecCodec->adecOut.bytes_consumed = pInputDataBuffer->bufferHeader->nFilledLen;
	} else {
		pAudioDecCodec->errorTolerance = 0;
	}

	/* input data consumed info */
	pInputDataBuffer->bufferHeader->nOffset += pAudioDecCodec->adecOut.bytes_consumed;
    pInputDataBuffer->bufferHeader->nFilledLen -= pAudioDecCodec->adecOut.bytes_consumed;
    pInputDataBuffer->nRemainDataLeft = pInputDataBuffer->bufferHeader->nFilledLen;
    pInputDataBuffer->bDataProcessed =  OMX_TRUE;

	/* output data filled info */
	pOutputDataBuffer->bufferHeader->nFilledLen = pAudioDecCodec->adecOut.output_len;
    IM_OSAL_Info("outlen = %ld", pAudioDecCodec->adecOut.output_len);

	/* timestamp info */
    if(pAudioDecCodec->adecOut.channels > 0)
        samplesPerFrame = pAudioDecCodec->adecOut.output_len / pAudioDecCodec->adecOut.channels / 2;
    else 
        samplesPerFrame = 0;
    if(pAudioDecCodec->isFirstFrame && samplesPerFrame) {
        IM_OSAL_Error("samplesPerFrame = %d, sample_rate = %d", samplesPerFrame, pAudioDecCodec->adecOut.sample_rate);
        pAudioDecCodec->timestampCalc->setParameters(pAudioDecCodec->timestampCalc, pAudioDecCodec->adecOut.sample_rate, samplesPerFrame);
    }

    if(pAudioDecCodec->adecOut.output_len > 0) {
        pOutputDataBuffer->bufferHeader->nTimeStamp = pAudioDecCodec->timestampCalc->getConvertedTimestamp(pAudioDecCodec->timestampCalc);

        IM_OSAL_Info("out timestamp :%lld", pOutputDataBuffer->bufferHeader->nTimeStamp);
        pAudioDecCodec->timestampCalc->updateTimestamp(pAudioDecCodec->timestampCalc, samplesPerFrame);
    } else {
        IM_OSAL_Warning("no output data");
    }

    /* eos is comming, set it when data is processed */
    if((pInputDataBuffer->bufferHeader->nFlags & OMX_BUFFERFLAG_EOS) && pInputDataBuffer->nRemainDataLeft <= 0) {
        pOutputDataBuffer->bufferHeader->nFlags |= OMX_BUFFERFLAG_EOS;
    }

    if (pAudioDecCodec->isFirstFrame || pAudioDecCodec->pcmParam.nChannels != pAudioDecCodec->adecOut.channels ||
            pAudioDecCodec->pcmParam.nSamplingRate != pAudioDecCodec->adecOut.sample_rate) {

        if(samplesPerFrame > 0)
            pAudioDecCodec->isFirstFrame = OMX_FALSE;
        if(pAudioDecCodec->adecOut.channels && pAudioDecCodec->adecOut.sample_rate > 0) {

            /* Change channel count and sampling rate information */
            pAudioDecCodec->pcmParam.nChannels = pAudioDecCodec->adecOut.channels;
            pAudioDecCodec->pcmParam.nSamplingRate = pAudioDecCodec->adecOut.sample_rate;

            /* Send Port Settings changed call back */
            (*(pIMComponent->pCallbacks->EventHandler))
                (pOMXComponent,
                 pIMComponent->callbackData,
                 OMX_EventPortSettingsChanged, /* The command was completed */
                 OMX_DirOutput, /* This is the port index */
                 0,
                 NULL);
        }
    }

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_AudioDec_bufferProcess(OMX_COMPONENTTYPE *pOMXComponent, IM_OMX_DATABUFFER *pInputDataBuffer, IM_OMX_DATABUFFER *pOutputDataBuffer)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_BASEPORT      *pInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    IM_OMX_BASEPORT      *pOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];

    IM_OSAL_Entering();

    if ((!CHECK_PORT_ENABLED(pInputPort)) || (!CHECK_PORT_ENABLED(pOutputPort)) ||
            (!CHECK_PORT_POPULATED(pInputPort)) || (!CHECK_PORT_POPULATED(pOutputPort))) {
        if (pInputDataBuffer->bufferHeader->nFlags & OMX_BUFFERFLAG_EOS)
            ret = OMX_ErrorNone;
        else
            ret = OMX_ErrorNone;

        goto EXIT;
    }
    if (OMX_FALSE == IM_Check_BufferProcess_State(pIMComponent)) {
        if (pInputDataBuffer->bufferHeader->nFlags & OMX_BUFFERFLAG_EOS)
            ret = OMX_ErrorNone;
        else
            ret = OMX_ErrorNone;

        goto EXIT;
    }

    ret = IM_Audio_Decode_Block(pOMXComponent, pInputDataBuffer, pOutputDataBuffer);

	if (ret != OMX_ErrorNone) {
		pIMComponent->pCallbacks->EventHandler((OMX_HANDLETYPE)pOMXComponent,
				pIMComponent->callbackData,
				OMX_EventError, ret, 0, NULL);
	}

EXIT:
	IM_OSAL_Exiting();

	return ret;
}

OSCL_EXPORT_REF OMX_ERRORTYPE IM_OMX_ComponentInit(OMX_HANDLETYPE hComponent, OMX_STRING componentName)
{
    OMX_ERRORTYPE                  ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE             *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT      *pIMComponent = NULL;
    IM_OMX_BASEPORT           *pIMPort = NULL;
    IM_OMX_AUDIODEC_COMPONENT *pAudioDec = NULL;
    IM_AUDIODEC_CODEC             *pAudioDecCodec = NULL;
    OMX_PTR                        pInputBuffer = NULL;
    OMX_PTR                        pOutputBuffer = NULL;
    unsigned int                   inputBufferSize = 0;
    unsigned int                   inputBufferNum = 0;
    unsigned int                   outputBufferSize = 0;
    unsigned int                   outputBufferNum = 0;
    OMX_S32                        returnCodec;
    int i = 0;

    IM_OSAL_Entering();

    if ((hComponent == NULL) || (componentName == NULL)) {
        IM_OSAL_Error("%s: parameters are null, ret: %X", __FUNCTION__, ret);
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    if (IM_OSAL_Strcmp(IM_OMX_COMPONENT_AUDIO_DEC, componentName) != 0) {
        IM_OSAL_Error("%s: componentName(%s) error, ret: %X", __FUNCTION__, componentName, ret);
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_AudioDecodeComponentInit(pOMXComponent);
    if (ret != OMX_ErrorNone) {
        IM_OSAL_Error("%s: IM_OMX_AudioDecodeComponentInit error, ret: %X", __FUNCTION__, ret);
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    pIMComponent->codecType = HW_AUDIO_CODEC;

    pIMComponent->componentName = (OMX_STRING)IM_OSAL_Malloc(MAX_OMX_COMPONENT_NAME_SIZE);
    if (pIMComponent->componentName == NULL) {
        IM_OSAL_Error("%s: componentName alloc error, ret: %X", __FUNCTION__, ret);
        ret = OMX_ErrorInsufficientResources;
        goto EXIT_ERROR_1;
    }
    IM_OSAL_Memset(pIMComponent->componentName, 0, MAX_OMX_COMPONENT_NAME_SIZE);
    IM_OSAL_Strcpy(pIMComponent->componentName, IM_OMX_COMPONENT_AUDIO_DEC);

    pAudioDecCodec = IM_OSAL_Malloc(sizeof(IM_AUDIODEC_CODEC));
    if (pAudioDecCodec == NULL) {
        IM_OSAL_Error("%s: IM_AUDIODEC_CODEC alloc error, ret: %X", __FUNCTION__, ret);
        ret = OMX_ErrorInsufficientResources;
        goto EXIT_ERROR_2;
    }
    IM_OSAL_Memset(pAudioDecCodec, 0, sizeof(IM_AUDIODEC_CODEC));
    pAudioDec = (IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle;
    pAudioDec->hCodecHandle = (OMX_HANDLETYPE)pAudioDecCodec;

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

    /* Input port */
    pIMPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    pIMPort->portDefinition.nBufferCountActual = MAX_AUDIO_INPUTBUFFER_NUM;
    pIMPort->portDefinition.nBufferCountMin = MAX_AUDIO_INPUTBUFFER_NUM;
    pIMPort->portDefinition.nBufferSize = DEFAULT_AUDIO_INPUT_BUFFER_SIZE;
    pIMPort->portDefinition.bEnabled = OMX_TRUE;
    IM_OSAL_Memset(pIMPort->portDefinition.format.audio.cMIMEType, 0, MAX_OMX_MIMETYPE_SIZE);
    IM_OSAL_Strcpy(pIMPort->portDefinition.format.audio.cMIMEType, "audio/mpeg");
    pIMPort->portDefinition.format.audio.pNativeRender = 0;
    pIMPort->portDefinition.format.audio.bFlagErrorConcealment = OMX_FALSE;
    pIMPort->portDefinition.format.audio.eEncoding = OMX_AUDIO_CodingMP3;

    /* Output port */
    pIMPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    pIMPort->portDefinition.nBufferCountActual = MAX_AUDIO_OUTPUTBUFFER_NUM;
    pIMPort->portDefinition.nBufferCountMin = MAX_AUDIO_OUTPUTBUFFER_NUM;
    pIMPort->portDefinition.nBufferSize = DEFAULT_AUDIO_OUTPUT_BUFFER_SIZE;
    pIMPort->portDefinition.bEnabled = OMX_TRUE;
    IM_OSAL_Memset(pIMPort->portDefinition.format.audio.cMIMEType, 0, MAX_OMX_MIMETYPE_SIZE);
    IM_OSAL_Strcpy(pIMPort->portDefinition.format.audio.cMIMEType, "audio/raw");
    pIMPort->portDefinition.format.audio.pNativeRender = 0;
    pIMPort->portDefinition.format.audio.bFlagErrorConcealment = OMX_FALSE;
    pIMPort->portDefinition.format.audio.eEncoding = OMX_AUDIO_CodingPCM;

#if 0
    /* Default values for AAC audio param */
    INIT_SET_SIZE_VERSION(&pAudioDecCodec->mp3Param, OMX_AUDIO_PARAM_MP3TYPE);
    pAudioDecCodec->mp3Param.nPortIndex      = INPUT_PORT_INDEX;
    pAudioDecCodec->mp3Param.nChannels       = DEFAULT_AUDIO_CHANNELS_NUM;
    pAudioDecCodec->mp3Param.nBitRate        = 0;
    pAudioDecCodec->mp3Param.nSampleRate     = DEFAULT_AUDIO_SAMPLING_FREQ;
    pAudioDecCodec->mp3Param.nAudioBandWidth = 0;
    pAudioDecCodec->mp3Param.eChannelMode    = OMX_AUDIO_ChannelModeStereo;
    pAudioDecCodec->mp3Param.eFormat         = OMX_AUDIO_MP3StreamFormatMP1Layer3;
#endif
    /* Default values for PCM audio param */
    INIT_SET_SIZE_VERSION(&pAudioDecCodec->pcmParam, OMX_AUDIO_PARAM_PCMMODETYPE);
    pAudioDecCodec->pcmParam.nPortIndex         = OUTPUT_PORT_INDEX;
    pAudioDecCodec->pcmParam.nChannels          = DEFAULT_AUDIO_CHANNELS_NUM;
    pAudioDecCodec->pcmParam.eNumData           = OMX_NumericalDataSigned;
    pAudioDecCodec->pcmParam.eEndian            = OMX_EndianLittle;
    pAudioDecCodec->pcmParam.bInterleaved       = OMX_TRUE;
    pAudioDecCodec->pcmParam.nBitPerSample      = DEFAULT_AUDIO_BIT_PER_SAMPLE;
    pAudioDecCodec->pcmParam.nSamplingRate      = DEFAULT_AUDIO_SAMPLING_FREQ;
    pAudioDecCodec->pcmParam.ePCMMode           = OMX_AUDIO_PCMModeLinear;
    pAudioDecCodec->pcmParam.eChannelMapping[0] = OMX_AUDIO_ChannelLF;
    pAudioDecCodec->pcmParam.eChannelMapping[1] = OMX_AUDIO_ChannelRF;

    pOMXComponent->GetParameter      = &IM_AudioDec_GetParameter;
    pOMXComponent->SetParameter      = &IM_AudioDec_SetParameter;
    pOMXComponent->GetConfig         = &IM_AudioDec_GetConfig;
    pOMXComponent->SetConfig         = &IM_AudioDec_SetConfig;
    pOMXComponent->GetExtensionIndex = &IM_AudioDec_GetExtensionIndex;
    pOMXComponent->ComponentRoleEnum = &IM_AudioDec_ComponentRoleEnum;
    pOMXComponent->ComponentDeInit   = &IM_OMX_ComponentDeinit;

    /* ToDo: Change the function name associated with a specific codec */
    pIMComponent->IM_componentInit      = &IM_AudioDec_Init;
    pIMComponent->IM_componentTerminate = &IM_AudioDec_Terminate;
    pIMComponent->IM_bufferProcess = &IM_AudioDec_bufferProcess;
    pIMComponent->IM_DecoderFlush      = &IM_AudioDec_Flush;
    //pAudioDec->IM_checkInputFrame = NULL;

    pIMComponent->currentState = OMX_StateLoaded;
    pIMComponent->targetState = OMX_StateLoaded;

    ret = OMX_ErrorNone;
    goto EXIT; /* This function is performed successfully. */

EXIT_ERROR_2:
    IM_OSAL_Free(pIMComponent->componentName);
    pIMComponent->componentName = NULL;
EXIT_ERROR_1:
    IM_OMX_AudioDecodeComponentDeinit(pOMXComponent);
EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OMX_ERRORTYPE IM_OMX_ComponentDeinit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE             ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE        *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_AUDIODEC_CODEC        *pAudioDecCodec = NULL;
    IM_OMX_BASEPORT      *pIMPort = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;

    IM_OSAL_Free(pIMComponent->componentName);
    pIMComponent->componentName = NULL;
    pIMPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];

    pAudioDecCodec = (IM_AUDIODEC_CODEC *)((IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle)->hCodecHandle;
    if (pAudioDecCodec != NULL) {
        IM_OSAL_Free(pAudioDecCodec);
        ((IM_OMX_AUDIODEC_COMPONENT *)pIMComponent->hComponentHandle)->hCodecHandle = NULL;
    }

    ret = IM_OMX_AudioDecodeComponentDeinit(pOMXComponent);
    if (ret != OMX_ErrorNone) {
        goto EXIT;
    }

    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}
