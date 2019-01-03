/*
 * Copyright (c) 2012, InfoTM Microelectronics Co.,Ltd
 * All rights reseRved.
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
 * @file        IM_RvHwDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#if TARGET_SYSTEM == FS_ANDROID
#define LOG_TAG "RvHwDec"
#endif

#include "IM_RvHwDec.h"
#include "IM_VdecLog.h"
#define  RV_SKIP_PIC_NUM  5//200
IM_RvHwDec::IM_RvHwDec(IM_VDEC_INTYPE *inType)
{
	IM_VDEC_INFO("++++");
	
	memcpy(&mVdecConfig.inType, inType, sizeof(IM_VDEC_INTYPE));

	getDefaultConfig(&mVdecConfig);
	mRvBuild = RvDecGetBuild(); 
	if(mVdecConfig.build.maxOutWidth > mRvBuild.hwConfig.maxPpOutPicWidth )
	{
		mVdecConfig.build.maxOutWidth  = mRvBuild.hwConfig.maxPpOutPicWidth;
	}

	if(mVdecConfig.build.maxOutHeight > 1088 )
	{
		mVdecConfig.build.maxOutWidth  = 1088;
	}

	mUseVideoFreezeConcealment = 1;
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
	memset(&mRvDecIn, 0, sizeof(RvDecInput));
	memset(&mRvDecOut, 0, sizeof(RvDecOutput));
	memset(&mRvDecInfo, 0, sizeof(RvDecInfo));
	memset(&mRvDecPicture, 0, sizeof(RvDecPicture));

	mRvDecInst = NULL;

	pMultiBuffList = NULL;
	nSkippedPicAfterFlush = 0;
	IM_VDEC_INFO("----");
}

IM_BOOL IM_RvHwDec::isSupported()
{
	IM_VDEC_INFO("++++");
	if(mRvBuild.hwConfig.rvSupport == RV_NOT_SUPPORTED)
	{
		return IM_FALSE;
	}

	if(mVdecConfig.inType.codec != IM_STRM_VIDEO_RV30 && mVdecConfig.inType.codec != IM_STRM_VIDEO_RV40)
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

IM_RET parserSpecData(IM_VDEC_SPEC_DATA *specData)
{	
		return IM_RET_VDEC_OK;
}


IM_RET IM_RvHwDec::Init(IM_VDEC_SPEC_DATA *specData,IM_REF_FRM_FORMAT refFrmFormat)
{
	RvDecRet  rvRet;
	PPResult   ppResult;
	IM_VDEC_INFO("++++");
	IM_RV8_MSGS *specdata = (IM_RV8_MSGS *)specData;
	if(specData == NULL)
	{
		IM_VDEC_ERR("rvHwInit specData is NULL");
		return IM_RET_VDEC_INIT_FAIL; 
	}

	if(!bVdecConfigSetted)
	{
		IM_VDEC_ERR("Vdec not config, please set config first");
		return IM_RET_VDEC_INIT_FAIL;
	}

	if(specdata->num_sizes != specData->size)
	{
		IM_VDEC_ERR("spec data size error");
		return IM_RET_VDEC_INIT_FAIL; 
	}
	
	if(mVdecConfig.inType.codec == IM_STRM_VIDEO_RV30)
	{
		mRvVersion = 0;
		//check rvMsgs
		IM_INT32 i = specdata->num_sizes;
		while(i--)
		{
			if(specdata->size[i] != 0)
			{
				break;
			}
		}

		if(i == 0)
		{
			IM_VDEC_ERR("RV MSG CHECK FAILED ");
			return IM_RET_VDEC_INIT_FAIL;
		}
		//check end

		memcpy(&mRvMsgs, specdata, sizeof(IM_RV8_MSGS));
		mFrameCodeLength = 1;
		while(mRvMsgs.num_sizes > (1U << mFrameCodeLength))
			mFrameCodeLength++;

		IM_VDEC_INFO("rvMsgs size = %d, frameCodeLength = %d", mRvMsgs.num_sizes, mFrameCodeLength);

	}
	else if(mVdecConfig.inType.codec == IM_STRM_VIDEO_RV40)
	{
		mRvVersion = 1;
		mFrameCodeLength = 0;
	}

	mMaxFrameWidth = mVdecConfig.inType.width;
	mMaxFrameHeight = mVdecConfig.inType.height;

	
#ifndef VDEC_LIB_G1
	rvRet = RvDecInit(&mRvDecInst, mUseVideoFreezeConcealment, mFrameCodeLength, mRvVersion == 1 ? NULL : mRvMsgs.size, 
			mRvVersion, mMaxFrameWidth, mMaxFrameHeight, nExtraFrameBuffers);
#else
	rvRet = RvDecInit(&mRvDecInst, mUseVideoFreezeConcealment, mFrameCodeLength, mRvVersion == 1 ? NULL : mRvMsgs.size, 
			mRvVersion, mMaxFrameWidth, mMaxFrameHeight, nExtraFrameBuffers, (DecRefFrmFormat)refFrmFormat);
#endif

	if(rvRet != RVDEC_OK){
		IM_VDEC_ERR("RVDecInit Failed error = %d", rvRet);
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
			if((ppResult = PPDecCombinedModeEnable(PpInst, mRvDecInst, PP_PIPELINED_DEC_TYPE_RV)) != PP_OK)
			{
				IM_VDEC_ERR("PPDecCombinedModeEnable Failed, err= %d", ppResult);
				return IM_RET_VDEC_INIT_FAIL;
			}
			bPpCombineModeEnable = IM_TRUE;
		}

		if((ppResult = PPGetConfig(PpInst, &PpCfg)) != PP_OK)
		{
			IM_VDEC_ERR("PP GetConfig Failed,err= %d", ppResult);
			return IM_RET_VDEC_INIT_FAIL;
		}
	}
	
#ifdef VDEC_LIB_G1
	IM_VDEC_INFO("set MMU %s", mVdecConfig.mode.bMMUEnable ? "Enable" : "Disable");
	RvDecMMUEnable(mRvDecInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	if(PpInst != NULL)
	{
		PPDecMMUEnable(PpInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	}
#endif 	

	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_RvHwDec::Decode(const IM_VDEC_IN *vdecIn, IM_VDEC_OUT *vdecOut, IM_VDEC_PICTURE *vdecPicture)
{
	RvDecRet rvRet; 
	PPResult ppResult;
	IM_RET ret;
	IM_VDEC_INFO("++++");
	IM_VDEC_ASSERT(mRvDecInst != NULL);

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
			if((ppResult = PPDecCombinedModeEnable(PpInst, mRvDecInst, PP_PIPELINED_DEC_TYPE_RV)) != PP_OK)
			{
				IM_VDEC_ERR("PPDecCombinedModeEnable Failed, ppResult = %d",ppResult);
				return IM_RET_VDEC_NOT_INITIALIZED;
			}
			bPpCombineModeEnable = IM_TRUE;
#endif
		}

		if(mVdecConfig.mode.bMultiBuffMode)
		{
			IM_RET ret = checkMultiBuffer(mRvDecInfo.multiBuffPpSize);
			if(ret == IM_RET_VDEC_LACK_OUTBUFF)                                                                                                           
			{
				vdecOut->multiBuffPpSize = mRvDecInfo.multiBuffPpSize;                                                                                  
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


	u8 *data; 
	u32 dataLen;
	u32 flagCount;
	u32 flagSize;

	data = (u8 *)vdecIn->pStream;
	dataLen = vdecIn->dataLen;
	flagCount = *data++;
	flagSize = (flagCount + 1) * 8;
	IM_VDEC_INFO("original len = %d, vdec datalen = %d", dataLen, vdecIn->dataLen);
	dataLen -= (flagSize + 8);
	IM_VDEC_INFO("flagCount  = %u, flagSize = %u, dataLen = %u",flagCount, flagSize, dataLen);

	//decode
	mRvDecIn.pStream = (u8 *)vdecIn->pStream + flagSize + 8;
	mRvDecIn.streamBusAddress = vdecIn->busAddr + flagSize + 8;
	IM_VDEC_INFO("pStream = %p, streamBusAddress = 0x%x", mRvDecIn.pStream, mRvDecIn.streamBusAddress );
	mRvDecIn.dataLen = dataLen;
	mRvDecIn.skipNonReference = (vdecIn->flags & IM_VDEC_PIC_SKIP) ? 1 : 0;
	mRvDecIn.picId = vdecIn->picId;
	mRvDecIn.timestamp = 0;
	mRvDecIn.sliceInfoNum = flagCount + 1;


	mRvDecIn.pSliceInfo = mSliceInfo;
	memcpy(mRvDecIn.pSliceInfo, data, flagSize);

	for(IM_UINT32 i = 0; i < mRvDecIn.sliceInfoNum; i++)
	{
		u32 tmp = mSliceInfo[i].isValid;
		mSliceInfo[i].isValid = mSliceInfo[i].offset;
		mSliceInfo[i].offset = tmp;
	}

	vdecOut->skipPicNum = 0;
	mDirectShow = (vdecIn->flags & IM_VDEC_PIC_DIRECT_SHOW) ? 1 : 0;

	IM_VDEC_INFO("SKIP FLAG = %d", mRvDecIn.skipNonReference );
	rvRet = RvDecDecode(mRvDecInst, &mRvDecIn, &mRvDecOut);

	//result process
	switch(rvRet)
	{
		case RVDEC_OK:
			ret = IM_RET_VDEC_NO_PIC_OUT;
			break;
		case RVDEC_STRM_PROCESSED:
			IM_VDEC_INFO("RVDEC_STRM_PROCESSED");
			mRvDecOut.dataLeft = 0;
			ret = IM_RET_VDEC_STRM_PROCESSED;
			break;
		case RVDEC_NONREF_PIC_SKIPPED:
			IM_VDEC_INFO("RVDEC_NONREF_PIC_SKIPPED");
			vdecOut->skipPicNum = 1;
			ret = IM_RET_VDEC_PIC_SKIPPED;
			break;
		case RVDEC_HDRS_RDY:
		{
			RvDecInfo *decInfo = NULL;
			
			if(RvDecGetInfo(mRvDecInst, &mRvDecInfo) != RVDEC_OK)
			{
				IM_VDEC_ERR("RVDecGetInfo failed");
				return IM_RET_VDEC_FAIL;
			}
			ret = IM_RET_VDEC_HDRS_RDY;

			if(bHeaderReady)
			{
				if(mVdecInfo.codedWidth != mRvDecInfo.codedWidth 
						|| mVdecInfo.codedHeight != mRvDecInfo.codedHeight
						|| mVdecInfo.multiBuffPpSize != mRvDecInfo.multiBuffPpSize)
				{
					IM_VDEC_INFO("IM_RET_VDEC_DECINFO_CHANGED");
					ret = IM_RET_VDEC_DECINFO_CHANGED;	
				}
			}

			bVdecInfoValid = IM_TRUE;
			bPicInfoValid = IM_FALSE;
			bRunTimeValid = IM_FALSE;

			if(mRvDecInfo.frameWidth == 0 || mRvDecInfo.frameHeight == 0)
			{
				if(!bNoInputSize)
				{
					mRvDecInfo.frameWidth = IM_SIZEALIGNED16(mVdecConfig.inType.width);
					mRvDecInfo.frameHeight = IM_SIZEALIGNED16(mVdecConfig.inType.height);
				}
				else
				{
					bVdecInfoValid = IM_FALSE;
				}
			}

			IM_VDEC_INFO("rvDec Header Ready::");
			IM_VDEC_INFO("mRvDecInfo::frameWidth = %d", mRvDecInfo.frameWidth);
			IM_VDEC_INFO("mRvDecInfo::frameHeight = %d", mRvDecInfo.frameHeight);
			IM_VDEC_INFO("mRvDecInfo::codedWidth = %d", mRvDecInfo.codedWidth);
			IM_VDEC_INFO("mRvDecInfo::codedHeight = %d", mRvDecInfo.codedHeight);
			IM_VDEC_INFO("mRvDecInfo::multiBuffPpSize= %d", mRvDecInfo.multiBuffPpSize);

			if(mRvDecInfo.frameWidth != IM_SIZEALIGNED16(mRvDecInfo.codedWidth + 15))
			{
				mRvDecInfo.frameWidth = IM_SIZEALIGNED16(mRvDecInfo.codedWidth + 15);
			}

			if(mRvDecInfo.frameHeight != IM_SIZEALIGNED16(mRvDecInfo.codedHeight + 15))
			{
				mRvDecInfo.frameHeight = IM_SIZEALIGNED16(mRvDecInfo.codedHeight + 15);
			}

			mVdecInfo.frameWidth = mRvDecInfo.frameWidth;
			mVdecInfo.frameHeight = mRvDecInfo.frameHeight;
			mVdecInfo.codedWidth = mRvDecInfo.codedWidth;
			mVdecInfo.codedHeight = mRvDecInfo.codedHeight;
			mVdecInfo.interlacedSequence = 0;
			mVdecInfo.multiBuffPpSize = mRvDecInfo.multiBuffPpSize;
			bHeaderReady = IM_TRUE;
			mDirectShow = 1;
			getPicture(vdecPicture, 1);
		}
		break;
		case RVDEC_PIC_DECODED:	
		{
			IM_VDEC_INFO("RVDEC_PIC_DECODED");
			ret = getPicture(vdecPicture, 0);
#ifndef MOSAIC_PIC_OUT
			while(ret == IM_RET_VDEC_MOSAIC_PIC_OUT)
			{
				vdecOut->skipPicNum++;
				ret = getPicture(vdecPicture, 0);
			}

			if(vdecOut->skipPicNum > 0)
			{
				IM_VDEC_INFO("skipPicNum = %d", vdecOut->skipPicNum);
				ret = IM_RET_VDEC_PIC_SKIPPED;
			}
#endif 
		}
		break;
		case RVDEC_MEMFAIL:
			ret = IM_RET_VDEC_MEM_FAIL;
			break;
		case RVDEC_STREAM_NOT_SUPPORTED:
			ret = IM_RET_VDEC_UNSUPPORT_STREAM;
			break;
		case RVDEC_STRM_ERROR:
		default:
			IM_VDEC_ERR("Rv decode error rvRet = %d", rvRet);	
			ret = IM_RET_VDEC_FAIL;
	};

	vdecOut->pCurStream = (void *)mRvDecOut.pStrmCurrPos;
	vdecOut->strmCurrBusAddress = mRvDecOut.strmCurrBusAddress;
	if(ret != IM_RET_VDEC_HDRS_RDY)
	{
		vdecOut->dataLeft = 0;//mRvDecOut.dataLeft;
	}

	IM_VDEC_INFO("dataLeft = %d", mRvDecOut.dataLeft);

	IM_VDEC_INFO("----");
	return ret;

}

IM_RET IM_RvHwDec::GetNextPicture(IM_VDEC_PICTURE *vdecPicture)
{
	IM_INT32 skip = 0;
	
	if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("set Pp config failed");
		skip = 1;
	}
	return getPicture(vdecPicture, skip);
}

IM_RET IM_RvHwDec::Flush()
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

    DeInit();
    IM_VDEC_SPEC_DATA specData;
    memcpy(&specData, &mRvMsgs, sizeof(IM_VDEC_SPEC_DATA));
    Init(&specData, (IM_REF_FRM_FORMAT)0);
	bPicInfoValid = IM_FALSE;
	bRunTimeValid = IM_FALSE;
	bFlushed = IM_TRUE;
	
	return IM_RET_VDEC_OK;
}

IM_RET IM_RvHwDec::DeInit()
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
			if(PP_OK != PPDecCombinedModeDisable(PpInst, mRvDecInst))
			{
				IM_VDEC_ERR("PPDecCombinedModeDisable failed");
			}
			else
			{
				bPpCombineModeEnable = IM_FALSE;
			}

			PPRelease(PpInst);
            PpInst = NULL;
		}
	}

	if(mRvDecInst)
	{
		RvDecRelease(mRvDecInst);
        mRvDecInst = NULL;
	}
	
	return IM_RET_VDEC_OK;
}


IM_RET IM_RvHwDec::getPicture(IM_VDEC_PICTURE *vdecPicture, IM_INT32 skip)
{
	IM_RET ret;
	RvDecRet rvRet;
	PPResult ppResult;
	IM_INT32 skipNum = 0;
	IM_VDEC_ASSERT(vdecPicture != NULL);

	IM_VDEC_INFO("++++");
	
	do{
		rvRet = RvDecNextPicture(mRvDecInst, &mRvDecPicture, mDirectShow);
		if(skip && rvRet == RVDEC_PIC_RDY){
			skipNum++;
		}
	}while(skip && rvRet == RVDEC_PIC_RDY);

	if(skip)
	{
		bPicInfoValid = IM_FALSE;
		bRunTimeValid = IM_FALSE;

		vdecPicture->skipNum = skipNum;
		return IM_RET_VDEC_FAIL;
	}

	
	if(rvRet == RVDEC_PIC_RDY)
	{
		IM_VDEC_INFO("rvDecPic::frameWidth = %d", mRvDecPicture.frameWidth);
		IM_VDEC_INFO("rvDecPic::frameHeight = %d", mRvDecPicture.frameHeight);
		IM_VDEC_INFO("rvDecPic::codedWidth = %d", mRvDecPicture.codedWidth);
		IM_VDEC_INFO("rvDecPic::codedHeight = %d", mRvDecPicture.codedHeight);
		IM_VDEC_INFO("rvDecPic::keyPicture = %d", mRvDecPicture.keyPicture);
		IM_VDEC_INFO("rvDecPic::picId = %d", mRvDecPicture.picId);

		if(mRvDecPicture.frameWidth == 0 || mRvDecPicture.frameHeight  == 0)
		{
			bPicInfoValid = IM_FALSE;
			bRunTimeValid = IM_TRUE;
			IM_VDEC_WARN("RVDEC_PIC_RDY, but out size is zero");
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
				return IM_RET_VDEC_FAIL;//getPicture(vdecPicture, 1);
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
					return IM_RET_VDEC_FAIL;//getPicture(vdecPicture, 1);
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
			vdecMemCpy(vdecPicture->buffer.virtAddr, (void*)mRvDecPicture.pOutputPicture, nDecodedPicSize);
		}

		nCropWidth = (mRvDecPicture.frameWidth - mRvDecInfo.codedWidth) *  (float)mVdecConfig.outType.width/ mRvDecInfo.codedWidth;
		nCropHeight = (mRvDecPicture.frameHeight - mRvDecInfo.codedHeight) * (float)mVdecConfig.outType.height / mRvDecInfo.codedHeight;
		vdecPicture->dataLen = nDecodedPicSize;
		vdecPicture->keyPicture = mRvDecPicture.keyPicture;
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
			if(!mRvDecPicture.keyPicture && nSkippedPicAfterFlush < RV_SKIP_PIC_NUM)
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
		IM_VDEC_INFO("rvRet = %d", rvRet);
		bPicInfoValid = IM_FALSE;
		bRunTimeValid = IM_TRUE;
		mDirectShow = 0;
		return  IM_RET_VDEC_NO_PIC_OUT;
	}
}

IM_RET IM_RvHwDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
	IM_VDEC_ASSERT(vdecPicture != NULL);
	IM_VDEC_ASSERT(PpInst != NULL);

	if(bPicInfoValid)
	{
		IM_VDEC_INFO("set  PpCfg by PicInfo");
		IM_UINT32 tmpWidth;
		IM_UINT32 tmpHeight;
		tmpWidth = IM_SIZEALIGNED16(mRvDecPicture.frameWidth);
		tmpHeight = IM_SIZEALIGNED16(mRvDecPicture.frameHeight);

		if(tmpWidth != IM_SIZEALIGNED16(mRvDecInfo.codedWidth) ||
				tmpHeight != IM_SIZEALIGNED16(mRvDecInfo.codedHeight))
		{
			if(bVdecInfoValid)
			{
				if(tmpWidth == 0 || tmpHeight == 0)
				{
					tmpWidth = IM_SIZEALIGNED16(mRvDecInfo.codedWidth);
					tmpHeight = IM_SIZEALIGNED16(mRvDecInfo.codedHeight);
				}
			}
		}

		PpCfg.ppInImg.width = tmpWidth;
		PpCfg.ppInImg.height = tmpHeight;
		if(mVdecConfig.mode.bStandAloneMode)
		{
			PpCfg.ppInImg.bufferBusAddr = mRvDecPicture.outputPictureBusAddress; 
			PpCfg.ppInImg.bufferCbBusAddr = mRvDecPicture.outputPictureBusAddress + mRvDecPicture.frameHeight * mRvDecPicture.frameWidth; 
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
		PpCfg.ppInImg.width = IM_SIZEALIGNED16(mRvDecInfo.codedWidth);
		PpCfg.ppInImg.height = IM_SIZEALIGNED16(mRvDecInfo.codedHeight);
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
