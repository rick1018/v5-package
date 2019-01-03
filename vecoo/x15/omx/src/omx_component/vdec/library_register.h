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
 * @file        library_register.h
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.03.01 : Create
*/

#ifndef IM_OMX_VDEC_REG
#define IM_OMX_VDEC_REG

#include "IM_OMX_Def.h"
#include "OMX_Component.h"
#include "IM_OMX_Component_Register.h"
#include "library_register.h"


#define OSCL_EXPORT_REF __attribute__((visibility("default")))
#define MAX_COMPONENT_NUM         1
#define MAX_COMPONENT_ROLE_NUM    12

#define IM_OMX_COMPONENT_INFOTM_VDEC              "OMX.Infotm.Video.Decoder"
#define IM_OMX_COMPONENT_MPEG4_DEC_ROLE    "video_decoder.mpeg4"
#define IM_OMX_COMPONENT_AVC_DEC_ROLE      "video_decoder.avc"
#define IM_OMX_COMPONENT_H263_DEC_ROLE     "video_decoder.h263"
#define IM_OMX_COMPONENT_WMV_DEC_ROLE      "video_decoder.wmv"
#define IM_OMX_COMPONENT_VC1_DEC_ROLE      "video_decoder.vc1"
#define IM_OMX_COMPONENT_MPEG2_DEC_ROLE    "video_decoder.mpeg2"
#define IM_OMX_COMPONENT_RV30_DEC_ROLE     "video_decoder.rv30"
#define IM_OMX_COMPONENT_RV40_DEC_ROLE     "video_decoder.rv40"
#define IM_OMX_COMPONENT_AVS_DEC_ROLE      "video_decoder.avs"
#define IM_OMX_COMPONENT_VP6_DEC_ROLE      "video_decoder.vp6"
#define IM_OMX_COMPONENT_VP8_DEC_ROLE      "video_decoder.vp8"
#define IM_OMX_COMPONENT_INFOTM_DEC_ROLE   	   "video_decoder.infotm"



#ifdef __cplusplus
extern "C" {
#endif

OSCL_EXPORT_REF int IM_OMX_COMPONENT_Library_Register(IMRegisterComponentType **ppIMComponent);

#ifdef __cplusplus
};
#endif

#endif

