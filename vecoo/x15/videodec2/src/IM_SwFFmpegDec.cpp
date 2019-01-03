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
 * @file        IM_SwFFmpegDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
*/
#if TARGET_SYSTEM == FS_ANDROID
#define LOG_TAG "SwFFmpegDec"
#endif

#include "IM_SwFFmpegDec.h"
#include "IM_VdecLog.h"
#define MULTIBUF_MODE1
IM_SwFFmpegDec::IM_SwFFmpegDec(IM_VDEC_INTYPE *inType)
{
	IM_VDEC_INFO("++++");
	
	memcpy(&mVdecConfig.inType, inType, sizeof(IM_VDEC_INTYPE));

	getDefaultConfig(&mVdecConfig);
	
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
	mAVCodec = NULL;
	mAVCtx = NULL;
	mAVFrame = NULL;
	mSwsCtx = NULL;
	PpInst = NULL;
	mAlcCtx = NULL;
	memset(&tempBuffer, 0, sizeof(IM_Buffer));


	pMultiBuffList = NULL;
	nSkippedPicAfterFlush = 0;
	IM_VDEC_INFO("----");
}

IM_BOOL IM_SwFFmpegDec::isSupported()
{
	return IM_TRUE;
}

IM_RET IM_SwFFmpegDec::Init(IM_VDEC_SPEC_DATA *specData,IM_REF_FRM_FORMAT refFrmFormat)
{
	IM_VDEC_INFO("++++");

	if(!bVdecConfigSetted)
	{
		IM_VDEC_ERR("VDEC CONFIG NOT SET, please set config first");
		return IM_RET_VDEC_INIT_FAIL;
	}

	if(specData == NULL)
	{
		IM_VDEC_ERR("specData is NULL");
		return IM_RET_VDEC_INIT_FAIL;
	}

	av_register_all();
	mAVCtx = avcodec_alloc_context();
	mAVFrame = avcodec_alloc_frame();
	if(mAVCtx == NULL || mAVFrame == NULL)
	{
		IM_VDEC_ERR("malloc contex failed");
		return IM_RET_VDEC_INIT_FAIL;
	}

	avcodec_copy_context(mAVCtx, (AVCodecContext *)specData);

	mAVCodec = avcodec_find_decoder(mAVCtx->codec_id);
	if(mAVCodec == NULL)
	{
		IM_VDEC_ERR("find decoder error");
		return IM_RET_VDEC_INIT_FAIL;
	}

	if(avcodec_open(mAVCtx, mAVCodec) < 0)
	{
		IM_VDEC_ERR("avcodec open error");
		return IM_RET_VDEC_INIT_FAIL;
	}

	if(alc_open(&mAlcCtx, (char *)"SwFFmpegDec") != IM_RET_OK)
	{
		return IM_RET_VDEC_INIT_FAIL;
	}

	IM_INT32 flags = ALC_FLAG_ALIGN_8BYTES;
	IM_UINT32 width = mVdecConfig.inType.width > mVdecConfig.outType.width ? mVdecConfig.inType.width : mVdecConfig.outType.width;
	IM_UINT32 height = mVdecConfig.inType.height > mVdecConfig.outType.height ? mVdecConfig.inType.height : mVdecConfig.outType.height;
	IM_UINT32 size = width * height * 4;
	flags |= mVdecConfig.mode.bMMUEnable ? (ALC_FLAG_DEVADDR | ALC_FLAG_PHY_LINEAR_PREFER) : ALC_FLAG_PHY_MUST;
	if(alc_alloc(mAlcCtx, size, &tempBuffer, flags) != IM_RET_OK)
	{
		IM_VDEC_ERR(" No linear buffer available");
		return IM_RET_VDEC_INIT_FAIL;
	}
#ifdef VDEC_LIB_G1
	if(mVdecConfig.mode.bMMUEnable)
	{
		IM_UINT32 dev_addr;
		if(alc_get_devaddr(mAlcCtx, &tempBuffer, &dev_addr) != IM_RET_OK)
		{
			IM_VDEC_ERR("can not get dev addr");
			return IM_RET_VDEC_INIT_FAIL;
		}

		tempBuffer.phy_addr = dev_addr;
	}
#endif
	IM_VDEC_INFO("----");
	return IM_RET_VDEC_OK;
}


