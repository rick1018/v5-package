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
 * @file        IM_Vp6HwDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#if TARGET_SYSTEM == FS_ANDROID
#define LOG_TAG "Vp6HwDec"
#endif

#include "IM_Vp6HwDec.h"
#include "IM_VdecLog.h"
#define MULTIBUF_MODE1
IM_Vp6HwDec::IM_Vp6HwDec(IM_VDEC_INTYPE *inType)
{
	IM_VDEC_INFO("++++");

	memcpy(&mVdecConfig.inType, inType, sizeof(IM_VDEC_INTYPE));

	getDefaultConfig(&mVdecConfig);

	//vp6 config 
	mVdecConfig.build.bMultiBuffModeSupport = IM_FALSE;
	mVdecConfig.mode.bMultiBuffMode = IM_FALSE;

	mVp6Build = VP6DecGetBuild(); 
	if(mVdecConfig.build.maxOutWidth > mVp6Build.hwConfig.maxPpOutPicWidth )
	{
		mVdecConfig.build.maxOutWidth  = mVp6Build.hwConfig.maxPpOutPicWidth;
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
	memset(&mVp6DecIn, 0, sizeof(VP6DecInput));
	memset(&mVp6DecOut, 0, sizeof(VP6DecOutput));
	memset(&mVp6DecInfo, 0, sizeof(VP6DecInfo));
	memset(&mVp6DecPicture, 0, sizeof(VP6DecPicture));

	mVp6DecInst = NULL;
	PpInst = NULL;


	pMultiBuffList = NULL;
	nSkippedPicAfterFlush = 0;
	IM_VDEC_INFO("----");
}

IM_BOOL IM_Vp6HwDec::isSupported()
{
	IM_VDEC_INFO("++++");
	if(mVp6Build.hwConfig.vp6Support == VP6_NOT_SUPPORTED)
	{
		IM_VDEC_ERR("HW VP6 NOT SUPPORT!!!");
		return IM_FALSE;
	}

	if(mVdecConfig.inType.codec != IM_STRM_VIDEO_VP6 &&
		mVdecConfig.inType.codec != IM_STRM_VIDEO_VP6F)
	{
		IM_VDEC_ERR("not vp6 stream");
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

IM_RET IM_Vp6HwDec::Init(IM_VDEC_SPEC_DATA *specData,IM_REF_FRM_FORMAT refFrmFormat)
{
	VP6DecRet  vp6Ret;
	PPResult   ppResult;
	IM_VDEC_INFO("++++");
	
#ifndef VDEC_LIB_G1
	vp6Ret = VP6DecInit(&mVp6DecInst, mUseVideoFreezeConcealment, nExtraFrameBuffers);
#else
	vp6Ret = VP6DecInit(&mVp6DecInst, mUseVideoFreezeConcealment, nExtraFrameBuffers, (DecRefFrmFormat)refFrmFormat);
#endif

	if(vp6Ret != VP6DEC_OK){
		IM_VDEC_ERR("VP6DecInit Failed error = %d", vp6Ret);
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
			if((ppResult = PPDecCombinedModeEnable(PpInst, mVp6DecInst, PP_PIPELINED_DEC_TYPE_VP6)) != PP_OK)
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
	VP6DecMMUEnable(mVp6DecInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	if(PpInst != NULL)
	{
		PPDecMMUEnable(PpInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	}
#endif 	

	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_Vp6HwDec::Decode(const IM_VDEC_IN *vdecIn, IM_VDEC_OUT *vdecOut, IM_VDEC_PICTURE *vdecPicture)
{
	VP6DecRet vp6Ret; 
	PPResult ppResult;
	IM_RET ret;
	IM_VDEC_INFO("++++");
	IM_VDEC_ASSERT(mVp6DecInst != NULL);

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
			if((ppResult = PPDecCombinedModeEnable(PpInst, mVp6DecInst, PP_PIPELINED_DEC_TYPE_VP6)) != PP_OK)
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
	mVp6DecIn.pStream = (const u8 *)vdecIn->pStream;
	mVp6DecIn.streamBusAddress = vdecIn->busAddr;
	mVp6DecIn.dataLen = vdecIn->dataLen;
	vdecOut->skipPicNum = 0;
	mDirectShow = (vdecIn->flags & IM_VDEC_PIC_DIRECT_SHOW) ? 1 : 0;

	vp6Ret = VP6DecDecode(mVp6DecInst, &mVp6DecIn, &mVp6DecOut);

	//result process
	switch(vp6Ret)
	{
		case VP6DEC_OK:
			ret = IM_RET_VDEC_NO_PIC_OUT;
			break;
		case VP6DEC_STRM_PROCESSED:
			IM_VDEC_INFO("VP6DEC_STRM_PROCESSED");
			ret = IM_RET_VDEC_STRM_PROCESSED;
			break;
		case VP6DEC_HDRS_RDY:
		{
			if(VP6DecGetInfo(mVp6DecInst, &mVp6DecInfo) != VP6DEC_OK)
			{
				IM_VDEC_ERR("VP6DecGetInfo failed");
				return IM_RET_VDEC_FAIL;
			}
			ret = IM_RET_VDEC_HDRS_RDY;

			if(bHeaderReady)
			{
				if(mVdecInfo.codedWidth != mVp6DecInfo.scaledWidth 
						|| mVdecInfo.codedHeight != mVp6DecInfo.scaledHeight)
				{
					ret = IM_RET_VDEC_DECINFO_CHANGED;	
				}
			}

			bVdecInfoValid = IM_TRUE;

			if(mVp6DecInfo.frameWidth == 0 || mVp6DecInfo.frameHeight == 0)
			{
				if(!bNoInputSize)
				{
					mVp6DecInfo.frameWidth = IM_SIZEALIGNED16(mVdecConfig.inType.width);
					mVp6DecInfo.frameHeight = IM_SIZEALIGNED16(mVdecConfig.inType.height);
				}
				else
				{
					bVdecInfoValid = IM_FALSE;
				}
			}

			IM_VDEC_INFO("Vp6Dec Header Ready::");
			IM_VDEC_INFO("mVp6DecInfo::vp6Version = %d", mVp6DecInfo.vp6Version);
			IM_VDEC_INFO("mVp6DecInfo::vp6Profile = %d", mVp6DecInfo.vp6Profile);
			IM_VDEC_INFO("mVp6DecInfo::frameWidth = %d", mVp6DecInfo.frameWidth);
			IM_VDEC_INFO("mVp6DecInfo::frameHeight = %d", mVp6DecInfo.frameHeight);
			IM_VDEC_INFO("mVp6DecInfo::scaledWidth = %d", mVp6DecInfo.scaledWidth);
			IM_VDEC_INFO("mVp6DecInfo::scaledHeight = %d", mVp6DecInfo.scaledHeight);
			IM_VDEC_INFO("mVp6DecInfo::scalingMode = %d", mVp6DecInfo.scalingMode);


			mVdecInfo.frameWidth = mVp6DecInfo.frameWidth;
			mVdecInfo.frameHeight = mVp6DecInfo.frameHeight;
			mVdecInfo.codedWidth = mVdecConfig.inType.width;//mVp6DecInfo.scaledWidth;
			mVdecInfo.codedHeight = mVdecConfig.inType.height;///mVp6DecInfo.scaledHeight;
			mVdecInfo.interlacedSequence = 0;
			mVdecInfo.multiBuffPpSize = 0;
			bHeaderReady = IM_TRUE;
		}
		break;
		case VP6DEC_PIC_DECODED:	
		{
			IM_VDEC_INFO("VP6DEC_PIC_DECODED");
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
		case VP6DEC_MEMFAIL:
			ret = IM_RET_VDEC_MEM_FAIL;
			break;
			break;
		case VP6DEC_STRM_ERROR:
		default:
			IM_VDEC_ERR("Vp6 decode error vp6Ret = %d", vp6Ret);	
			ret = IM_RET_VDEC_FAIL;
	};

	vdecOut->pCurStream = (void *)(mVp6DecIn.pStream + mVp6DecIn.dataLen);
	vdecOut->strmCurrBusAddress = mVp6DecIn.streamBusAddress + mVp6DecIn.dataLen;
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

IM_RET IM_Vp6HwDec::GetNextPicture(IM_VDEC_PICTURE *vdecPicture)
{
	IM_INT32 skip = 0;
	
	if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("set Pp config failed");
		skip = 1;
	}
	return getPicture(vdecPicture, skip);
}

IM_RET IM_Vp6HwDec::Flush()
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
	//bFlushed = IM_TRUE;
	
	return IM_RET_VDEC_OK;
}

IM_RET IM_Vp6HwDec::DeInit()
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
			if(PP_OK != PPDecCombinedModeDisable(PpInst, mVp6DecInst))
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

	if(mVp6DecInst)
	{
		VP6DecRelease(mVp6DecInst);
	}
	
	return IM_RET_VDEC_OK;
}


IM_RET IM_Vp6HwDec::getPicture(IM_VDEC_PICTURE *vdecPicture, IM_INT32 skip)
{
	IM_RET ret;
	VP6DecRet vp6Ret;
	PPResult ppResult;
	IM_INT32 skipNum = 0;
	IM_VDEC_ASSERT(vdecPicture != NULL);

	IM_VDEC_INFO("++++");
	
	do{
		vp6Ret = VP6DecNextPicture(mVp6DecInst, &mVp6DecPicture, mDirectShow);
		if(skip && vp6Ret == VP6DEC_PIC_RDY){
			skipNum++;
		}
	}while(skip && vp6Ret == VP6DEC_PIC_RDY);

	if(skip)
	{
		vdecPicture->skipNum = skipNum;
		return IM_RET_VDEC_FAIL;
	}

	
	if(vp6Ret == VP6DEC_PIC_RDY)
	{
		IM_VDEC_INFO("Vp6DecPic::frameWidth = %d", mVp6DecPicture.frameWidth);
		IM_VDEC_INFO("Vp6DecPic::frameHeight = %d", mVp6DecPicture.frameHeight);
		IM_VDEC_INFO("Vp6DecPic::keyPicture = %d", mVp6DecPicture.isIntraFrame);
		IM_VDEC_INFO("Vp6DecPic::picId = %d", mVp6DecPicture.picId);
		IM_VDEC_INFO("Vp6DecPic::isGoldenFrame = %d", mVp6DecPicture.isGoldenFrame);

		if(mVp6DecPicture.frameWidth == 0 || mVp6DecPicture.frameHeight  == 0)
		{
			bPicInfoValid = IM_FALSE;
			bRunTimeValid = IM_TRUE;
			IM_VDEC_WARN("VP6DEC_PIC_RDY, but out size is zero");
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
				bPicInfoValid = IM_FALSE;
				IM_VDEC_ERR("get Pp Result failed, ret = %d", ppResult);
				return getPicture(vdecPicture, 1);
			}

			if(mVdecConfig.mode.bMultiBuffMode)
			{
				PPOutput tmpOutBuffer;
				MULTIBUFF_ELEM qElem1;
				MULTIBUFF_ELEM qElem2;

				if((ppResult = PPGetNextOutput(PpInst, &tmpOutBuffer)) != PP_OK)
				{
					bPicInfoValid = IM_FALSE;
					IM_VDEC_ERR("get Pp next output failed, ret = %d", ppResult);
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
			vdecMemCpy(vdecPicture->buffer.virtAddr, (void*)mVp6DecPicture.pOutputFrame, nDecodedPicSize);
		}

		nCropWidth = (mVp6DecPicture.frameWidth - mVp6DecInfo.scaledWidth) *  (float)mVdecConfig.outType.width/ mVp6DecInfo.scaledWidth;
		nCropHeight = (mVp6DecPicture.frameHeight - mVp6DecInfo.scaledHeight) * (float)mVdecConfig.outType.height / mVp6DecInfo.scaledHeight;
		vdecPicture->dataLen = nDecodedPicSize;
		vdecPicture->keyPicture = mVp6DecPicture.isIntraFrame;
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
			if(!mVp6DecPicture.isIntraFrame && nSkippedPicAfterFlush < MAX_SKIP_PIC_NUM)
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
		IM_VDEC_INFO("vp6Ret = %d", vp6Ret);
		bPicInfoValid = IM_FALSE;
		bRunTimeValid = IM_TRUE;
		mDirectShow = 0;
		return  IM_RET_VDEC_NO_PIC_OUT;
	}
}

IM_RET IM_Vp6HwDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
	IM_VDEC_ASSERT(vdecPicture != NULL);
	IM_VDEC_ASSERT(PpInst != NULL);

	if(bPicInfoValid)
	{
		IM_VDEC_INFO("set  PpCfg by PicInfo");
		PpCfg.ppInImg.width = mVp6DecPicture.frameWidth;
		PpCfg.ppInImg.height = mVp6DecPicture.frameHeight;
		if(mVdecConfig.mode.bStandAloneMode)
		{
			PpCfg.ppInImg.bufferBusAddr = mVp6DecPicture.outputFrameBusAddress; 
			PpCfg.ppInImg.bufferCbBusAddr = mVp6DecPicture.outputFrameBusAddress + mVp6DecPicture.frameHeight * mVp6DecPicture.frameWidth; 
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
		mVp6DecInfo.frameHeight = IM_SIZEALIGNED16(mVp6DecInfo.frameHeight);
		PpCfg.ppInImg.width = mVp6DecInfo.frameWidth;
		PpCfg.ppInImg.height = mVp6DecInfo.frameHeight;
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

