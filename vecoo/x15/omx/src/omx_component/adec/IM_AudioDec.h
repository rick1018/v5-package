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
 * @file      IM_AudioDec.h
 * @brief
 * @author    ayakashi (eric.xu@infotmic.com.cn)
 * @version   1.1.0
 * @history
 *   2012.04.15 : Create
 */

#ifndef IM_OMX_AUDIO_DEC_COMPONENT
#define IM_OMX_AUDIO_DEC_COMPONENT

#include "IM_OMX_Def.h"
#include "OMX_Component.h"
#include "InfotmMedia.h"
#include "IM_adecapi.h"
#include "IM_Audio_Timestamp.h"
#define MAX_FAILED_TIMES 100

typedef struct _IM_AUDIO_DEC_CODEC
{
	OMX_AUDIO_PARAM_PCMMODETYPE pcmParam;
    OMX_AUDIO_PARAM_AACPROFILETYPE aacParam;

	AudioDec_TimeStamp 		   *timestampCalc;

	ADECCTX						adecCtx;
	ADEC_INPUT_TYPE 			inType;
	ADEC_OUTPUT_TYPE 			outType;
	ADEC_IN						adecIn;
	ADEC_OUT					adecOut;

	OMX_BOOL					isFirstFrame;
	OMX_U32 					errorTolerance;
} IM_AUDIODEC_CODEC;

#ifdef __cplusplus
extern "C" {
#endif

OSCL_EXPORT_REF OMX_ERRORTYPE IM_OMX_ComponentInit(OMX_HANDLETYPE hComponent, OMX_STRING componentName);
                OMX_ERRORTYPE IM_OMX_ComponentDeinit(OMX_HANDLETYPE hComponent);

#ifdef __cplusplus
};
#endif

#endif /* IM_OMX_MP3_DEC_COMPONENT */