IM_RET IM_SwFFmpegDec::Decode(const IM_VDEC_IN *vdecIn, IM_VDEC_OUT *vdecOut, IM_VDEC_PICTURE *vdecPicture)
{
	IM_RET ret = IM_RET_VDEC_OK;
	IM_INT32 finish = 0;
	AVPacket packet;
	av_init_packet(&packet);

	packet.data = (IM_UINT8 *)vdecIn->pStream;
	packet.size = vdecIn->dataLen;
	packet.flags = AV_PKT_FLAG_KEY;

	vdecOut->dataLeft = vdecIn->dataLen;
#ifdef TIME_DEBUG
	int64_t time1 = GetNowUs();
#endif
	if(avcodec_decode_video2(mAVCtx, mAVFrame, &finish, &packet) < 0)
	{
		vdecOut->dataLeft = 0;
		IM_VDEC_ERR("decode error");
		return IM_RET_VDEC_FAIL;
	}

#ifdef TIME_DEBUG
	int64_t time2 = GetNowUs();
	IM_VDEC_INFO("sw decode time = %lld(us)", time2 - time1);
#endif 
	if(finish)
	{
		IM_VDEC_INFO("AVFrame Info::");
		IM_VDEC_INFO("key_frame = %d",mAVFrame->key_frame);
		IM_VDEC_INFO("format = %d",mAVFrame->format);
		IM_VDEC_INFO("width = %d",mAVFrame->width);
		IM_VDEC_INFO("height = %d",mAVFrame->height);
		if(bFlushed && !mAVFrame->key_frame && nSkippedPicAfterFlush < MAX_SKIP_PIC_NUM)
		{
			nSkippedPicAfterFlush++;
			vdecOut->dataLeft = 0;
			vdecOut->skipPicNum = 1;
			return IM_RET_VDEC_PIC_SKIPPED;
		}

		nSkippedPicAfterFlush = 0;
		bFlushed = IM_FALSE;

		ret = postProcess(vdecPicture);
		vdecPicture->dataLen = nDecodedPicSize;
		vdecPicture->keyPicture = mAVFrame->key_frame;
		vdecPicture->cropWidth = 0;
		vdecPicture->cropHeight = 0;
		ret = IM_RET_VDEC_MORE_PIC_OUT;
	}
	else
	{
		ret = IM_RET_VDEC_STRM_PROCESSED;
	}

	vdecOut->pCurStream = 	NULL;
	vdecOut->strmCurrBusAddress = 0;
	vdecOut->dataLeft = 0;
	

	IM_VDEC_INFO("dataLeft = %d", vdecOut->dataLeft);

	IM_VDEC_INFO("----");
	return ret;

}

IM_RET IM_SwFFmpegDec::GetNextPicture(IM_VDEC_PICTURE *vdecPicture)
{
	return IM_RET_VDEC_OK;
}

IM_RET IM_SwFFmpegDec::Flush()
{
	IM_VDEC_PICTURE vdecPicture;

	bPicInfoValid = IM_FALSE;
	bRunTimeValid = IM_FALSE;
	bFlushed = IM_TRUE;
	
	return IM_RET_VDEC_OK;
}

IM_RET IM_SwFFmpegDec::DeInit()
{	
	if(PpInst)
	{
		PPRelease(PpInst);
	}
	
	if(mAVCtx)
	{
		avcodec_close(mAVCtx);

		if(mSwsCtx)
		{
			sws_freeContext(mSwsCtx);
		}

		if(mAVFrame)
		{
			av_free(mAVFrame);
		}

		if(mAVCtx)
		{

    		av_freep(&mAVCtx->rc_override);
    		av_freep(&mAVCtx->intra_matrix);
    		av_freep(&mAVCtx->inter_matrix);
    		av_freep(&mAVCtx->extradata);
    		av_freep(&mAVCtx->rc_eq);
			av_free(mAVCtx);
		}

	}

	if(tempBuffer.vir_addr)
	{
		alc_free(mAlcCtx, &tempBuffer);
		alc_close(mAlcCtx);
	}
	
	return IM_RET_VDEC_OK;
}

