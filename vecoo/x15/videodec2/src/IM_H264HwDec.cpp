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
 * @file        IM_H264HwDec.cpp
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0.0
 * @history
 *   2012.05.03 : Create
 */

#if TARGET_SYSTEM == FS_ADNROID
#define LOG_TAG "HwH264Dec"
#endif 
#include "IM_H264HwDec.h"
#include "IM_VdecLog.h"
IM_H264HwDec::IM_H264HwDec(IM_VDEC_INTYPE *inType)
{
    IM_VDEC_INFO("++++");


    memcpy(&mVdecConfig.inType, inType, sizeof(IM_VDEC_INTYPE));

    getDefaultConfig(&mVdecConfig);
    mH264Build = H264DecGetBuild(); 
    if(mVdecConfig.build.maxOutWidth > mH264Build.hwConfig.maxPpOutPicWidth )
    {
        mVdecConfig.build.maxOutWidth  = mH264Build.hwConfig.maxPpOutPicWidth;
    }

    if(mVdecConfig.build.maxOutHeight > 1088 )
    {
        mVdecConfig.build.maxOutWidth  = 1088;
    }

    mUseVideoFreezeConcealment = 0;
    noOutputReordering = 0;
    useDisplaySmoothing = 0;

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
    memset(&mH264DecIn, 0, sizeof(H264DecInput));
    memset(&mH264DecOut, 0, sizeof(H264DecOutput));
    memset(&mH264DecInfo, 0, sizeof(H264DecInfo));
    memset(&mH264DecPicture, 0, sizeof(H264DecPicture));

    mH264DecInst = NULL;

    pMultiBuffList = NULL;
    nSkippedPicAfterFlush = 0;
    IM_VDEC_INFO("----");
}

