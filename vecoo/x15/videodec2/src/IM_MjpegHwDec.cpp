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
 * @file        IM_MjpegHwDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#if TARGET_SYSTEM == FS_ANDROID
#define LOG_TAG "MjpegHwDec"
#endif

#include "IM_MjpegHwDec.h"
#include "IM_VdecLog.h"
#define JPEG_SLICE_LIMIT_WIDTH		1280
#define JPEG_SLICE_LIMIT_HEIGHT		720
#define JPEG_SLICE_MBS_COUNT		15

IM_MjpegHwDec::IM_MjpegHwDec(IM_VDEC_INTYPE *inType)
{
	IM_VDEC_INFO("++++");

	memcpy(&mVdecConfig.inType, inType, sizeof(IM_VDEC_INTYPE));

	getDefaultConfig(&mVdecConfig);
	mJpegBuild = JpegDecGetBuild(); 

	//jpeg mode
	mVdecConfig.build.bMultiBuffModeSupport = IM_FALSE;
	mVdecConfig.build.bStandAloneModeSupport = IM_FALSE;
	mVdecConfig.mode.bMultiBuffMode = IM_FALSE;
	mVdecConfig.mode.bStandAloneMode = IM_FALSE;
	if(mVdecConfig.build.maxOutWidth > mJpegBuild.hwConfig.maxPpOutPicWidth )
	{
		mVdecConfig.build.maxOutWidth  = mJpegBuild.hwConfig.maxPpOutPicWidth;
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
	bFlushed = IM_FALSE;
	memset(&mVdecIn, 0, sizeof(IM_VDEC_IN));
	memset(&mVdecOut, 0, sizeof(IM_VDEC_OUT));
	memset(&mVdecInfo, 0, sizeof(IM_VDEC_INFO));
	memset(&mJpegDecIn, 0, sizeof(JpegDecInput));
	memset(&mJpegDecOut, 0, sizeof(JpegDecOutput));
	memset(&mJpegDecInfo, 0, sizeof(JpegDecImageInfo));

	mJpegDecInst = NULL;
	PpInst = NULL;

	pMultiBuffList = NULL;
	nSkippedPicAfterFlush = 0;
	IM_VDEC_INFO("----");
}

IM_BOOL IM_MjpegHwDec::isSupported()
{
	IM_VDEC_INFO("++++");
	if(mJpegBuild.hwConfig.jpegESupport == JPEG_NOT_SUPPORTED)
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

IM_RET IM_MjpegHwDec::Init(IM_VDEC_SPEC_DATA *specData,IM_REF_FRM_FORMAT refFrmFormat)
{
	JpegDecRet  jpegRet;
	PPResult   ppResult;
	IM_VDEC_INFO("++++");

	if(!bVdecConfigSetted)
	{
		IM_VDEC_ERR("VDEC CONFIG NOT SET, please set config first");
		return IM_RET_VDEC_INIT_FAIL;
	}

	jpegRet = JpegDecInit(&mJpegDecInst);

	if(jpegRet != JPEGDEC_OK){
		IM_VDEC_ERR("JPEGDecInit Failed error = %d", jpegRet);
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
			if((ppResult = PPDecCombinedModeEnable(PpInst, mJpegDecInst, PP_PIPELINED_DEC_TYPE_JPEG)) != PP_OK)
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
	JpegDecMMUEnable(mJpegDecInst, mVdecConfig.mode.bMMUEnable);
	if(PpInst != NULL)
	{
		PPDecMMUEnable(PpInst, mVdecConfig.mode.bMMUEnable);
	}
#endif 	
	bInited = IM_TRUE;
	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_MjpegHwDec::Decode(const IM_VDEC_IN *vdecIn, IM_VDEC_OUT *vdecOut, IM_VDEC_PICTURE *vdecPicture)
{
	JpegDecRet jpegRet; 
	PPResult ppResult;
	IM_RET ret = IM_RET_VDEC_OK;
	IM_VDEC_INFO("++++");

	if(vdecIn == NULL || vdecOut == NULL || vdecPicture == NULL)
	{
		IM_VDEC_ERR("IM_RET_VDEC_INVALID_PARAMETERS");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	if(!bInited)
	{
		Init(NULL);
	}

	//decode
	mJpegDecIn.streamBuffer.pVirtualAddress  = (u32 *)vdecIn->pStream;
	mJpegDecIn.streamBuffer.busAddress = vdecIn->busAddr;
	IM_VDEC_INFO("STREAM IN data %p, busaddr=0x%x, dataLen = %d", vdecIn->pStream, vdecIn->busAddr, vdecIn->dataLen);
	mJpegDecIn.streamLength  = vdecIn->dataLen;
	mJpegDecIn.decImageType = 0;

	vdecOut->skipPicNum = 0;

	jpegRet = JpegDecGetImageInfo(mJpegDecInst, &mJpegDecIn, &mJpegDecInfo);

	if(jpegRet != JPEGDEC_OK)
	{
		IM_VDEC_ERR("Mjpeg get Image Info failed");
		return IM_RET_VDEC_FAIL;
	}

	IM_VDEC_ASSERT(mJpegDecInst != NULL);
	if((mJpegDecInfo.codingMode & JPEGDEC_PROGRESSIVE) || (mJpegDecInfo.codingMode & JPEGDEC_NONINTERLEAVED))
	{
		IM_VDEC_ERR("HW jpeg decoder not support  %s stream!!!", (mJpegDecInfo.codingMode & JPEGDEC_PROGRESSIVE) ? "progressive" : "non interleaved");
		return IM_RET_VDEC_UNSUPPORT_STREAM; 
	}

	IM_VDEC_INFO("Jpeg Info::");
	IM_VDEC_INFO("   displayWidth = %d", mJpegDecInfo.displayWidth);
	IM_VDEC_INFO("   displayHeight = %d", mJpegDecInfo.displayHeight);
	IM_VDEC_INFO("   outputWidth = %d", mJpegDecInfo.outputWidth);
	IM_VDEC_INFO("   outputHeight = %d", mJpegDecInfo.outputHeight);
	IM_VDEC_INFO("   version = %d", mJpegDecInfo.version);
	IM_VDEC_INFO("   units = %d", mJpegDecInfo.units);
	IM_VDEC_INFO("   xDensity = %d", mJpegDecInfo.xDensity);
	IM_VDEC_INFO("   yDensity = %d", mJpegDecInfo.yDensity);
	IM_VDEC_INFO("   outputFormat = %x", mJpegDecInfo.outputFormat);
	IM_VDEC_INFO("   codingMode = %d", mJpegDecInfo.codingMode);
	IM_VDEC_INFO("   thumbnailType = %d", mJpegDecInfo.thumbnailType);
	IM_VDEC_INFO("   displayHeightThumb = %d", mJpegDecInfo.displayHeightThumb);
	IM_VDEC_INFO("   outputWidthThumb = %d", mJpegDecInfo.outputWidthThumb);
	IM_VDEC_INFO("   outputHeightThumb = %d", mJpegDecInfo.outputHeightThumb);
	IM_VDEC_INFO("   outputFormatThumb = %d", mJpegDecInfo.outputFormatThumb);
	IM_VDEC_INFO("   codingModeThumb = %d", mJpegDecInfo.codingModeThumb);
	IM_VDEC_INFO("Jpeg Info:: END");
	bHeaderReady = IM_TRUE;
	bVdecInfoValid = IM_TRUE;

	mVdecInfo.frameWidth = mJpegDecInfo.outputWidth;
	mVdecInfo.frameHeight = mJpegDecInfo.outputHeight;
	mVdecInfo.codedWidth = mJpegDecInfo.displayWidth;
	mVdecInfo.codedHeight = mJpegDecInfo.displayHeight;
	
	/* check whether jpeg decode slice mode is neccessary */
	if((mJpegDecInfo.outputWidth > JPEG_SLICE_LIMIT_WIDTH) || \
			(mJpegDecInfo.outputHeight > JPEG_SLICE_LIMIT_HEIGHT)){
		mJpegDecIn.sliceMbSet  = JPEG_SLICE_MBS_COUNT;
	}

	if(bHeaderReady && mVdecConfig.mode.bCombineMode)
	{
		//setPpConfig
		if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
		{
			IM_VDEC_ERR("setPpCfg failed");
			return IM_RET_VDEC_FAIL;
		}
	}


	do{
		jpegRet = JpegDecDecode(mJpegDecInst, &mJpegDecIn, &mJpegDecOut);
		switch(jpegRet)
		{
			case JPEGDEC_FRAME_READY:
				IM_VDEC_INFO("JPEGDEC_FRAME_READY");
				if(PPGetResult(PpInst) != PP_OK)
				{
					IM_VDEC_ERR("PpGetResult() error\n");
					ret = IM_RET_VDEC_FAIL;
				}
				ret = IM_RET_VDEC_MORE_PIC_OUT;
				nCropWidth = (mJpegDecInfo.outputWidth - mJpegDecInfo.displayWidth) \
							 *  (float)mVdecConfig.outType.width/ mJpegDecInfo.displayWidth;
				nCropHeight = (mJpegDecInfo.outputHeight - mJpegDecInfo.displayHeight) \
							  * (float)mVdecConfig.outType.height / mJpegDecInfo.displayHeight;
				vdecPicture->dataLen = nDecodedPicSize;
				vdecPicture->keyPicture = 1;
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
				break;
			case JPEGDEC_SLICE_READY:
				IM_VDEC_INFO("JPEGDEC_SLICE_READY");
				break;
			case JPEGDEC_STRM_PROCESSED:
				IM_VDEC_INFO("JPEGDEC_SLICE_READY");
				jpegRet = JPEGDEC_FRAME_READY;
				ret = IM_RET_VDEC_NO_PIC_OUT;
				break;

			default:
				IM_VDEC_ERR("error happen,err %d", jpegRet);
				jpegRet = JPEGDEC_FRAME_READY;
				ret = IM_RET_VDEC_FAIL;
		}
	}while(jpegRet != JPEGDEC_FRAME_READY);

	//vdecOut->pCurStream = (void *)mJpegDecOut.pStrmCurrPos;
	//vdecOut->strmCurrBusAddress = mJpegDecOut.strmCurrBusAddress;
	vdecOut->dataLeft = 0;
	DeInit();

	IM_VDEC_INFO("----");
	return ret;

}

IM_RET IM_MjpegHwDec::GetNextPicture(IM_VDEC_PICTURE *vdecPicture)
{
	IM_INT32 skip = 0;
	
	return getPicture(vdecPicture, skip);
}

IM_RET IM_MjpegHwDec::Flush()
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

IM_RET IM_MjpegHwDec::DeInit()
{	
	if(!bInited)
	{
		return IM_RET_VDEC_OK;
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
			if(PP_OK != PPDecCombinedModeDisable(PpInst, mJpegDecInst))
			{
				IM_VDEC_ERR("PPDecCombinedModeDisable failed");
			}
			else
			{
				bPpCombineModeEnable = IM_FALSE;
			}
		}
		PPRelease(PpInst);	
		PpInst = NULL;
	}

	if(mJpegDecInst)
	{
		JpegDecRelease(mJpegDecInst);
		mJpegDecInst = NULL;
	}
	bInited = IM_FALSE;
	
	return IM_RET_VDEC_OK;
}


IM_RET IM_MjpegHwDec::getPicture(IM_VDEC_PICTURE *vdecPicture, IM_INT32 skip)
{
	return IM_RET_VDEC_NO_PIC_OUT;
	
}

IM_RET IM_MjpegHwDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
	IM_VDEC_ASSERT(vdecPicture != NULL);
	IM_VDEC_ASSERT(PpInst != NULL);

	if(bVdecInfoValid)
	{
		IM_VDEC_INFO("set  PpCfg by DecInfo");

		PpCfg.ppInImg.width = mJpegDecInfo.outputWidth;
		PpCfg.ppInImg.height = mJpegDecInfo.outputHeight;
		switch(mJpegDecInfo.outputFormat)
		{
			case JPEGDEC_YCbCr400:
				PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_0_0;
				break;

			case JPEGDEC_YCbCr420_SEMIPLANAR:
				PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
				break;

			case JPEGDEC_YCbCr422_SEMIPLANAR:
				PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR;
				break;

			case JPEGDEC_YCbCr440:
				PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_0_0;
				break;

			case JPEGDEC_YCbCr411_SEMIPLANAR:
				PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_1_1_SEMIPLANAR;
				break;

			case JPEGDEC_YCbCr444_SEMIPLANAR:
				PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_4_4_SEMIPLANAR;
				break;

			default:
				IM_VDEC_ERR("unsupported jpeg format %d",mJpegDecInfo.outputFormat);
				return IM_RET_VDEC_FAIL;
		}
	}
	else
	{
		PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
		PpCfg.ppInImg.width = IM_SIZEALIGNED16(mVdecConfig.inType.width);
		PpCfg.ppInImg.height = IM_SIZEALIGNED16(mVdecConfig.inType.height);
	}

	PpCfg.ppInImg.picStruct   = PP_PIC_FRAME_OR_TOP_FIELD;
	PpCfg.ppOutImg.width = mVdecConfig.outType.width;
	PpCfg.ppOutImg.height = mVdecConfig.outType.height;

	return IM_VideoDec::setPpCfg(vdecPicture);
}

