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
 * @file        Video_Utils.c
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.03.01 : Create
*/
#include "IM_OMX_On2Dec.h"
#include "IM_OMX_Vdec.h"
#include "IM_OSAL_Buffer.h"
#include "IM_OSAL_Trace.h"
#include "IM_OSAL_Memory.h"
#include "IM_OSAL_ETC.h"
#include "IM_OSAL_Semaphore.h"
#include "IM_OSAL_Mutex.h"
#include "IM_OSAL_Thread.h"
#undef  IM_LOG_TAG
#define IM_LOG_TAG    "IM_VDEC"
#define IM_LOG_OFF

#define LATE_TO_DROP_TRIL   600000
#define LATE_TO_DROP_DOUBLE 460000
#define LATE_TO_DROP_SINGLE 200000
#define DROP_ONE_THRESHOLD 1
#define DROP_TWO_THRESHOLD 2
#define SKIP_FRAME
//#define TIMESTAMP_WORKAROUND
/* H.264 Decoder Supported Levels & profiles */
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


struct formatMap {
	IM_UINT32			format1;
	OMX_COLOR_FORMATTYPE format2;
};

static const struct formatMap kFormatMap[] = {
	{IM_PIC_FMT_12BITS_YUV420SP, OMX_COLOR_FormatYUV420SemiPlanar},
	{IM_PIC_FMT_16BITS_YUV422I_YUYV, OMX_COLOR_FormatYCbYCr},
	{IM_PIC_FMT_16BITS_YUV422I_YVYU, OMX_COLOR_FormatYCrYCb},
	{IM_PIC_FMT_16BITS_YUV422I_UYVY, OMX_COLOR_FormatCbYCrY},
	{IM_PIC_FMT_16BITS_YUV422I_VYUY, OMX_COLOR_FormatCrYCbY},
	{IM_PIC_FMT_16BITS_YUV422SP, OMX_COLOR_FormatYUV422SemiPlanar},
	{IM_PIC_FMT_16BITS_0RGB_1555, OMX_COLOR_Format16bitARGB1555},
	{IM_PIC_FMT_16BITS_RGB_565, OMX_COLOR_Format16bitRGB565},
	{IM_PIC_FMT_16BITS_0BGR_1555, OMX_COLOR_FormatKhronosExtensions},
	{IM_PIC_FMT_16BITS_BGR_565, OMX_COLOR_Format16bitBGR565},
	{IM_PIC_FMT_16BITS_BGR0_4444, OMX_COLOR_FormatKhronosExtensions},
	{IM_PIC_FMT_32BITS_ARGB_8888, OMX_COLOR_Format32bitARGB8888},
	{IM_PIC_FMT_32BITS_BGRA_8888, OMX_COLOR_FormatKhronosExtensions},
};

static const OMX_U32 kNumEntries = sizeof(kFormatMap) / sizeof(kFormatMap[0]);
static OMX_S32 getFormatIndex(OMX_COLOR_FORMATTYPE format)
{
	OMX_S32 i = 0;
	for(i = 0; i < (OMX_S32)kNumEntries; i++)
	{
		if(kFormatMap[i].format2 == format)
			return i;
	}

	return -1;
}

