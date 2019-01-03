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
 * @file        IM_OMX_H264Enc.h
 * @brief
 * @author      ayakashi (eric.xu@infotmic.com.cn)
 * @version     1.0
 * @history
 *   2013.10.10 : Create
*/
#ifndef IM_OMX_H264_ENC_COMPONENT

#define IM_OMX_H264_ENC_COMPONENT

#include "IM_OMX_Def.h"
#include "OMX_Component.h"
#include "OMX_Video.h"
#include "InfotmMedia.h"
#include "h264encapi.h"


typedef struct _EXTRA_DATA
{
    OMX_PTR pHeaderSPS;
    OMX_U32 SPSLen;
    OMX_PTR pHeaderPPS;
    OMX_U32 PPSLen;
} EXTRA_DATA;

typedef struct _IM_ON2_ENC_HANDLE
{   
    H264EncInst                 h264EncInst;
    H264EncApiVersion           h264EncVersion;
    H264EncBuild                h264EncBuild;
    H264EncConfig               h264EncConfig;
    H264EncRateCtrl             h264EncRateCtrl;
    H264EncCodingCtrl           h264EncCodingCtrl;
    H264EncPreProcessingCfg     h264EncPreProc;
    H264EncIn                   h264EncIn;
    H264EncOut                  h264EncOut;
} IM_ON2_ENC_HANDLE;

typedef struct _IM_H264ENC_HANDLE
{
    /* OMX Codec specific */
    OMX_VIDEO_PARAM_AVCTYPE AVCComponent[ALL_PORT_NUM];
    OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE errorCorrectionType[ALL_PORT_NUM];

    /* IM  Codec specific */
    IM_ON2_ENC_HANDLE hOn2EncHandle;

    OMX_HANDLETYPE     hMemoryHandle;
    OMX_BOOL            bUseMMU;
    OMX_PTR             pSelfOutBuffer;
    OMX_PTR             pSelfInBuffer;
    OMX_U32             frameCount;
    OMX_U32             inSize;
    OMX_TICKS           lastTimeStamp;
    OMX_TICKS           timeInterval;
    OMX_PTR             pSpsPps;
    OMX_U32             spsPpsSize;

    OMX_BOOL bFirstFrame;
    OMX_BOOL bKeyFrameRequested;
    OMX_BOOL bSpsPpsRequested;
} IM_H264ENC_HANDLE;

#ifdef __cplusplus
extern "C" {
#endif

OSCL_EXPORT_REF OMX_ERRORTYPE IM_OMX_ComponentInit(OMX_HANDLETYPE hComponent, OMX_STRING componentName);
OMX_ERRORTYPE IM_OMX_ComponentDeinit(OMX_HANDLETYPE hComponent);

#ifdef __cplusplus
};
#endif

#endif
