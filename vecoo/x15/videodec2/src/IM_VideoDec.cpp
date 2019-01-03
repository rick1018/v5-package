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
 * @file        IM_VideoDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#include "IM_VideoDec.h"
#include "IM_VdecLog.h"
IM_RET IM_VideoDec::GetConfig(IM_VDEC_CONFIG *vdecConfig)
{
	IM_VDEC_INFO("++++");
	memcpy(vdecConfig, &mVdecConfig, sizeof(IM_VDEC_CONFIG));

	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_VideoDec::SetConfig(IM_VDEC_CONFIG *vdecConfig)
{
	IM_RET ret;
	IM_VDEC_INFO("++++");
	ret = checkConfig(vdecConfig);

	if(ret != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("check config failed ret = %d",ret);
		return ret;
	}

	memcpy(&mVdecConfig, vdecConfig, sizeof(IM_VDEC_CONFIG));

	nDecodedPicSize = mVdecConfig.outType.width * mVdecConfig.outType.height * IM_PIC_BITS(mVdecConfig.outType.format) / 8;
	bVdecConfigSetted = IM_TRUE;
	IM_VDEC_INFO("nDecodedSize = %d", nDecodedPicSize);

	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_VideoDec::GetVdecInfo(IM_VDEC_INFO *vdecInfo)
{
	IM_VDEC_INFO("++++");
	memcpy(vdecInfo, &mVdecInfo, sizeof(IM_VDEC_INFO));
	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_VideoDec::SetMultiBuffers(IM_VDEC_BUFFERS *vdecBuffers)
{
	IM_VDEC_INFO("++++");

	if(!mVdecConfig.mode.bMultiBuffMode)
	{
		IM_VDEC_ERR("NOT MULTIBUFFER MODE");
		return IM_RET_VDEC_FAIL;
	}

	if(mVdecConfig.mode.bMultiBuffMode && pMultiBuffList == NULL)
	{
		IM_VDEC_ERR("DEC IS NOT INTIALIZED");
		return IM_RET_VDEC_NOT_INITIALIZED;
	}

	pMultiBuffList->buildMultiBufferList(vdecBuffers);

	return  setMultiBuffers(pMultiBuffList);

}

//==========================================================================
//==========================================================================

IM_RET IM_VideoDec::getDefaultConfig(IM_VDEC_CONFIG *vdecConfig)
{
	vdecConfig->build.codecType  = HW_G1;
	vdecConfig->build.bStandAloneModeSupport = IM_TRUE;
	vdecConfig->build.bCombineModeSupport = IM_TRUE;
	vdecConfig->build.bMultiBuffModeSupport = IM_TRUE;
	vdecConfig->build.maxOutWidth = IM_SIZEALIGNED16(vdecConfig->inType.width) * 3;
	vdecConfig->build.maxOutHeight = IM_SIZEALIGNED16(vdecConfig->inType.height) * 3 - 2;
#ifdef VDEC_LIB_G1
	vdecConfig->build.bMMUSupported = IM_TRUE;
#else
	vdecConfig->build.bMMUSupported = IM_FALSE;
#endif

	vdecConfig->mode.bStandAloneMode = IM_FALSE;
	vdecConfig->mode.bCombineMode = IM_TRUE;
	vdecConfig->mode.bMultiBuffMode = IM_TRUE;
#ifdef VDEC_LIB_G1
	vdecConfig->mode.bMMUEnable = IM_TRUE;
#else
	vdecConfig->mode.bMMUEnable = IM_FALSE;
#endif
	return IM_RET_VDEC_OK;
}

IM_RET IM_VideoDec::checkConfig(IM_VDEC_CONFIG *vdecConfig)
{
	IM_VDEC_INFO("++++");
	IM_RET ret = IM_RET_VDEC_OK;
	IM_VDEC_INTYPE *inType;
	IM_VDEC_OUTTYPE *outType;
	IM_VDEC_MODE *mode;

	if(vdecConfig == NULL){
		IM_VDEC_ERR("Invalid Params");
		return IM_RET_VDEC_INVALID_PARAMETERS;
	}

	inType = &vdecConfig->inType;
	outType = &vdecConfig->outType;
	mode = &vdecConfig->mode;
	memcpy(&mSuggestConfig, vdecConfig, sizeof(IM_VDEC_CONFIG));

	ret = checkInputType(inType);

	if(ret != IM_RET_VDEC_OK)
	{
		IM_VDEC_ERR("UNSUPPORT IN TYPE");
		return ret;
	}

	ret = checkVdecMode(mode);
	
	ret = checkOutputType(outType);
	
	IM_VDEC_INFO("----");
	return ret;
}

IM_RET IM_VideoDec::checkInputType(IM_VDEC_INTYPE *inType)
{
	if(mVdecConfig.build.codecType == SW_FFMPEG)
	{
		return IM_RET_VDEC_OK;
	}

	if(inType->codec == IM_STRM_VIDEO_H263)
	{
		if(inType->width > 720 || inType->height > 576)
		{
			return IM_RET_VDEC_UNSUPPORT_INPUT_TYPE;
		}
	}
	else if(inType->codec == IM_STRM_VIDEO_MJPEG)
	{
		//do nothing;
	}
	else if(inType->width > 1920 || inType->height > 1088)
	{
		return IM_RET_VDEC_UNSUPPORT_INPUT_TYPE;
	}

	if(inType->width == 0 || inType->height == 0){
		IM_VDEC_WARN("input size is 0");
		bNoInputSize = IM_FALSE;
	}

	return IM_RET_VDEC_OK;
}

IM_RET IM_VideoDec::checkOutputType(IM_VDEC_OUTTYPE *outType)
{
	IM_RET ret = IM_RET_VDEC_OK;
	IM_VDEC_INFO("output type:");
	IM_VDEC_INFO("width = %d", outType->width);
	IM_VDEC_INFO("height = %d",outType->height);
	IM_VDEC_INFO("format  = %d",outType->format);
	IM_VDEC_INFO("rotation = %d", outType->rotation);
	if(!mVdecConfig.build.bCombineModeSupport && !mVdecConfig.build.bStandAloneModeSupport){
		IM_VDEC_WARN("Do not use pp");
		if(outType->width != IM_SIZEALIGNED16(mVdecConfig.inType.width) 
				|| outType->height != IM_SIZEALIGNED16(mVdecConfig.inType.height) 
				|| outType->rotation != IM_VDEC_ROTATION_NONE 
				|| outType->format != IM_PIC_FMT_12BITS_YUV420SP)
		{
			mSuggestConfig.outType.width = IM_SIZEALIGNED16(mVdecConfig.inType.width);
			mSuggestConfig.outType.height = IM_SIZEALIGNED16(mVdecConfig.inType.height);
			mSuggestConfig.outType.rotation = IM_VDEC_ROTATION_NONE;
			mSuggestConfig.outType.format  = IM_PIC_FMT_12BITS_YUV420SP;
			IM_VDEC_INFO("IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE");
			return IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
		}
	}

	if(outType->width < 16)
	{
		IM_VDEC_WARN("IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE");
		mSuggestConfig.outType.width = 16;
		ret = IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
	}
#if 0
	if(outType->width > 1920)
	{
		IM_VDEC_WARN("IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE");
		mSuggestConfig.outType.width = 1920;
		ret = IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
	}
#endif
	if(!bNoInputSize && outType->width > IM_SIZEALIGNED16(mVdecConfig.inType.width) * 3)
	{
		IM_VDEC_WARN("IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE");
		mSuggestConfig.outType.width  = IM_SIZEALIGNED16(mVdecConfig.inType.width) * 3;
		ret = IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
	}
	if(outType->height > 1920)
	{
		IM_VDEC_WARN("out height is not supported by vdec not support ");
		mSuggestConfig.outType.height = 1920;
		ret = IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
	}
	if(outType->height < 16)
	{
		IM_VDEC_WARN("IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE");
		mSuggestConfig.outType.height = 16;
		ret = IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
	}

	if(!bNoInputSize && outType->height > (mVdecConfig.inType.height &(~0x1)) * 3 - 2)
	{
		IM_VDEC_WARN("IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE");
		mSuggestConfig.outType.height  = (mVdecConfig.inType.height & ~0x1) * 3 - 2;
		ret = IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
	}

	if(mSuggestConfig.mode.bCombineMode && outType->rotation != IM_VDEC_ROTATION_NONE)
	{
		IM_VDEC_WARN("IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE");
		mSuggestConfig.outType.rotation = IM_VDEC_ROTATION_NONE;
		ret = IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
	}

	//check out format;

	return ret;
}

IM_RET IM_VideoDec::checkVdecMode(IM_VDEC_MODE *mode)
{
	IM_RET ret = IM_RET_VDEC_OK;
	if(!mVdecConfig.build.bCombineModeSupport && mode->bCombineMode)
	{
		mSuggestConfig.mode.bCombineMode = IM_FALSE;
		ret = IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
	}

	if(!mVdecConfig.build.bMultiBuffModeSupport && mode->bMultiBuffMode)
	{
		mSuggestConfig.mode.bMultiBuffMode = IM_FALSE;
		ret = IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
	}

	if(!mVdecConfig.build.bStandAloneModeSupport && mode->bStandAloneMode)
	{
		mSuggestConfig.mode.bStandAloneMode = IM_FALSE;
		ret = IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE; 
	}
	return ret;
}


IM_RET IM_VideoDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
	IM_VDEC_INFO("++++");
	PPResult ppResult;
	if(!mVdecConfig.mode.bMultiBuffMode)
	{
		PpCfg.ppOutImg.bufferBusAddr = vdecPicture->buffer.busAddr;
		PpCfg.ppOutImg.bufferChromaBusAddr  = 0;
	}	

	IM_VDEC_INFO("out format %u", mVdecConfig.outType.format);
	if(mVdecConfig.outType.format== IM_PIC_FMT_12BITS_YUV420SP)
	{
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;
		if(!mVdecConfig.mode.bMultiBuffMode)
		{
			PpCfg.ppOutImg.bufferChromaBusAddr = 
				PpCfg.ppOutImg.bufferBusAddr  + mVdecConfig.outType.width * mVdecConfig.outType.height;		
		}
	}
	else if(mVdecConfig.outType.format == IM_PIC_FMT_16BITS_YUV422I_YUYV)
	{
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED;
	}
	else if(mVdecConfig.outType.format == IM_PIC_FMT_16BITS_YUV422I_YVYU)
	{
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED;
	}
#if 0
	else if(mVdecConfig.outType.format == IM_PIC_FMT_16BITS_YUV422I_UYUV)
	{
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED;
	}
	else if(mVdecConfig.outType.format == IM_PIC_FMT_16BITS_YUV422I_VYUY)
	{
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED;
	}
#endif
	else if(mVdecConfig.outType.format == IM_PIC_FMT_16BITS_YUV422SP)
	{
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR;
		if(!mVdecConfig.mode.bMultiBuffMode)
		{
			PpCfg.ppOutImg.bufferChromaBusAddr =
				PpCfg.ppOutImg.bufferBusAddr + mVdecConfig.outType.width * mVdecConfig.outType.height;
		}
	}
	else if(mVdecConfig.outType.format	== IM_PIC_FMT_16BITS_RGB_565)
	{
		PpCfg.ppOutRgb.ditheringEnable =1;
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_RGB16_5_6_5;
	}
	else if(mVdecConfig.outType.format == IM_PIC_FMT_16BITS_BGR_565)
	{
		PpCfg.ppOutRgb.ditheringEnable =1;
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_BGR16_5_6_5;
	}
#if 0
	else if(mVdecConfig.outType.format == IM_PIC_FMT_16BITS_RGB_555)
	{
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_RGB16_5_5_5;
	}
	else if(mVdecConfig.outType.format == IM_PIC_FMT_16BITS_BGR_555)
	{
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_BGR16_5_5_5;
	}
	else if(mVdecConfig.outType.format == IM_PIC_FMT_16BITS_CUSTOM)
	{
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_RGB16_CUSTOM;
		PpCfg.ppOutRgb.rgbBitmask.maskB = 0x0F00;
		PpCfg.ppOutRgb.rgbBitmask.maskG = 0x00F0;
		PpCfg.ppOutRgb.rgbBitmask.maskR = 0x000F;
		PpCfg.ppOutRgb.rgbBitmask.maskAlpha = 0xF000;
	}
#endif
	else if(mVdecConfig.outType.format== IM_PIC_FMT_32BITS_0BGR_8888)
	{
		PpCfg.ppOutRgb.ditheringEnable =1;
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_RGB32;
	}
	else if(mVdecConfig.outType.format== IM_PIC_FMT_32BITS_ARGB_8888)
	{		
		PpCfg.ppOutRgb.ditheringEnable =1;
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_BGR32;
	}
	else if(mVdecConfig.outType.format== IM_PIC_FMT_32BITS_0RGB_8888)
	{
		PpCfg.ppOutRgb.ditheringEnable =1;
		PpCfg.ppOutImg.pixFormat = PP_PIX_FMT_RGB32_CUSTOM;
		PpCfg.ppOutRgb.rgbBitmask.maskR = 0xFF000000;
		PpCfg.ppOutRgb.rgbBitmask.maskG = 0x00FF0000;
		PpCfg.ppOutRgb.rgbBitmask.maskB = 0x0000FF00;
		PpCfg.ppOutRgb.rgbBitmask.maskAlpha = 0x000000FF;
	}
	else
	{
		IM_VDEC_ERR("not support format");
		return IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE;                                                           
	}        

	if((ppResult = PPSetConfig(PpInst, &PpCfg)) != PP_OK)
	{
		IM_VDEC_ERR("PPSetConfig() failed! ppResult = %d", ppResult);   
		return IM_RET_VDEC_UNSUPPORT_OUTPUT_TYPE;                                                   
	}          

	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_VideoDec::checkMultiBuffer(IM_UINT32 multiBufferNum)
{
	if(pMultiBuffList == NULL)     
	{
		pMultiBuffList = new IM_MultiBuffer(multiBufferNum);                                                                        
		if(pMultiBuffList == NULL)     
		{
			IM_VDEC_ERR("malloc pMultiBuffList failed");
			return IM_RET_VDEC_MEM_FAIL;                                                                                                          
		}
	}

	if((u32)pMultiBuffList->maxSize() != multiBufferNum)                                                                            
	{
		IM_VDEC_ERR("multibuff num changed, need reconfig");
		delete pMultiBuffList;                                                                                                                    

		pMultiBuffList = new IM_MultiBuffer(multiBufferNum);                                                                        
		if(pMultiBuffList == NULL)     
		{
			IM_VDEC_ERR("malloc pMultiBuffList failed");
			return IM_RET_VDEC_MEM_FAIL;                                                                                                          
		}

		return IM_RET_VDEC_LACK_OUTBUFF;                                                                                                          
	}

	if((u32)pMultiBuffList->curSize() != multiBufferNum)
	{
		IM_VDEC_INFO("multibuffermode need reconfig");
		pMultiBuffList->release();
		return IM_RET_VDEC_LACK_OUTBUFF; 
	}   

	return IM_RET_VDEC_OK;
}

IM_RET IM_VideoDec::setMultiBuffers(IM_MultiBuffer *multiBuffer)
{
	PPOutputBuffers  outBuffers;
	IM_INT32 yLength = 0;
	IM_VDEC_INFO("++++");

	memset(&outBuffers, 0, sizeof(PPOutputBuffers));

	outBuffers.nbrOfBuffers = mVdecInfo.multiBuffPpSize;

	if(mVdecConfig.outType.format == IM_PIC_FMT_12BITS_YUV420SP
			|| mVdecConfig.outType.format == IM_PIC_FMT_16BITS_YUV422SP)
	{
		yLength = mVdecConfig.outType.width *  mVdecConfig.outType.height;
	}

	for(IM_UINT32 i = 0; i < mVdecInfo.multiBuffPpSize; i++)
	{
		MULTIBUFF_ELEM qElem;

		if(multiBuffer->getElembyIndex(i, &qElem) != IM_RET_VDEC_OK)
		{
			IM_VDEC_ERR("getElemby Index failed");
			return IM_RET_VDEC_FAIL;
		}

		outBuffers.ppOutputBuffers[i].bufferBusAddr = qElem.buffer.busAddr;
		outBuffers.ppOutputBuffers[i].bufferChromaBusAddr = qElem.buffer.busAddr + yLength;
	}

	if(PPDecSetMultipleOutput(PpInst, &outBuffers) != PP_OK)
	{
		return IM_RET_VDEC_FAIL;
	}

	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}

IM_RET IM_VideoDec::vdecMemCpy(void *dst, void *src, IM_UINT32 size)
{
	memcpy(dst, src, size);
	return IM_RET_VDEC_OK;
}