IM_RET IM_SwFFmpegDec::postProcess(IM_VDEC_PICTURE *vdecPicture)
{
	if(mAVCtx->pix_fmt == PIX_FMT_YUV420P || 
			mAVCtx->pix_fmt== PIX_FMT_YUYV422 ||
			mAVCtx->pix_fmt == PIX_FMT_UYVY422)
	{
		IM_VDEC_INFO("can use hw pp");
		if(hwPostProcess(vdecPicture) == IM_RET_VDEC_OK)
		{
			return IM_RET_VDEC_OK;
		}
	}

	return swPostProcess(vdecPicture);
}

IM_RET IM_SwFFmpegDec::swPostProcess(IM_VDEC_PICTURE *vdecPicture)
{
	IM_UINT8 *dst[3];
	IM_INT32 dstStride[3];
	IM_INT32  	outSliceHeight=0;
	IM_INT32    tmpOutSize = 0;
	PixelFormat dstFormat;

	if(mVdecConfig.outType.format == IM_PIC_FMT_12BITS_YUV420SP)
	{
		dstFormat = PIX_FMT_YUV420P;
	}
	else if(mVdecConfig.outType.format == IM_PIC_FMT_16BITS_RGB_565)
	{
		dstFormat = PIX_FMT_RGB565LE;
	}
	else
	{
		return IM_RET_VDEC_FAIL;
	}

	if(mSwsCtx == NULL)
	{	
		mSwsCtx = sws_getCachedContext(NULL, mVdecConfig.inType.width, mVdecConfig.inType.height, 
				mAVCtx->pix_fmt, mVdecConfig.outType.width, mVdecConfig.outType.height, dstFormat, 
				SWS_BICUBIC, NULL, NULL, NULL);
		if(mSwsCtx == NULL)
		{
			IM_VDEC_ERR("get swscontext failed");
			return IM_RET_VDEC_FAIL;
		}
	}

	switch(dstFormat)
	{
		case PIX_FMT_YUV420P:
		{
			dst[0] = (IM_UINT8 *)tempBuffer.vir_addr;
			dst[1] = dst[0] + mVdecConfig.outType.width * mVdecConfig.outType.height;
			dst[2] = dst[1] + mVdecConfig.outType.width * mVdecConfig.outType.height / 4;
			dstStride[0] = mVdecConfig.outType.width;
			dstStride[1] = mVdecConfig.outType.width >> 1;
			dstStride[2] = mVdecConfig.outType.width >> 1;

			break;
		}
		case PIX_FMT_RGB565LE:
		{
			dst[0] = (IM_UINT8 *)vdecPicture->buffer.virtAddr;
			dstStride[0] = mVdecConfig.outType.width * 2;
			dstStride[1] = 0;
			dstStride[2] = 0;
			break;
		}
		default:
		return  IM_RET_VDEC_FAIL;
	}
	
	outSliceHeight = sws_scale(mSwsCtx, (const uint8_t **)mAVFrame->data, mAVFrame->linesize, 
				0, mVdecConfig.outType.height, dst, dstStride);
	if(outSliceHeight <= 0)
	{
		return IM_RET_VDEC_FAIL;
	}

	if(dstFormat == PIX_FMT_YUV420P)
	{
		Y420PToY420SP(dst, vdecPicture->buffer.virtAddr, dstStride);
	}
	
	return IM_RET_VDEC_OK;
}

