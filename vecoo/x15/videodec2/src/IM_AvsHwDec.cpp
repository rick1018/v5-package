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
 * @file        IM_AvsHwDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#if TARGET_SYSTEM == FS_ANDROID
#define LOG_TAG "AvsHwDec"
#endif

#include "IM_AvsHwDec.h"
#include "IM_VdecLog.h"
IM_AvsHwDec::IM_AvsHwDec(IM_VDEC_INTYPE *inType)
{
	IM_VDEC_INFO("++++");
	memcpy(&mVdecConfig.inType, inType, sizeof(IM_VDEC_INTYPE));

	getDefaultConfig(&mVdecConfig);
	mAvsBuild = AvsDecGetBuild(); 
	if(mVdecConfig.build.maxOutWidth > mAvsBuild.hwConfig.maxPpOutPicWidth )
	{
		mVdecConfig.build.maxOutWidth  = mAvsBuild.hwConfig.maxPpOutPicWidth;
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
	memset(&mAvsDecIn, 0, sizeof(AvsDecInput));
	memset(&mAvsDecOut, 0, sizeof(AvsDecOutput));
	memset(&mAvsDecInfo, 0, sizeof(AvsDecInfo));
	memset(&mAvsDecPicture, 0, sizeof(AvsDecPicture));

	mAvsDecInst = NULL;


	pMultiBuffList = NULL;
	nSkippedPicAfterFlush = 0;
	IM_VDEC_INFO("----");
}

IM_BOOL IM_AvsHwDec::isSupported()
{
	IM_VDEC_INFO("++++");
	if(mAvsBuild.hwConfig.avsSupport == AVS_NOT_SUPPORTED)
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

IM_RET IM_AvsHwDec::Init(IM_VDEC_SPEC_DATA *specData,IM_REF_FRM_FORMAT refFrmFormat)
{
	AvsDecRet  avsRet;
	PPResult   ppResult;
	IM_VDEC_INFO("++++");

	if(!bVdecConfigSetted)
	{
		IM_VDEC_ERR("VDEC CONFIG NOT SET, please set config");
		return IM_RET_VDEC_FAIL;
	}

#ifndef VDEC_LIB_G1
	return IM_RET_VDEC_FAIL;
#else
	avsRet = AvsDecInit(&mAvsDecInst, mUseVideoFreezeConcealment, nExtraFrameBuffers, (DecRefFrmFormat)refFrmFormat);
#endif

	if(avsRet != AVSDEC_OK){
		IM_VDEC_ERR("AvsDecInit Failed error = %d", avsRet);
		return IM_RET_VDEC_INIT_FAIL;
	}

	if(mVdecConfig.mode.bCombineMode || mVdecConfig.mode.bStandAloneMode)
	{
		if((ppResult = PPInit(&PpInst)) != PP_OK)
		{
			IM_VDEC_ERR("PPInit Failed err= %d", ppResult);
			return IM_RET_VDEC_INIT_FAIL;
		}

		if(mVdecConfig.mode.bCombineMode)
		{
			if((ppResult = PPDecCombinedModeEnable(PpInst, mAvsDecInst, PP_PIPELINED_DEC_TYPE_AVS)) != PP_OK)
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
	AvsDecMMUEnable(mAvsDecInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	if(PpInst != NULL)
	{
		PPDecMMUEnable(PpInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	}
#endif 	

	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_AvsHwDec::Decode(const IM_VDEC_IN *vdecIn, IM_VDEC_OUT *vdecOut, IM_VDEC_PICTURE *vdecPicture)
{
	AvsDecRet avsRet; 
	PPResult ppResult;
	IM_RET ret;
	IM_VDEC_INFO("++++");
	IM_VDEC_ASSERT(mAvsDecInst != NULL);

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
			if((ppResult = PPDecCombinedModeEnable(PpInst, mAvsDecInst, PP_PIPELINED_DEC_TYPE_AVS)) != PP_OK)
			{
				IM_VDEC_ERR("PPDecCombinedModeEnable Failed, ppResult = %d",ppResult);
				return IM_RET_VDEC_NOT_INITIALIZED;
			}
			bPpCombineModeEnable = IM_TRUE;
#endif
		}

		if(mVdecConfig.mode.bMultiBuffMode)
		{
			IM_RET ret = checkMultiBuffer(mAvsDecInfo.multiBuffPpSize);
			if(ret == IM_RET_VDEC_LACK_OUTBUFF)                                                                                                           
			{
				vdecOut->multiBuffPpSize = mAvsDecInfo.multiBuffPpSize;                                                                                  
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
	mAvsDecIn.pStream = (u8 *)vdecIn->pStream;
	mAvsDecIn.streamBusAddress = vdecIn->busAddr;
	mAvsDecIn.dataLen = vdecIn->dataLen;
	mAvsDecIn.skipNonReference = (vdecIn->flags & IM_VDEC_PIC_SKIP) ? 1 : 0;
	mAvsDecIn.picId = 0;
	vdecOut->skipPicNum = 0;
	vdecOut->dataLeft = vdecIn->dataLen;
	mDirectShow = (vdecIn->flags & IM_VDEC_PIC_DIRECT_SHOW) ? 1 : 0;

	avsRet = AvsDecDecode(mAvsDecInst, &mAvsDecIn, &mAvsDecOut);

	//result process
	switch(avsRet)
	{
		case AVSDEC_OK:
			ret = IM_RET_VDEC_NO_PIC_OUT;
			break;
		case AVSDEC_STRM_PROCESSED:
			IM_VDEC_INFO("AVSDEC_STRM_PROCESSED");
			ret = IM_RET_VDEC_STRM_PROCESSED;
			break;
		case AVSDEC_NONREF_PIC_SKIPPED:
			ret = IM_RET_VDEC_PIC_SKIPPED;
			break;
		case AVSDEC_HDRS_RDY:
		{
			AvsDecInfo *decInfo = NULL;
			
			if(AvsDecGetInfo(mAvsDecInst, &mAvsDecInfo) != AVSDEC_OK)
			{
				IM_VDEC_ERR("AvsDecGetInfo failed");
				return IM_RET_VDEC_FAIL;
			}
			ret = IM_RET_VDEC_HDRS_RDY;

			if(bHeaderReady)
			{
				if(mVdecInfo.codedWidth != mAvsDecInfo.codedWidth 
						|| mVdecInfo.codedHeight != mAvsDecInfo.codedHeight
						|| mVdecInfo.multiBuffPpSize != mAvsDecInfo.multiBuffPpSize
						|| mVdecInfo.interlacedSequence != mAvsDecInfo.interlacedSequence)
				{
					ret = IM_RET_VDEC_DECINFO_CHANGED;	
				}
			}

			bVdecInfoValid = IM_TRUE;
			bPicInfoValid = IM_TRUE;
			bRunTimeValid = IM_TRUE;

			if(mAvsDecInfo.frameWidth == 0 || mAvsDecInfo.frameHeight == 0)
			{
				if(!bNoInputSize)
				{
					mAvsDecInfo.frameWidth = IM_SIZEALIGNED16(mVdecConfig.inType.width);
					mAvsDecInfo.frameHeight = IM_SIZEALIGNED16(mVdecConfig.inType.height);
				}
				else
				{
					bVdecInfoValid = IM_FALSE;
				}
			}

			IM_VDEC_INFO("AvsDec Header Ready::");
			IM_VDEC_INFO("mAvsDecInfo::frameWidth = %d", mAvsDecInfo.frameWidth);
			IM_VDEC_INFO("mAvsDecInfo::frameHeight = %d", mAvsDecInfo.frameHeight);
			IM_VDEC_INFO("mAvsDecInfo::codedWidth = %d", mAvsDecInfo.codedWidth);
			IM_VDEC_INFO("mAvsDecInfo::codedHeight = %d", mAvsDecInfo.codedHeight);
			IM_VDEC_INFO("mAvsDecInfo::profileId = %d", mAvsDecInfo.profileId);
			IM_VDEC_INFO("mAvsDecInfo::levelId = %d", mAvsDecInfo.levelId);
			IM_VDEC_INFO("mAvsDecInfo::displayAspectRatio = %d", mAvsDecInfo.displayAspectRatio);
			IM_VDEC_INFO("mAvsDecInfo::videoFormat = %d", mAvsDecInfo.videoFormat);
			IM_VDEC_INFO("mAvsDecInfo::videoRange = %d", mAvsDecInfo.videoRange);

			IM_VDEC_INFO("mAvsDecInfo::interlaced = %d", mAvsDecInfo.interlacedSequence);
			IM_VDEC_INFO("mAvsDecInfo::multiBuffPpSize= %d", mAvsDecInfo.multiBuffPpSize);


			mVdecInfo.frameWidth = mAvsDecInfo.frameWidth;
			mVdecInfo.frameHeight = mAvsDecInfo.frameHeight;
			mVdecInfo.codedWidth = mAvsDecInfo.codedWidth;
			mVdecInfo.codedHeight = mAvsDecInfo.codedHeight;
			mVdecInfo.interlacedSequence = mAvsDecInfo.interlacedSequence;
			mVdecInfo.multiBuffPpSize = mAvsDecInfo.multiBuffPpSize;
			bHeaderReady = IM_TRUE;
		}
		break;
		case AVSDEC_PIC_DECODED:	
		{
			IM_VDEC_INFO("AVSDEC_PIC_DECODED");
			ret = getPicture(vdecPicture, 0);
#if 0
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
		case AVSDEC_MEMFAIL:
			ret = IM_RET_VDEC_MEM_FAIL;
		case AVSDEC_STRM_ERROR:
		default:
			mAvsDecOut.dataLeft = 0;
			IM_VDEC_ERR("Avs decode error avsRet = %d", avsRet);	
			ret = IM_RET_VDEC_FAIL;
	};

	vdecOut->pCurStream = (void *)mAvsDecOut.pStrmCurrPos;
	vdecOut->strmCurrBusAddress = mAvsDecOut.strmCurrBusAddress;
	vdecOut->dataLeft = mAvsDecOut.dataLeft > 0 ? mAvsDecOut.dataLeft : 0;
	IM_VDEC_INFO("dataLeft = %d", mAvsDecOut.dataLeft);

	IM_VDEC_INFO("----");
	return ret;

}

IM_RET IM_AvsHwDec::GetNextPicture(IM_VDEC_PICTURE *vdecPicture)
{
	IM_INT32 skip = 0;
	
	if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("set Pp config failed");
		skip = 1;
	}
	return getPicture(vdecPicture, skip);
}

IM_RET IM_AvsHwDec::Flush()
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

IM_RET IM_AvsHwDec::DeInit()
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
			if(PP_OK != PPDecCombinedModeDisable(PpInst, mAvsDecInst))
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

	if(mAvsDecInst)
	{
		AvsDecRelease(mAvsDecInst);
	}
	
	return IM_RET_VDEC_OK;
}


IM_RET IM_AvsHwDec::getPicture(IM_VDEC_PICTURE *vdecPicture, IM_INT32 skip)
{
	IM_RET ret;
	AvsDecRet avsRet;
	PPResult ppResult;
	IM_INT32 skipNum = 0;
	IM_VDEC_ASSERT(vdecPicture != NULL);

	IM_VDEC_INFO("++++");
	
	do{
		avsRet = AvsDecNextPicture(mAvsDecInst, &mAvsDecPicture, mDirectShow);
		if(skip && avsRet == AVSDEC_PIC_RDY){
			skipNum++;
		}
	}while(skip && avsRet == AVSDEC_PIC_RDY);

	if(skip)
	{
		vdecPicture->skipNum = skipNum;
		return IM_RET_VDEC_FAIL;
	}

	
	if(avsRet == AVSDEC_PIC_RDY)
	{
		IM_VDEC_INFO("AvsDecPic::frameWidth = %d", mAvsDecPicture.frameWidth);
		IM_VDEC_INFO("AvsDecPic::frameHeight = %d", mAvsDecPicture.frameHeight);
		IM_VDEC_INFO("AvsDecPic::codedWidth = %d", mAvsDecPicture.codedWidth);
		IM_VDEC_INFO("AvsDecPic::codedHeight = %d", mAvsDecPicture.codedHeight);
		IM_VDEC_INFO("AvsDecPic::keyPicture = %d", mAvsDecPicture.keyPicture);
		IM_VDEC_INFO("AvsDecPic::picId = %d", mAvsDecPicture.picId);
		IM_VDEC_INFO("AvsDecPic::interlaced = %d", mAvsDecPicture.interlaced);
		IM_VDEC_INFO("AvsDecPic::fieldPicture = %d", mAvsDecPicture.fieldPicture);
		IM_VDEC_INFO("AvsDecPic::topField = %d", mAvsDecPicture.topField);

		if(mAvsDecPicture.frameWidth == 0 || mAvsDecPicture.frameHeight  == 0)
		{
			bPicInfoValid = IM_FALSE;
			bRunTimeValid = IM_TRUE;
			IM_VDEC_WARN("AVSDEC_PIC_RDY, but out size is zero");
			return IM_RET_VDEC_NO_PIC_OUT;
		}

		bPicInfoValid = IM_TRUE;
		if(mVdecConfig.mode.bStandAloneMode)
		{
			if(setPpCfg(vdecPicture) != IM_RET_OK)
			{
				return getPicture(vdecPicture, 1);
			}
		}
		if(mVdecConfig.mode.bCombineMode || mVdecConfig.mode.bStandAloneMode)
		{
			if((ppResult = PPGetResult(PpInst)) != PP_OK)
			{
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
					IM_VDEC_ERR("get Pp next output failed, ret = %d", ppResult);
					return getPicture(vdecPicture, 1);
				}

				IM_VDEC_INFO("out pic addr = %x", tmpOutBuffer.bufferBusAddr);
				pMultiBuffList->getElembybusAddr(tmpOutBuffer.bufferBusAddr, &qElem1);
				qElem2.buffer.virtAddr = vdecPicture->buffer.virtAddr;
				qElem2.buffer.busAddr = vdecPicture->buffer.busAddr;
				qElem2.buffer.size = vdecPicture->buffer.size;
				pMultiBuffList->fillElem(qElem1.index, &qElem2);
				setMultiBuffers(pMultiBuffList);
				vdecPicture->buffer.virtAddr = qElem1.buffer.virtAddr;
				vdecPicture->buffer.busAddr = qElem1.buffer.busAddr; 
				vdecPicture->buffer.size    = qElem1.buffer.size;
			}
		}
		else
		{
			//do not use 
			vdecMemCpy(vdecPicture->buffer.virtAddr, (void*)mAvsDecPicture.pOutputPicture, nDecodedPicSize);
		}

		nCropWidth = (mAvsDecPicture.frameWidth - mAvsDecInfo.codedWidth) *  (float)mVdecConfig.outType.width/ mAvsDecInfo.codedWidth;
		nCropHeight = (mAvsDecPicture.frameHeight - mAvsDecInfo.codedHeight) * (float)mVdecConfig.outType.height / mAvsDecInfo.codedHeight;
		vdecPicture->dataLen = nDecodedPicSize;
		vdecPicture->keyPicture = mAvsDecPicture.keyPicture;
		vdecPicture->interlaced = mAvsDecPicture.interlaced;
		
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
			if(!mAvsDecPicture.keyPicture && nSkippedPicAfterFlush < MAX_SKIP_PIC_NUM)
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
		IM_VDEC_INFO("avsRet = %d", avsRet);
		bPicInfoValid = IM_FALSE;
		bRunTimeValid = IM_TRUE;
		mDirectShow = 0;
		return  IM_RET_VDEC_NO_PIC_OUT;
	}
}

IM_RET IM_AvsHwDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
	IM_VDEC_ASSERT(vdecPicture != NULL);
	IM_VDEC_ASSERT(PpInst != NULL);

	if(bPicInfoValid)
	{
		IM_VDEC_INFO("set  PpCfg by PicInfo");
		PpCfg.ppInImg.width = mAvsDecPicture.frameWidth;
		PpCfg.ppInImg.height = mAvsDecPicture.frameHeight;
		if(mVdecConfig.mode.bStandAloneMode)
		{
			PpCfg.ppInImg.bufferBusAddr = mAvsDecPicture.outputPictureBusAddress; 
			PpCfg.ppInImg.bufferCbBusAddr = mAvsDecPicture.outputPictureBusAddress + mAvsDecPicture.frameHeight * mAvsDecPicture.frameWidth; 
			PpCfg.ppInImg.bufferCrBusAddr = PpCfg.ppInImg.bufferCbBusAddr;
		}


		if(mAvsDecPicture.interlaced == 0 ||
			PpCfg.ppOutDeinterlace.enable == 1)
		{
			//PpCfg.ppInImg.height = mAvsDecPicture.frameHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
		}
		else
		{
			if(mAvsDecPicture.fieldPicture != 0)
			{
				PpCfg.ppInImg.height = mAvsDecPicture.frameHeight >> 1;
				if(mAvsDecPicture.topField != 0)
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
				//PpCfg.ppInImg.height = mAvsDecPicture.frameHeight;
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
		mAvsDecInfo.frameHeight = IM_SIZEALIGNED16(mAvsDecInfo.frameHeight);
		PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
		PpCfg.ppInImg.width = mAvsDecInfo.frameWidth;
		PpCfg.ppInImg.height = mAvsDecInfo.frameHeight;
		if(mAvsDecInfo.interlacedSequence == 0){
			PpCfg.ppInImg.height = mAvsDecInfo.frameHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
		}else{
			PpCfg.ppInImg.height = mAvsDecInfo.frameHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_TOP_AND_BOT_FIELD_FRAME;
			PpCfg.ppOutDeinterlace.enable = 1 ;
		}

		PpCfg.ppInImg.videoRange = mAvsDecInfo.videoRange;
	}
	else
	{
		PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
		PpCfg.ppInImg.width = IM_SIZEALIGNED16(mVdecConfig.inType.width);
		PpCfg.ppInImg.height = IM_SIZEALIGNED16(mVdecConfig.inType.height);
	}

	PpCfg.ppOutImg.width = mVdecConfig.outType.width;
	PpCfg.ppOutImg.height = mVdecConfig.outType.height;

	return IM_VideoDec::setPpCfg(vdecPicture);
}

