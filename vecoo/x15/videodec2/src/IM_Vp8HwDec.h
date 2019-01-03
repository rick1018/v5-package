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
 * @file        IM_Vp8HwDec.h
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#ifndef IM_VP8HWDEC_H
#define IM_VP8HWDEC_H

#include "IM_VideoDec.h"
#include "vp8decapi.h"

class IM_Vp8HwDec: public IM_VideoDec{
public:
	IM_Vp8HwDec(IM_VDEC_INTYPE *inType);
	~IM_Vp8HwDec(){};
	virtual IM_RET Init(IM_VDEC_SPEC_DATA *specData, IM_REF_FRM_FORMAT refFrmFormat = IM_DEC_REF_FRM_RASTER_SCAN);
	virtual	IM_BOOL isSupported();
	virtual IM_RET Decode(const IM_VDEC_IN *pDecIn, IM_VDEC_OUT *pDecOut, IM_VDEC_PICTURE *pVdecPicture);
	virtual IM_RET GetNextPicture(IM_VDEC_PICTURE *pVdecPicture);
	virtual IM_RET Flush();
	virtual IM_RET DeInit();

private:
	IM_RET setPpCfg(IM_VDEC_PICTURE *vdecPicture);
	IM_RET getPicture(IM_VDEC_PICTURE *pVdecPicture, IM_INT32 skip);

	VP8DecInst mVp8DecInst;
	VP8DecBuild mVp8Build;
	VP8DecFormat mVp8DecFormat;

	VP8DecInput mVp8DecIn;
	VP8DecOutput mVp8DecOut;
	VP8DecInfo mVp8DecInfo;
	VP8DecPicture mVp8DecPicture;

	
};

#endif