OMX_ERRORTYPE IM_VDEC_GetParameter(
		OMX_IN OMX_HANDLETYPE hComponent,
		OMX_IN OMX_INDEXTYPE  nParamIndex,
		OMX_INOUT OMX_PTR     pComponentParameterStructure)
{
	OMX_ERRORTYPE          ret = OMX_ErrorNone;
	OMX_COMPONENTTYPE     *pOMXComponent = NULL;
	IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_VIDEODEC_COMPONENT *pOn2DecComponent = NULL;

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
    pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;

	if (pIMComponent->currentState == OMX_StateInvalid ) {
		ret = OMX_StateInvalid;
		goto EXIT;
	}

	switch (nParamIndex) {
		case OMX_IndexParamStandardComponentRole:
			{
				OMX_PARAM_COMPONENTROLETYPE *pComponentRole = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;
				ret = IM_OMX_Check_SizeVersion(pComponentRole, sizeof(OMX_PARAM_COMPONENTROLETYPE));
				if (ret != OMX_ErrorNone) {
					goto EXIT;
				}

                if(pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.video.eCompressionFormat == OMX_VIDEO_CodingAVC) {
				    IM_OSAL_Strcpy(pComponentRole->cRole, IM_OMX_COMPONENT_AVC_DEC_ROLE);
                } else  if(pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG4) {
				    IM_OSAL_Strcpy(pComponentRole->cRole, IM_OMX_COMPONENT_MPEG4_DEC_ROLE);
                } else  if(pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG2) {
				    IM_OSAL_Strcpy(pComponentRole->cRole, IM_OMX_COMPONENT_MPEG2_DEC_ROLE);
                } else {
				    IM_OSAL_Strcpy(pComponentRole->cRole, IM_OMX_COMPONENT_AVC_DEC_ROLE);
                }
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
		default:
			ret = IM_OMX_VideoDecodeGetParameter(hComponent, nParamIndex, pComponentParameterStructure);
			break;
	}
EXIT:
	IM_OSAL_Exiting();

	return ret;
}

OMX_ERRORTYPE IM_VDEC_SetParameter(
		OMX_IN OMX_HANDLETYPE hComponent,
		OMX_IN OMX_INDEXTYPE  nIndex,
		OMX_IN OMX_PTR        pComponentParameterStructure)
{
	OMX_ERRORTYPE           ret = OMX_ErrorNone;
	OMX_COMPONENTTYPE     *pOMXComponent = NULL;
	IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_VIDEODEC_COMPONENT *pOn2DecComponent = NULL;
    IM_ON2DEC_HANDLE  *pOn2DecHandle = NULL;

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
    pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;
    pOn2DecHandle = (IM_ON2DEC_HANDLE *)pOn2DecComponent->hCodecHandle;
	if (pIMComponent->currentState == OMX_StateInvalid ) {
		ret = OMX_StateInvalid;
		goto EXIT;
	}

	switch (nIndex) {
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

                
				if (!IM_OSAL_Strcmp((OMX_S8*)pComponentRole->cRole, IM_OMX_COMPONENT_AVC_DEC_ROLE)) {
					pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
				} else if (!IM_OSAL_Strcmp((OMX_S8*)pComponentRole->cRole, IM_OMX_COMPONENT_MPEG4_DEC_ROLE)) {
					pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
				} else if (!IM_OSAL_Strcmp((OMX_S8*)pComponentRole->cRole, IM_OMX_COMPONENT_H263_DEC_ROLE)) {
					pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingH263;
				} else if (!IM_OSAL_Strcmp((OMX_S8*)pComponentRole->cRole, IM_OMX_COMPONENT_MPEG2_DEC_ROLE)) {
					pIMComponent->pIMPort[INPUT_PORT_INDEX].portDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG2;
                } else {
					ret = OMX_ErrorBadParameter;
					goto EXIT;
				}
			}
			break;
		case OMX_IndexParamPortDefinition:
			{
				OMX_PARAM_PORTDEFINITIONTYPE *pPortDefinition = (OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParameterStructure;
				OMX_U32                       portIndex = pPortDefinition->nPortIndex;
				IM_OMX_BASEPORT             *pIMPort;
				OMX_U32 width, height, size;

				if (portIndex >= pIMComponent->portParam.nPorts) {
					ret = OMX_ErrorBadPortIndex;
					goto EXIT;
				}
				ret = IM_OMX_Check_SizeVersion(pPortDefinition, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
				if (ret != OMX_ErrorNone) {
					goto EXIT;
				}

				pIMPort = &pIMComponent->pIMPort[portIndex];

				IM_OSAL_Info("currentState : %d\n",pIMComponent->currentState);
				if ((pIMComponent->currentState != OMX_StateLoaded) && (pIMComponent->currentState != OMX_StateWaitForResources)) {
					if (pIMPort->portDefinition.bEnabled == OMX_FALSE)
					{
						//SET size changed 
						if(pPortDefinition->nBufferCountActual != pIMPort->portDefinition.nBufferCountActual 
								|| pPortDefinition->nBufferSize != pIMPort->portDefinition.nBufferSize)
						{
							IM_OSAL_Error("size changed from application,old buffer count (%d) ,old buffer size (%d),new buffer count(%d,new buffer size(%d))\n",pIMPort->portDefinition.nBufferCountActual,pIMPort->portDefinition.nBufferSize,pPortDefinition->nBufferCountActual,pPortDefinition->nBufferSize);
							IM_OSAL_MutexLock(pIMPort->portMutex);
							pOn2DecHandle->bOutportChangedPending = OMX_FALSE;
							IM_OSAL_MutexUnlock(pIMPort->portMutex);
							//pIMComponent->IM_PortReset(pOMXComponent, portIndex);
						}else{
							IM_OSAL_Error("can not set other params except nBufferCountActual and nBufferSize in the %d state ",pIMComponent->currentState);
							ret = OMX_ErrorIncorrectStateOperation;
							goto EXIT;
						}
					}
				}
				if(pPortDefinition->nBufferCountActual < pIMPort->portDefinition.nBufferCountMin) {
					IM_OSAL_Error("can not set the buffer count smaller than we asked");
					ret = OMX_ErrorBadParameter;
					goto EXIT;
				}

				//IM_OSAL_Memcpy(&pIMPort->portDefinition, pPortDefinition, pPortDefinition->nSize);
				//should not set port enable and populate params 
				pIMPort->portDefinition.nBufferCountActual = pPortDefinition->nBufferCountActual;
				pIMPort->portDefinition.nBufferCountMin = pPortDefinition->nBufferCountMin;
				pIMPort->portDefinition.nBufferSize = pPortDefinition->nBufferSize;
				pIMPort->portDefinition.eDomain = pPortDefinition->eDomain;
				pIMPort->portDefinition.bBuffersContiguous = pPortDefinition->bBuffersContiguous;
				pIMPort->portDefinition.nBufferAlignment = pPortDefinition->nBufferAlignment;

				IM_OSAL_Memcpy(&pIMPort->portDefinition.format.video, &pPortDefinition->format.video, sizeof(OMX_VIDEO_PORTDEFINITIONTYPE));

				if(portIndex == OUTPUT_PORT_INDEX){
					width = ((pIMPort->portDefinition.format.video.nFrameWidth + 15) & (~15));
					height = ((pIMPort->portDefinition.format.video.nFrameHeight + 1) & (~1));
					pIMPort->portDefinition.format.video.nStride = width;
					pIMPort->portDefinition.format.video.nSliceHeight = height;
					pIMPort->cropRectangle.nLeft = 0;
					pIMPort->cropRectangle.nTop = 0;
					pIMPort->cropRectangle.nWidth = width;
					pIMPort->cropRectangle.nHeight = height;
					switch (pIMPort->portDefinition.format.video.eColorFormat) {
						case OMX_COLOR_FormatYUV420Planar:
						case OMX_COLOR_FormatYUV420SemiPlanar:
							pIMPort->portDefinition.nBufferSize = (width * height * 3) / 2;
							break;
						case OMX_COLOR_Format16bitRGB565:
							IM_OSAL_Info("OMX_COLOR_Format16bitRGB565");
							pIMPort->portDefinition.nBufferSize = width * height * 2;
							break;
						case OMX_COLOR_Format32bitARGB8888:
							IM_OSAL_Info("OMX_COLOR_Format32bitARGB8888");
							pIMPort->portDefinition.nBufferSize = width * height * 4;
							break;
						default:
							IM_OSAL_Info("Color format is not support!! use default YUV size!!");
							ret = OMX_ErrorUnsupportedSetting;
							break;
					}
				}else{
					pIMPort->portDefinition.nBufferSize  = 
						pIMPort->portDefinition.format.video.nFrameWidth * pIMPort->portDefinition.format.video.nFrameHeight;

				}
				IM_OSAL_Info("video.eColorFormat = %x",pIMComponent->pIMPort[OUTPUT_PORT_INDEX].portDefinition.format.video.eColorFormat);
			}
			break;
#ifdef USE_ANDROID_EXTENSION
		case OMX_IndexParamUseMMU:
			{
				IM_OSAL_Info("set OMX_IndexParamUseMMU");

				pOn2DecComponent->bUseMMU = *(OMX_BOOL *)pComponentParameterStructure;
				pIMComponent->useMMU = *(OMX_BOOL *)pComponentParameterStructure;
				ret = OMX_ErrorNone;
				break;
			}
#endif 
		default:
			ret = IM_OMX_VideoDecodeSetParameter(hComponent, nIndex, pComponentParameterStructure);
			break;
	}
EXIT:
	IM_OSAL_Exiting();

	return ret;
}

OMX_ERRORTYPE IM_VDEC_GetConfig(
		OMX_HANDLETYPE hComponent,
		OMX_INDEXTYPE nIndex,
		OMX_PTR pComponentConfigStructure)
{
	OMX_ERRORTYPE           ret = OMX_ErrorNone;
	OMX_COMPONENTTYPE     *pOMXComponent = NULL;
	IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_VIDEODEC_COMPONENT *pOn2DecComponent = NULL;

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
    pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;

	if (pComponentConfigStructure == NULL) {
		ret = OMX_ErrorBadParameter;
		goto EXIT;
	}
	if (pIMComponent->currentState == OMX_StateInvalid) {
		ret = OMX_ErrorInvalidState;
		goto EXIT;
	}

	switch (nIndex) {
		case OMX_IndexConfigCommonOutputCrop:
			{
				OMX_CONFIG_RECTTYPE *pSrcRectType = NULL;
				OMX_CONFIG_RECTTYPE *pDstRectType = NULL;

				if (pOn2DecComponent->bInitialized == OMX_FALSE) {
					ret = OMX_ErrorNotReady;
					IM_OSAL_Error("has not been bInitialized!\n");
					break;
				}

				pDstRectType = (OMX_CONFIG_RECTTYPE *)pComponentConfigStructure;

				if ((pDstRectType->nPortIndex != INPUT_PORT_INDEX) &&
						(pDstRectType->nPortIndex != OUTPUT_PORT_INDEX)) {
					ret = OMX_ErrorBadPortIndex;
					IM_OSAL_Error("bad port index!\n");
					goto EXIT;
				}

				IM_OMX_BASEPORT *pIMPort = &pIMComponent->pIMPort[pDstRectType->nPortIndex];

				pSrcRectType = &(pIMPort->cropRectangle);

				pDstRectType->nTop = pSrcRectType->nTop;
				pDstRectType->nLeft = pSrcRectType->nLeft;
				pDstRectType->nHeight = pSrcRectType->nHeight;
				pDstRectType->nWidth = pSrcRectType->nWidth;
			}
			break;
		default:
			ret = IM_OMX_GetConfig(hComponent, nIndex, pComponentConfigStructure);
			break;
	}

EXIT:
	IM_OSAL_Exiting();

	return ret;
}

OMX_ERRORTYPE IM_VDEC_SetConfig(
		OMX_HANDLETYPE hComponent,
		OMX_INDEXTYPE nIndex,
		OMX_PTR pComponentConfigStructure)
{
	OMX_ERRORTYPE           ret = OMX_ErrorNone;
	OMX_COMPONENTTYPE     *pOMXComponent = NULL;
	IM_OMX_BASECOMPONENT *pIMComponent = NULL;
    IM_OMX_VIDEODEC_COMPONENT *pOn2DecComponent = NULL;

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
    pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)(IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;

	if (pComponentConfigStructure == NULL) {
		IM_OSAL_Error("bad parameter!\n");
		ret = OMX_ErrorBadParameter;
		goto EXIT;
	}
	if (pIMComponent->currentState == OMX_StateInvalid) {
		IM_OSAL_Error("current state is invalid!\n");
		ret = OMX_ErrorInvalidState;
		goto EXIT;
	}

	switch (nIndex) {
		case OMX_IndexVendorThumbnailMode:
			{
				pOn2DecComponent->bThumbnailMode = *((OMX_BOOL *)pComponentConfigStructure);
				ret = OMX_ErrorNone;
			}
			break;
		case OMX_IndexConfigMetadataItem:
			{
				IM_ON2DEC_HANDLE   *pOn2DecHandle = (IM_ON2DEC_HANDLE *)pOn2DecComponent->hCodecHandle;

				IM_OSAL_Memcpy((void *)&pOn2DecHandle->videoMeta,(OMX_S8 *)pComponentConfigStructure,sizeof(IM_VIDEO_FORMAT));

				ret = OMX_ErrorNone;
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

OMX_ERRORTYPE IM_VDEC_GetExtensionIndex(
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
		IM_OSAL_Error("bad parameter : name is null or indextype is null!\n");
		ret = OMX_ErrorBadParameter;
		goto EXIT;
	}
	if (pIMComponent->currentState == OMX_StateInvalid) {
		IM_OSAL_Error("invalid state!\n");
		ret = OMX_ErrorInvalidState;
		goto EXIT;
	}

	if (IM_OSAL_Strcmp(cParameterName, IM_INDEX_PARAM_ENABLE_THUMBNAIL) == 0) {
		*pIndexType = OMX_IndexVendorThumbnailMode;
		ret = OMX_ErrorNone;
		IM_OSAL_Info("IM_INDEX_PARAM_ENABLE_THUMBNAIL\n");
#ifdef USE_ANDROID_EXTENSION
	} else if (IM_OSAL_Strcmp(cParameterName, IM_INDEX_PARAM_ENABLE_ANB) == 0) {
		IM_OSAL_Info("IM_INDEX_PARAM_ENABLE_ANB\n");
		*pIndexType = OMX_IndexParamEnableAndroidBuffers;
		ret = OMX_ErrorNone;
	} else if (IM_OSAL_Strcmp(cParameterName, IM_INDEX_PARAM_GET_ANB) == 0) {
		IM_OSAL_Info("IM_INDEX_PARAM_GET_ANB\n");
		*pIndexType = OMX_IndexParamGetAndroidNativeBuffer;
		ret = OMX_ErrorNone;
	} else if (IM_OSAL_Strcmp(cParameterName, IM_INDEX_PARAM_USE_ANB) == 0) {
		IM_OSAL_Info("IM_INDEX_PARAM_USE_ANB\n");
		*pIndexType = OMX_IndexParamUseAndroidNativeBuffer;
		ret = OMX_ErrorNone;
	}else if(IM_OSAL_Strcmp(cParameterName,IM_INDEX_PARAM_FLUSH_BEFORE_DECODING) == 0){
		IM_OSAL_Info("IM_INDEX_PARAM_FLUSH_BEFORE_DECODING\n");
		*pIndexType = OMX_IndexParamFlushBeforeDecoding;
		ret = OMX_ErrorNone;
	}else if(IM_OSAL_Strcmp(cParameterName,IM_INDEX_PARAM_SMALL_MEMORY_DEVICE) == 0){
        IM_OSAL_Info("IM_INDEX_PARAM_SMALL_MEMORY_DEVICE\n");
        *pIndexType = OMX_IndexParamSmallMemoryDevice; 
    }else if (IM_OSAL_Strcmp(cParameterName, IM_INDEX_PARAM_USE_MMU) == 0){
		IM_OSAL_Info("IM_INDEX_PARAM_USE_MMU\n");
		*pIndexType = OMX_IndexParamUseMMU;
		ret = OMX_ErrorNone;
#endif
	} else {
		ret = IM_OMX_GetExtensionIndex(hComponent, cParameterName, pIndexType);
	}

EXIT:
	IM_OSAL_Exiting();

	return ret;
}

OMX_ERRORTYPE IM_VDEC_ComponentRoleEnum(OMX_HANDLETYPE hComponent, OMX_U8 *cRole, OMX_U32 nIndex)
{
	OMX_ERRORTYPE            ret = OMX_ErrorNone;
	OMX_COMPONENTTYPE       *pOMXComponent = NULL;
	IM_OMX_BASECOMPONENT   *pIMComponent = NULL;

	IM_OSAL_Entering();

	if ((hComponent == NULL) || (cRole == NULL)) {
		ret = OMX_ErrorBadParameter;
		goto EXIT;
	}
	if (nIndex == 0) {
		IM_OSAL_Strcpy((OMX_S8 *)cRole, IM_OMX_COMPONENT_AVC_DEC_ROLE);
		ret = OMX_ErrorNone;
    } else if (nIndex == 1) {
		IM_OSAL_Strcpy((OMX_S8 *)cRole, IM_OMX_COMPONENT_MPEG4_DEC_ROLE);
		ret = OMX_ErrorNone;
    }else if (nIndex == 2) {
		IM_OSAL_Strcpy((OMX_S8 *)cRole, IM_OMX_COMPONENT_MPEG2_DEC_ROLE);
		ret = OMX_ErrorNone;
	} else {
		ret = OMX_ErrorNoMore;
	}

EXIT:
	IM_OSAL_Exiting();

	return ret;
}

/* On2Dec Init */
OMX_ERRORTYPE IM_VDEC_Init(OMX_COMPONENTTYPE *pOMXComponent)
{
	OMX_ERRORTYPE               ret = OMX_ErrorNone;
	IM_OMX_BASECOMPONENT       *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
	IM_OMX_BASEPORT            *pIMInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
	IM_OMX_BASEPORT            *pIMOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    IM_OMX_VIDEODEC_COMPONENT  *pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;
	IM_ON2DEC_HANDLE           *pOn2DecHandle = (IM_ON2DEC_HANDLE *)pOn2DecComponent->hCodecHandle;
	IM_VIDEO_FORMAT            *pVdecFormat = &pOn2DecHandle->videoMeta;

	OMX_S32   format_index = -1;
	IM_RET    status;

	IM_VDEC_CONFIG *pVdecConfig = &pOn2DecHandle->vdecConfig;
	IM_VDEC_SPEC_DATA *pSpecData = NULL;

	IM_OSAL_Entering();

	pVdecConfig->inType.codec = (IM_STREAM_SUBTYPE)pVdecFormat->subtype;
	if(pVdecFormat->subtype == IM_STRM_VIDEO_FFMPEG)
	{
		pSpecData = (IM_VDEC_SPEC_DATA *)pVdecFormat->privdata;
	}
	else if(pVdecFormat->subtype == IM_STRM_VIDEO_RV30 || pVdecFormat->subtype == IM_STRM_VIDEO_RV40)
	{
		pSpecData = (IM_VDEC_SPEC_DATA *)IM_OSAL_Malloc(sizeof(IM_RV8_MSGS));
		IM_OSAL_Memcpy(pSpecData, (void *)(pVdecFormat->rv8msgs) ,sizeof(IM_RV8_MSGS));
	}
	else if(pVdecFormat->subtype == IM_STRM_VIDEO_WMV3 || pVdecFormat->subtype == IM_STRM_VIDEO_VC1)
	{
		pSpecData = (IM_VDEC_SPEC_DATA *)IM_OSAL_Malloc(sizeof(IM_UINT32) + pVdecFormat->extradata_size);

		pSpecData->size = (IM_UINT32)pVdecFormat->extradata_size;
		IM_OSAL_Memcpy(pSpecData->data, pVdecFormat->extradata, pVdecFormat->extradata_size);
	}
	else
	{
		pSpecData = NULL;
	}

	pVdecConfig->inType.width = pIMInputPort->portDefinition.format.video.nFrameWidth;
	pVdecConfig->inType.height = pIMInputPort->portDefinition.format.video.nFrameHeight;
	if(IM_RET_VDEC_OK != vdec_create_ctx(&pOn2DecHandle->hOn2DecLib, &pVdecConfig->inType, kPreferHardwareCodecs))
	{
		IM_OSAL_Error("create decoder class failed");
		ret = OMX_ErrorHardware;
		goto EXIT;
	}

	vdec_get_config(pOn2DecHandle->hOn2DecLib, pVdecConfig);

	//check output type
	format_index = getFormatIndex(pIMOutputPort->portDefinition.format.video.eColorFormat);	
	if(format_index < 0){
		ret = OMX_ErrorUnsupportedIndex;
		IM_OSAL_Error("not support the out format ");
		goto EXIT;
	}
	pVdecConfig->outType.format = kFormatMap[format_index].format1;
	pVdecConfig->outType.width = pIMOutputPort->portDefinition.format.video.nStride;
	pVdecConfig->outType.height = pIMOutputPort->portDefinition.format.video.nSliceHeight;
	pVdecConfig->outType.rotation = 0;
	if(pOn2DecComponent->bThumbnailMode == OMX_TRUE || 
            pIMComponent->bSmallMemoryDevice == OMX_TRUE || pIMOutputPort->portDefinition.bBuffersContiguous == OMX_FALSE)
		pVdecConfig->mode.bMultiBuffMode = IM_FALSE;

	pVdecConfig->mode.bMMUEnable = pOn2DecComponent->bUseMMU;

	if(pVdecConfig->outType.width > 3 * pVdecConfig->inType.width)
	{
		pVdecConfig->outType.width = 3 * ((pVdecConfig->inType.width + 15) & ~15);
		pIMOutputPort->portDefinition.format.video.nStride = pVdecConfig->outType.width;
		pIMOutputPort->portDefinition.format.video.nFrameWidth  = pVdecConfig->outType.width;
		pOn2DecHandle->bResizeNeeded = OMX_TRUE;
	}

	if(pVdecConfig->outType.height > 3 * pVdecConfig->inType.height - 2)
	{
		pVdecConfig->outType.height = 3 * ((pVdecConfig->inType.height + 1) & ~1)  - 2;
		pIMOutputPort->portDefinition.format.video.nSliceHeight = pVdecConfig->outType.height;
		pIMOutputPort->portDefinition.format.video.nFrameHeight  = pVdecConfig->outType.height;
		pOn2DecHandle->bResizeNeeded = OMX_TRUE;
	}

	if(pVdecConfig->build.codecType == SW_FFMPEG)
	{
		if(!pVdecFormat->privdata)
		{
			IM_OSAL_Error("doesnot have ffmpeg context");
			goto EXIT;
		}

		pSpecData = (IM_VDEC_SPEC_DATA *)pVdecFormat->privdata;
        //TODO
		pVdecConfig->outType.width = (pVdecConfig->inType.width + 31) & ~31;
		pVdecConfig->outType.height = (pVdecConfig->inType.height + 1) & ~1;
		pIMOutputPort->portDefinition.format.video.nStride = pVdecConfig->outType.width;
		pIMOutputPort->portDefinition.format.video.nSliceHeight = pVdecConfig->outType.height;
		pIMOutputPort->portDefinition.format.video.nFrameWidth  = pVdecConfig->outType.width;
		pIMOutputPort->portDefinition.format.video.nFrameHeight  = pVdecConfig->outType.height;
		pOn2DecHandle->bResizeNeeded = OMX_TRUE;
	}

	if(vdec_set_config(pOn2DecHandle->hOn2DecLib, pVdecConfig) != IM_RET_VDEC_OK)
	{
		IM_OSAL_Error("vdec set config failed");
		goto EXIT;
	}
	
	//use Init data to init our hw decode lib
	status = vdec_init(pOn2DecHandle->hOn2DecLib, pSpecData);
	if(status != IM_RET_VDEC_OK){
		IM_OSAL_Error("create decoder class failed");
		ret = OMX_ErrorHardware;
		goto EXIT;
	}
	
    IM_OSAL_ContiguousAllocator_Create(&pOn2DecComponent->hMemoryHandle, pOn2DecComponent->bUseMMU, OMX_FALSE);

	if(pIMInputPort->portDefinition.bBuffersContiguous == OMX_FALSE){
		pOn2DecHandle->pSelfInBuffer = IM_OSAL_ContiguousBuffer_Malloc(pOn2DecComponent->hMemoryHandle, \
				pVdecConfig->inType.width * pVdecConfig->inType.height);
		if(pOn2DecHandle->pSelfInBuffer == NULL){
			ret = OMX_ErrorInsufficientResources;
			IM_OSAL_Error("Malloc  Contiguous in Buffer  failed");
			goto EXIT;
		}
	}

    if(pIMOutputPort->portDefinition.bBuffersContiguous == OMX_FALSE){
        pOn2DecHandle->pSelfOutBuffer = IM_OSAL_ContiguousBuffer_Malloc(pOn2DecComponent->hMemoryHandle, \
                pIMOutputPort->portDefinition.nBufferSize);
        if(pOn2DecHandle->pSelfOutBuffer == NULL){
            ret = OMX_ErrorInsufficientResources;
            IM_OSAL_Error("Malloc Contiguous out buffer failed");
            goto EXIT;
        }
    }
    if(pIMComponent->bFlushBeforeDecoding == OMX_TRUE)
	{
		IM_OSAL_Error("Need Flush before decoding!!!! ");
		vdec_flush(pOn2DecHandle->hOn2DecLib);
		pIMComponent->bFlushBeforeDecoding = OMX_FALSE;
	}

	pOn2DecComponent->bInitialized = OMX_TRUE;

EXIT:	
	if(pVdecConfig->build.codecType != SW_FFMPEG && pSpecData != NULL)
	{
		IM_OSAL_Free(pSpecData);
	}

	IM_OSAL_Exiting();
	return ret;
}

/* VDec Terminate */
OMX_ERRORTYPE IM_VDEC_Terminate(OMX_COMPONENTTYPE *pOMXComponent)
{
	OMX_ERRORTYPE               ret = OMX_ErrorNone;
	IM_OMX_BASECOMPONENT       *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_VIDEODEC_COMPONENT  *pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;
	IM_ON2DEC_HANDLE           *pOn2DecHandle = (IM_ON2DEC_HANDLE *)pOn2DecComponent->hCodecHandle;

	IM_OSAL_Entering();

	if(pOn2DecHandle->hOn2DecLib){
		vdec_deinit(pOn2DecHandle->hOn2DecLib);
		vdec_destroy(pOn2DecHandle->hOn2DecLib);
		pOn2DecHandle->hOn2DecLib = NULL; 
	}

	if(pOn2DecComponent->hMemoryHandle) {
		if(pOn2DecHandle->pSelfInBuffer) {
			IM_OSAL_ContiguousBuffer_Free(pOn2DecComponent->hMemoryHandle,  \
					pOn2DecHandle->pSelfInBuffer);

            pOn2DecHandle->pSelfInBuffer = NULL;
		}
		if(pOn2DecHandle->pSelfOutBuffer){
			IM_OSAL_ContiguousBuffer_Free(pOn2DecComponent->hMemoryHandle, \
					pOn2DecHandle->pSelfOutBuffer);
            pOn2DecHandle->pSelfOutBuffer = NULL;
		}
	}

	pOn2DecComponent->bInitialized = OMX_FALSE;

EXIT:
	IM_OSAL_Exiting();

	return ret;
}

OMX_TICKS IM_VDEC_GetTimeStamp(IM_ON2DEC_HANDLE *pIMVDec)
{
	OMX_TICKS timestamp = 0LL;
    OMX_BOOL skipCalc = OMX_FALSE;
	if(pIMVDec->iTSMgmtHead == NULL){
		IM_OSAL_Warning("timestamp list is null ,caculate it");
		timestamp = pIMVDec->mPreTimestamp + pIMVDec->mAverageTimeStamp;
	}else{
		IM_OSAL_Info("timestamp list not null ,get timestamp from TS list");
        OMX_BOOL discard = OMX_FALSE;
        OMX_TICKS tmpts = 0;

        do {
            TSMgmt *TSNode = pIMVDec->iTSMgmtHead;
            TSMgmt *delNode = pIMVDec->iTSMgmtHead;
            if(pIMVDec->iTSMgmtHead != NULL) 
                timestamp = pIMVDec->iTSMgmtHead->timeStamp;
            else {
                timestamp =  tmpts;
		        IM_OSAL_Error("timestamp list is null, interlace timestamp working");
                break;
            }
            while(TSNode)
            {
                if((OMX_TICKS)TSNode->timeStamp < (OMX_TICKS)timestamp)
                {
                    timestamp = TSNode->timeStamp;
                    delNode = TSNode;
                }
                TSNode = TSNode->next;
            }

            /* workaround for h264 interlace timestamp */
            if(pIMVDec->bInterlacedFrame && pIMVDec->vdecConfig.inType.codec == IM_STRM_VIDEO_H264) {
                if(pIMVDec->mPreTimestamp > 0 && timestamp - pIMVDec->mPreTimestamp < 25000) {
                    discard = OMX_TRUE;
                    tmpts = timestamp;
                } else {
                    discard = OMX_FALSE;
                }
            }

            IM_OMX_ListNodeDelete(del, iTSMgmt);
            IM_OSAL_Memset(delNode, 0, sizeof(TSMgmt));
        }while(discard);
    }


    //	IM_OSAL_Error("get a timestamp = %lld, mPreTimestamp = %lld,averageTimeStamp = %lld,",timestamp,pIM_VDEC->mPreTimestamp,pIM_VDEC->mAverageTimeStamp);
    //	IM_OSAL_Error("total outgoing frame number : %d,first timestamp :%lld\n",pIM_VDEC->nOutGoingFrameCount,pIM_VDEC->mFirstFrameTimestamp);
    if(timestamp <= pIMVDec->mPreTimestamp && timestamp != 0){
        IM_OSAL_Error("parser cheat us pretimestamp: %lld   nowtimestamp: %lld\n",pIMVDec->mPreTimestamp,timestamp);
#ifdef TIMESTAMP_WORKAROUND
        pIMVDec->mPreTimestamp = timestamp;
#else
        skipCalc = true;
#endif

	}else{
	    pIMVDec->mPreTimestamp = timestamp;
    }
	if(pIMVDec->nOutGoingFrameCount == 0){ 
		pIMVDec->mFirstFrameTimestamp = timestamp;
	}else if(pIMVDec->nOutGoingFrameCount > 2 && !skipCalc){
		pIMVDec->mAverageTimeStamp = (timestamp - pIMVDec->mFirstFrameTimestamp)/(pIMVDec->nOutGoingFrameCount - 1);
	}
    if(!skipCalc)
	    pIMVDec->nOutGoingFrameCount++;

	return timestamp;

}

void IM_VDEC_StoreTimeStamp(IM_ON2DEC_HANDLE *pIMVDec, OMX_TICKS timestamp)
{
	IM_OSAL_Info("store timestamp %lld(us)\n", timestamp);
#if 0
	//indicate that this is an h264 interlaced video stream
	if(pIMVDec->bInterlacedFrame && pIMVDec->vdecConfig.inType.codec == IM_STRM_VIDEO_H264){
		if(pIMVDec->bFirstFieldFrame){
			//we must prove that all the h264 interlaced video streams have the feather: two input package make an output frame
			//if can't , this check is wrong!!!!
			pIMVDec->bFirstFieldFrame = OMX_FALSE;
			IM_OSAL_Warning("discard the timestamp because of the interlaced \n");
			return;
		}
	}
#endif 
	TSMgmt *TSNode = NULL;
	IM_OMX_FindStructAddr(iTSMgmt, TSNode);

	if(TSNode == NULL){
		IM_OSAL_Warning("timestamp list is full,discard the smallest timestamp!\n");
		IM_VDEC_GetTimeStamp(pIMVDec);
		IM_OMX_FindStructAddr(iTSMgmt, TSNode);
		if(TSNode == NULL){
			return;
		}
	}

	TSNode->timeStamp = timestamp;
	IM_OMX_InsertListNodeAtTail(TS, iTSMgmt);
	if(pIMVDec->bFirstFieldFrame == OMX_FALSE){
		pIMVDec->bFirstFieldFrame = OMX_TRUE;
	}

	return;
}

void IM_VDEC_DiscardTimeStamp(IM_ON2DEC_HANDLE *pIMVDec)
{
	IM_OSAL_Info("DiscardTimestamp()\n");
	if(pIMVDec->iTSMgmtTail == NULL)
	{
		IM_OSAL_Warning("no timesttamp should be discard\n");
		return ;
	}else{
		if(pIMVDec->nOutGoingFrameCount == 0){
			pIMVDec->mFirstFrameTimestamp = pIMVDec->iTSMgmtTail->timeStamp;	
		}
		TSMgmt *TsNode = pIMVDec->iTSMgmtTail;
		pIMVDec->iTSMgmtTail = pIMVDec->iTSMgmtTail->prev;

		if(pIMVDec->iTSMgmtTail == NULL){
			pIMVDec->iTSMgmtHead = NULL;
		}else{
			pIMVDec->iTSMgmtTail->next = NULL;
		}
		IM_OSAL_Memset(TsNode, 0, sizeof(TSMgmt));
	}
	pIMVDec->nOutGoingFrameCount++;
	return ;
}

OMX_ERRORTYPE IM_VDEC_setBufferFlags(
		IM_OMX_BASECOMPONENT *pIMComponent, 
		OMX_BUFFERHEADERTYPE *bufferHeader, 
		OMX_U32 flags, 
		OMX_U32 nPortIndex)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	IM_OMX_BASEPORT      *pIMPort = &pIMComponent->pIMPort[nPortIndex];
	OMX_U32  i = 0;

	for (i = 0; i < pIMPort->portDefinition.nBufferCountActual; i++) {
		if(pIMPort->extendBufferHeader[i].OMXBufferHeader == bufferHeader){
			pIMPort->bufferStateAllocate[i] |= flags;
			break;
		}
	}

	if(i == pIMPort->portDefinition.nBufferCountActual){
		ret = OMX_ErrorBadParameter; 
		IM_OSAL_Error("can not found the bufferheader");
	}
	return ret;
}

OMX_ERRORTYPE IM_VDEC_clearBufferFlags(
		IM_OMX_BASECOMPONENT *pIMComponent, 
		OMX_BUFFERHEADERTYPE *bufferHeader, 
		OMX_U32 flags, 
		OMX_U32 nPortIndex)
{	

	OMX_ERRORTYPE          ret = OMX_ErrorNone;
	IM_OMX_BASEPORT      *pIMPort = &pIMComponent->pIMPort[nPortIndex];
	OMX_U32   i = 0;

	for (i = 0; i < pIMPort->portDefinition.nBufferCountActual; i++) {
		if(pIMPort->extendBufferHeader[i].OMXBufferHeader == bufferHeader){
			pIMPort->bufferStateAllocate[i] &= ~flags;
			break;
		}
	}

	if(i == pIMPort->portDefinition.nBufferCountActual){
		ret = OMX_ErrorBadParameter; 
		IM_OSAL_Error("can not found the bufferheader");
	}

	return ret;
}

OMX_ERRORTYPE IM_VDEC_FlushDecLib(IM_ON2DEC_HANDLE *pOn2DecHandle, OMX_U32 flag)
{
	IM_OSAL_Info("IM_VDEC_FlushDecLib %d\n",flag);
	vdec_flush(pOn2DecHandle->hOn2DecLib);
	return 	OMX_ErrorNone;
}

OMX_ERRORTYPE IM_VDEC_Flush(OMX_COMPONENTTYPE *pOMXComponent, OMX_U32 nPortIndex)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	IM_OMX_BASECOMPONENT     *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
	IM_OMX_BASEPORT        	 *pIMInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
	IM_OMX_BASEPORT          *pIMOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    IM_OMX_VIDEODEC_COMPONENT *pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;
    IM_ON2DEC_HANDLE           *pOn2DecHandle = (IM_ON2DEC_HANDLE *)pOn2DecComponent->hCodecHandle;
	OMX_U32 i;
	IM_OSAL_Info("IM_VDEC_Flush : portIndex : %d\n",nPortIndex);

	if(nPortIndex != OUTPUT_PORT_INDEX){
		IM_VDEC_FlushDecLib(pOn2DecHandle, 2);
		//flush timestampQ
		while(pOn2DecHandle->iTSMgmtHead != NULL)
		{
			pTSMgmt TSNode = pOn2DecHandle->iTSMgmtHead; 
			pOn2DecHandle->iTSMgmtHead = pOn2DecHandle->iTSMgmtHead->next;
			//TSNode->used = 0;
			IM_OSAL_Memset(TSNode, 0, sizeof(TSMgmt));
		}
		//reset some flags 
		pOn2DecHandle->iTSMgmtTail = NULL;
		pOn2DecHandle->nOutGoingFrameCount = 0;
		pOn2DecHandle->mPreTimestamp = 0;		
		pOn2DecHandle->mAverageTimeStamp = 0;	
		pOn2DecHandle->mFirstFrameTimestamp = 0;
	}

	if(nPortIndex != INPUT_PORT_INDEX ){
		IM_VDEC_FlushDecLib(pOn2DecHandle, 0);
		//flush the buffer owned by declib
		for (i = 0; i < pIMOutputPort->portDefinition.nBufferCountActual; i++) {
			if(pIMOutputPort->bufferStateAllocate[i] & BUFFER_OWNED_BY_DECLIB){
				pIMComponent->pCallbacks->FillBufferDone(pOMXComponent, \
						pIMComponent->callbackData, pIMOutputPort->extendBufferHeader[i].OMXBufferHeader);
				pIMOutputPort->bufferStateAllocate[i] &= ~BUFFER_OWNED_BY_DECLIB;
			}
		}
	}
	//reset some flags 
	pOn2DecHandle->bHaveFrameOut = OMX_FALSE;
	pOn2DecHandle->bFirstFieldFrame = OMX_FALSE;
	pOn2DecHandle->bStreamEOS = OMX_FALSE;

    if(pOn2DecHandle->nConfigMultiBufferNum != 0) {
        IM_OSAL_Free(pOn2DecHandle->vdecBuffers);
        pOn2DecHandle->nConfigMultiBufferNum = 0;
    }
    
	return ret;
}

OMX_BOOL IM_VDEC_PostProcessVideoFrame(OMX_BUFFERHEADERTYPE *buffer)
{
	//TODO
	return OMX_TRUE;
}

OMX_BOOL IM_VDEC_SkipFrame(OMX_COMPONENTTYPE *pOMXComponent,OMX_S64 delayTime)
{
	OMX_BOOL rstValue = OMX_FALSE;
	IM_OMX_BASECOMPONENT     *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_VIDEODEC_COMPONENT *pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;
	IM_ON2DEC_HANDLE           *pOn2DecHandle = (IM_ON2DEC_HANDLE *)pOn2DecComponent->hCodecHandle;

	if(!pOn2DecHandle->bHeaderReady)
		return rstValue;

	if(delayTime > LATE_TO_DROP_SINGLE){
		if(delayTime > LATE_TO_DROP_TRIL)
			rstValue = OMX_TRUE;
		else if(delayTime > LATE_TO_DROP_DOUBLE
				&& pOn2DecHandle->nContiguousSkip < DROP_TWO_THRESHOLD)

			rstValue = OMX_TRUE;
		else if(pOn2DecHandle->nContiguousSkip < DROP_ONE_THRESHOLD)
			rstValue = OMX_TRUE;
	}
	return rstValue;
}



OMX_BOOL IM_VDEC_GetVideoFrame(OMX_COMPONENTTYPE *pOMXComponent, IM_VDEC_PICTURE *pVdecPic, OMX_ERRORTYPE *ret)
{
	IM_OMX_BASECOMPONENT     *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_VIDEODEC_COMPONENT *pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;
	IM_ON2DEC_HANDLE           *pOn2DecHandle = (IM_ON2DEC_HANDLE *)pOn2DecComponent->hCodecHandle;
	IM_OMX_BASEPORT          *pIMOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
	IM_OMX_DATABUFFER 		*pOutputUseBuffer = &pIMComponent->IMDataBuffer[OUTPUT_PORT_INDEX];
	IM_OMX_DATABUFFER 		*pInputUseBuffer = &pIMComponent->IMDataBuffer[INPUT_PORT_INDEX];
	OMX_S32 ii = 0;
    *ret = OMX_ErrorNone;

	IM_RET retCode = vdec_get_next_picture(pOn2DecHandle->hOn2DecLib, pVdecPic);

	IM_OSAL_Info("try to get decoded frame");
	if(retCode == IM_RET_VDEC_FAIL){
		//we ignore this error, this is a potential bug?
		IM_OSAL_Error("vc_get_decoded_frame() failed\n");
		pOn2DecHandle->bHaveFrameOut = OMX_FALSE;
	}else if(retCode == IM_RET_VDEC_MORE_PIC_OUT || retCode == IM_RET_VDEC_MOSAIC_PIC_OUT){
		IM_OSAL_Info("get reference frame \n");
		pOn2DecHandle->bHaveFrameOut = OMX_TRUE;
		//pOutputUseBuffer->bufferHeader = IM_VDEC_GetOutBufferByPhyAddr(mDecOut.pDecBufferInfo.OutUnitbuf.bus_addr);

		if(pOutputUseBuffer->phyAddr != pVdecPic->buffer.busAddr){
			IM_OSAL_Info("UseBuffer is in declib");
			IM_VDEC_setBufferFlags(pIMComponent, pOutputUseBuffer->bufferHeader, BUFFER_OWNED_BY_DECLIB, OUTPUT_PORT_INDEX);
			pOutputUseBuffer->bufferHeader = getbufferHeaderbyPhyAddr(pOMXComponent, OUTPUT_PORT_INDEX, 
					pVdecPic->buffer.busAddr);

            IM_VDEC_clearBufferFlags(pIMComponent, pOutputUseBuffer->bufferHeader, BUFFER_OWNED_BY_DECLIB, OUTPUT_PORT_INDEX);

        } else if(pIMOutputPort->portDefinition.bBuffersContiguous == OMX_FALSE) {
            IM_OSAL_Memcpy(pOutputUseBuffer->bufferHeader->pBuffer, pVdecPic->buffer.virtAddr, pVdecPic->dataLen);
        }
        IM_OSAL_Info("out buffer phy addr %x", pVdecPic->buffer.busAddr);
        pOutputUseBuffer->bufferHeader->nFilledLen = pVdecPic->dataLen; 
        pOutputUseBuffer->bufferHeader->nTimeStamp = IM_VDEC_GetTimeStamp(pOn2DecHandle);
        //pOutputUseBuffer->bufferHeader->nTimestamp = IM_VDEC_GetTimeStamp();
#ifdef USE_ANDROID_EXTENSION
		if(pIMComponent->pIMPort[OUTPUT_PORT_INDEX].bUseAndroidNativeBuffer == OMX_TRUE){
			putVirtAddrtoANB((OMX_PTR)pOutputUseBuffer->bufferHeader->pBuffer);
		}
#endif
		if(pOn2DecHandle->bStreamEOS == OMX_TRUE){
            IM_OSAL_Info("set eos2");
			pOutputUseBuffer->bufferHeader->nFlags |= OMX_BUFFERFLAG_EOS;
		}

		if(pOn2DecHandle->nPrerollNum > 0 || (retCode == IM_RET_VDEC_MOSAIC_PIC_OUT &&
                    ((pOn2DecHandle->bFirstFrame && pOn2DecHandle->nSkipFrameCount < 5) 
                    || (!pOn2DecHandle->bFirstFrame && pOn2DecHandle->nSkipFrameCount < 20)))){
            pOn2DecHandle->nSkipFrameCount++;
			if(pOn2DecHandle->nPrerollNum > 0)
				pOn2DecHandle->nPrerollNum--;
			pOutputUseBuffer->bufferHeader->nFilledLen = 0;
		}
	} else if(retCode == IM_RET_VDEC_HW_TIME_OUT) {
            *ret = OMX_ErrorHardware;
    }else{
		IM_OSAL_Info("there is no decoded frame to get");
		pOn2DecHandle->bHaveFrameOut = OMX_FALSE;
	}
	return pOn2DecHandle->bHaveFrameOut;
}

OMX_ERRORTYPE IM_VDEC_Decode(OMX_COMPONENTTYPE *pOMXComponent, IM_OMX_DATABUFFER *pInputUseBuffer, IM_OMX_DATABUFFER *pOutputUseBuffer)
{
	OMX_ERRORTYPE               ret = OMX_ErrorNone;
	IM_OMX_BASECOMPONENT       *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_VIDEODEC_COMPONENT  *pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;

	IM_OMX_BASEPORT        	   *pIMInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
	IM_OMX_BASEPORT            *pIMOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
	IM_ON2DEC_HANDLE           *pOn2DecHandle = (IM_ON2DEC_HANDLE *)pOn2DecComponent->hCodecHandle;
	OMX_S64 				    delayTime = 0;
	OMX_BOOL 			        trySkipThisTimestamp = OMX_FALSE;
	IM_VDEC_IN				   *pVdecIn = &pOn2DecHandle->vdecIn;
	IM_VDEC_OUT				   *pVdecOut = &pOn2DecHandle->vdecOut;
	IM_VDEC_PICTURE			   *pVdecPic = &pOn2DecHandle->vdecPic;
	IM_RET 	                    decodeRet;


	IM_OSAL_Entering();
	if(pIMInputPort->portDefinition.bBuffersContiguous == OMX_TRUE){
		pInputUseBuffer->phyAddr = getPhyAddrfromNB(pInputUseBuffer->bufferHeader->pBuffer, pOn2DecComponent->bUseMMU);
		pInputUseBuffer->virAddr = getVirtAddrfromNB(pInputUseBuffer->bufferHeader->pBuffer);
	}else{
		pInputUseBuffer->virAddr = getVirtAddrfromNB(pOn2DecHandle->pSelfInBuffer);
		pInputUseBuffer->phyAddr = getPhyAddrfromNB(pOn2DecHandle->pSelfInBuffer, pOn2DecComponent->bUseMMU);

		if(pInputUseBuffer->bDataProcessed == OMX_FALSE)
			IM_OSAL_Memcpy(pInputUseBuffer->virAddr, pInputUseBuffer->bufferHeader->pBuffer + pInputUseBuffer->bufferHeader->nOffset,\
					pInputUseBuffer->bufferHeader->nFilledLen);		
	}

#ifdef USE_ANDROID_EXTENSION
	if(pIMOutputPort->bUseAndroidNativeBuffer == OMX_TRUE){
		pOutputUseBuffer->phyAddr = getPhyAddrfromANB(pOMXComponent, pOn2DecComponent->hMemoryHandle, OUTPUT_PORT_INDEX, pOutputUseBuffer->bufferHeader->pBuffer);
		pOutputUseBuffer->virAddr = getVirtAddrfromANB(pOutputUseBuffer->bufferHeader->pBuffer);
		IM_OSAL_Info("androidbuffer virt addr %x, phy addr %x",pOutputUseBuffer->virAddr, pOutputUseBuffer->phyAddr);
	}
#endif
	if(pIMOutputPort->portDefinition.bBuffersContiguous == OMX_TRUE) {
        if(pIMOutputPort->bUseAndroidNativeBuffer == OMX_FALSE){
            pOutputUseBuffer->phyAddr = getPhyAddrfromNB(pOutputUseBuffer->bufferHeader->pBuffer, pOn2DecComponent->bUseMMU);
            pOutputUseBuffer->virAddr = getVirtAddrfromNB(pOutputUseBuffer->bufferHeader->pBuffer);
        }
    } else {
        pOutputUseBuffer->phyAddr = getPhyAddrfromNB(pOn2DecHandle->pSelfOutBuffer, pOn2DecComponent->bUseMMU);
        pOutputUseBuffer->virAddr = getVirtAddrfromNB(pOn2DecHandle->pSelfOutBuffer);
    }
    if(pOn2DecHandle->bResizeNeeded == OMX_TRUE && pOn2DecHandle->bHeaderReady == OMX_TRUE){
        //Buffer Size or Count Changed, we must notify the client 
        IM_OSAL_Info("resize needed(multibuffer mode[%d],bConfigMultiBufferNeeded[%d])",pOn2DecHandle->bMultiBufferMode,pOn2DecHandle->bConfigMultiBufferNeeded);
        IM_OSAL_Info("nBufferCountActual[%d],nMultiBufferNum[%d]\n",pIMOutputPort->portDefinition.nBufferCountActual,pOn2DecHandle->nMultiBufferNum);
        if(pOn2DecHandle->bConfigMultiBufferNeeded == OMX_TRUE) {
            if(pIMOutputPort->portDefinition.nBufferCountActual <  pOn2DecHandle->nMultiBufferNum + MIN_VIDEO_OUTPUTBUFFER_NUM){
                pIMOutputPort->portDefinition.nBufferCountMin = pOn2DecHandle->nMultiBufferNum + MIN_VIDEO_OUTPUTBUFFER_NUM;
            } else {
                pOn2DecHandle->bResizeNeeded = OMX_FALSE;
            }
        }

        if(pOn2DecHandle->bVideoSizeChanged) {
            pIMOutputPort->portDefinition.format.video.nFrameWidth  = pOn2DecHandle->vdecInfo.codedWidth;
            pIMOutputPort->portDefinition.format.video.nFrameHeight  = pOn2DecHandle->vdecInfo.codedHeight;
            pOn2DecHandle->bResizeNeeded = OMX_TRUE;
        }

        if(pOn2DecHandle->bResizeNeeded == OMX_TRUE) {
            pIMComponent->pCallbacks->EventHandler(
                    (OMX_HANDLETYPE)pOMXComponent,
                    pIMComponent->callbackData,
                    OMX_EventPortSettingsChanged, //The command was completed
                    OUTPUT_PORT_INDEX,
                    0,
                    NULL);
            IM_OSAL_Info("resize out port return");

            IM_OSAL_MutexLock(pIMOutputPort->portMutex);
            pOn2DecHandle->bOutportChangedPending = OMX_TRUE;
            IM_OSAL_MutexUnlock(pIMOutputPort->portMutex);
            pOn2DecHandle->bResizeNeeded = OMX_FALSE;
        }
        goto EXIT;
    }

    IM_OSAL_MutexLock(pIMOutputPort->portMutex);
    if(pOn2DecHandle->bOutportChangedPending == OMX_TRUE){
        IM_OSAL_MutexUnlock(pIMOutputPort->portMutex);
        IM_OSAL_Info("wait port reconfig....");
        ret = IM_OMX_WAIT_PORT_RECONFIG;
        goto EXIT;
    }else{
        IM_OSAL_MutexUnlock(pIMOutputPort->portMutex);
    }

    if(pOn2DecHandle->bConfigMultiBufferNeeded == OMX_TRUE){
        if(pOn2DecHandle->nConfigMultiBufferNum == 0) {
            pOn2DecHandle->vdecBuffers = IM_OSAL_Malloc(sizeof(IM_INT32) + sizeof(IM_VDEC_BUFFER) * pOn2DecHandle->nMultiBufferNum); 
            pOn2DecHandle->vdecBuffers->num = pOn2DecHandle->nMultiBufferNum;
        }
        if(pOn2DecHandle->nConfigMultiBufferNum < pOn2DecHandle->nMultiBufferNum){
            pOn2DecHandle->vdecBuffers->buffer[pOn2DecHandle->nConfigMultiBufferNum].virtAddr = pOutputUseBuffer->virAddr;
            pOn2DecHandle->vdecBuffers->buffer[pOn2DecHandle->nConfigMultiBufferNum].busAddr = pOutputUseBuffer->phyAddr;
            pOn2DecHandle->vdecBuffers->buffer[pOn2DecHandle->nConfigMultiBufferNum].size = pOutputUseBuffer->bufferHeader->nAllocLen;
            pOn2DecHandle->nConfigMultiBufferNum++;

            IM_OSAL_Info("multibuffer  buffer num = %d, now =%d, ", pOn2DecHandle->nMultiBufferNum, pOn2DecHandle->nConfigMultiBufferNum);
            IM_VDEC_setBufferFlags(pIMComponent, pOutputUseBuffer->bufferHeader, BUFFER_OWNED_BY_DECLIB, OUTPUT_PORT_INDEX);
            pIMComponent->IM_BufferReset(pOMXComponent, OUTPUT_PORT_INDEX);
            goto EXIT;
        }

        if(IM_RET_VDEC_OK != vdec_set_multibuffer(pOn2DecHandle->hOn2DecLib, pOn2DecHandle->vdecBuffers)){
            IM_OSAL_Error("vdec_set_multibuffer() failed\n");
            ret = OMX_ErrorBadParameter;
            goto EXIT;
        }

        IM_OSAL_Free(pOn2DecHandle->vdecBuffers);
        IM_OSAL_Info("CONFIG MULTIBUFFER SUCCESS");
        pOn2DecHandle->nConfigMultiBufferNum = 0;
        pOn2DecHandle->bConfigMultiBufferNeeded = OMX_FALSE;
    }

    if(pOn2DecHandle->bVideoSizeChanged == OMX_TRUE) {
        vdec_get_config(pOn2DecHandle->hOn2DecLib, &pOn2DecHandle->vdecConfig);
        pOn2DecHandle->vdecConfig.outType.width = pIMOutputPort->portDefinition.format.video.nFrameWidth;
        pOn2DecHandle->vdecConfig.outType.height = pIMOutputPort->portDefinition.format.video.nFrameHeight;
        IM_OSAL_Warning("new size reconfig [%d x %d]", pOn2DecHandle->vdecConfig.outType.width, pOn2DecHandle->vdecConfig.outType.height);

        if(vdec_set_config(pOn2DecHandle->hOn2DecLib, &pOn2DecHandle->vdecConfig) != IM_RET_VDEC_OK) {
            ret = OMX_ErrorBadParameter;
            goto EXIT;
        }
        pOn2DecHandle->bVideoSizeChanged = OMX_FALSE;
    }

    pVdecPic->buffer.virtAddr = pOutputUseBuffer->virAddr;
    pVdecPic->buffer.busAddr = pOutputUseBuffer->phyAddr;
    pVdecPic->buffer.size = pOutputUseBuffer->bufferHeader->nAllocLen;
    IM_OSAL_Info("input vir addr = %x, offset = %d", pInputUseBuffer->virAddr, pInputUseBuffer->bufferHeader->nOffset);
    pVdecIn->pStream  = (OMX_U8 *)pInputUseBuffer->virAddr + pInputUseBuffer->bufferHeader->nOffset;
    pVdecIn->busAddr = pInputUseBuffer->phyAddr + pInputUseBuffer->bufferHeader->nOffset;
    pVdecIn->dataLen = pInputUseBuffer->bufferHeader->nFilledLen;
    pVdecIn->flags = 0;

    if(pInputUseBuffer->bDataProcessed == OMX_FALSE){
        OMX_S64 *inputPrivate = NULL;
        inputPrivate = (OMX_S64 *)(pInputUseBuffer->bufferHeader->pInputPortPrivate);
        if(inputPrivate != NULL)
            delayTime = inputPrivate[0];
    }

#ifdef SKIP_FRAME
    if(IM_VDEC_SkipFrame(pOMXComponent,delayTime) == OMX_TRUE){
        pVdecIn->flags = IM_VDEC_PIC_SKIP;
    }	
#endif 

    if(pOn2DecHandle->bHaveFrameOut == OMX_TRUE){
        if(IM_VDEC_GetVideoFrame(pOMXComponent, pVdecPic, &ret) == OMX_TRUE){
            goto EXIT;
        }
    } 

    if(pInputUseBuffer->bDataProcessed == OMX_TRUE && pInputUseBuffer->nRemainDataLeft > 1){
    }else{
        if(pInputUseBuffer->bufferHeader->nFlags & OMX_BUFFERFLAG_EOS){
            pOn2DecHandle->bStreamEOS = OMX_TRUE;
            IM_OSAL_Info("input buffer signal eos ");
            if(pInputUseBuffer->bufferHeader->nFilledLen == 0){
                IM_OSAL_Warning("set  eos");
                pInputUseBuffer->bDataProcessed =  OMX_TRUE;
                pOutputUseBuffer->bufferHeader->nFlags |= OMX_BUFFERFLAG_EOS;
                goto EXIT;
            }
        }

        if(!(pInputUseBuffer->bufferHeader->nFlags & OMX_BUFFERFLAG_CODECCONFIG)){
            IM_VDEC_StoreTimeStamp(pOn2DecHandle, pInputUseBuffer->bufferHeader->nTimeStamp);
            IM_OSAL_Info("in timestamp = %lld", pInputUseBuffer->bufferHeader->nTimeStamp);
        }
        if(pInputUseBuffer->bufferHeader->nFlags & OMX_BUFFERFLAG_DECODEONLY){
            pOn2DecHandle->nPrerollNum++;
        }
    }

    pVdecOut->skipPicNum = 0;
    decodeRet = vdec_decode(pOn2DecHandle->hOn2DecLib,pVdecIn, pVdecOut, pVdecPic);
    IM_OSAL_Info("vc_decode return %d",decodeRet);

    switch(decodeRet) {
        case IM_RET_VDEC_MORE_PIC_OUT:
        case IM_RET_VDEC_MOSAIC_PIC_OUT:
            {
                pInputUseBuffer->bHardwareUsed = OMX_TRUE;
                IM_OSAL_Info("decode a frame out");
                pOn2DecHandle->bHaveFrameOut = OMX_TRUE;

                if(pOutputUseBuffer->phyAddr != pVdecPic->buffer.busAddr){
                    IM_OSAL_Info("UseBuffer is in declib");
                    IM_VDEC_setBufferFlags(pIMComponent, pOutputUseBuffer->bufferHeader, BUFFER_OWNED_BY_DECLIB, OUTPUT_PORT_INDEX);
                    pOutputUseBuffer->bufferHeader = getbufferHeaderbyPhyAddr(pOMXComponent, OUTPUT_PORT_INDEX, 
                            pVdecPic->buffer.busAddr);
                    IM_VDEC_clearBufferFlags(pIMComponent, pOutputUseBuffer->bufferHeader, BUFFER_OWNED_BY_DECLIB, OUTPUT_PORT_INDEX);
                } else if(pIMOutputPort->portDefinition.bBuffersContiguous == OMX_FALSE) {
                    IM_OSAL_Memcpy(pOutputUseBuffer->bufferHeader->pBuffer, pVdecPic->buffer.virtAddr, pVdecPic->dataLen);
                }
                IM_OSAL_Info("out phy buffer xxxx %x",pVdecPic->buffer.busAddr);
                pOutputUseBuffer->bufferHeader->nFilledLen = pVdecPic->dataLen; 
                pOutputUseBuffer->bufferHeader->nTimeStamp = IM_VDEC_GetTimeStamp(pOn2DecHandle);
                IM_OSAL_Info("out timestamp0 = %lld", pOutputUseBuffer->bufferHeader->nTimeStamp);
                //IM_OSAL_Error("out decode size = %d", mDecOut.pDecBufferInfo.DecodeOutSize);
                //pOutputUseBuffer->bufferHeader->nTimestamp = IM_VDEC_GetTimeStamp();
#ifdef USE_ANDROID_EXTENSION
                if(pIMOutputPort->bUseAndroidNativeBuffer == OMX_TRUE){
                    putVirtAddrtoANB(pOutputUseBuffer->bufferHeader->pBuffer);
                }
#endif

                if(pOn2DecHandle->bStreamEOS == OMX_TRUE){
                    IM_OSAL_Info("set  eos1");
                    pOutputUseBuffer->bufferHeader->nFlags |= OMX_BUFFERFLAG_EOS;
                }

                if(pOn2DecHandle->nPrerollNum > 0 || (decodeRet == IM_RET_VDEC_MOSAIC_PIC_OUT &&
                    ((pOn2DecHandle->bFirstFrame && pOn2DecHandle->nSkipFrameCount < 5) 
                    || (!pOn2DecHandle->bFirstFrame && pOn2DecHandle->nSkipFrameCount < 20)))){
                    pOn2DecHandle->nSkipFrameCount++;                 
                    if(pOn2DecHandle->nPrerollNum > 0)
                        pOn2DecHandle->nPrerollNum--;
                    pOutputUseBuffer->bufferHeader->nFilledLen = 0;
                } else if(pOn2DecHandle->bFirstFrame){
                    pOn2DecHandle->bFirstFrame = OMX_FALSE;
                    pOn2DecHandle->nSkipFrameCount = 50;                 
                }
                if(pOn2DecComponent->bThumbnailMode == OMX_TRUE && pOutputUseBuffer->bufferHeader->nFilledLen > 0){
                    //we should no some process:
                    //1,change the format to yuv420planar
                    //2,cut down some lines which are filled autoly by decoded
                    IM_VDEC_PostProcessVideoFrame(pOutputUseBuffer->bufferHeader);
                }
            }
            break;
        case IM_RET_VDEC_HDRS_RDY:
            {
                pInputUseBuffer->bHardwareUsed = OMX_TRUE;
                vdec_get_info(pOn2DecHandle->hOn2DecLib, &pOn2DecHandle->vdecInfo);

                IM_OSAL_Info("header ready ");
                pOn2DecHandle->bHeaderReady = OMX_TRUE;
                pOn2DecHandle->bInterlacedFrame = (OMX_BOOL)pOn2DecHandle->vdecInfo.interlacedSequence;
                pOn2DecHandle->bHaveFrameOut = OMX_FALSE;
                pInputUseBuffer->bHardwareUsed = OMX_TRUE; 
            }
            break;	
        case IM_RET_VDEC_LACK_OUTBUFF:
            {
                IM_OSAL_Info("VC_LACK_OUTBUFF, buffer num = %d", pVdecOut->multiBuffPpSize);

                pOn2DecHandle->nMultiBufferNum = pVdecOut->multiBuffPpSize;
                pOn2DecHandle->bConfigMultiBufferNeeded = OMX_TRUE;
                pOn2DecHandle->bResizeNeeded = OMX_TRUE;
                pOn2DecHandle->bMultiBufferMode = OMX_TRUE;
                pOn2DecHandle->bHeaderReady = OMX_TRUE;
                pOn2DecHandle->bHaveFrameOut = OMX_FALSE;
                pInputUseBuffer->bHardwareUsed = OMX_TRUE; 
            }
            break;
        case IM_RET_VDEC_PIC_SKIPPED:
            {
                //plus the lateless time
                if((pOn2DecHandle->vdecConfig.inType.codec == IM_STRM_VIDEO_MPEG1VIDEO ||
                            pOn2DecHandle->vdecConfig.inType.codec == IM_STRM_VIDEO_MPEG2VIDEO) &&
                        pOn2DecHandle->bHeaderReady){
                    IM_OSAL_Info("MPEG1/2 could discard the left data!\n");
                    pVdecOut->dataLeft = 0;
                }
            }
            pOn2DecHandle->nContiguousSkip++;
            IM_OSAL_Info("skip frame\n");
            trySkipThisTimestamp = OMX_TRUE;
            pOn2DecHandle->bHaveFrameOut = OMX_FALSE;
            break;
        case IM_RET_VDEC_DECINFO_CHANGED:
            vdec_get_info(pOn2DecHandle->hOn2DecLib, &pOn2DecHandle->vdecInfo);
            pOn2DecHandle->bInterlacedFrame = (OMX_BOOL)pOn2DecHandle->vdecInfo.interlacedSequence;
            if(pOn2DecHandle->nMultiBufferNum != (OMX_U32)pOn2DecHandle->vdecInfo.multiBuffPpSize &&
                    pOn2DecHandle->nMultiBufferNum != 0) {
                IM_OSAL_Warning("multiBuffer num changed %d ==> %d", pOn2DecHandle->nMultiBufferNum, pOn2DecHandle->vdecInfo.multiBuffPpSize);
                pOn2DecHandle->nMultiBufferNum = pOn2DecHandle->vdecInfo.multiBuffPpSize;
                pOn2DecHandle->bConfigMultiBufferNeeded = OMX_TRUE;
                pOn2DecHandle->bResizeNeeded = OMX_TRUE;
            }

            if(pOn2DecHandle->vdecInfo.codedWidth != pOn2DecHandle->vdecConfig.inType.width
                    || pOn2DecHandle->vdecInfo.codedHeight != pOn2DecHandle->vdecConfig.inType.height)
            {
                IM_OSAL_Warning("input size Changed!!");
                pOn2DecHandle->vdecConfig.inType.width = pOn2DecHandle->vdecInfo.codedWidth;
                pOn2DecHandle->vdecConfig.inType.height = pOn2DecHandle->vdecInfo.codedHeight;
                pOn2DecHandle->bVideoSizeChanged = OMX_TRUE;
                pOn2DecHandle->bResizeNeeded = OMX_TRUE;
            }

            pInputUseBuffer->bHardwareUsed = OMX_TRUE; 
            break;
        case IM_RET_VDEC_NO_PIC_OUT:
            pInputUseBuffer->bHardwareUsed = OMX_TRUE; 
            pOn2DecHandle->bHaveFrameOut = OMX_FALSE;
            break;
        default:
            if(decodeRet < 0 && decodeRet != IM_RET_VDEC_STRM_PROCESSED){
                IM_OSAL_Error("vc_failed");
            }else{
                IM_OSAL_Info("no picture out! %d ", decodeRet);
            }
            pOn2DecHandle->bHaveFrameOut = OMX_FALSE;
            trySkipThisTimestamp = OMX_TRUE;
    }

    if(decodeRet != IM_RET_VDEC_PIC_SKIPPED){
        pOn2DecHandle->nContiguousSkip = 0;
    }

    IM_OSAL_Info("data left = %d",	pVdecOut->dataLeft);
    //set inputUseBuffer offset
    pInputUseBuffer->bufferHeader->nOffset += pVdecIn->dataLen - pVdecOut->dataLeft;
    pInputUseBuffer->bufferHeader->nFilledLen = pVdecOut->dataLeft;
    pInputUseBuffer->nRemainDataLeft = pVdecOut->dataLeft;
    pInputUseBuffer->bDataProcessed =  OMX_TRUE;

    if(pOn2DecHandle->bStreamEOS == OMX_TRUE && decodeRet < 0){
        IM_OSAL_Info("set  eos4");
        pOutputUseBuffer->bufferHeader->nFlags |= OMX_BUFFERFLAG_EOS;
    }
    if(decodeRet < 0){
        pOn2DecHandle->nDecodedErrorNum++;
        if(pOn2DecHandle->nDecodedErrorNum > MAX_ERROR_NUM){
            ret = OMX_ErrorStreamCorrupt;
        }

        if(decodeRet == IM_RET_VDEC_HW_TIME_OUT) {
            ret = OMX_ErrorHardware;
        }
        if(decodeRet != IM_RET_VDEC_STRM_PROCESSED)
            IM_OSAL_Error("vc_decode failed,return %d\n",decodeRet);
    }else{
        pOn2DecHandle->nDecodedErrorNum = 0;
    }
    if(trySkipThisTimestamp &&
            pInputUseBuffer->bHardwareUsed == OMX_FALSE &&
            pInputUseBuffer->nRemainDataLeft <= 0){
        //this means the all data of this input buffer has been proecessed,and it never indicated to be used by hardware decode	
        //now we could discard the smallest timestamp.
        IM_OSAL_Warning("decode lib process the whole buffer ,skip the timestamp\n");
        IM_VDEC_DiscardTimeStamp(pOn2DecHandle);
    }
EXIT:
    if(ret == OMX_ErrorNone &&
            pOn2DecHandle->bHaveFrameOut == OMX_TRUE &&
            (pIMOutputPort->cropRectangle.nWidth != (pIMOutputPort->portDefinition.format.video.nStride - pVdecPic->cropWidth) || 
             pIMOutputPort->cropRectangle.nHeight != (pIMOutputPort->portDefinition.format.video.nSliceHeight - pVdecPic->cropHeight))){
        IM_OSAL_Warning("cut num changed ,old(%d,%d)  vs new(%d,%d)\n",pIMOutputPort->portDefinition.format.video.nStride -pIMOutputPort->cropRectangle.nWidth,pIMOutputPort->portDefinition.format.video.nSliceHeight -pIMOutputPort->cropRectangle.nHeight,
                pVdecPic->cropWidth, pVdecPic->cropHeight);
        pIMOutputPort->cropRectangle.nWidth = (pIMOutputPort->portDefinition.format.video.nStride - pVdecPic->cropWidth); 
        pIMOutputPort->cropRectangle.nHeight = (pIMOutputPort->portDefinition.format.video.nSliceHeight - pVdecPic->cropHeight);

        pIMComponent->pCallbacks->EventHandler(
                (OMX_HANDLETYPE)pOMXComponent,
                pIMComponent->callbackData,
                OMX_EventPortSettingsChanged,
                OUTPUT_PORT_INDEX,
                OMX_IndexConfigCommonOutputCrop,
                NULL);
    }
    IM_OSAL_Exiting();

    return ret;
}

/* Video Decode */
OMX_ERRORTYPE IM_VDEC_bufferProcess(OMX_COMPONENTTYPE *pOMXComponent, IM_OMX_DATABUFFER *pInputUseBuffer, IM_OMX_DATABUFFER *pOutputUseBuffer)
{
    OMX_ERRORTYPE               ret = OMX_ErrorNone;
    IM_OMX_BASECOMPONENT       *pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    IM_OMX_BASEPORT            *pIMInputPort = &pIMComponent->pIMPort[INPUT_PORT_INDEX];
    IM_OMX_BASEPORT            *pIMOutputPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];

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

    ret = IM_VDEC_Decode(pOMXComponent, pInputUseBuffer, pOutputUseBuffer);
    if (ret != OMX_ErrorNone) {
        if (ret == IM_OMX_WAIT_PORT_RECONFIG) {
        } else {
            pIMComponent->pCallbacks->EventHandler((OMX_HANDLETYPE)pOMXComponent,
                    pIMComponent->callbackData,
                    OMX_EventError, ret, 0, NULL);
        }
    } 

EXIT:
    IM_OSAL_Exiting();

    return ret;
}

OSCL_EXPORT_REF OMX_ERRORTYPE IM_OMX_ComponentInit(OMX_HANDLETYPE hComponent, OMX_STRING componentName)
{
    OMX_ERRORTYPE               ret = OMX_ErrorNone;
    OMX_COMPONENTTYPE          *pOMXComponent = NULL;
    IM_OMX_BASECOMPONENT       *pIMComponent = NULL;
    IM_OMX_BASEPORT            *pIMPort = NULL;
    IM_OMX_VIDEODEC_COMPONENT  *pOn2DecComponent;
    IM_ON2DEC_HANDLE           *pOn2DecHandle = NULL;
    int i = 0;

    IM_OSAL_Entering();

    if ((hComponent == NULL) || (componentName == NULL)) {
        ret = OMX_ErrorBadParameter;
        IM_OSAL_Error("OMX_ErrorBadParameter, Line:%d", __LINE__);
        goto EXIT;
    }
#if 0
    if (IM_OSAL_Strcmp(IM_OMX_, componentName) != 0) {
        ret = OMX_ErrorBadParameter;
        IM_OSAL_Error(IM_LOG_ERROR, "OMX_ErrorBadParameter, componentName:%s, Line:%d", componentName, __LINE__);
        goto EXIT;
    }
#endif 

    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    ret = IM_OMX_VideoDecodeComponentInit(pOMXComponent);
    if (ret != OMX_ErrorNone) {
        IM_OSAL_Error("OMX_Error, Line:%d", __LINE__);
        goto EXIT;
    }
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;

    pIMComponent->componentName = (OMX_STRING)IM_OSAL_Malloc(MAX_OMX_COMPONENT_NAME_SIZE);
    if (pIMComponent->componentName == NULL) {
        IM_OMX_VideoDecodeComponentDeinit(pOMXComponent);
        ret = OMX_ErrorInsufficientResources;
        IM_OSAL_Error("OMX_ErrorInsufficientResources, Line:%d", __LINE__);
        goto EXIT;
    }
    IM_OSAL_Memset(pIMComponent->componentName, 0, MAX_OMX_COMPONENT_NAME_SIZE);

    pOn2DecHandle = (IM_ON2DEC_HANDLE *)IM_OSAL_Malloc(sizeof(IM_ON2DEC_HANDLE));
    pOn2DecComponent->hCodecHandle = (OMX_HANDLETYPE)pOn2DecHandle;

    IM_OSAL_Memset(pOn2DecHandle, 0, sizeof(IM_ON2DEC_HANDLE));

    IM_OSAL_Strcpy(pIMComponent->componentName, componentName);
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
    pIMPort->portDefinition.format.video.nSliceHeight = 0;
    pIMPort->portDefinition.nBufferSize = DEFAULT_VIDEO_INPUT_BUFFER_SIZE;
    pIMPort->portDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingAutoDetect;
    IM_OSAL_Memset(pIMPort->portDefinition.format.video.cMIMEType, 0, MAX_OMX_MIMETYPE_SIZE);
    IM_OSAL_Strcpy(pIMPort->portDefinition.format.video.cMIMEType, "video/*");
    pIMPort->portDefinition.format.video.pNativeRender = 0;
    pIMPort->portDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
    pIMPort->portDefinition.format.video.eColorFormat = OMX_COLOR_FormatUnused;
    pIMPort->portDefinition.bBuffersContiguous = OMX_TRUE;
    pIMPort->portDefinition.bEnabled = OMX_TRUE;

    /* Output port */
    pIMPort = &pIMComponent->pIMPort[OUTPUT_PORT_INDEX];
    pIMPort->portDefinition.format.video.nFrameWidth = DEFAULT_FRAME_WIDTH;
    pIMPort->portDefinition.format.video.nFrameHeight= DEFAULT_FRAME_HEIGHT;
    pIMPort->portDefinition.format.video.nStride = 0; /*DEFAULT_FRAME_WIDTH;*/
    pIMPort->portDefinition.format.video.nSliceHeight = 0;
    pIMPort->portDefinition.nBufferSize = DEFAULT_VIDEO_OUTPUT_BUFFER_SIZE;
    pIMPort->portDefinition.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
    IM_OSAL_Memset(pIMPort->portDefinition.format.video.cMIMEType, 0, MAX_OMX_MIMETYPE_SIZE);
    IM_OSAL_Strcpy(pIMPort->portDefinition.format.video.cMIMEType, "raw/video");
    pIMPort->portDefinition.format.video.pNativeRender = 0;
    pIMPort->portDefinition.format.video.bFlagErrorConcealment = OMX_FALSE;
    pIMPort->portDefinition.format.video.eColorFormat = OMX_COLOR_Format16bitRGB565;
    pIMPort->portDefinition.bBuffersContiguous = OMX_TRUE;
    pIMPort->portDefinition.bEnabled = OMX_TRUE;

    pOMXComponent->GetParameter      = &IM_VDEC_GetParameter;
    pOMXComponent->SetParameter      = &IM_VDEC_SetParameter;
    pOMXComponent->GetConfig         = &IM_VDEC_GetConfig;
    pOMXComponent->SetConfig         = &IM_VDEC_SetConfig;
    pOMXComponent->GetExtensionIndex = &IM_VDEC_GetExtensionIndex;
    pOMXComponent->ComponentRoleEnum = &IM_VDEC_ComponentRoleEnum;
    pOMXComponent->ComponentDeInit   = &IM_OMX_ComponentDeinit;

    pIMComponent->IM_componentInit      = &IM_VDEC_Init;
    pIMComponent->IM_componentTerminate = &IM_VDEC_Terminate;
    pIMComponent->IM_bufferProcess      = &IM_VDEC_bufferProcess;
    pIMComponent->IM_DecoderFlush      = &IM_VDEC_Flush;

    pIMComponent->currentState = OMX_StateLoaded;
    pIMComponent->targetState = OMX_StateLoaded;
    pIMComponent->bFlushBeforeDecoding = OMX_FALSE;

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
    IM_OMX_VIDEODEC_COMPONENT *pOn2DecComponent;
    IM_ON2DEC_HANDLE      *pOn2DecHandle = NULL;

    IM_OSAL_Entering();

    if (hComponent == NULL) {
        ret = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pOMXComponent = (OMX_COMPONENTTYPE *)hComponent;
    pIMComponent = (IM_OMX_BASECOMPONENT *)pOMXComponent->pComponentPrivate;
    pOn2DecComponent = (IM_OMX_VIDEODEC_COMPONENT *)pIMComponent->hComponentHandle;

    IM_OSAL_Free(pIMComponent->componentName);
    pIMComponent->componentName = NULL;

    pOn2DecHandle = (IM_ON2DEC_HANDLE *)pOn2DecComponent->hCodecHandle;
    if (pOn2DecHandle != NULL) {
        IM_OSAL_Free(pOn2DecHandle);
        pOn2DecHandle = pOn2DecComponent->hCodecHandle = NULL;
    }

    if(pOn2DecComponent != NULL) {
        IM_OSAL_Info("release memory handle");
        IM_OSAL_ContiguousAllocator_Destroy(pOn2DecComponent->hMemoryHandle);
    }
    ret = IM_OMX_VideoDecodeComponentDeinit(pOMXComponent);
    if(ret != OMX_ErrorNone) {
        goto EXIT;
    }

    ret = OMX_ErrorNone;

EXIT:
    IM_OSAL_Exiting();

    return ret;
}
