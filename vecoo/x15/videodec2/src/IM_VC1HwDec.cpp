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
 * @file        IM_VC1HwDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#if TARGET_SYSTEM == FS_ANDROID
#define LOG_TAG "VC1HwDec"
#endif

#include "IM_VC1HwDec.h"
#include "IM_VdecLog.h"

#define SHOW1(p) (p[0]); p+=1;
#define SHOW4(p) (p[0]) | (p[1]<<8) | (p[2]<<16) | (p[3]<<24); p+=4;

#define    BIT0(tmp)  ((tmp & 1)   >> 0);
#define    BIT1(tmp)  ((tmp & 2)   >> 1);
#define    BIT2(tmp)  ((tmp & 4)   >> 2);
#define    BIT3(tmp)  ((tmp & 8)   >> 3);
#define    BIT4(tmp)  ((tmp & 16)  >> 4);
#define    BIT5(tmp)  ((tmp & 32)  >> 5);
#define    BIT6(tmp)  ((tmp & 64)  >> 6);
#define    BIT7(tmp)  ((tmp & 128) >> 7);


IM_VC1HwDec::IM_VC1HwDec(IM_VDEC_INTYPE *inType)
{

	memcpy(&mVdecConfig.inType, inType, sizeof(IM_VDEC_INTYPE));

	getDefaultConfig(&mVdecConfig);
	mVC1Build = VC1DecGetBuild(); 
	if(mVdecConfig.build.maxOutWidth > mVC1Build.hwConfig.maxPpOutPicWidth )
	{
		mVdecConfig.build.maxOutWidth  = mVC1Build.hwConfig.maxPpOutPicWidth;
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
	bMetaDataValid = IM_FALSE;
	bPpCombineModeEnable = IM_FALSE;
	bVdecConfigSetted = IM_FALSE;
	bMultiBufferReset = IM_FALSE;
	bFlushed = IM_TRUE;
	bIsWMV3 = IM_FALSE;
	memset(&mVdecIn, 0, sizeof(IM_VDEC_IN));
	memset(&mVdecOut, 0, sizeof(IM_VDEC_OUT));
	memset(&mVdecInfo, 0, sizeof(IM_VDEC_INFO));
	memset(&mVC1DecIn, 0, sizeof(VC1DecInput));
	memset(&mVC1DecOut, 0, sizeof(VC1DecOutput));
	memset(&mVC1DecInfo, 0, sizeof(VC1DecInfo));
	memset(&mVC1DecPicture, 0, sizeof(VC1DecPicture));

	mVC1DecInst = NULL;


	pMultiBuffList = NULL;
	pVC1SpecData = NULL;
	nSkippedPicAfterFlush = 0;
	IM_VDEC_INFO("----");
}

IM_BOOL IM_VC1HwDec::isSupported()
{
	IM_VDEC_INFO("++++");
	if(mVC1Build.hwConfig.vc1Support == VC1_NOT_SUPPORTED)
	{
		return IM_FALSE;
	}

	if(mVdecConfig.inType.codec != IM_STRM_VIDEO_WMV3 && mVdecConfig.inType.codec != IM_STRM_VIDEO_VC1)
	{
		IM_VDEC_ERR("CODEC TYPE NOT SUPPORT, codec = %x", mVdecConfig.inType.codec);
		return IM_FALSE;
	}

	if(mVdecConfig.inType.codec == IM_STRM_VIDEO_WMV3)
	{
		bIsWMV3 = IM_TRUE;
	}

	if(checkInputType(&mVdecConfig.inType) != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("IN SIZE NOT SUPPORT");
		return IM_FALSE;
	}

	
	IM_VDEC_INFO("----");

	return IM_TRUE;
}

IM_RET IM_VC1HwDec::parserMetaData(IM_VDEC_SPEC_DATA *specData)
{
	VC1DecRet vc1Ret;
	IM_VDEC_INFO("++++");
	if(specData == NULL)
	{
		IM_VDEC_ERR("invalid parameters");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	if(specData->size == 0)
	{
		IM_VDEC_ERR("SPEC DATA SIZE IS 0!");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	mVC1DecMetaData.maxCodedWidth = mVdecConfig.inType.width;
	mVC1DecMetaData.maxCodedHeight = mVdecConfig.inType.height;


	pVC1SpecData = (IM_VDEC_SPEC_DATA *)malloc(specData->size + sizeof(IM_UINT32));
	if(pVC1SpecData == NULL)
	{
		IM_VDEC_ERR("MALLOC VC1 extradata failed");
		return IM_RET_VDEC_MEM_FAIL;
	}
	pVC1SpecData->size = specData->size;

	memcpy(pVC1SpecData->data, specData->data, specData->size);

	if(mVdecConfig.inType.codec == IM_STRM_VIDEO_VC1)
	{
		mVC1DecMetaData.profile = 8;
		return IM_RET_VDEC_OK;
	}


	vc1Ret = VC1DecUnpackMetaData(pVC1SpecData->data, pVC1SpecData->size, &mVC1DecMetaData);	
	if(vc1Ret != VC1DEC_OK)
	{
		IM_VDEC_ERR("VC1DecUnpackMetaData failed");
		return IM_RET_VDEC_FAIL;
	}

	IM_VDEC_INFO("VC1DecMetaData::");
	IM_VDEC_INFO("   ::maxCodedWidth = %u",mVC1DecMetaData.maxCodedWidth);
	IM_VDEC_INFO("   ::maxCodedHeight = %u",mVC1DecMetaData.maxCodedHeight);
	IM_VDEC_INFO("   ::vsTransform = %u",mVC1DecMetaData.vsTransform);
	IM_VDEC_INFO("   ::overlap = %u",mVC1DecMetaData.overlap);
	IM_VDEC_INFO("   ::syncMarker = %u",mVC1DecMetaData.syncMarker);
	IM_VDEC_INFO("   ::quantizer = %u",mVC1DecMetaData.quantizer);
	IM_VDEC_INFO("   ::frameInterp = %u",mVC1DecMetaData.frameInterp);
	IM_VDEC_INFO("   ::maxBframes = %u",mVC1DecMetaData.maxBframes);
	IM_VDEC_INFO("   ::fastUvMc = %u",mVC1DecMetaData.fastUvMc);
	IM_VDEC_INFO("   ::extendedMv = %u",mVC1DecMetaData.extendedMv);
	IM_VDEC_INFO("   ::multiRes = %u",mVC1DecMetaData.multiRes);
	IM_VDEC_INFO("   ::rangeRed = %u",mVC1DecMetaData.rangeRed);
	IM_VDEC_INFO("   ::dquant = %u",mVC1DecMetaData.dquant);
	IM_VDEC_INFO("   ::loopFilter = %u",mVC1DecMetaData.loopFilter);
	IM_VDEC_INFO("   ::profile = %u",mVC1DecMetaData.profile);
	IM_VDEC_INFO("VC1DecMetaData::END");
	
	if(mVC1DecMetaData.profile == 8)
	{
		IM_VDEC_WARN("maybe parser error, wmv3 profile cannot be 8");
		if(!unPackedMetaData(pVC1SpecData, &mVC1DecMetaData))
		{
			IM_VDEC_ERR("IM UnpackMetaData failed");
			return IM_RET_VDEC_FAIL;
		}
	}

	bMetaDataValid = IM_TRUE;

	return IM_RET_VDEC_OK;
}

IM_RET IM_VC1HwDec::Init(IM_VDEC_SPEC_DATA *specData,IM_REF_FRM_FORMAT refFrmFormat)
{
	IM_RET ret;
	VC1DecRet  vc1Ret;
	PPResult   ppResult;
	IM_VDEC_INFO("++++");

	if(IM_RET_VDEC_OK != parserMetaData(specData))
	{
		IM_VDEC_ERR("parser vc1metadata failed");
		return IM_RET_VDEC_INIT_FAIL;
	}
	
#ifndef VDEC_LIB_G1
	vc1Ret = VC1DecInit(&mVC1DecInst, &mVC1DecMetaData, mUseVideoFreezeConcealment, nExtraFrameBuffers);
#else
	vc1Ret = VC1DecInit(&mVC1DecInst, &mVC1DecMetaData, mUseVideoFreezeConcealment, nExtraFrameBuffers, (DecRefFrmFormat)refFrmFormat);
#endif

	if(vc1Ret != VC1DEC_OK){
		IM_VDEC_ERR("VC1DecInit Failed error = %d", vc1Ret);
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
			if((ppResult = PPDecCombinedModeEnable(PpInst, mVC1DecInst, PP_PIPELINED_DEC_TYPE_VC1)) != PP_OK)
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
	VC1DecMMUEnable(mVC1DecInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	if(PpInst != NULL)
	{
		PPDecMMUEnable(PpInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
	}
#endif 	

	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}


IM_RET IM_VC1HwDec::Decode(const IM_VDEC_IN *vdecIn, IM_VDEC_OUT *vdecOut, IM_VDEC_PICTURE *vdecPicture)
{
	VC1DecRet vc1Ret; 
	PPResult ppResult;
	IM_RET ret;
	IM_VDEC_INFO("++++");
	IM_VDEC_ASSERT(mVC1DecInst != NULL);

	if(vdecIn == NULL || vdecOut == NULL || vdecPicture == NULL)
	{
		IM_VDEC_ERR("IM_RET_VDEC_INVALID_PARAMETERS");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	vdecOut->dataLeft = vdecIn->dataLen;
	if(!bHeaderReady && bIsWMV3)
	{
		return handleHeaderInfo();
	}

	//check config
	if(bHeaderReady && mVdecConfig.mode.bCombineMode)
	{
		if(!bPpCombineModeEnable)
		{
#if 0
			return IM_RET_VDEC_NOT_INITIALIZED;
#else
			if((ppResult = PPDecCombinedModeEnable(PpInst, mVC1DecInst, PP_PIPELINED_DEC_TYPE_VC1)) != PP_OK)
			{
				IM_VDEC_ERR("PPDecCombinedModeEnable Failed, ppResult = %d",ppResult);
				return IM_RET_VDEC_NOT_INITIALIZED;
			}
			bPpCombineModeEnable = IM_TRUE;
#endif
		}

		if(mVdecConfig.mode.bMultiBuffMode)
		{
			IM_RET ret = checkMultiBuffer(mVC1DecInfo.multiBuffPpSize);
			if(ret == IM_RET_VDEC_LACK_OUTBUFF)                                                                                                           
			{
				vdecOut->multiBuffPpSize = mVC1DecInfo.multiBuffPpSize;                                                                                  
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
	mVC1DecIn.pStream = (const u8 *)vdecIn->pStream;
	mVC1DecIn.streamBusAddress = vdecIn->busAddr;
	mVC1DecIn.streamSize = vdecIn->dataLen;
	mVC1DecIn.skipNonReference = (vdecIn->flags & IM_VDEC_PIC_SKIP) ? 1 : 0;
	mVC1DecIn.picId = 0;
	vdecOut->skipPicNum = 0;
	mDirectShow = (vdecIn->flags & IM_VDEC_PIC_DIRECT_SHOW) ? 1 : 0;

	vc1Ret = VC1DecDecode(mVC1DecInst, &mVC1DecIn, &mVC1DecOut);

	//result process
	switch(vc1Ret)
	{
		case VC1DEC_OK:
			ret = IM_RET_VDEC_NO_PIC_OUT;
			break;
		case VC1DEC_END_OF_SEQ:
			IM_VDEC_INFO("VC1DEC_END_OF_SEQ");
		case VC1DEC_STRM_PROCESSED:
			IM_VDEC_INFO("VC1DEC_STRM_PROCESSED");
			ret = IM_RET_VDEC_STRM_PROCESSED;
			break;
		case VC1DEC_NONREF_PIC_SKIPPED:
			IM_VDEC_INFO("VC1DEC_NONREF_PIC_SKIPPED");
			vdecOut->skipPicNum = 1;
			ret = IM_RET_VDEC_PIC_SKIPPED;
			break;
		case VC1DEC_HDRS_RDY:
		case VC1DEC_RESOLUTION_CHANGED:
			ret = handleHeaderInfo();
			break;
		case VC1DEC_PIC_DECODED:	
		{
			IM_VDEC_INFO("VC1DEC_PIC_DECODED");
			ret = getPicture(vdecPicture, 0);
#ifndef MOSAIC_PIC_OUT
			while(ret == IM_RET_VDEC_MOSAIC_PIC_OUT)
			{
				vdecOut->skipPicNum++;
				ret = getPicture(vdecPicture, 0);
			}

			if(vdecOut->skipPicNum)
			{
				IM_VDEC_INFO("skipPicNum = %d", vdecOut->skipPicNum);
				ret = IM_RET_VDEC_PIC_SKIPPED;
			}
#endif
		}
		break;
		case VC1DEC_MEMFAIL:
			ret = IM_RET_VDEC_MEM_FAIL;
			break;
		case VC1DEC_STRM_ERROR:
		default:
			IM_VDEC_ERR("VC1 decode error vc1Ret = %d", vc1Ret);	
			mVC1DecOut.dataLeft = 0;
			ret = IM_RET_VDEC_FAIL;
	};

	vdecOut->pCurStream = (void *)mVC1DecOut.pStreamCurrPos;
	vdecOut->strmCurrBusAddress = mVC1DecOut.strmCurrBusAddress;
	vdecOut->dataLeft = mVC1DecOut.dataLeft;
	IM_VDEC_INFO("dataLeft = %d", mVC1DecOut.dataLeft);

	IM_VDEC_INFO("----");
	return ret;

}

IM_RET IM_VC1HwDec::GetNextPicture(IM_VDEC_PICTURE *vdecPicture)
{
	IM_INT32 skip = 0;
	
	if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("set Pp config failed");
		skip = 1;
	}
	return getPicture(vdecPicture, skip);
}

IM_RET IM_VC1HwDec::Flush()
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

IM_RET IM_VC1HwDec::DeInit()
{	
	if(pVC1SpecData)
	{
		free(pVC1SpecData);
		pVC1SpecData = NULL;
	}
	if(pMultiBuffList)
	{
		delete pMultiBuffList;
		pMultiBuffList = NULL;
	}

	if(PpInst)
	{
		if(bPpCombineModeEnable)
		{
			if(PP_OK != PPDecCombinedModeDisable(PpInst, mVC1DecInst))
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

	if(mVC1DecInst)
	{
		VC1DecRelease(mVC1DecInst);
	}
	
	return IM_RET_VDEC_OK;
}

IM_RET IM_VC1HwDec::handleHeaderInfo()
{
	IM_RET ret = IM_RET_VDEC_HDRS_RDY;
	
	if(VC1DecGetInfo(mVC1DecInst, &mVC1DecInfo) != VC1DEC_OK)
	{
		IM_VDEC_ERR("VC1DecGetInfo failed");
		return IM_RET_VDEC_FAIL;
	}

	if(bHeaderReady)
	{
		if(mVdecInfo.codedWidth != mVC1DecInfo.codedWidth 
				|| mVdecInfo.codedHeight != mVC1DecInfo.codedHeight
				|| mVdecInfo.multiBuffPpSize != mVC1DecInfo.multiBuffPpSize
				|| mVdecInfo.interlacedSequence != mVC1DecInfo.interlacedSequence)
		{
			IM_VDEC_WARN("VC1DEC_RESOLUTION_CHANGED");
			ret = IM_RET_VDEC_DECINFO_CHANGED;	
		}
	}

	bVdecInfoValid = IM_TRUE;
	bPicInfoValid = IM_FALSE;
	bRunTimeValid = IM_FALSE;

	if(mVC1DecInfo.codedWidth == 0 || mVC1DecInfo.codedHeight == 0)
	{
		if(!bNoInputSize)
		{
			mVC1DecInfo.codedWidth = mVdecConfig.inType.width;
			mVC1DecInfo.codedHeight = mVdecConfig.inType.height;
			mVC1DecInfo.maxCodedWidth = mVdecConfig.inType.width;
			mVC1DecInfo.maxCodedHeight = mVdecConfig.inType.height;
		}
		else
		{
			bVdecInfoValid = IM_FALSE;
		}
	}

	IM_VDEC_INFO(" %s Header Ready::", bIsWMV3 ? "WMV3" : "VC1");
	IM_VDEC_INFO("    ::maxCodedWidth = %d", mVC1DecInfo.maxCodedWidth);
	IM_VDEC_INFO("    ::maxCodedHeight = %d", mVC1DecInfo.maxCodedHeight);
	IM_VDEC_INFO("    ::codedWidth = %d", mVC1DecInfo.codedWidth);
	IM_VDEC_INFO("    ::codedHeight = %d", mVC1DecInfo.codedHeight);
	IM_VDEC_INFO("    ::parWidth = %d", mVC1DecInfo.parWidth);
	IM_VDEC_INFO("    ::parHeight = %d", mVC1DecInfo.parHeight);

	IM_VDEC_INFO("    ::interlaced = %d", mVC1DecInfo.interlacedSequence);
	IM_VDEC_INFO("    ::multiBuffPpSize= %d", mVC1DecInfo.multiBuffPpSize);

	mVdecInfo.frameWidth = IM_SIZEALIGNED16(mVC1DecInfo.maxCodedWidth);
	mVdecInfo.frameHeight = IM_SIZEALIGNED16(mVC1DecInfo.maxCodedHeight);
	mVdecInfo.codedWidth = mVC1DecInfo.codedWidth;
	mVdecInfo.codedHeight = mVC1DecInfo.codedHeight;
	mVdecInfo.interlacedSequence = mVC1DecInfo.interlacedSequence;
	mVdecInfo.multiBuffPpSize = mVC1DecInfo.multiBuffPpSize;
	bHeaderReady = IM_TRUE;

	IM_VDEC_PICTURE vdecPicture; 

	mDirectShow = 1;
	getPicture(&vdecPicture, 1);
	mDirectShow = 0;

	return ret;
}

IM_RET IM_VC1HwDec::getPicture(IM_VDEC_PICTURE *vdecPicture, IM_INT32 skip)
{
	IM_RET ret;
	VC1DecRet vc1Ret;
	PPResult ppResult;
	IM_INT32 skipNum = 0;
	IM_VDEC_ASSERT(vdecPicture != NULL);

	IM_VDEC_INFO("++++");
	
	do{
		vc1Ret = VC1DecNextPicture(mVC1DecInst, &mVC1DecPicture, mDirectShow);
		if(skip && vc1Ret == VC1DEC_PIC_RDY){
			skipNum++;
		}
	}while(skip && vc1Ret == VC1DEC_PIC_RDY);

	if(skip)
	{
		vdecPicture->skipNum = skipNum;
		return IM_RET_VDEC_FAIL;
	}

	
	if(vc1Ret == VC1DEC_PIC_RDY)
	{
		IM_VDEC_INFO("VC1DecPic::frameWidth = %d", mVC1DecPicture.frameWidth);
		IM_VDEC_INFO("VC1DecPic::frameHeight = %d", mVC1DecPicture.frameHeight);
		IM_VDEC_INFO("VC1DecPic::codedWidth = %d", mVC1DecPicture.codedWidth);
		IM_VDEC_INFO("VC1DecPic::codedHeight = %d", mVC1DecPicture.codedHeight);
		IM_VDEC_INFO("VC1DecPic::keyPicture = %d", mVC1DecPicture.keyPicture);
		IM_VDEC_INFO("VC1DecPic::picId = %d", mVC1DecPicture.picId);
		IM_VDEC_INFO("VC1DecPic::interlaced = %d", mVC1DecPicture.interlaced);
		IM_VDEC_INFO("VC1DecPic::fieldPicture = %d", mVC1DecPicture.fieldPicture);
		IM_VDEC_INFO("VC1DecPic::topField = %d", mVC1DecPicture.topField);

		if(mVC1DecPicture.frameWidth == 0 || mVC1DecPicture.frameHeight  == 0)
		{
			bPicInfoValid = IM_FALSE;
			bRunTimeValid = IM_TRUE;
			IM_VDEC_WARN("VC1DEC_PIC_RDY, but out size is zero");
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
			vdecMemCpy(vdecPicture->buffer.virtAddr, (void*)mVC1DecPicture.pOutputPicture, nDecodedPicSize);
		}

		nCropWidth = (mVC1DecPicture.frameWidth - mVC1DecInfo.codedWidth) *  (float)mVdecConfig.outType.width/ mVC1DecInfo.codedWidth;
		nCropHeight = (mVC1DecPicture.frameHeight - mVC1DecInfo.codedHeight) * (float)mVdecConfig.outType.height / mVC1DecInfo.codedHeight;
		vdecPicture->dataLen = nDecodedPicSize;
		vdecPicture->keyPicture = mVC1DecPicture.keyPicture;
		vdecPicture->interlaced = mVC1DecPicture.interlaced;
		
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
			if(!mVC1DecPicture.keyPicture && nSkippedPicAfterFlush < MAX_SKIP_PIC_NUM)
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
		IM_VDEC_INFO("vc1Ret = %d", vc1Ret);
		bPicInfoValid = IM_FALSE;
		bRunTimeValid = IM_TRUE;
		mDirectShow = 0;
		return  IM_RET_VDEC_NO_PIC_OUT;
	}
}


IM_RET IM_VC1HwDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
	IM_VDEC_ASSERT(vdecPicture != NULL);
	IM_VDEC_ASSERT(PpInst != NULL);

	if(bPicInfoValid)
	{
		IM_VDEC_INFO("set  PpCfg by PicInfo");
		PpCfg.ppInImg.width = IM_SIZEALIGNED8(mVC1DecPicture.codedWidth);
		PpCfg.ppInImg.height = IM_SIZEALIGNED8(mVC1DecPicture.codedHeight);
		if(mVdecConfig.mode.bStandAloneMode)
		{
			PpCfg.ppInImg.width = mVC1DecPicture.frameWidth;
			PpCfg.ppInImg.height = mVC1DecPicture.frameHeight;
			PpCfg.ppInImg.bufferBusAddr = mVC1DecPicture.outputPictureBusAddress; 
			PpCfg.ppInImg.bufferCbBusAddr = mVC1DecPicture.outputPictureBusAddress + mVC1DecPicture.frameHeight * mVC1DecPicture.frameWidth; 
			PpCfg.ppInImg.bufferCrBusAddr = PpCfg.ppInImg.bufferCbBusAddr;
		}


		if(mVC1DecPicture.interlaced == 0 ||
			PpCfg.ppOutDeinterlace.enable == 1)
		{
			//PpCfg.ppInImg.height = mVC1DecPicture.frameHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
		}
		else
		{
			if(mVC1DecPicture.fieldPicture != 0)
			{
				PpCfg.ppInImg.height = IM_SIZEALIGNED8(mVC1DecPicture.codedHeight) >> 1;
				if(mVC1DecPicture.topField != 0)
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
				//PpCfg.ppInImg.height = mVC1DecPicture.frameHeight;
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
		PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
		PpCfg.ppInImg.width = IM_SIZEALIGNED8(mVC1DecInfo.maxCodedWidth);
		PpCfg.ppInImg.height = IM_SIZEALIGNED8(mVC1DecInfo.maxCodedHeight);
		if(mVC1DecInfo.interlacedSequence == 0){
			//PpCfg.ppInImg.height = mVC1DecInfo.maxCodedHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
		}else{
			//PpCfg.ppInImg.height = mVC1DecInfo.maxCodedHeight;
			PpCfg.ppInImg.picStruct = PP_PIC_TOP_AND_BOT_FIELD_FRAME;
			PpCfg.ppOutDeinterlace.enable = 1 ;
		}
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

IM_BOOL IM_VC1HwDec::unPackedMetaData(IM_VDEC_SPEC_DATA *pSpecData, VC1DecMetaData* pMetaData)
{
	IM_UINT8 *data; 
	IM_UINT32 tmp1, tmp2, size;

	if(pSpecData == NULL || pMetaData == NULL)
	{
		IM_VDEC_ERR("IM_RET_VDEC_INVALID_PARAMETERS");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}
	
	/* Initialize sequence header C elements */
	pMetaData->loopFilter =
		pMetaData->multiRes =
		pMetaData->fastUvMc =
		pMetaData->extendedMv =
		pMetaData->dquant =
		pMetaData->vsTransform =
		pMetaData->overlap =
		pMetaData->syncMarker =
		pMetaData->rangeRed =
		pMetaData->maxBframes =
		pMetaData->quantizer =
		pMetaData->frameInterp = 0;

	data = pSpecData->data;
	size = pSpecData->size;

	tmp1 = SHOW1(data);
	tmp2  = BIT7(tmp1); tmp2 <<= 1;
	tmp2 |= BIT6(tmp1); tmp2 <<= 1;
	tmp2 |= BIT5(tmp1); tmp2 <<= 1;
	tmp2 |= BIT4(tmp1);

	if (tmp2 == 8){ /* Advanced profile */
		tmp2 = 4;//SET TO MAIN PROFILE 
	}else{
		IM_VDEC_ERR("should not be here");
		return IM_FALSE;
	}
	pMetaData->profile = tmp2;

	tmp1 = SHOW1(data);

	tmp2 = BIT3(tmp1);
	IM_VDEC_INFO("LOOPFILTER:%d",tmp2);
	pMetaData->loopFilter = tmp2;

	tmp2 = BIT2(tmp1);
	IM_VDEC_INFO("Reserved3:%d", tmp2);

	tmp2 = BIT1(tmp1);
	IM_VDEC_INFO("MULTIRES:%d ", tmp2);
	pMetaData->multiRes = tmp2;

	tmp2 = BIT0(tmp1);
	IM_VDEC_INFO("Reserved4:%d\n", tmp2);

	tmp1 = SHOW1(data);
	tmp2 = BIT7(tmp1);
	IM_VDEC_INFO("FASTUVMC:%d",tmp2);
	pMetaData->fastUvMc = tmp2;

	tmp2 = BIT6(tmp1);
	IM_VDEC_INFO("EXTENDED_MV:%d", tmp2);
	pMetaData->extendedMv = tmp2;

	tmp2 = BIT5(tmp1);
	tmp2 <<=1;
	tmp2 |= BIT4(tmp1);
	IM_VDEC_INFO("DQUANT:%d", tmp2);
	pMetaData->dquant = tmp2;
	if(pMetaData->dquant > 2)
		return IM_FALSE;

	tmp2 = BIT3(tmp1);
	pMetaData->vsTransform = tmp2;
	IM_VDEC_INFO("VTRANSFORM:%d",tmp2);

	tmp2 = BIT2(tmp1);
	IM_VDEC_INFO("Reserved5:%d", tmp2);
	/* Reserved5 needs to be checked, it affects stream syntax. */
	if( tmp2 != 0 )
		return IM_FALSE; 

	tmp2 = BIT1(tmp1);
	pMetaData->overlap = tmp2;
	IM_VDEC_INFO("OVERLAP:%d", tmp2);
	tmp2 = BIT0(tmp1);
	pMetaData->syncMarker = tmp2;

	tmp1 = SHOW1(data);
	tmp2 = BIT7(tmp1);
	pMetaData->rangeRed = tmp2;
	tmp2 =  BIT6(tmp1); tmp2 <<=1;
	tmp2 |= BIT5(tmp1); tmp2 <<=1;
	tmp2 |= BIT4(tmp1);
	pMetaData->maxBframes = tmp2;
	tmp2 = BIT3(tmp1); tmp2 <<=1;
	tmp2 |= BIT2(tmp1);
	pMetaData->quantizer = tmp2;
	tmp2 = BIT1(tmp1);
	pMetaData->frameInterp = tmp2;
	tmp2 = BIT0(tmp1);
	/* Reserved6 needs to be checked, it affects stream syntax. */
	if( tmp2 != 1 )
		return IM_FALSE;

	return IM_TRUE;
}
