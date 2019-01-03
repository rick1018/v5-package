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
 * @file        IM_Mpeg2HwDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/

#if TARGET_SYSTEM == FS_ANDROID
#define LOG_TAG "Mpeg2HwDec"
#endif

#include "IM_Mpeg2HwDec.h"
#include "IM_VdecLog.h"
#define MULTIBUF_MODE1
IM_Mpeg2HwDec::IM_Mpeg2HwDec(IM_VDEC_INTYPE *inType)
{
	IM_VDEC_INFO("++++");

	memcpy(&mVdecConfig.inType, inType, sizeof(IM_VDEC_INTYPE));

	getDefaultConfig(&mVdecConfig);
	mMpeg2Build = Mpeg2DecGetBuild(); 
	if(mVdecConfig.build.maxOutWidth > mMpeg2Build.hwConfig.maxPpOutPicWidth )
	{
		mVdecConfig.build.maxOutWidth  = mMpeg2Build.hwConfig.maxPpOutPicWidth;
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
	memset(&mMpeg2DecIn, 0, sizeof(Mpeg2DecInput));
	memset(&mMpeg2DecOut, 0, sizeof(Mpeg2DecOutput));
	memset(&mMpeg2DecInfo, 0, sizeof(Mpeg2DecInfo));
	memset(&mMpeg2DecPicture, 0, sizeof(Mpeg2DecPicture));

	mMpeg2DecInst = NULL;


	pMultiBuffList = NULL;
	nSkippedPicAfterFlush = 0;
	IM_VDEC_INFO("----");
}

IM_BOOL IM_Mpeg2HwDec::isSupported()
{
	IM_VDEC_INFO("++++");
	if(mMpeg2Build.hwConfig.mpeg2Support == MPEG2_NOT_SUPPORTED)
	{
		return IM_FALSE;
	}

	if(mVdecConfig.inType.codec != IM_STRM_VIDEO_MPEG1VIDEO &&
			mVdecConfig.inType.codec != IM_STRM_VIDEO_MPEG2VIDEO )
	{
		IM_VDEC_ERR("CODEC TYPE NOT SUPPORT, codec = %x", mVdecConfig.inType.codec);
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

IM_RET IM_Mpeg2HwDec::Init(IM_VDEC_SPEC_DATA *specData,IM_REF_FRM_FORMAT refFrmFormat)
{
	Mpeg2DecRet  mpeg2Ret;
	PPResult   ppResult;
	IM_VDEC_INFO("++++");

#ifndef VDEC_LIB_G1
	mpeg2Ret = Mpeg2DecInit(&mMpeg2DecInst, mUseVideoFreezeConcealment, nExtraFrameBuffers);
#else
	mpeg2Ret = Mpeg2DecInit(&mMpeg2DecInst, mUseVideoFreezeConcealment, nExtraFrameBuffers, (DecRefFrmFormat)refFrmFormat);
#endif

	if(mpeg2Ret != MPEG2DEC_OK){
		IM_VDEC_ERR("Mpeg2DecInit Failed error = %d", mpeg2Ret);
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
			if((ppResult = PPDecCombinedModeEnable(PpInst, mMpeg2DecInst, PP_PIPELINED_DEC_TYPE_MPEG2)) != PP_OK)
			{
				IM_VDEC_ERR("PPDecCombinedModeEnable Failed");
				return IM_RET_VDEC_INIT_FAIL;
			}
			bPpCombineModeEnable = IM_TRUE;
		}

		if((ppResult = PPGetConfig(PpInst, &PpCfg)) != PP_OK)
		{
			IM_VDEC_ERR("PP GetConfig Failed");
			return IM_RET_VDEC_INIT_FAIL;
		}
	}

#ifdef VDEC_LIB_G1
	IM_VDEC_INFO("set MMU %s", mVdecConfig.mode.bMMUEnable ? "Enable" : "Disable");
	Mpeg2DecMMUEnable(mMpeg2DecInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	if(PpInst != NULL)
	{
		PPDecMMUEnable(PpInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	}
#endif 	

	
	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_Mpeg2HwDec::Decode(const IM_VDEC_IN *vdecIn, IM_VDEC_OUT *vdecOut, IM_VDEC_PICTURE *vdecPicture)
{
	Mpeg2DecRet mpeg2Ret; 
	PPResult ppResult;
	IM_RET ret;
	IM_VDEC_INFO("++++");
	IM_VDEC_ASSERT(mMpeg2DecInst != NULL);

	if(vdecIn == NULL || vdecOut == NULL || vdecPicture == NULL)
	{
		IM_VDEC_ERR("IM_RET_VDEC_INVALID_PARAMETERS");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	vdecOut->dataLeft = vdecIn->dataLen;
	//check config
	if(bHeaderReady && mVdecConfig.mode.bCombineMode)
	{
		if(!bPpCombineModeEnable)
		{
#if 0
			return IM_RET_VDEC_NOT_INITIALIZED;
#else
			if((ppResult = PPDecCombinedModeEnable(PpInst, mMpeg2DecInst, PP_PIPELINED_DEC_TYPE_MPEG2)) != PP_OK)
			{
				IM_VDEC_ERR("PPDecCombinedModeEnable Failed, ppResult = %d",ppResult);
				return IM_RET_VDEC_NOT_INITIALIZED;
			}
			bPpCombineModeEnable = IM_TRUE;
#endif
		}

		if(mVdecConfig.mode.bMultiBuffMode)
		{
			IM_RET ret = checkMultiBuffer(mMpeg2DecInfo.multiBuffPpSize);
			if(ret == IM_RET_VDEC_LACK_OUTBUFF)                                                                                                           
			{
				vdecOut->multiBuffPpSize = mMpeg2DecInfo.multiBuffPpSize;                                                                                  
				return ret;
			}
			else if(ret != IM_RET_VDEC_OK) 
			{             
				return ret;
			}
		}
		//setPpConfig
		if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
		{
			IM_VDEC_ERR("setPpCfg failed");
			return IM_RET_VDEC_FAIL;
		}

	}

	//decode
	mMpeg2DecIn.pStream = (u8 *)vdecIn->pStream;
	mMpeg2DecIn.streamBusAddress = vdecIn->busAddr;
	mMpeg2DecIn.dataLen = vdecIn->dataLen;
	mMpeg2DecIn.skipNonReference = (vdecIn->flags & IM_VDEC_PIC_SKIP) ? 1 : 0;
	mMpeg2DecIn.picId = 0;
	vdecOut->skipPicNum = 0;
	mDirectShow = (vdecIn->flags & IM_VDEC_PIC_DIRECT_SHOW) ? 1 : 0;

	mpeg2Ret = Mpeg2DecDecode(mMpeg2DecInst, &mMpeg2DecIn, &mMpeg2DecOut);

	//result process
	switch(mpeg2Ret)
	{
		case MPEG2DEC_OK:
			ret = IM_RET_VDEC_NO_PIC_OUT;
			break;
		case MPEG2DEC_STRM_PROCESSED:
			IM_VDEC_INFO("MPEG2DEC_STRM_PROCESSED");
			ret = IM_RET_VDEC_STRM_PROCESSED;
			break;
		case MPEG2DEC_NONREF_PIC_SKIPPED:
			IM_VDEC_INFO("MPEG2DEC_NONREF_PIC_SKIPPED");
			vdecOut->skipPicNum = 1;
			ret = IM_RET_VDEC_PIC_SKIPPED;
			break;
		case MPEG2DEC_HDRS_RDY:
		{
			if(Mpeg2DecGetInfo(mMpeg2DecInst, &mMpeg2DecInfo) != MPEG2DEC_OK)
			{
				IM_VDEC_ERR("MPEG2DecGetInfo failed");
				return IM_RET_VDEC_FAIL;
			}
			ret = IM_RET_VDEC_HDRS_RDY;

			if(bHeaderReady)
			{
				if(mVdecInfo.codedWidth != mMpeg2DecInfo.codedWidth 
						|| mVdecInfo.codedHeight != mMpeg2DecInfo.codedHeight
						|| mVdecInfo.multiBuffPpSize != mMpeg2DecInfo.multiBuffPpSize
						|| mVdecInfo.interlacedSequence != mMpeg2DecInfo.interlacedSequence)
				{
					ret = IM_RET_VDEC_DECINFO_CHANGED;	
				}
			}

			bVdecInfoValid = IM_TRUE;
			bPicInfoValid = IM_FALSE;
			bRunTimeValid = IM_FALSE;

			if(mMpeg2DecInfo.frameWidth == 0 || mMpeg2DecInfo.frameHeight == 0)
			{
				if(!bNoInputSize)
				{
					mMpeg2DecInfo.frameWidth = IM_SIZEALIGNED16(mVdecConfig.inType.width);
					mMpeg2DecInfo.frameHeight = IM_SIZEALIGNED16(mVdecConfig.inType.height);
				}
				else
				{
					bVdecInfoValid = IM_FALSE;
				}
			}

			IM_VDEC_INFO("mpeg2Dec Header Ready::");
			IM_VDEC_INFO("mMpeg2DecInfo::frameWidth = %d", mMpeg2DecInfo.frameWidth);
			IM_VDEC_INFO("mMpeg2DecInfo::frameHeight = %d", mMpeg2DecInfo.frameHeight);
			IM_VDEC_INFO("mMpeg2DecInfo::codedWidth = %d", mMpeg2DecInfo.codedWidth);
			IM_VDEC_INFO("mMpeg2DecInfo::codedHeight = %d", mMpeg2DecInfo.codedHeight);
			IM_VDEC_INFO("mMpeg2DecInfo::profileAndLevelIndication= %d", mMpeg2DecInfo.profileAndLevelIndication);
			IM_VDEC_INFO("mMpeg2DecInfo::interlaced = %d", mMpeg2DecInfo.interlacedSequence);
			IM_VDEC_INFO("mMpeg2DecInfo::multiBuffPpSize= %d", mMpeg2DecInfo.multiBuffPpSize);

			mVdecInfo.frameWidth = mMpeg2DecInfo.frameWidth;
			mVdecInfo.frameHeight = mMpeg2DecInfo.frameHeight;
			mVdecInfo.codedWidth = mMpeg2DecInfo.codedWidth;
			mVdecInfo.codedHeight = mMpeg2DecInfo.codedHeight;
			mVdecInfo.profileAndLevelIndication = mMpeg2DecInfo.profileAndLevelIndication;
			mVdecInfo.interlacedSequence = mMpeg2DecInfo.interlacedSequence;
			mVdecInfo.multiBuffPpSize = mMpeg2DecInfo.multiBuffPpSize;
			bHeaderReady = IM_TRUE;
			mDirectShow = 1;
			getPicture(vdecPicture, 1);
			mDirectShow = 0;
		}
		break;
		case MPEG2DEC_PIC_DECODED:	
		{
			IM_VDEC_INFO("MPEG2DEC_PIC_DECODED");
			ret = getPicture(vdecPicture, 0);
#ifndef MOSAIC_PIC_OUT
			while(ret == IM_RET_VDEC_MOSAIC_PIC_OUT)
			{
				vdecOut->skipPicNum++;
				ret = getPicture(vdecPicture, 0);
			}

			if(vdecOut->skipPicNum)
			{
				IM_VDEC_INFO("skipped pic num = %d", nSkippedPicAfterFlush);
				ret = IM_RET_VDEC_PIC_SKIPPED;
			}
#endif
		}
		break;
		case MPEG2DEC_MEMFAIL:
			ret = IM_RET_VDEC_MEM_FAIL;
			break;
		case MPEG2DEC_STREAM_NOT_SUPPORTED:
			ret = IM_RET_VDEC_UNSUPPORT_STREAM;
			break;
		case MPEG2DEC_STRM_ERROR:
		default:
			IM_VDEC_ERR("mpeg2 decode error mpeg2Ret = %d", mpeg2Ret);	
			ret = IM_RET_VDEC_FAIL;
	};

	vdecOut->pCurStream = (void *)mMpeg2DecOut.pStrmCurrPos;
	vdecOut->strmCurrBusAddress = mMpeg2DecOut.strmCurrBusAddress;
	if(mMpeg2DecOut.dataLeft > (u32)vdecIn->dataLen)
	{
		IM_VDEC_WARN("invalid Mpeg2 data left");
		mMpeg2DecOut.dataLeft = 0;
	}

	vdecOut->dataLeft = mMpeg2DecOut.dataLeft;
	IM_VDEC_INFO("dataLeft = %d", mMpeg2DecOut.dataLeft);

	IM_VDEC_INFO("----");
	return ret;

}

IM_RET IM_Mpeg2HwDec::GetNextPicture(IM_VDEC_PICTURE *vdecPicture)
{
	IM_INT32 skip = 0;
	
	if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("set Pp config failed");
		skip = 1;
	}
	return getPicture(vdecPicture, skip);
}

IM_RET IM_Mpeg2HwDec::Flush()
{
	IM_VDEC_PICTURE vdecPicture;
	mDirectShow = 1;
	if(bHeaderReady)
	getPicture(&vdecPicture, 1);

	if(mVdecConfig.mode.bMultiBuffMode)
	{
		if(pMultiBuffList)
		{
			pMultiBuffList->release();
		}
	}

	//bPicInfoValid = IM_FALSE;
	//bRunTimeValid = IM_FALSE;
	bFlushed = IM_TRUE;
	
	return IM_RET_VDEC_OK;
}

IM_RET IM_Mpeg2HwDec::DeInit()
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
			if(PP_OK != PPDecCombinedModeDisable(PpInst, mMpeg2DecInst))
			{
				IM_VDEC_ERR("PPDecCombinedModeDisable failed");
			}
			else
			{
				bPpCombineModeEnable = IM_FALSE;
			}

			PPRelease(PpInst);
		}
	}

	if(mMpeg2DecInst)
	{
		Mpeg2DecRelease(mMpeg2DecInst);
	}
	
	return IM_RET_VDEC_OK;
}


IM_RET IM_Mpeg2HwDec::getPicture(IM_VDEC_PICTURE *vdecPicture, IM_INT32 skip)
{
	IM_RET ret;
	Mpeg2DecRet mpeg2Ret;
	PPResult ppResult;
	IM_INT32 skipNum = 0;
	IM_VDEC_ASSERT(vdecPicture != NULL);

	IM_VDEC_INFO("++++");
	
	do{
		mpeg2Ret = Mpeg2DecNextPicture(mMpeg2DecInst, &mMpeg2DecPicture, mDirectShow);
		if(skip && mpeg2Ret == MPEG2DEC_PIC_RDY){
			skipNum++;
		}
	}while(skip && mpeg2Ret == MPEG2DEC_PIC_RDY);

	if(skip)
	{
		vdecPicture->skipNum = skipNum;
		return IM_RET_VDEC_FAIL;
	}

	IM_VDEC_INFO("mpeg2DecPic::frameWidth = %d", mMpeg2DecPicture.frameWidth);
	IM_VDEC_INFO("mpeg2DecPic::frameHeight = %d", mMpeg2DecPicture.frameHeight);
	IM_VDEC_INFO("mpeg2DecPic::codedWidth = %d", mMpeg2DecPicture.codedWidth);
	IM_VDEC_INFO("mpeg2DecPic::codedHeight = %d", mMpeg2DecPicture.codedHeight);
	IM_VDEC_INFO("mpeg2DecPic::keyPicture = %d", mMpeg2DecPicture.keyPicture);
	IM_VDEC_INFO("mpeg2DecPic::picId = %d", mMpeg2DecPicture.picId);
	IM_VDEC_INFO("mpeg2DecPic::interlaced = %d", mMpeg2DecPicture.interlaced);
	IM_VDEC_INFO("mpeg2DecPic::fieldPicture = %d", mMpeg2DecPicture.fieldPicture);
	IM_VDEC_INFO("mpeg2DecPic::topField = %d", mMpeg2DecPicture.topField);
	
	if(mpeg2Ret == MPEG2DEC_PIC_RDY)
	{
		if(mMpeg2DecPicture.frameWidth == 0 || mMpeg2DecPicture.frameHeight  == 0)
		{
			bPicInfoValid = IM_FALSE;
			bRunTimeValid = IM_TRUE;
			IM_VDEC_WARN("MPEG2DEC_PIC_RDY, but out size is zero");
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
			vdecMemCpy(vdecPicture->buffer.virtAddr, (void*)mMpeg2DecPicture.pOutputPicture, nDecodedPicSize);
		}

		nCropWidth = (mMpeg2DecPicture.frameWidth - mMpeg2DecInfo.codedWidth) *  (float)mVdecConfig.outType.width/ mMpeg2DecInfo.codedWidth;
		nCropHeight = (mMpeg2DecPicture.frameHeight - mMpeg2DecInfo.codedHeight) * (float)mVdecConfig.outType.height / mMpeg2DecInfo.codedHeight;
		vdecPicture->dataLen = nDecodedPicSize;
		vdecPicture->keyPicture = mMpeg2DecPicture.keyPicture;
		vdecPicture->interlaced = mMpeg2DecPicture.interlaced;
		
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
			if(!mMpeg2DecPicture.keyPicture && nSkippedPicAfterFlush < MAX_SKIP_PIC_NUM)
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
		IM_VDEC_INFO("mpeg2Ret = %d", mpeg2Ret);
		bPicInfoValid = IM_FALSE;
		bRunTimeValid = IM_TRUE;
		mDirectShow = 0;
		return  IM_RET_VDEC_NO_PIC_OUT;
	}
}

IM_RET IM_Mpeg2HwDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
	IM_VDEC_ASSERT(vdecPicture != NULL);
	IM_VDEC_ASSERT(PpInst != NULL);

	if(bPicInfoValid)
	{
		IM_VDEC_INFO("set  PpCfg by PicInfo");
		PpCfg.ppInImg.width = mMpeg2DecPicture.frameWidth;
		PpCfg.ppInImg.height = mMpeg2DecPicture.frameHeight;
		if(mVdecConfig.mode.bStandAloneMode)
		{
			PpCfg.ppInImg.bufferBusAddr = mMpeg2DecPicture.outputPictureBusAddress; 
			PpCfg.ppInImg.bufferCbBusAddr = mMpeg2DecPicture.outputPictureBusAddress + mMpeg2DecPicture.frameHeight * mMpeg2DecPicture.frameWidth; 
			PpCfg.ppInImg.bufferCrBusAddr = PpCfg.ppInImg.bufferCbBusAddr;
		}


		if(mMpeg2DecPicture.frameWidth != mMpeg2DecInfo.frameHeight ||
				mMpeg2DecPicture.frameHeight != mMpeg2DecInfo.frameHeight)
		{
			if(bVdecInfoValid)
			{
				if(mMpeg2DecPicture.frameWidth == 0 || mMpeg2DecPicture.frameHeight == 0)
				{
					PpCfg.ppInImg.width = mMpeg2DecInfo.frameWidth;
					PpCfg.ppInImg.height = mMpeg2DecInfo.frameHeight;
				}
			}
		}

		if(mMpeg2DecPicture.interlaced == 0 ||
			PpCfg.ppOutDeinterlace.enable == 1)
		{
			//PpCfg.ppInImg.height = mMpeg2DecPicture.frameHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
		}
		else
		{
			if(mMpeg2DecPicture.fieldPicture != 0)
			{
				PpCfg.ppInImg.height >>= 1;//mMpeg2DecPicture.frameHeight >> 1;
				if(mMpeg2DecPicture.topField != 0)
				{
					PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
				}
				else
				{
					PpCfg.ppInImg.picStruct = PP_PIC_BOT_FIELD;
				}
			}
			else
			{
				//PpCfg.ppInImg.height = mMpeg2DecPicture.frameHeight;
				PpCfg.ppInImg.picStruct = PP_PIC_TOP_AND_BOT_FIELD_FRAME;
			}
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
		mMpeg2DecInfo.frameHeight = IM_SIZEALIGNED16(mMpeg2DecInfo.frameHeight);
		PpCfg.ppInImg.width = mMpeg2DecInfo.frameWidth;
		PpCfg.ppInImg.height = mMpeg2DecInfo.frameHeight;
		if(mMpeg2DecInfo.interlacedSequence == 0){
			//PpCfg.ppInImg.height = mMpeg2DecInfo.frameHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
		}else{
			//PpCfg.ppInImg.height = mMpeg2DecInfo.frameHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_TOP_AND_BOT_FIELD_FRAME;
			PpCfg.ppOutDeinterlace.enable = 1 ;
		}

		PpCfg.ppInImg.videoRange = mMpeg2DecInfo.videoRange;
	}
	else
	{
		PpCfg.ppInImg.width = IM_SIZEALIGNED16(mVdecConfig.inType.width);
		PpCfg.ppInImg.height = IM_SIZEALIGNED16(mVdecConfig.inType.height);
	}

	PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
	PpCfg.ppOutImg.width = mVdecConfig.outType.width;
	PpCfg.ppOutImg.height = mVdecConfig.outType.height;

	return IM_VideoDec::setPpCfg(vdecPicture);
}