IM_BOOL IM_H264HwDec::isSupported()
{
    IM_VDEC_INFO("++++");
    if(mH264Build.hwConfig.h264Support == H264_NOT_SUPPORTED)
    {
        return IM_FALSE;
    }

    if(mVdecConfig.inType.codec != IM_STRM_VIDEO_H264)
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

IM_RET IM_H264HwDec::Init(IM_VDEC_SPEC_DATA *specData,IM_REF_FRM_FORMAT refFrmFormat)
{
    H264DecRet  h264Ret;
    PPResult   ppResult;
    IM_VDEC_INFO("++++");

    if(!bVdecConfigSetted)
    {
        IM_VDEC_ERR("video config not set");
        return IM_RET_VDEC_INIT_FAIL;
    }

#ifndef VDEC_LIB_G1
    h264Ret = H264DecInit(&mH264DecInst, noOutputReordering, mUseVideoFreezeConcealment, useDisplaySmoothing);
#else
    h264Ret = H264DecInit(&mH264DecInst, noOutputReordering, mUseVideoFreezeConcealment, useDisplaySmoothing, (DecRefFrmFormat)refFrmFormat);
#endif

    if(h264Ret != H264DEC_OK){
        IM_VDEC_ERR("mH264DecInit Failed error = %d", h264Ret);
        return IM_RET_VDEC_INIT_FAIL;
    }

#ifdef VDEC_LIB_G1
#if 0
    if(H264DecSetMvc(mH264DecInst) != H264DEC_OK)
    {
        IM_VDEC_WARN("H264DecSetMvc FAILED, not support mvc mode");	
    }
#endif
#endif
    if(mVdecConfig.mode.bCombineMode || mVdecConfig.mode.bStandAloneMode)
    {
        if((ppResult = PPInit(&PpInst)) != PP_OK)
        {
            IM_VDEC_ERR("PPInit Failed err= %d", ppResult);
            return IM_RET_VDEC_INIT_FAIL;
        }

        if(mVdecConfig.mode.bCombineMode)
        {
            if((ppResult = PPDecCombinedModeEnable(PpInst, mH264DecInst, PP_PIPELINED_DEC_TYPE_H264)) != PP_OK)
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
    H264DecMMUEnable(mH264DecInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
    if(PpInst != NULL)
    {
        PPDecMMUEnable(PpInst, mVdecConfig.mode.bMMUEnable ? 1 : 0);
    }
#endif 	

    IM_VDEC_INFO("----");
    return IM_RET_VDEC_OK;
}

IM_RET IM_H264HwDec::Decode(const IM_VDEC_IN *vdecIn, IM_VDEC_OUT *vdecOut, IM_VDEC_PICTURE *vdecPicture)
{
    H264DecRet h264Ret; 
    PPResult ppResult;
    IM_RET ret;
    IM_VDEC_INFO("++++");
    IM_VDEC_ASSERT(mH264DecInst != NULL);

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
            if((ppResult = PPDecCombinedModeEnable(PpInst, mH264DecInst, PP_PIPELINED_DEC_TYPE_H264)) != PP_OK)
            {
                IM_VDEC_ERR("PPDecCombinedModeEnable Failed, ppResult = %d",ppResult);
                return IM_RET_VDEC_FAIL;
            }
            bPpCombineModeEnable = IM_TRUE;
#endif
        }

        if(mVdecConfig.mode.bMultiBuffMode)
        {
			IM_RET ret = checkMultiBuffer(mH264DecInfo.multiBuffPpSize);
			if(ret == IM_RET_VDEC_LACK_OUTBUFF)
			{
                vdecOut->multiBuffPpSize = mH264DecInfo.multiBuffPpSize;
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
    IM_VDEC_INFO("in stream dataLen = %d", vdecIn->dataLen);
    mH264DecIn.pStream = (u8 *)vdecIn->pStream;
    mH264DecIn.streamBusAddress = vdecIn->busAddr;
    mH264DecIn.dataLen = vdecIn->dataLen;
    mH264DecIn.skipNonReference = ((vdecIn->flags & IM_VDEC_PIC_SKIP) && !mH264DecInfo.interlacedSequence) ? 1 : 0;
    mH264DecIn.picId = 0;
    vdecOut->skipPicNum = 0;

    mDirectShow = (vdecIn->flags & IM_VDEC_PIC_DIRECT_SHOW) ? 1 : 0;

    h264Ret = H264DecDecode(mH264DecInst, &mH264DecIn, &mH264DecOut);

    //result process
    switch(h264Ret)
    {
        case H264DEC_OK:
            ret = IM_RET_VDEC_NO_PIC_OUT;
            break;
        case H264DEC_STRM_PROCESSED:
            IM_VDEC_INFO("H264DEC_STRM_PROCESSED");
            ret = IM_RET_VDEC_STRM_PROCESSED;
            break;
        case H264DEC_NONREF_PIC_SKIPPED:
            IM_VDEC_INFO("H264DEC_NONREF_PIC_SKIPPED");
            vdecOut->skipPicNum = 1;
            ret = IM_RET_VDEC_PIC_SKIPPED;
            break;
        case H264DEC_HDRS_RDY:
        case H264DEC_ADVANCED_TOOLS:
            {
                H264DecInfo *decInfo = NULL;

                if(H264DecGetInfo(mH264DecInst, &mH264DecInfo) != H264DEC_OK)
                {
                    IM_VDEC_ERR("H264DecGetInfo failed");
                    return IM_RET_VDEC_FAIL;
                }
                ret = IM_RET_VDEC_HDRS_RDY;

                if(bHeaderReady)
                {
                    if(mH264DecInfo.picWidth != IM_SIZEALIGNED16(mVdecConfig.inType.width) 
                            || mH264DecInfo.picHeight != IM_SIZEALIGNED16(mVdecConfig.inType.height) 
                            || mVdecInfo.multiBuffPpSize != mH264DecInfo.multiBuffPpSize
                            || mVdecInfo.interlacedSequence != mH264DecInfo.interlacedSequence)
                    {
                        IM_VDEC_WARN("header ready,  size changed %d x %d, old size %d x %d", 
								mH264DecInfo.picWidth, mH264DecInfo.picHeight, mVdecInfo.frameWidth, mVdecInfo.frameHeight);
                        ret = IM_RET_VDEC_DECINFO_CHANGED;	
                    }
                }

                bVdecInfoValid = IM_TRUE;
				bPicInfoValid = IM_FALSE;
				bRunTimeValid = IM_FALSE;

                if(mH264DecInfo.picWidth == 0 || mH264DecInfo.picHeight == 0)
                {
                    if(!bNoInputSize)
                    {
                        mH264DecInfo.picWidth = IM_SIZEALIGNED16(mVdecConfig.inType.width);
                        mH264DecInfo.picHeight = IM_SIZEALIGNED16(mVdecConfig.inType.height);
                    }
                    else
                    {
                        bVdecInfoValid = IM_FALSE;
                    }
                }

                IM_VDEC_INFO("H264Dec Header Ready::");
                IM_VDEC_INFO("mH264DecInfo::picWidth = %d", mH264DecInfo.picWidth);
                IM_VDEC_INFO("mH264DecInfo::picHeight = %d", mH264DecInfo.picHeight);
                IM_VDEC_INFO("mH264DecInfo::interlaced = %d", mH264DecInfo.interlacedSequence);
                IM_VDEC_INFO("mH264DecInfo::multiBuffPpSize= %d", mH264DecInfo.multiBuffPpSize);


                mVdecInfo.frameWidth = mH264DecInfo.picWidth;
                mVdecInfo.frameHeight = mH264DecInfo.picHeight;
                mVdecInfo.codedWidth = mH264DecInfo.picWidth;
                mVdecInfo.codedHeight = mH264DecInfo.picHeight;
                mVdecInfo.interlacedSequence = mH264DecInfo.interlacedSequence;
                mVdecInfo.multiBuffPpSize = mH264DecInfo.multiBuffPpSize;

                bHeaderReady = IM_TRUE;
                mDirectShow = 1;
                getPicture(vdecPicture, 1);
                mDirectShow = 0;
            }
            break;
        case H264DEC_PENDING_FLUSH:
            IM_VDEC_INFO("H264DEC_PENDING_FLUSH");
        case H264DEC_PIC_DECODED:	
            {
                IM_VDEC_INFO("H264DEC_PIC_DECODED");
                ret = getPicture(vdecPicture, 0);
#ifndef MOSAIC_PIC_OUT
                while(ret == IM_RET_VDEC_MOSAIC_PIC_OUT)
                {
                    vdecOut->skipPicNum++;
                    ret = getPicture(vdecPicture, 0);
                }

                if(vdecOut->skipPicNum)
                {
                    IM_VDEC_INFO("After flush, Skipped Pic num = %d", nSkippedPicAfterFlush);
                    ret = IM_RET_VDEC_PIC_SKIPPED;
                }
#endif
            }
            break;
        case H264DEC_MEMFAIL:
            ret = IM_RET_VDEC_MEM_FAIL;
            break;
        case H264DEC_STREAM_NOT_SUPPORTED:
            ret = IM_RET_VDEC_UNSUPPORT_STREAM;
            break;
        case H264DEC_STRM_ERROR:
        default:
            IM_VDEC_ERR("H264 decode error h264Ret = %d", h264Ret);	
            mH264DecOut.dataLeft = 0;
            ret = IM_RET_VDEC_FAIL;
    };

    vdecOut->pCurStream = (void *)mH264DecOut.pStrmCurrPos;
    vdecOut->strmCurrBusAddress = mH264DecOut.strmCurrBusAddress;

    if(mH264DecOut.dataLeft > 1)
    {
        vdecOut->dataLeft = mH264DecOut.dataLeft;
    }
    else
    {
        vdecOut->dataLeft = mH264DecOut.dataLeft = 0;
        vdecOut->strmCurrBusAddress  = 0;
        vdecOut->pCurStream  = NULL;
    }

    IM_VDEC_INFO("pStrmCurrPos= %p", mH264DecOut.pStrmCurrPos);
    IM_VDEC_INFO("strmCurrBusAddress = 0x%x", mH264DecOut.strmCurrBusAddress);
    IM_VDEC_INFO("dataLeft = %d", mH264DecOut.dataLeft);

    IM_VDEC_INFO("----");
    return ret;

}

IM_RET IM_H264HwDec::GetNextPicture(IM_VDEC_PICTURE *vdecPicture)
{
    IM_INT32 skip = 0;

    if(setPpCfg(vdecPicture) != IM_RET_VDEC_OK)
    {
        IM_VDEC_ERR("set Pp config failed");
        skip = 1;
    }
    return getPicture(vdecPicture, skip);
}

IM_RET IM_H264HwDec::Flush()
{
    IM_VDEC_INFO("+++");
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

    //bPicInfoValid = IM_FALSE;
    //bRunTimeValid = IM_FALSE;
    bFlushed = IM_TRUE;
    nSkippedPicAfterFlush = 0;
    if(mH264DecInfo.interlacedSequence)
    {
        DeInit();
        Init(NULL, IM_DEC_REF_FRM_RASTER_SCAN);
        bHeaderReady = IM_FALSE;
        bPicInfoValid = IM_FALSE;
        bRunTimeValid = IM_FALSE;
    }
    IM_VDEC_INFO("---");

    return IM_RET_VDEC_OK;
}

IM_RET IM_H264HwDec::DeInit()
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
            if(PP_OK != PPDecCombinedModeDisable(PpInst, mH264DecInst))
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

    if(mH264DecInst)
    {
        H264DecRelease(mH264DecInst);
        mH264DecInst = NULL;
    }

    return IM_RET_VDEC_OK;
}


IM_RET IM_H264HwDec::getPicture(IM_VDEC_PICTURE *vdecPicture, IM_INT32 skip)
{
    IM_RET ret = IM_RET_VDEC_OK;
    H264DecRet h264Ret;
    PPResult ppResult;
    IM_INT32 skipNum = 0;
    IM_VDEC_ASSERT(vdecPicture != NULL);

    IM_VDEC_INFO("++++");
    vdecPicture->skipNum = 0; 
    do{
        h264Ret = H264DecNextPicture(mH264DecInst, &mH264DecPicture, mDirectShow);
        if(skip && h264Ret == H264DEC_PIC_RDY){
            skipNum++;
        }
    }while(skip && h264Ret == H264DEC_PIC_RDY);

    if(skip)
    {
        vdecPicture->skipNum = skipNum;
        return IM_RET_VDEC_FAIL;
    }

    if(h264Ret == H264DEC_PIC_RDY)
    {
        IM_VDEC_INFO("h264DecPic::picWidth = %d", mH264DecPicture.picWidth);
        IM_VDEC_INFO("h264DecPic::picHeight = %d", mH264DecPicture.picHeight);
        IM_VDEC_INFO("h264DecPic::cropLeftOffset = %d", mH264DecPicture.cropParams.cropLeftOffset);
        IM_VDEC_INFO("h264DecPic::cropOutWidth = %d", mH264DecPicture.cropParams.cropOutWidth);
        IM_VDEC_INFO("h264DecPic::cropTopOffset = %d", mH264DecPicture.cropParams.cropTopOffset);
        IM_VDEC_INFO("h264DecPic::cropOutHeight = %d", mH264DecPicture.cropParams.cropOutHeight);
        IM_VDEC_INFO("h264DecPic::keyPicture = %d", mH264DecPicture.isIdrPicture);
        IM_VDEC_INFO("h264DecPic::picId = %d", mH264DecPicture.picId);
        IM_VDEC_INFO("h264DecPic::interlaced = %d", mH264DecPicture.interlaced);
        IM_VDEC_INFO("h264DecPic::fieldPicture = %d", mH264DecPicture.fieldPicture);
        IM_VDEC_INFO("h264DecPic::topField = %d", mH264DecPicture.topField);

        if(mH264DecPicture.picWidth == 0 || mH264DecPicture.picHeight  == 0)
        {
            bPicInfoValid = IM_FALSE;
            bRunTimeValid = IM_TRUE;
            IM_VDEC_WARN("H264DEC_PIC_RDY, but out size is zero");
            return IM_RET_VDEC_NO_PIC_OUT;
        }

        bPicInfoValid = IM_TRUE;
        if(mVdecConfig.mode.bStandAloneMode)
        {
            if(setPpCfg(vdecPicture) != IM_RET_OK)
            {
                bPicInfoValid = IM_FALSE;
                return IM_RET_VDEC_FAIL;//getPicture(vdecPicture, 1);
            }
        }
        if(mVdecConfig.mode.bCombineMode || mVdecConfig.mode.bStandAloneMode)
        {
            if((ppResult = PPGetResult(PpInst)) != PP_OK)
            {
                bPicInfoValid = IM_FALSE;
                IM_VDEC_ERR("get Pp Result failed, ret = %d", ppResult);
                IM_VDEC_ERR("h264DecPic::picWidth = %d", mH264DecPicture.picWidth);
                IM_VDEC_ERR("h264DecPic::picHeight = %d", mH264DecPicture.picHeight);
                ret = IM_RET_VDEC_FAIL;
                //return IM_RET_VDEC_FAIL;//getPicture(vdecPicture, 1);
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
            vdecMemCpy(vdecPicture->buffer.virtAddr, (void*)mH264DecPicture.pOutputPicture, nDecodedPicSize);
        }

        if(ret == IM_RET_VDEC_OK)
        {
            nCropWidth = (mH264DecPicture.picWidth - mVdecConfig.inType.width) * \
                     (float) mVdecConfig.outType.width / mVdecConfig.inType.width;
            nCropHeight = (mH264DecPicture.picHeight - mVdecConfig.inType.height) * \
                      (float) mVdecConfig.outType.height / mVdecConfig.inType.height;
        }
        else
        {
            nCropWidth = 0;
            nCropHeight = 0;
        }
        vdecPicture->dataLen = nDecodedPicSize;
        vdecPicture->keyPicture = mH264DecPicture.isIdrPicture;
        vdecPicture->interlaced = mH264DecPicture.interlaced;

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
          	if(!mH264DecPicture.isIdrPicture && nSkippedPicAfterFlush < MAX_SKIP_PIC_NUM)
            {
                nSkippedPicAfterFlush++;
                IM_VDEC_WARN("MOSAIC PIC OUT");
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
        IM_VDEC_INFO("h264Ret = %d", h264Ret);
        bPicInfoValid = IM_FALSE;
        bRunTimeValid = IM_TRUE;
        mDirectShow = 0;
        return  IM_RET_VDEC_NO_PIC_OUT;
    }
}

IM_RET IM_H264HwDec::setPpCfg(IM_VDEC_PICTURE *vdecPicture)
{
    IM_VDEC_ASSERT(vdecPicture != NULL);
    IM_VDEC_ASSERT(PpInst != NULL);

    if(bPicInfoValid)
    {
        IM_VDEC_INFO("set  PpCfg by PicInfo");
        PpCfg.ppInImg.width = mH264DecPicture.picWidth;
        PpCfg.ppInImg.height = mH264DecPicture.picHeight;
        if(mVdecConfig.mode.bStandAloneMode)
        {
            PpCfg.ppInImg.bufferBusAddr = mH264DecPicture.outputPictureBusAddress; 
            PpCfg.ppInImg.bufferCbBusAddr = mH264DecPicture.outputPictureBusAddress + mH264DecPicture.picHeight * mH264DecPicture.picWidth; 
            PpCfg.ppInImg.bufferCrBusAddr = PpCfg.ppInImg.bufferCbBusAddr;
        }


        if(mH264DecPicture.interlaced == 0 ||
                PpCfg.ppOutDeinterlace.enable == 1)
        {
            //PpCfg.ppInImg.height = mH264DecPicture.picHeight;
            PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
        }
        else
        {
            if(mH264DecPicture.fieldPicture != 0)
            {
                PpCfg.ppInImg.height = mH264DecPicture.picHeight >> 1;
                if(mH264DecPicture.topField != 0)
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
                //PpCfg.ppInImg.height = mH264DecPicture.picHeight;
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

        mH264DecInfo.picHeight = IM_SIZEALIGNED16(mH264DecInfo.picHeight);
        PpCfg.ppInImg.width = mH264DecInfo.picWidth;
        PpCfg.ppInImg.height = mH264DecInfo.picHeight;
        if(mH264DecInfo.interlacedSequence == 0){
            PpCfg.ppInImg.height = mH264DecInfo.picHeight;
            PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
        }else{
            PpCfg.ppInImg.height = mH264DecInfo.picHeight;
            if(PpCfg.ppOutDeinterlace.enable)
            {
                PpCfg.ppInImg.picStruct = PP_PIC_FRAME_OR_TOP_FIELD;
            }
            else
            {
                PpCfg.ppInImg.picStruct = PP_PIC_TOP_AND_BOT_FIELD_FRAME;
                PpCfg.ppOutDeinterlace.enable = 1 ;
            }
        }

        PpCfg.ppInImg.videoRange = mH264DecInfo.videoRange;
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