IM_RET IM_SwFFmpegDec::hwPostProcess(IM_VDEC_PICTURE *vdecPicture)
{
	PPResult ppResult;
	
	if(PpInst == NULL)
	{
		if((ppResult = PPInit(&PpInst)) != PP_OK)
		{
			IM_VDEC_ERR("PPInit Failed err= %d", ppResult);
			return IM_RET_VDEC_INIT_FAIL;
		}

		if((ppResult = PPGetConfig(PpInst, &PpCfg)) != PP_OK)
		{
			IM_VDEC_ERR("PP GetConfig Failed, err = %d", ppResult);
			return IM_RET_VDEC_INIT_FAIL;
		}
#ifdef VDEC_LIB_G1
		IM_VDEC_INFO("set MMU %s", mVdecConfig.mode.bMMUEnable ? "Enable" : "Disable");
		PPDecMMUEnable(PpInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
#endif 	
	}
	
	if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
	{
		return IM_RET_VDEC_FAIL;
	}
	
	if((ppResult = PPGetResult(PpInst)) != PP_OK)
	{
		IM_VDEC_ERR("PPGetResult failed result = %d", ppResult);
		return IM_RET_VDEC_OK;
	}

	return IM_RET_VDEC_OK;
}

IM_RET IM_SwFFmpegDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
	IM_VDEC_ASSERT(vdecPicture != NULL);
	IM_VDEC_ASSERT(PpInst != NULL);

	IM_VDEC_INFO("linesize = %d, width = %d ,height = %d", mAVFrame->linesize[0],mVdecConfig.inType.width, mVdecConfig.inType.height);

	IM_UINT32 width = IM_SIZEALIGNED16(mVdecConfig.inType.width);
	IM_UINT32 height = IM_SIZEALIGNED16(mVdecConfig.inType.height);
	IM_UINT32 yLength = width * height;
	IM_UINT32 uLength;
	IM_UINT32 vLength;

	PpCfg.ppInImg.width = IM_SIZEALIGNED16(mVdecConfig.inType.width);
	PpCfg.ppInImg.height = IM_SIZEALIGNED16(mVdecConfig.inType.height);
	PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
	PpCfg.ppInImg.bufferBusAddr = tempBuffer.phy_addr;
	if(mAVCtx->pix_fmt == PIX_FMT_YUV420P)
	{
		IM_VDEC_INFO("PIX_FMT_YUV420P");
		uLength = vLength = yLength / 4;
		IM_UINT8 *srcY = mAVFrame->data[0];
		IM_UINT8 *srcU = mAVFrame->data[1];
		IM_UINT8 *srcV = mAVFrame->data[2];

		IM_UINT8 *dstY = (IM_UINT8 *)tempBuffer.vir_addr;
		IM_UINT8 *dstU = dstY + yLength;
		IM_UINT8 *dstV = dstU + uLength;
		//copy Y plane
		for(IM_UINT32 copyline = 0;copyline < height; copyline++)
		{
			memcpy(dstY,srcY, width);
			dstY += width;
			srcY += mAVFrame->linesize[0];	
		}
		//copy U plane
		for(IM_UINT32 i = 0;i< height / 2;i++)
		{
			memcpy(dstU,srcU,width / 2);
			dstU += width / 2;
			srcU += mAVFrame->linesize[1];
		} 

		//copy V plane
		for(IM_UINT32 j = 0; j < height / 2; j++)
		{
			memcpy(dstV,srcV, width / 2);
			dstV += width / 2;
			srcV += mAVFrame->linesize[2];
		}
		PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_0_PLANAR;
		PpCfg.ppInImg.bufferCbBusAddr = tempBuffer.phy_addr + yLength; 
		PpCfg.ppInImg.bufferCrBusAddr = PpCfg.ppInImg.bufferCbBusAddr + uLength;
	} 
	else if(mAVCtx->pix_fmt== PIX_FMT_YUYV422)
	{
		IM_VDEC_INFO("PIX_FMT_YUYV422");
		IM_UINT8 *srcY = mAVFrame->data[0];
		IM_UINT8 *dstY = (IM_UINT8 *)tempBuffer.vir_addr;

		for(IM_UINT32 i = 0; i < height; i++)
		{
			memcpy(dstY, srcY, width);
			dstY += width;
			srcY += mAVFrame->linesize[0];
		}
		PpCfg.ppInImg.pixFormat = PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED;
	}
	else if(mAVCtx->pix_fmt == PIX_FMT_UYVY422)
	{
		IM_VDEC_INFO("PIX_FMT_UYVY422");
		IM_UINT8 *srcY = mAVFrame->data[0];
		IM_UINT8 *dstY = (IM_UINT8 *)tempBuffer.vir_addr;

		for(IM_UINT32 i = 0; i < height; i++)
		{
			memcpy(dstY, srcY, width);
			dstY += width;
			srcY += mAVFrame->linesize[0];
		}

		PpCfg.ppInImg.pixFormat = PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED;
	}
	else
	{
		return IM_RET_VDEC_FAIL;
	}

	PpCfg.ppOutImg.width = mVdecConfig.outType.width;
	PpCfg.ppOutImg.height = mVdecConfig.outType.height;
	return IM_VideoDec::setPpCfg(vdecPicture);
}

