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
 * @file        IM_Vp8HwDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#if TARGET_SYSTEM == FS_ANDROID
#define LOG_TAG "Vp8HwDec"
#endif

#include "IM_Vp8HwDec.h"
#include "IM_VdecLog.h"
#define MULTIBUF_MODE1
IM_Vp8HwDec::IM_Vp8HwDec(IM_VDEC_INTYPE *inType)
{
	IM_VDEC_INFO("++++");

	memcpy(&mVdecConfig.inType, inType, sizeof(IM_VDEC_INTYPE));

	if(mVdecConfig.inType.codec == IM_STRM_VIDEO_VP8)
	{
		mVp8DecFormat = VP8DEC_VP8;
	}
	else if(mVdecConfig.inType.codec == IM_STRM_VIDEO_VP7)
	{
		mVp8DecFormat = VP8DEC_VP7;
	}
	else
	{
		mVp8DecFormat = VP8DEC_VP8;
	}


	getDefaultConfig(&mVdecConfig);

	//vp8 config 
	mVdecConfig.build.bMultiBuffModeSupport = IM_FALSE;
	mVdecConfig.mode.bMultiBuffMode = IM_FALSE;

	mVp8Build = VP8DecGetBuild(); 
	if(mVdecConfig.build.maxOutWidth > mVp8Build.hwConfig.maxPpOutPicWidth )
	{
		mVdecConfig.build.maxOutWidth  = mVp8Build.hwConfig.maxPpOutPicWidth;
	}

	if(mVdecConfig.build.maxOutHeight > 1088 )
	{
		mVdecConfig.build.maxOutWidth  = 1088;
	}

	mUseVideoFreezeConcealment = 0;
	nExtraFrameBuffers = 0;

	bHeaderReady = IM_FALSE;
	bNoInputSize = IM_FALSE;
	bVdecInfoValid = IM_FALSE;
	bPicInfoValid = IM_FALSE;
	bRunTimeValid = IM_FALSE;
	bPpCombineModeEnable = IM_FALSE;
	bVdecConfigSetted = IM_FALSE;
	bMultiBufferReset = IM_FALSE;
	bFlushed = IM_TRUE;
	memset(&mVdecIn, 0, sizeof(IM_VDEC_IN));
	memset(&mVdecOut, 0, sizeof(IM_VDEC_OUT));
	memset(&mVdecInfo, 0, sizeof(IM_VDEC_INFO));
	memset(&mVp8DecIn, 0, sizeof(VP8DecInput));
	memset(&mVp8DecOut, 0, sizeof(VP8DecOutput));
	memset(&mVp8DecInfo, 0, sizeof(VP8DecInfo));
	memset(&mVp8DecPicture, 0, sizeof(VP8DecPicture));

	mVp8DecInst = NULL;
	PpInst = NULL;


	pMultiBuffList = NULL;
	nSkippedPicAfterFlush = 0;
	IM_VDEC_INFO("----");
}

IM_BOOL IM_Vp8HwDec::isSupported()
{
	IM_VDEC_INFO("++++");
	if(mVp8Build.hwConfig.vp8Support == VP8_NOT_SUPPORTED)
	{
		return IM_FALSE;
	}

	if(mVdecConfig.inType.codec != IM_STRM_VIDEO_VP8 &&
		mVdecConfig.inType.codec != IM_STRM_VIDEO_VP7)
	{
		return IM_FALSE;
	}

	if(checkInputType(&mVdecConfig.inType) != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("IN SIZE NOT SUPPORT");
		return IM_FALSE;
	}
	IM_VDEC_INFO("----");

	return IM_TRUE;
}

