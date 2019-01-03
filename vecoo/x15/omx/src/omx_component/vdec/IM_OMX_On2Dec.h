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
 * @file        IM_OMX_On2Dec.h
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     2.0
 * @history
 *   2012.03.01 : Create
 *   2014.01.09 : rearrangement
*/

#ifndef _VIDEO_UTILS
#define _VIDEO_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IM_OMX_Macros.h"
#include "IM_OSAL_Event.h"
#include "IM_OMX_Vdec.h"
#include "IM_OMX_Basecomponent.h"
#include "IM_OSAL_Thread.h"
#include "list_operation.h"
#include "InfotmMedia.h"
#include "IM_vdeccommon.h"
#include "IM_vdecapi2.h"
#include "library_register.h"

#define MAX_ERROR_NUM 100


typedef struct _IM_ON2DEC_HANDLE{
	IM_VIDEO_FORMAT 	videoMeta; 

    //ON2 DEC params
	IM_VDEC_CTX			hOn2DecLib;
	IM_VDEC_CONFIG		vdecConfig;
	IM_VDEC_SPEC_DATA	pSpecData;
	IM_VDEC_INFO        vdecInfo;
	IM_VDEC_IN			vdecIn;
	IM_VDEC_OUT 		vdecOut;
	IM_VDEC_PICTURE		vdecPic;
	IM_VDEC_BUFFERS		*vdecBuffers;

    
	TSMgmt 				iTSMgmtArray[30]; 	//timestamp managment 
	pTSMgmt 			iTSMgmtHead;		//timestamp head
	pTSMgmt 			iTSMgmtTail;		//timestamp tail

	OMX_PTR             pSelfInBuffer;
	OMX_PTR             pSelfOutBuffer;

	/* Flags */
	OMX_BOOL 			bFirstFrame;
    OMX_BOOL            bHeaderReady;

	OMX_BOOL			bFirstFieldFrame;
    OMX_BOOL            bInterlacedFrame;
	OMX_BOOL            bHaveFrameOut;
    
	OMX_BOOL 			bSkipFrameEnable;

    OMX_BOOL            bVideoSizeChanged;
	OMX_BOOL			bResizeNeeded;
	OMX_BOOL            bOutportChangedPending;

    OMX_BOOL            bOutportSettingChanged;
	OMX_BOOL			bConfigMultiBufferNeeded;
    OMX_BOOL            bMultiBufferMode;
	OMX_U32             nConfigMultiBufferNum;
	OMX_U32             nMultiBufferNum;

	OMX_BOOL			bStreamEOS;
	OMX_U32				nPrerollNum;
	OMX_U32				nDecodedErrorNum;

    //frame & timestamp static
	OMX_U32             nOutGoingFrameCount;	//decoded out images if flush i set it to 0
    OMX_U32             nSkipFrameCount;
	OMX_TICKS		    mPreTimestamp;		//pre in timestamp
	OMX_TICKS           mAverageTimeStamp;	//average timestamp:
	OMX_TICKS           mFirstFrameTimestamp;	//first in timestamp(outgingframecount==0 's frame timestamp)

	/*skip frame*/
	OMX_U32 nContiguousSkip;
}IM_ON2DEC_HANDLE;


#ifdef __cplusplus
extern "C" {
#endif

OSCL_EXPORT_REF OMX_ERRORTYPE IM_OMX_ComponentInit(OMX_HANDLETYPE hComponent, OMX_STRING componentName);
                OMX_ERRORTYPE IM_OMX_ComponentDeinit(OMX_HANDLETYPE hComponent);

#ifdef __cplusplus
};
#endif
#endif 