IM_RET IM_SwFFmpegDec::getDefaultConfig(IM_VDEC_CONFIG *vdecConfig)
{
	vdecConfig->build.codecType = SW_FFMPEG;
	vdecConfig->build.bStandAloneModeSupport = IM_TRUE;
	vdecConfig->build.bCombineModeSupport = IM_FALSE;
	vdecConfig->build.bMultiBuffModeSupport = IM_FALSE;
	vdecConfig->build.maxOutWidth = 1920;
	vdecConfig->build.maxOutHeight = 1080;
#ifdef VDEC_LIB_G1
	vdecConfig->build.bMMUSupported = IM_TRUE;
#else
	vdecConfig->build.bMMUSupported = IM_FALSE;
#endif
	vdecConfig->mode.bStandAloneMode = IM_TRUE;
	vdecConfig->mode.bCombineMode = IM_FALSE;
	vdecConfig->mode.bMultiBuffMode = IM_FALSE;
#ifdef VDEC_LIB_G1
	vdecConfig->mode.bMMUEnable = IM_TRUE;
#else
	vdecConfig->mode.bMMUEnable = IM_FALSE;
#endif
	return IM_RET_VDEC_OK;
}

IM_BOOL IM_SwFFmpegDec::Y420PToY420SP(IM_UINT8 **srcPlane,void *dstSemiPlane,IM_INT32 *linesize)
{
	if(srcPlane==NULL || 
			dstSemiPlane == NULL ||
			linesize == NULL)
	{
		return IM_FALSE;
	}
	
	IM_UINT8 *srcY = srcPlane[0];
#ifdef FFMPEG_SWAPUV_ENABLE
	IM_UINT8 *srcU = srcPlane[1];
	IM_UINT8 *srcV = srcPlane[2];
#else
	IM_UINT8 *srcU = srcPlane[2];
	IM_UINT8 *srcV = srcPlane[1];
#endif
	IM_UINT8 *dstY = (IM_UINT8 *)dstSemiPlane;
	IM_UINT8 *dstUV = (IM_UINT8 *)((IM_UINT8 *)dstSemiPlane + mVdecConfig.outType.width * mVdecConfig.outType.height);


	//copy y plane
	for(IM_UINT32 copyline= 0;copyline<mVdecConfig.outType.height;copyline++)
	{
		memcpy(dstY,srcY,mVdecConfig.outType.width);
		dstY += mVdecConfig.outType.width;
		srcY += linesize[0];	
	}
	//copy uv plane
	for(IM_UINT32 i=0;i< mVdecConfig.outType.height / 2;i++){
#ifdef FFMPEG_SWAPUV_ENABLE
		srcU = srcPlane[1]+i*linesize[1];
		srcV = srcPlane[2]+i*linesize[2];
		for(IM_UINT32 j=0;j< mVdecConfig.outType.width/2;j++){
			*dstUV++ = *srcU++;
			*dstUV++ = *srcV++;
		}
#else
		srcU = srcPlane[2]+i*linesize[2];
		srcV = srcPlane[1]+i*linesize[1];
		for(IM_UINT32 j=0;j< mVdecConfig.outType.width/2;j++){
			*dstUV++ = *srcV++;
			*dstUV++ = *srcU++;
		}
#endif
	}
	return IM_TRUE;
}