IM_RET IM_Vp8HwDec::Init(IM_VDEC_SPEC_DATA *specData,IM_REF_FRM_FORMAT refFrmFormat)
{
	VP8DecRet  vp8Ret;
	PPResult   ppResult;
	IM_VDEC_INFO("++++");
	
#ifndef VDEC_LIB_G1
	vp8Ret = VP8DecInit(&mVp8DecInst, mVp8DecFormat,mUseVideoFreezeConcealment, nExtraFrameBuffers);
#else
	vp8Ret = VP8DecInit(&mVp8DecInst, mVp8DecFormat,mUseVideoFreezeConcealment, nExtraFrameBuffers, (DecRefFrmFormat)refFrmFormat);
#endif

	if(vp8Ret != VP8DEC_OK){
		IM_VDEC_ERR("VP8DecInit Failed error = %d", vp8Ret);
		return IM_RET_VDEC_INIT_FAIL;
	}

	if(bVdecConfigSetted && (mVdecConfig.mode.bCombineMode || mVdecConfig.mode.bStandAloneMode))
	{
		if((ppResult = PPInit(&PpInst)) != PP_OK)
		{
			IM_VDEC_ERR("PPInit Failed err= %d", ppResult);
			return IM_RET_VDEC_INIT_FAIL;
		}

		if(mVdecConfig.mode.bCombineMode)
		{
			if((ppResult = PPDecCombinedModeEnable(PpInst, mVp8DecInst, PP_PIPELINED_DEC_TYPE_VP8)) != PP_OK)
			{
				IM_VDEC_ERR("PPDecCombinedModeEnable Failed, err = %d", ppResult);
				return IM_RET_VDEC_INIT_FAIL;
			}
			bPpCombineModeEnable = IM_TRUE;
		}

		if((ppResult = PPGetConfig(PpInst, &PpCfg)) != PP_OK)
		{
			IM_VDEC_ERR("PP GetConfig Failed, err = %d", ppResult);
			return IM_RET_VDEC_INIT_FAIL;
		}
	}

#ifdef VDEC_LIB_G1
	IM_VDEC_INFO("set MMU %s", mVdecConfig.mode.bMMUEnable ? "Enable" : "Disable");
	VP8DecMMUEnable(mVp8DecInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	if(PpInst != NULL)
	{
		PPDecMMUEnable(PpInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	}
#endif 	

	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_Vp8HwDec::Decode(const IM_VDEC_IN *vdecIn, IM_VDEC_OUT *vdecOut, IM_VDEC_PICTURE *vdecPicture)
{
	VP8DecRet vp8Ret; 
	PPResult ppResult;
	IM_RET ret;
	IM_VDEC_INFO("++++");
	IM_VDEC_ASSERT(mVp8DecInst != NULL);

	if(vdecIn == NULL || vdecOut == NULL || vdecPicture == NULL)
	{
		IM_VDEC_ERR("IM_RET_VDEC_INVALID_PARAMETERS");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	//check config
	if(bHeaderReady && mVdecConfig.mode.bCombineMode)
	{
		if(!bPpCombineModeEnable)
		{
#if 0
			return IM_RET_VDEC_NOT_INITIALIZED;
#else
			if((ppResult = PPDecCombinedModeEnable(PpInst, mVp8DecInst, PP_PIPELINED_DEC_TYPE_VP8)) != PP_OK)
			{
				IM_VDEC_ERR("PPDecCombinedModeEnable Failed, ppResult = %d",ppResult);
				return IM_RET_VDEC_NOT_INITIALIZED;
			}
			bPpCombineModeEnable = IM_TRUE;
#endif
		}

		//setPpConfig
		if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
		{
			IM_VDEC_ERR("setPpCfg failed");
			return IM_RET_VDEC_FAIL;
		}
		
	}


	//decode
	mVp8DecIn.pStream = (const u8 *)vdecIn->pStream;
	mVp8DecIn.streamBusAddress = vdecIn->busAddr;
	mVp8DecIn.dataLen = vdecIn->dataLen;
	vdecOut->skipPicNum = 0;
	mDirectShow = (vdecIn->flags & IM_VDEC_PIC_DIRECT_SHOW) ? 1 : 0;

	vp8Ret = VP8DecDecode(mVp8DecInst, &mVp8DecIn, &mVp8DecOut);

	//result process
	switch(vp8Ret)
	{
		case VP8DEC_SLICE_RDY:
			IM_VDEC_INFO("VP8DEC_STRM_PROCESSED");
		case VP8DEC_OK:
			ret = IM_RET_VDEC_NO_PIC_OUT;
			break;
		case VP8DEC_STRM_PROCESSED:
			IM_VDEC_INFO("VP8DEC_STRM_PROCESSED");
			ret = IM_RET_VDEC_STRM_PROCESSED;
			break;
		case VP8DEC_HDRS_RDY:
		{
			if(VP8DecGetInfo(mVp8DecInst, &mVp8DecInfo) != VP8DEC_OK)
			{
				IM_VDEC_ERR("VP8DecGetInfo failed");
				return IM_RET_VDEC_FAIL;
			}
			ret = IM_RET_VDEC_HDRS_RDY;

			if(bHeaderReady)
			{
				if(mVdecInfo.codedWidth != mVp8DecInfo.scaledWidth 
						|| mVdecInfo.codedHeight != mVp8DecInfo.scaledHeight)
				{
					ret = IM_RET_VDEC_DECINFO_CHANGED;	
				}
			}

			bVdecInfoValid = IM_TRUE;

			if(mVp8DecInfo.frameWidth == 0 || mVp8DecInfo.frameHeight == 0)
			{
				if(!bNoInputSize)
				{
					mVp8DecInfo.frameWidth = IM_SIZEALIGNED16(mVdecConfig.inType.width);
					mVp8DecInfo.frameHeight = IM_SIZEALIGNED16(mVdecConfig.inType.height);
				}
				else
				{
					bVdecInfoValid = IM_FALSE;
				}
			}

			IM_VDEC_INFO("Vp8Dec Header Ready::");
			IM_VDEC_INFO("mVp8DecInfo::vp8Version = %d", mVp8DecInfo.vpVersion);
			IM_VDEC_INFO("mVp8DecInfo::vp8Profile = %d", mVp8DecInfo.vpProfile);
			IM_VDEC_INFO("mVp8DecInfo::frameWidth = %d", mVp8DecInfo.frameWidth);
			IM_VDEC_INFO("mVp8DecInfo::frameHeight = %d", mVp8DecInfo.frameHeight);
			IM_VDEC_INFO("mVp8DecInfo::codedWidth = %d", mVp8DecInfo.codedWidth);
			IM_VDEC_INFO("mVp8DecInfo::codedHeight = %d", mVp8DecInfo.codedHeight);
			IM_VDEC_INFO("mVp8DecInfo::scaledWidth = %d", mVp8DecInfo.scaledWidth);
			IM_VDEC_INFO("mVp8DecInfo::scaledHeight = %d", mVp8DecInfo.scaledHeight);


			mVdecInfo.frameWidth = mVp8DecInfo.frameWidth;
			mVdecInfo.frameHeight = mVp8DecInfo.frameHeight;
			mVdecInfo.codedWidth = mVp8DecInfo.codedWidth;
			mVdecInfo.codedHeight = mVp8DecInfo.codedHeight;
			mVdecInfo.interlacedSequence = 0;
			mVdecInfo.multiBuffPpSize = 0;
			bHeaderReady = IM_TRUE;
		}
		break;
		case VP8DEC_PIC_DECODED:	
		{
			IM_VDEC_INFO("VP8DEC_PIC_DECODED");
			ret = getPicture(vdecPicture, 0);
#ifndef MOSAIC_PIC_OUT
			while(ret == IM_RET_VDEC_MOSAIC_PIC_OUT)
			{
				vdecOut->skipPicNum++;
				ret = getPicture(vdecPicture, 0);
			}

			if(vdecOut->skipPicNum != 0)
			{
				ret = IM_RET_VDEC_PIC_SKIPPED;
			}
#endif
		}
		break;
		case VP8DEC_MEMFAIL:
			ret = IM_RET_VDEC_MEM_FAIL;
			break;
			break;
		case VP8DEC_STRM_ERROR:
		default:
			IM_VDEC_ERR("Vp8 decode error vp8Ret = %d", vp8Ret);	
			ret = IM_RET_VDEC_FAIL;
	};

	vdecOut->pCurStream = (void *)(mVp8DecIn.pStream + mVp8DecIn.dataLen);
	vdecOut->strmCurrBusAddress = mVp8DecIn.streamBusAddress + mVp8DecIn.dataLen;
	if(ret != IM_RET_VDEC_HDRS_RDY)
	{
		vdecOut->dataLeft = 0;
	}
	else
	{
		vdecOut->dataLeft = vdecIn->dataLen;
	}
	IM_VDEC_INFO("----");
	return ret;

}

IM_RET IM_Vp8HwDec::GetNextPicture(IM_VDEC_PICTURE *vdecPicture)
{
	IM_INT32 skip = 0;
	
	if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("set Pp config failed");
		skip = 1;
	}
	return getPicture(vdecPicture, skip);
}

IM_RET IM_Vp8HwDec::Flush()
{
	IM_VDEC_PICTURE vdecPicture;
	getPicture(&vdecPicture, 1);

	if(mVdecConfig.mode.bMultiBuffMode)
	{
		if(pMultiBuffList)
		{
			pMultiBuffList->release();
		}
	}

	bPicInfoValid = IM_FALSE;
	bRunTimeValid = IM_FALSE;
	bFlushed = IM_TRUE;
	
	return IM_RET_VDEC_OK;
}

IM_RET IM_Vp8HwDec::DeInit()
{	
	if(pMultiBuffList)
	{
		delete pMultiBuffList;
		pMultiBuffList = NULL;
	}

	if(PpInst)
	{
		if(bPpCombineModeEnable)
		{
			if(PP_OK != PPDecCombinedModeDisable(PpInst, mVp8DecInst))
			{
				IM_VDEC_ERR("PPDecCombinedModeDisable failed");
			}
			else
			{
				bPpCombineModeEnable = IM_FALSE;
			}
		}
		PPRelease(PpInst);
	}

	if(mVp8DecInst)
	{
		VP8DecRelease(mVp8DecInst);
	}
	
	return IM_RET_VDEC_OK;
}


IM_RET IM_Vp8HwDec::getPicture(IM_VDEC_PICTURE *vdecPicture, IM_INT32 skip)
{
	IM_RET ret;
	VP8DecRet vp8Ret;
	PPResult ppResult;
	IM_INT32 skipNum = 0;
	IM_VDEC_ASSERT(vdecPicture != NULL);

	IM_VDEC_INFO("++++");
	
	do{
		vp8Ret = VP8DecNextPicture(mVp8DecInst, &mVp8DecPicture, mDirectShow);
		if(skip && vp8Ret == VP8DEC_PIC_RDY){
			skipNum++;
		}
	}while(skip && vp8Ret == VP8DEC_PIC_RDY);

	if(skip)
	{
		vdecPicture->skipNum = skipNum;
		return IM_RET_VDEC_FAIL;
	}

	
	if(vp8Ret == VP8DEC_PIC_RDY)
	{
		IM_VDEC_INFO("Vp8DecPic::frameWidth = %d", mVp8DecPicture.frameWidth);
		IM_VDEC_INFO("Vp8DecPic::frameHeight = %d", mVp8DecPicture.frameHeight);
		IM_VDEC_INFO("Vp8DecPic::keyPicture = %d", mVp8DecPicture.isIntraFrame);
		IM_VDEC_INFO("Vp8DecPic::picId = %d", mVp8DecPicture.picId);
		IM_VDEC_INFO("Vp8DecPic::isGoldenFrame = %d", mVp8DecPicture.isGoldenFrame);

		if(mVp8DecPicture.frameWidth == 0 || mVp8DecPicture.frameHeight  == 0)
		{
			bPicInfoValid = IM_FALSE;
			bRunTimeValid = IM_TRUE;
			IM_VDEC_WARN("VP8DEC_PIC_RDY, but out size is zero");
			return IM_RET_VDEC_NO_PIC_OUT;
		}

		bPicInfoValid = IM_TRUE;
		if(mVdecConfig.mode.bStandAloneMode)
		{
			if(setPpCfg(vdecPicture) != IM_RET_OK)
			{
				bPicInfoValid = IM_FALSE;
				return getPicture(vdecPicture, 1);
			}
		}
		if(mVdecConfig.mode.bCombineMode || mVdecConfig.mode.bStandAloneMode)
		{
			if((ppResult = PPGetResult(PpInst)) != PP_OK)
			{
				IM_VDEC_ERR("get Pp Result failed, ret = %d", ppResult);
				bPicInfoValid = IM_FALSE;
				return getPicture(vdecPicture, 1);
			}

			if(mVdecConfig.mode.bMultiBuffMode)
			{
				PPOutput tmpOutBuffer;
				MULTIBUFF_ELEM qElem1;
				MULTIBUFF_ELEM qElem2;

				if((ppResult = PPGetNextOutput(PpInst, &tmpOutBuffer)) != PP_OK)
				{
					IM_VDEC_ERR("get Pp next output failed, ret = %d", ppResult);
					bPicInfoValid = IM_FALSE;
					return getPicture(vdecPicture, 1);
				}

				IM_VDEC_INFO("out pic addr = %x", tmpOutBuffer.bufferBusAddr);
				pMultiBuffList->getElembybusAddr(tmpOutBuffer.bufferBusAddr, &qElem1);
				qElem2.buffer.virtAddr = vdecPicture->buffer.virtAddr;
				qElem2.buffer.busAddr = vdecPicture->buffer.busAddr;
				qElem2.buffer.size = vdecPicture->buffer.size;
				pMultiBuffList->fillElem(qElem1.index, &qElem2);
#ifdef MULTIBUF_MODE1 
				setMultiBuffers(pMultiBuffList);
#else
				bMultiBufferReset = IM_TRUE;
#endif 
				vdecPicture->buffer.virtAddr = qElem1.buffer.virtAddr;
				vdecPicture->buffer.busAddr = qElem1.buffer.busAddr; 
				vdecPicture->buffer.size    = qElem1.buffer.size;
			}
		}
		else
		{
			//do not use 
			vdecMemCpy(vdecPicture->buffer.virtAddr, (void*)mVp8DecPicture.pOutputFrame, nDecodedPicSize);
		}

		nCropWidth = (mVp8DecPicture.frameWidth - mVp8DecInfo.scaledWidth) *  (float)mVdecConfig.outType.width/ mVp8DecInfo.scaledWidth;
		nCropHeight = (mVp8DecPicture.frameHeight - mVp8DecInfo.scaledHeight) * (float)mVdecConfig.outType.height / mVp8DecInfo.scaledHeight;
		vdecPicture->dataLen = nDecodedPicSize;
		vdecPicture->keyPicture = mVp8DecPicture.isIntraFrame;
		vdecPicture->interlaced = 0;
		
		vdecPicture->cropWidth = nCropWidth;
		vdecPicture->cropHeight = nCropHeight;
		IM_VDEC_INFO("outPic::buffer.virtAddr =%p", vdecPicture->buffer.virtAddr);
		IM_VDEC_INFO("outPic::buffer.busAddr =%x", vdecPicture->buffer.busAddr);
		IM_VDEC_INFO("outPic::dataLen =%d", vdecPicture->dataLen);
		IM_VDEC_INFO("outPic::keyPicture =%d", vdecPicture->keyPicture);
		IM_VDEC_INFO("outPic::interlaced =%d", vdecPicture->interlaced);
		IM_VDEC_INFO("outPic::cropWidth =%d", vdecPicture->cropWidth);
		IM_VDEC_INFO("outPic::cropHeight =%d", vdecPicture->cropHeight);
		if(bFlushed)
		{
			if(!mVp8DecPicture.isIntraFrame && nSkippedPicAfterFlush < MAX_SKIP_PIC_NUM)
			{
				nSkippedPicAfterFlush++;
				IM_VDEC_WARN("MOSAIC PIC OUT");
				IM_VDEC_INFO("Skip frame num = %d", nSkippedPicAfterFlush);
				return IM_RET_VDEC_MOSAIC_PIC_OUT;
			}
			else
			{
				nSkippedPicAfterFlush = 0;
				bFlushed = IM_FALSE;
			}
		}

		return IM_RET_VDEC_MORE_PIC_OUT;
	}
	else
	{
		IM_VDEC_INFO("vp8Ret = %d", vp8Ret);
		bPicInfoValid = IM_FALSE;
		bRunTimeValid = IM_TRUE;
		mDirectShow = 0;
		return  IM_RET_VDEC_NO_PIC_OUT;
	}
}

IM_RET IM_Vp8HwDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
	IM_VDEC_ASSERT(vdecPicture != NULL);
	IM_VDEC_ASSERT(PpInst != NULL);

	if(bPicInfoValid)
	{
		IM_VDEC_INFO("set  PpCfg by PicInfo");
		PpCfg.ppInImg.width = mVp8DecPicture.frameWidth;
		PpCfg.ppInImg.height = mVp8DecPicture.frameHeight;
		if(mVdecConfig.mode.bStandAloneMode)
		{
			PpCfg.ppInImg.bufferBusAddr = mVp8DecPicture.outputFrameBusAddress; 
			PpCfg.ppInImg.bufferCbBusAddr = mVp8DecPicture.outputFrameBusAddress + mVp8DecPicture.frameHeight * mVp8DecPicture.frameWidth; 
			PpCfg.ppInImg.bufferCrBusAddr = PpCfg.ppInImg.bufferCbBusAddr;
		}
	}
	else if(bRunTimeValid)
	{
		IM_VDEC_INFO("set PpCfg by Runtime");
		//do nothing;
	}
	else if(bVdecInfoValid)
	{
		IM_VDEC_INFO("set  PpCfg by DecInfo");
		mVp8DecInfo.frameHeight = IM_SIZEALIGNED16(mVp8DecInfo.frameHeight);
		PpCfg.ppInImg.width = mVp8DecInfo.frameWidth;
		PpCfg.ppInImg.height = mVp8DecInfo.frameHeight;
	}
	else
	{
		PpCfg.ppInImg.width = IM_SIZEALIGNED16(mVdecConfig.inType.width);
		PpCfg.ppInImg.height = IM_SIZEALIGNED16(mVdecConfig.inType.height);
	}

	PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
	PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
	PpCfg.ppOutImg.width = mVdecConfig.outType.width;
	PpCfg.ppOutImg.height = mVdecConfig.outType.height;

	return IM_VideoDec::setPpCfg(vdecPicture);
}

