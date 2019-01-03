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
 * @file        IM_Mp4HwDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#if TARGET_SYSTEM == FS_ANDROID
#define LOG_TAG "Mp4HwDec"
#endif

#include "IM_Mp4HwDec.h"
#include "IM_VdecLog.h"
IM_Mp4HwDec::IM_Mp4HwDec(IM_VDEC_INTYPE *inType)
{
	IM_VDEC_INFO("++++");
	if(inType->codec == IM_STRM_VIDEO_MPEG4 ||
#if 0
			inType->codec == IM_STRM_VIDEO_DIVX4 ||
			inType->codec == IM_STRM_VIDEO_DIVX5 ||
			inType->codec == IM_STRM_VIDEO_DIVX6 ||
#endif
			inType->codec == IM_STRM_VIDEO_H263){
		mMp4StrFmt = MP4DEC_MPEG4;
	}else if(inType->codec == IM_STRM_VIDEO_FLV1){
		mMp4StrFmt = MP4DEC_SORENSON;
	}else{
		mMp4StrFmt = MP4DEC_CUSTOM_1;
	}

	memcpy(&mVdecConfig.inType, inType, sizeof(IM_VDEC_INTYPE));

	getDefaultConfig(&mVdecConfig);
	mMp4Build = MP4DecGetBuild(); 
	if(mVdecConfig.build.maxOutWidth > mMp4Build.hwConfig.maxPpOutPicWidth )
	{
		mVdecConfig.build.maxOutWidth  = mMp4Build.hwConfig.maxPpOutPicWidth;
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
	bOutputChanged = IM_FALSE;
	memset(&mVdecIn, 0, sizeof(IM_VDEC_IN));
	memset(&mVdecOut, 0, sizeof(IM_VDEC_OUT));
	memset(&mVdecInfo, 0, sizeof(IM_VDEC_INFO));
	memset(&mMp4DecIn, 0, sizeof(MP4DecInput));
	memset(&mMp4DecOut, 0, sizeof(MP4DecOutput));
	memset(&mMp4DecInfo, 0, sizeof(MP4DecInfo));
	memset(&mMp4DecPicture, 0, sizeof(MP4DecPicture));

	mMp4DecInst = NULL;
	PpInst = NULL;


	pMultiBuffList = NULL;
	nSkippedPicAfterFlush = 0;
	IM_VDEC_INFO("----");
}

IM_BOOL IM_Mp4HwDec::isSupported()
{
	IM_VDEC_INFO("++++");
	if((mMp4StrFmt == MP4DEC_MPEG4 && mMp4Build.hwConfig.mpeg4Support == MPEG4_NOT_SUPPORTED))
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

IM_RET IM_Mp4HwDec::Init(IM_VDEC_SPEC_DATA *specData,IM_REF_FRM_FORMAT refFrmFormat)
{
	MP4DecRet  mp4Ret;
	PPResult   ppResult;
	IM_VDEC_INFO("++++");

	if(!bVdecConfigSetted)
	{
		IM_VDEC_ERR("VDEC CONFIG NOT SET, please set config first");
		return IM_RET_VDEC_INIT_FAIL;
	}

#ifndef VDEC_LIB_G1
	mp4Ret = MP4DecInit(&mMp4DecInst, mMp4StrFmt, mUseVideoFreezeConcealment, nExtraFrameBuffers);
#else
	mp4Ret = MP4DecInit(&mMp4DecInst, mMp4StrFmt, mUseVideoFreezeConcealment, nExtraFrameBuffers, (DecRefFrmFormat)refFrmFormat);
#endif

	if(mp4Ret != MP4DEC_OK){
		IM_VDEC_ERR("MP4DecInit Failed error = %d", mp4Ret);
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
			if((ppResult = PPDecCombinedModeEnable(PpInst, mMp4DecInst, PP_PIPELINED_DEC_TYPE_MPEG4)) != PP_OK)
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
	MP4DecMMUEnable(mMp4DecInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	if(PpInst != NULL)
	{
		PPDecMMUEnable(PpInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	}
#endif 	
	
	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_Mp4HwDec::Decode(const IM_VDEC_IN *vdecIn, IM_VDEC_OUT *vdecOut, IM_VDEC_PICTURE *vdecPicture)
{
	MP4DecRet mp4Ret; 
	PPResult ppResult;
	IM_RET ret;
	IM_VDEC_INFO("++++");
	IM_VDEC_ASSERT(mMp4DecInst != NULL);

	if(vdecIn == NULL || vdecOut == NULL || vdecPicture == NULL)
	{
		IM_VDEC_ERR("IM_RET_VDEC_INVALID_PARAMETERS");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	vdecOut->dataLeft  = vdecIn->dataLen;

	//check config
	if(bHeaderReady && mVdecConfig.mode.bCombineMode)
	{
		if(!bPpCombineModeEnable)
		{
#if 0
			return IM_RET_VDEC_NOT_INITIALIZED;
#else
			if((ppResult = PPDecCombinedModeEnable(PpInst, mMp4DecInst, PP_PIPELINED_DEC_TYPE_MPEG4)) != PP_OK)
			{
				IM_VDEC_ERR("PPDecCombinedModeEnable Failed, ppResult = %d",ppResult);
				return IM_RET_VDEC_NOT_INITIALIZED;
			}
			bPpCombineModeEnable = IM_TRUE;
#endif
		}

		if(mVdecConfig.mode.bMultiBuffMode)
		{
			IM_RET ret = checkMultiBuffer(mMp4DecInfo.multiBuffPpSize);
			if(ret == IM_RET_VDEC_LACK_OUTBUFF)
			{
                vdecOut->multiBuffPpSize = mMp4DecInfo.multiBuffPpSize;
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


	if(mMp4StrFmt == MP4DEC_CUSTOM_1 && !bHeaderReady)
	{
		MP4DecSetInfo(&mMp4DecInst, mVdecConfig.inType.width, mVdecConfig.inType.height);
	}
	//decode
	mMp4DecIn.pStream = (const u8 *)vdecIn->pStream;
	mMp4DecIn.streamBusAddress = vdecIn->busAddr;
	mMp4DecIn.dataLen = vdecIn->dataLen;
	mMp4DecIn.skipNonReference = (vdecIn->flags & IM_VDEC_PIC_SKIP) ? 1 : 0;
	mMp4DecIn.enableDeblock = 0;
	mMp4DecIn.picId = vdecIn->picId;
	vdecOut->skipPicNum = 0;
	mDirectShow = (vdecIn->flags & IM_VDEC_PIC_DIRECT_SHOW) ? 1 : 0;

	mp4Ret = MP4DecDecode(mMp4DecInst, &mMp4DecIn, &mMp4DecOut);

	//result process
	switch(mp4Ret)
	{
		case MP4DEC_VOS_END:
			IM_VDEC_INFO("MP4DEC_VOS_END");		
		case MP4DEC_OK:
			ret = IM_RET_VDEC_NO_PIC_OUT;
			break;
		case MP4DEC_STRM_PROCESSED:
			IM_VDEC_INFO("MP4DEC_STRM_PROCESSED");
			ret = IM_RET_VDEC_STRM_PROCESSED;
			break;
		case MP4DEC_NONREF_PIC_SKIPPED:
			IM_VDEC_INFO("MP4DEC_NONREF_PIC_SKIPPED");
			vdecOut->skipPicNum++;
			ret = IM_RET_VDEC_PIC_SKIPPED;
			break;
		case MP4DEC_DP_HDRS_RDY:
			IM_VDEC_ERR("MP4DEC_DP_HDRS_RDY");
		case MP4DEC_HDRS_RDY:
		{
			if(MP4DecGetInfo(mMp4DecInst, &mMp4DecInfo) != MP4DEC_OK)
			{
				IM_VDEC_ERR("MP4DecGetInfo failed");
				return IM_RET_VDEC_FAIL;
			}
			ret = IM_RET_VDEC_HDRS_RDY;

			if(bHeaderReady)
			{
				if(mVdecInfo.codedWidth != mMp4DecInfo.codedWidth 
						|| mVdecInfo.codedHeight != mMp4DecInfo.codedHeight
						|| mVdecInfo.multiBuffPpSize != mMp4DecInfo.multiBuffPpSize
						|| mVdecInfo.interlacedSequence != mMp4DecInfo.interlacedSequence)
				{
					ret = IM_RET_VDEC_DECINFO_CHANGED;	
				}
			}

			bVdecInfoValid = IM_TRUE;
			bPicInfoValid = IM_FALSE;
			bRunTimeValid = IM_FALSE;

			if(mMp4DecInfo.frameWidth == 0 || mMp4DecInfo.frameHeight == 0)
			{
				if(!bNoInputSize)
				{
					mMp4DecInfo.frameWidth = IM_SIZEALIGNED16(mVdecConfig.inType.width);
					mMp4DecInfo.frameHeight = IM_SIZEALIGNED16(mVdecConfig.inType.height);
				}
				else
				{
					bVdecInfoValid = IM_FALSE;
				}
			}

			IM_VDEC_INFO("Mp4Dec Header Ready::");
			IM_VDEC_INFO("mMp4DecInfo::frameWidth = %d", mMp4DecInfo.frameWidth);
			IM_VDEC_INFO("mMp4DecInfo::frameHeight = %d", mMp4DecInfo.frameHeight);
			IM_VDEC_INFO("mMp4DecInfo::codedWidth = %d", mMp4DecInfo.codedWidth);
			IM_VDEC_INFO("mMp4DecInfo::codedHeight = %d", mMp4DecInfo.codedHeight);
			IM_VDEC_INFO("mMp4DecInfo::profileAndLevelIndication= %d", mMp4DecInfo.profileAndLevelIndication);
			IM_VDEC_INFO("mMp4DecInfo::interlaced = %d", mMp4DecInfo.interlacedSequence);
			IM_VDEC_INFO("mMp4DecInfo::multiBuffPpSize= %d", mMp4DecInfo.multiBuffPpSize);


			mVdecInfo.frameWidth = mMp4DecInfo.frameWidth;
			mVdecInfo.frameHeight = mMp4DecInfo.frameHeight;
			mVdecInfo.codedWidth = mMp4DecInfo.codedWidth;
			mVdecInfo.codedHeight = mMp4DecInfo.codedHeight;
			mVdecInfo.profileAndLevelIndication = mMp4DecInfo.profileAndLevelIndication;
			mVdecInfo.interlacedSequence = mMp4DecInfo.interlacedSequence;
			mVdecInfo.multiBuffPpSize = mMp4DecInfo.multiBuffPpSize;
			bHeaderReady = IM_TRUE;
			mDirectShow = 1;
			IM_VDEC_PICTURE vdecPicture;
			getPicture(&vdecPicture, 1);
		}
		break;
		case MP4DEC_PIC_DECODED:	
		{
			IM_VDEC_INFO("MP4DEC_PIC_DECODED");
			ret = getPicture(vdecPicture, 0);
#ifndef MOSAIC_PIC_OUT 
			while(ret == IM_RET_VDEC_MOSAIC_PIC_OUT)
			{
				vdecOut->skipPicNum++;
				ret = getPicture(vdecPicture, 0);
			}

			if(vdecOut->skipPicNum != 0)
			{
				IM_VDEC_INFO("After flush,skipPicNum = %d", vdecOut->skipPicNum);
				ret = IM_RET_VDEC_PIC_SKIPPED;
			}
#endif
		}
		break;
		case MP4DEC_MEMFAIL:
			ret = IM_RET_VDEC_MEM_FAIL;
			break;
		case MP4DEC_STRM_NOT_SUPPORTED:
			ret = IM_RET_VDEC_UNSUPPORT_STREAM;
			break;
		case MP4DEC_STRM_ERROR:
		default:
			IM_VDEC_ERR("Mp4 decode error mp4Ret = %d", mp4Ret);	
			ret = IM_RET_VDEC_FAIL;
	};

	vdecOut->pCurStream = (void *)mMp4DecOut.pStrmCurrPos;
	vdecOut->strmCurrBusAddress = mMp4DecOut.strmCurrBusAddress;
	vdecOut->dataLeft = mMp4DecOut.dataLeft;
	IM_VDEC_INFO("dataLeft = %d", mMp4DecOut.dataLeft);

	IM_VDEC_INFO("----");
	return ret;

}

IM_RET IM_Mp4HwDec::GetNextPicture(IM_VDEC_PICTURE *vdecPicture)
{
	IM_INT32 skip = 0;
	
	if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("set Pp config failed");
		skip = 1;
	}
	return getPicture(vdecPicture, skip);
}

IM_RET IM_Mp4HwDec::Flush()
{
	IM_VDEC_PICTURE vdecPicture;
	mDirectShow = 1;
	getPicture(&vdecPicture, 1);
	mDirectShow = 0;

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

IM_RET IM_Mp4HwDec::DeInit()
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
			if(PP_OK != PPDecCombinedModeDisable(PpInst, mMp4DecInst))
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

	if(mMp4DecInst)
	{
		MP4DecRelease(mMp4DecInst);
	}
	
	return IM_RET_VDEC_OK;
}


IM_RET IM_Mp4HwDec::getPicture(IM_VDEC_PICTURE *vdecPicture, IM_INT32 skip)
{
	IM_RET ret;
	MP4DecRet mp4Ret;
	PPResult ppResult;
	IM_INT32 skipNum = 0;
	IM_VDEC_ASSERT(vdecPicture != NULL);

	IM_VDEC_INFO("++++");
	
	do{
		mp4Ret = MP4DecNextPicture(mMp4DecInst, &mMp4DecPicture, mDirectShow);
		if(skip && mp4Ret == MP4DEC_PIC_RDY){
			skipNum++;
		}
	}while(skip && mp4Ret == MP4DEC_PIC_RDY);

	if(skip)
	{
		vdecPicture->skipNum = skipNum;
		return IM_RET_VDEC_FAIL;
	}

	
	if(mp4Ret == MP4DEC_PIC_RDY)
	{
		IM_VDEC_INFO("Mp4DecPic::frameWidth = %d", mMp4DecPicture.frameWidth);
		IM_VDEC_INFO("Mp4DecPic::frameHeight = %d", mMp4DecPicture.frameHeight);
		IM_VDEC_INFO("Mp4DecPic::codedWidth = %d", mMp4DecPicture.codedWidth);
		IM_VDEC_INFO("Mp4DecPic::codedHeight = %d", mMp4DecPicture.codedHeight);
		IM_VDEC_INFO("Mp4DecPic::keyPicture = %d", mMp4DecPicture.keyPicture);
		IM_VDEC_INFO("Mp4DecPic::picId = %d", mMp4DecPicture.picId);
		IM_VDEC_INFO("Mp4DecPic::interlaced = %d", mMp4DecPicture.interlaced);
		IM_VDEC_INFO("Mp4DecPic::fieldPicture = %d", mMp4DecPicture.fieldPicture);
		IM_VDEC_INFO("Mp4DecPic::topField = %d", mMp4DecPicture.topField);

		if(mMp4DecPicture.frameWidth == 0 || mMp4DecPicture.frameHeight  == 0)
		{
			bPicInfoValid = IM_FALSE;
			bRunTimeValid = IM_TRUE;
			IM_VDEC_WARN("MP4DEC_PIC_RDY, but out size is zero");
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
				setMultiBuffers(pMultiBuffList);
				vdecPicture->buffer.virtAddr = qElem1.buffer.virtAddr;
				vdecPicture->buffer.busAddr = qElem1.buffer.busAddr; 
				vdecPicture->buffer.size    = qElem1.buffer.size;
			}
		}
		else
		{
			//do not use 
			vdecMemCpy(vdecPicture->buffer.virtAddr, (void*)mMp4DecPicture.pOutputPicture, nDecodedPicSize);
		}

		nCropWidth = (mMp4DecPicture.frameWidth - mMp4DecInfo.codedWidth) *  (float)mVdecConfig.outType.width/ mMp4DecInfo.codedWidth;
		nCropHeight = (mMp4DecPicture.frameHeight - mMp4DecInfo.codedHeight) * (float)mVdecConfig.outType.height / mMp4DecInfo.codedHeight;
		vdecPicture->dataLen = nDecodedPicSize;
		vdecPicture->keyPicture = mMp4DecPicture.keyPicture;
		vdecPicture->interlaced = mMp4DecPicture.interlaced;
		
		vdecPicture->cropWidth = bOutputChanged ?  nCropWidth + mVdecConfig.outType.width - PpCfg.ppOutImg.width  : nCropWidth;

		vdecPicture->cropHeight = bOutputChanged ? nCropHeight + mVdecConfig.outType.height - PpCfg.ppOutImg.height : nCropHeight;
		IM_VDEC_INFO("outPic::buffer.virtAddr =%p", vdecPicture->buffer.virtAddr);
		IM_VDEC_INFO("outPic::buffer.busAddr =%x", vdecPicture->buffer.busAddr);
		IM_VDEC_INFO("outPic::dataLen =%d", vdecPicture->dataLen);
		IM_VDEC_INFO("outPic::keyPicture =%d", vdecPicture->keyPicture);
		IM_VDEC_INFO("outPic::interlaced =%d", vdecPicture->interlaced);
		IM_VDEC_INFO("outPic::cropWidth =%d", vdecPicture->cropWidth);
		IM_VDEC_INFO("outPic::cropHeight =%d", vdecPicture->cropHeight);
		if(bFlushed)
		{
			if(!mMp4DecPicture.keyPicture && nSkippedPicAfterFlush < MAX_SKIP_PIC_NUM)
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
		IM_VDEC_INFO("mp4Ret = %d", mp4Ret);
		bPicInfoValid = IM_FALSE;
		bRunTimeValid = IM_TRUE;
		mDirectShow = 0;
		return  IM_RET_VDEC_NO_PIC_OUT;
	}
}

IM_RET IM_Mp4HwDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
	IM_VDEC_ASSERT(vdecPicture != NULL);
	IM_VDEC_ASSERT(PpInst != NULL);

	if(bPicInfoValid)
	{
		IM_VDEC_INFO("set  PpCfg by PicInfo");
		PpCfg.ppInImg.width = mMp4DecPicture.frameWidth;
		PpCfg.ppInImg.height = mMp4DecPicture.frameHeight;
		if(mVdecConfig.mode.bStandAloneMode)
		{
			PpCfg.ppInImg.bufferBusAddr = mMp4DecPicture.outputPictureBusAddress; 
			PpCfg.ppInImg.bufferCbBusAddr = mMp4DecPicture.outputPictureBusAddress + mMp4DecPicture.frameHeight * mMp4DecPicture.frameWidth; 
			PpCfg.ppInImg.bufferCrBusAddr = PpCfg.ppInImg.bufferCbBusAddr;
		}

		if(mMp4DecPicture.frameWidth != mMp4DecInfo.frameHeight ||
				mMp4DecPicture.frameHeight != mMp4DecInfo.frameHeight)
		{
			if(bVdecInfoValid)
			{
				if(mMp4DecPicture.frameWidth == 0 || mMp4DecPicture.frameHeight == 0)
				{
					PpCfg.ppInImg.width = mMp4DecInfo.frameWidth;
					PpCfg.ppInImg.height = mMp4DecInfo.frameHeight;
				}
			}
		}

		if(mMp4DecPicture.interlaced == 0 ||
			PpCfg.ppOutDeinterlace.enable == 1)
		{
			//PpCfg.ppInImg.height = mMp4DecPicture.frameHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
		}
		else
		{
			if(mMp4DecPicture.fieldPicture != 0)
			{
				PpCfg.ppInImg.height >>= 1;//mMp4DecPicture.frameHeight >> 1;
				if(mMp4DecPicture.topField != 0)
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
				//PpCfg.ppInImg.height = mMp4DecPicture.frameHeight;
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
		mMp4DecInfo.frameHeight = IM_SIZEALIGNED16(mMp4DecInfo.frameHeight);
		PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
		PpCfg.ppInImg.width = mMp4DecInfo.frameWidth;
		PpCfg.ppInImg.height = mMp4DecInfo.frameHeight;
		if(mMp4DecInfo.interlacedSequence == 0){
			PpCfg.ppInImg.height = mMp4DecInfo.frameHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
		}else{
			PpCfg.ppInImg.height = mMp4DecInfo.frameHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_TOP_AND_BOT_FIELD_FRAME;
			PpCfg.ppOutDeinterlace.enable = 1 ;
		}

		PpCfg.ppInImg.videoRange = mMp4DecInfo.videoRange;
	}
	else
	{
		PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
		PpCfg.ppInImg.width = IM_SIZEALIGNED16(mVdecConfig.inType.width);
		PpCfg.ppInImg.height = IM_SIZEALIGNED16(mVdecConfig.inType.height);
	}

	PpCfg.ppOutImg.width = mVdecConfig.outType.width;
	PpCfg.ppOutImg.height = mVdecConfig.outType.height;
	if(PpCfg.ppOutImg.width > 3 * PpCfg.ppInImg.width)
	{
		bOutputChanged = IM_TRUE;
		PpCfg.ppOutImg.width = 3 * (PpCfg.ppInImg.width & ~0xF); 
	}


	if(PpCfg.ppOutImg.height > 3 * PpCfg.ppInImg.height - 2)
	{
		bOutputChanged = IM_TRUE;
		PpCfg.ppOutImg.height = 3 * (PpCfg.ppInImg.height & ~0x1) - 2; 
	}
	return IM_VideoDec::setPpCfg(vdecPicture);
}

