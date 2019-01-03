/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : layerControl_de.cpp
* Description : Display2
* History :
*/

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include "cdx_config.h"
#include <cdx_log.h>
#include "layerControl.h"
#include <ion_mem_alloc.h>
#include <videoOutPort.h>
#include "iniparserapi.h"
#include "tdisp_ctrl.h"
#define SAVE_PIC (0)

#define GPU_BUFFER_NUM 32

static VideoPicture* gLastPicture = NULL;

#define BUF_MANAGE (0)

#define NUM_OF_PICTURES_KEEP_IN_NODE (GetConfigParamterInt("pic_4list_num", 3)+1)

int LayerCtrlHideVideo(LayerCtrl* l);

typedef struct VPictureNode_t VPictureNode;
struct VPictureNode_t
{
    VideoPicture* pPicture;
    VPictureNode* pNext;
    int           bUsed;
};

typedef struct BufferInfoS
{
    VideoPicture pPicture;
    int          nUsedFlag;
}BufferInfoT;

typedef struct LayerContext
{
	LayerCtrl			base;
	dispOutPort			*mDispOutPort;
	enum EPIXELFORMAT	eDisplayPixelFormat;
	int					nWidth;
	int					nHeight;
	int					nLeftOff;
	int					nTopOff;
	int					nDisplayWidth;
	int					nDisplayHeight;
	int					bHoldLastPictureFlag;
	int					bVideoWithTwoStreamFlag;
	int					bIsSoftDecoderFlag;
	int					bLayerInitialized;
	int					bProtectFlag;
	int					setSrcRectFlag;
	int					setDstRectFlag;
	VoutRect			src_rect;
	VoutRect			dst_rect;
    struct SunxiMemOpsS *pMemops;

    //* use when render derect to hardware layer.
    VPictureNode*        pPictureListHeader;
    VPictureNode         picNodes[16];

    int                  nGpuBufferCount;
    BufferInfoT          mBufferInfo[GPU_BUFFER_NUM];
    int                  bLayerShowed;

    int                  fdDisplay;
    int                  nScreenWidth;
    int                  nScreenHeight;
    VideoFrameCallback mVideoframeCallback;
    void*                pUserData;
}LayerContext;

/* set usage, scaling_mode, pixelFormat, buffers_geometry, buffers_count, crop */
static int setLayerBuffer(LayerContext* lc)
{
    int          pixelFormat;
    unsigned int nGpuBufWidth;
    unsigned int nGpuBufHeight;
    int i = 0;
    char* pVirBuf;
    char* pPhyBuf;

    printf("%s:Fmt(%d),(%d %d, %d x %d)",
			__FUNCTION__, lc->eDisplayPixelFormat, lc->nWidth,
			lc->nHeight, lc->nLeftOff, lc->nTopOff);
    printf("Disp(%dx%d)buf_cnt(%d),ProFlag(%d),SoftDecFlag(%d)",
			lc->nDisplayWidth, lc->nDisplayHeight, lc->nGpuBufferCount,
			lc->bProtectFlag, lc->bIsSoftDecoderFlag);
    nGpuBufWidth  = lc->nWidth;
    nGpuBufHeight = lc->nHeight;

    /* We should double the height if the video with two stream,
     * so the nativeWindow will malloc double buffer
     */
    if(lc->bVideoWithTwoStreamFlag == 1)
    {
        nGpuBufHeight = 2*nGpuBufHeight;
    }

    if(lc->nGpuBufferCount <= 0)
    {
        printf("error: the lc->nGpuBufferCount[%d] is invalid!",lc->nGpuBufferCount);
        return -1;
    }

    for(i=0; i<lc->nGpuBufferCount; i++)
    {
        pVirBuf = (char*)SunxiMemPalloc(lc->pMemops, nGpuBufWidth * nGpuBufHeight * 3/2);
        pPhyBuf = (char*)SunxiMemGetPhysicAddressCpu(lc->pMemops, pVirBuf);
        lc->mBufferInfo[i].nUsedFlag    = 0;

        lc->mBufferInfo[i].pPicture.nWidth  = lc->nWidth;
        lc->mBufferInfo[i].pPicture.nHeight = lc->nHeight;
        lc->mBufferInfo[i].pPicture.nLineStride  = lc->nWidth;
        lc->mBufferInfo[i].pPicture.pData0       = pVirBuf;
        lc->mBufferInfo[i].pPicture.pData1       = pVirBuf + (lc->nHeight * lc->nWidth);
        lc->mBufferInfo[i].pPicture.pData2       =
                lc->mBufferInfo[i].pPicture.pData1 + (lc->nHeight * lc->nWidth)/4;
        lc->mBufferInfo[i].pPicture.phyYBufAddr  = (uintptr_t)pPhyBuf;
        lc->mBufferInfo[i].pPicture.phyCBufAddr  =
                lc->mBufferInfo[i].pPicture.phyYBufAddr + (lc->nHeight * lc->nWidth);
        lc->mBufferInfo[i].pPicture.nBufId       = i;
        lc->mBufferInfo[i].pPicture.ePixelFormat = lc->eDisplayPixelFormat;
    }

    return 0;
}

static int SetLayerParam(LayerContext* lc, VideoPicture* pPicture)
{
	unsigned int args[4];
	unsigned int fbAddr[3];
	videoParam vparam;
	renderBuf rBuf;
    int ret = 0;
    //* close the layer first, otherwise, in case when last frame is kept showing,
    //* the picture showed will not valid because parameters changed.
    if(lc->bLayerShowed == 1)
    {
        lc->bLayerShowed = 0;
    }

	fbAddr[0] = (unsigned long)SunxiMemGetPhysicAddressCpu(lc->pMemops,
			pPicture->pData0);
	fbAddr[1] = (unsigned long)SunxiMemGetPhysicAddressCpu(lc->pMemops,
			pPicture->pData1);
	fbAddr[2] = (unsigned long)SunxiMemGetPhysicAddressCpu(lc->pMemops,
			pPicture->pData2);
    if (lc->setSrcRectFlag) {
		vparam.srcInfo.crop_x = lc->src_rect.x;
		vparam.srcInfo.crop_y = lc->src_rect.y;
		vparam.srcInfo.crop_w = lc->src_rect.width;
		vparam.srcInfo.crop_h = lc->src_rect.height;
    } else {
		vparam.srcInfo.crop_x = pPicture->nLeftOffset;
		vparam.srcInfo.crop_y = pPicture->nTopOffset;
		vparam.srcInfo.crop_w = pPicture->nRightOffset
			- pPicture->nLeftOffset;
		vparam.srcInfo.crop_h = pPicture->nBottomOffset
			- pPicture->nTopOffset;
	}
	vparam.srcInfo.w = pPicture->nWidth;
	vparam.srcInfo.h = pPicture->nHeight;
	vparam.srcInfo.format = lc->eDisplayPixelFormat;
	vparam.srcInfo.color_space = VIDEO_BT601;
	rBuf.y_phaddr = fbAddr[0];
	rBuf.u_phaddr = fbAddr[1];
	rBuf.v_phaddr = fbAddr[2];
        rBuf.isExtPhy = VIDEO_USE_EXTERN_ION_BUF;
	lc->mDispOutPort->queueToDisplay(lc->mDispOutPort,
			vparam.srcInfo.w*vparam.srcInfo.h*3/2, &vparam, &rBuf);
	lc->mDispOutPort->setEnable(lc->mDispOutPort, 1);

    return 0;
}

static int __LayerReset(LayerCtrl* l)
{
    LayerContext* lc;
    int i;

    logd("LayerInit.");

    lc = (LayerContext*)l;

    for(i=0; i<lc->nGpuBufferCount; i++)
    {
        SunxiMemPfree(lc->pMemops, lc->mBufferInfo[i].pPicture.pData0);
    }

    return 0;
}

static void __LayerRelease(LayerCtrl* l)
{
    LayerContext* lc;
    int i;

    lc = (LayerContext*)l;

    logv("Layer release");
    for(i=0; i<lc->nGpuBufferCount; i++)
    {
        SunxiMemPfree(lc->pMemops, lc->mBufferInfo[i].pPicture.pData0);
    }
}

static void __LayerDestroy(LayerCtrl* l)
{
    LayerContext* lc;
    int i;

    lc = (LayerContext*)l;
	lc->mDispOutPort->deinit(lc->mDispOutPort);
    SunxiMemClose(lc->pMemops);

    free(lc);
}

static int __LayerSetDisplayBufferSize(LayerCtrl* l, int nWidth, int nHeight)
{
    LayerContext* lc;

    lc = (LayerContext*)l;

    lc->nWidth         = nWidth;
    lc->nHeight        = nHeight;
    lc->nDisplayWidth  = nWidth;
    lc->nDisplayHeight = nHeight;
    lc->nLeftOff       = 0;
    lc->nTopOff        = 0;
    lc->bLayerInitialized = 0;

    if(lc->bVideoWithTwoStreamFlag == 1)
    {
        /* display the whole buffer region when 3D
           * as we had init align-edge-region to black. so it will be look ok.
           */
        int nScaler = 2;
        lc->nDisplayHeight = lc->nDisplayHeight*nScaler;
    }

    return 0;
}

/* Description: set initial param -- display region */
static int __LayerSetDisplayRegion(LayerCtrl* l, int nLeftOff, int nTopOff,
                                        int nDisplayWidth, int nDisplayHeight)
{
    LayerContext* lc;

    lc = (LayerContext*)l;
    logd("Layer set display rect,(%d %d, %dx%d)\n",
            nLeftOff, nTopOff, nDisplayWidth, nDisplayHeight);

    if(nDisplayWidth == 0 && nDisplayHeight == 0)
    {
        return -1;
    }

    lc->nDisplayWidth     = nDisplayWidth;
    lc->nDisplayHeight    = nDisplayHeight;
    lc->nLeftOff          = nLeftOff;
    lc->nTopOff           = nTopOff;

    if(lc->bVideoWithTwoStreamFlag == 1)
    {
        int nScaler = 2;
        lc->nDisplayHeight = lc->nHeight*nScaler;
    }

    return 0;
}

/* Description: set initial param -- display pixelFormat */
static int __LayerSetDisplayPixelFormat(LayerCtrl* l, enum EPIXELFORMAT ePixelFormat)
{
    LayerContext* lc;

    lc = (LayerContext*)l;
    logd("Layer set expected pixel format, format = %d", (int)ePixelFormat);

    if(ePixelFormat == PIXEL_FORMAT_NV12 ||
       ePixelFormat == PIXEL_FORMAT_NV21 ||
       ePixelFormat == PIXEL_FORMAT_YV12)
    {
        lc->eDisplayPixelFormat = ePixelFormat;
    }
    else
    {
        logv("receive pixel format is %d, not match.", lc->eDisplayPixelFormat);
        return -1;
    }

    return 0;
}

//* Description: set initial param -- deinterlace flag
static int __LayerSetDeinterlaceFlag(LayerCtrl* l,int bFlag)
{
    LayerContext* lc;
    (void)bFlag;
    lc = (LayerContext*)l;

    return 0;
}

//* Description: set buffer timestamp -- set this param every frame
static int __LayerSetBufferTimeStamp(LayerCtrl* l, int64_t nPtsAbs)
{
    LayerContext* lc;
    (void)nPtsAbs;

    lc = (LayerContext*)l;

    return 0;
}

static int __LayerGetRotationAngle(LayerCtrl* l)
{
    LayerContext* lc;
    int nRotationAngle = 0;

    lc = (LayerContext*)l;

    return 0;
}

static int __LayerCtrlShowVideo(LayerCtrl* l)
{
    LayerContext* lc;
    int i;

    lc = (LayerContext*)l;

    lc->bLayerShowed = 1;

    return 0;
}

static int __LayerCtrlHideVideo(LayerCtrl* l)
{
    LayerContext* lc;
    int i;

    lc = (LayerContext*)l;

    lc->bLayerShowed = 0;

    return 0;
}

static int __LayerCtrlIsVideoShow(LayerCtrl* l)
{
    LayerContext* lc;

    lc = (LayerContext*)l;

     return lc->bLayerShowed;
}

static int  __LayerCtrlHoldLastPicture(LayerCtrl* l, int bHold)
{
    logd("LayerCtrlHoldLastPicture, bHold = %d", bHold);

    LayerContext* lc;
    lc = (LayerContext*)l;

    return 0;
}

static int __LayerDequeueBuffer(LayerCtrl* l, VideoPicture** ppVideoPicture, int bInitFlag)
{
    LayerContext* lc;
    int i = 0;
    VPictureNode*     nodePtr;
    BufferInfoT bufInfo;
    VideoPicture* pPicture = NULL;

    lc = (LayerContext*)l;

    if(lc->bLayerInitialized == 0)
    {
        if(setLayerBuffer(lc) != 0)
        {
            loge("can not initialize layer.");
            return -1;
        }

        lc->bLayerInitialized = 1;
    }

    if(bInitFlag == 1)
    {
        for(i = 0; i < lc->nGpuBufferCount; i++)
        {
            if(lc->mBufferInfo[i].nUsedFlag == 0)
            {
                //* set the buffer address
                pPicture = *ppVideoPicture;
                pPicture = &lc->mBufferInfo[i].pPicture;

                lc->mBufferInfo[i].nUsedFlag = 1;
                break;
            }
        }
    }
    else
    {
        if(lc->pPictureListHeader != NULL)
        {
            nodePtr = lc->pPictureListHeader;
            i=1;
            while(nodePtr->pNext != NULL)
            {
                i++;
                nodePtr = nodePtr->pNext;
            }

           unsigned int     args[3];
                    //* return one picture.
            if(i > GetConfigParamterInt("pic_4list_num", 3))
            {
                nodePtr = lc->pPictureListHeader;
                lc->pPictureListHeader = lc->pPictureListHeader->pNext;
                /*{
                    int nCurFrameId;
                    int nWaitTime;
                    int disp0 = 0;
                    int disp1 = 0;

                    nWaitTime = 50000;  //* max frame interval is 1000/24fps = 41.67ms,
                                        //  we wait 50ms at most.
                    args[1] = 0; //chan(0 for video)
                    args[2] = 0; //layer_id
                    do
                    {
                        args[0] = 0;
                        nCurFrameId = ioctl(lc->fdDisplay, DISP_LAYER_GET_FRAME_ID, args);
                        logv("nCurFrameId hdmi = %d, pPicture id = %d",
                             nCurFrameId, nodePtr->pPicture->nID);
                        if(nCurFrameId != nodePtr->pPicture->nID)
                        {
                            break;
                        }
                        else
                        {
                            if(nWaitTime <= 0)
                            {
                                loge("check frame id fail, maybe something error happen.");
                                break;
                            }
                            else
                            {
                                usleep(5000);
                                nWaitTime -= 5000;
                            }
                        }
                    }while(1);
                }*/
                pPicture = nodePtr->pPicture;
                nodePtr->bUsed = 0;
                i--;

            }
        }
    }

    logv("** dequeue  pPicture(%p)",pPicture);
    *ppVideoPicture = pPicture;
    return 0;
}

// this method should block here,
static int __LayerQueueBuffer(LayerCtrl* l, VideoPicture* pBuf, int bValid)
{
    LayerContext* lc  = NULL;

    int               i;
    VPictureNode*     newNode;
    VPictureNode*     nodePtr;
    BufferInfoT    bufInfo;

    lc = (LayerContext*)l;
    logv("** queue , pPicture(%p)", pBuf);
//printf("%s,l:%d\n", __FUNCTION__, __LINE__);
    if(bValid == 0)
    {
        return 0;
    }
//    printf("%s,l:%d\n", __FUNCTION__, __LINE__);
    lc->mVideoframeCallback(lc->pUserData,pBuf);
    if(lc->bLayerInitialized == 0)
    {
        if(setLayerBuffer(lc) != 0)
        {
            loge("can not initialize layer.");
            return -1;
        }

        lc->bLayerInitialized = 1;
    }

    if(SetLayerParam(lc, pBuf) != 0)
    {
        loge("can not initialize layer.");
        return -1;
    }

    // *****************************************
    // *****************************************
    //  TODO: Display this video picture here () blocking
    //
    // *****************************************

    newNode = NULL;
    for(i = 0; i < NUM_OF_PICTURES_KEEP_IN_NODE; i++)
    {
        if(lc->picNodes[i].bUsed == 0)
        {
            newNode = &lc->picNodes[i];
            newNode->pNext = NULL;
            newNode->bUsed = 1;
            newNode->pPicture = pBuf;
            break;
        }
    }
    if(i == NUM_OF_PICTURES_KEEP_IN_NODE)
    {
        loge("*** picNode is full when queue buffer");
        return -1;
    }

    if(lc->pPictureListHeader != NULL)
    {
        nodePtr = lc->pPictureListHeader;
        while(nodePtr->pNext != NULL)
        {
            nodePtr = nodePtr->pNext;
        }
        nodePtr->pNext = newNode;
    }
    else
    {
        lc->pPictureListHeader = newNode;
    }

    return 0;
}

static int __LayerSetDisplayBufferCount(LayerCtrl* l, int nBufferCount)
{
    LayerContext* lc;

    lc = (LayerContext*)l;

    logv("LayerSetBufferCount: count = %d",nBufferCount);

    lc->nGpuBufferCount = nBufferCount;

    if(lc->nGpuBufferCount > GPU_BUFFER_NUM)
        lc->nGpuBufferCount = GPU_BUFFER_NUM;

    return lc->nGpuBufferCount;
}

static int __LayerGetBufferNumHoldByGpu(LayerCtrl* l)
{
    (void)l;
    return GetConfigParamterInt("pic_4list_num", 3);
}

static int __LayerGetDisplayFPS(LayerCtrl* l)
{
    (void)l;
    return 60;
}

static void __LayerResetNativeWindow(LayerCtrl* l,void* pNativeWindow)
{
    logd("LayerResetNativeWindow : %p ",pNativeWindow);

    LayerContext* lc;
    VideoPicture mPicBufInfo;

    lc = (LayerContext*)l;
    lc->bLayerInitialized = 0;

    return ;
}

static VideoPicture* __LayerGetBufferOwnedByGpu(LayerCtrl* l)
{
    LayerContext* lc;
    VideoPicture* pPicture = NULL;
    BufferInfoT bufInfo;

    lc = (LayerContext*)l;
    int i;
    for(i = 0; i<lc->nGpuBufferCount; i++)
    {
        bufInfo = lc->mBufferInfo[i];
        if(bufInfo.nUsedFlag == 1)
        {
            bufInfo.nUsedFlag = 0;
            pPicture = &bufInfo.pPicture;
            break;
        }
    }
    return pPicture;
}

static int __LayerSetVideoWithTwoStreamFlag(LayerCtrl* l, int bVideoWithTwoStreamFlag)
{
    LayerContext* lc;

    lc = (LayerContext*)l;

    logv("LayerSetIsTwoVideoStreamFlag, flag = %d",bVideoWithTwoStreamFlag);
    lc->bVideoWithTwoStreamFlag = bVideoWithTwoStreamFlag;

    return 0;
}

static int __LayerSetIsSoftDecoderFlag(LayerCtrl* l, int bIsSoftDecoderFlag)
{
    LayerContext* lc;

    lc = (LayerContext*)l;

    logv("LayerSetIsSoftDecoderFlag, flag = %d",bIsSoftDecoderFlag);
    lc->bIsSoftDecoderFlag = bIsSoftDecoderFlag;

    return 0;
}

//* Description: the picture buf is secure
static int __LayerSetSecure(LayerCtrl* l, int bSecure)
{
    logv("__LayerSetSecure, bSecure = %d", bSecure);
    LayerContext* lc;

    lc = (LayerContext*)l;

    lc->bProtectFlag = bSecure;

    return 0;
}

static int __LayerReleaseBuffer(LayerCtrl* l, VideoPicture* pPicture)
{
    logv("***LayerReleaseBuffer");
    LayerContext* lc;

    lc = (LayerContext*)l;

    SunxiMemPfree(lc->pMemops, pPicture->pData0);
    return 0;
}

static LayerControlOpsT mLayerControlOps =
{
    release:                    __LayerRelease                   ,

    setSecureFlag:              __LayerSetSecure                 ,
    setDisplayBufferSize:       __LayerSetDisplayBufferSize      ,
    setDisplayBufferCount:      __LayerSetDisplayBufferCount     ,
    setDisplayRegion:           __LayerSetDisplayRegion          ,
    setDisplayPixelFormat:      __LayerSetDisplayPixelFormat     ,
    setVideoWithTwoStreamFlag:  __LayerSetVideoWithTwoStreamFlag ,
    setIsSoftDecoderFlag:       __LayerSetIsSoftDecoderFlag      ,
    setBufferTimeStamp:         __LayerSetBufferTimeStamp        ,

    resetNativeWindow :         __LayerResetNativeWindow         ,
    getBufferOwnedByGpu:        __LayerGetBufferOwnedByGpu       ,
    getDisplayFPS:              __LayerGetDisplayFPS             ,
    getBufferNumHoldByGpu:      __LayerGetBufferNumHoldByGpu     ,

    ctrlShowVideo :             __LayerCtrlShowVideo             ,
    ctrlHideVideo:              __LayerCtrlHideVideo             ,
    ctrlIsVideoShow:            __LayerCtrlIsVideoShow           ,
    ctrlHoldLastPicture :       __LayerCtrlHoldLastPicture       ,

    dequeueBuffer:              __LayerDequeueBuffer             ,
    queueBuffer:                __LayerQueueBuffer               ,
    releaseBuffer:              __LayerReleaseBuffer             ,
    reset:                      __LayerReset                     ,
    destroy:                    __LayerDestroy
};

LayerCtrl* LayerCreate(VideoFrameCallback callback,void* pUser)
{
    LayerContext* lc;
    unsigned int args[4];
    int screen_id;
    int enable = 1;
	int rotate = 0;
	VoutRect rect;
    logd("LayerCreate.");

    lc = (LayerContext*)malloc(sizeof(LayerContext));
    if(lc == NULL)
    {
        loge("malloc memory fail.");
        return NULL;
    }
    memset(lc, 0, sizeof(LayerContext));
	lc->mDispOutPort = CreateVideoOutport(0);
	rect.x = 0;
	rect.y = 0;
	rect.width = 400;
	rect.height = 1280;
	lc->mDispOutPort->init(lc->mDispOutPort, enable, rotate, &rect);
	lc->mDispOutPort->setRect(lc->mDispOutPort,&rect);
    lc->base.ops = &mLayerControlOps;
    lc->eDisplayPixelFormat = PIXEL_FORMAT_YV12;

    lc->mVideoframeCallback = callback;
    lc->pUserData = pUser;
    logd("==== callback: %p, pUser: %p", callback, pUser);


    /* get screen size. */
    lc->nScreenWidth = lc->mDispOutPort->getScreenWidth(lc->mDispOutPort);
	lc->nScreenHeight = lc->mDispOutPort->getScreenHeight(lc->mDispOutPort);
    logd("screen:w %d, screen:h %d", lc->nScreenWidth, lc->nScreenHeight);

    lc->pMemops = GetMemAdapterOpsS();
    SunxiMemOpen(lc->pMemops);

    return &lc->base;
}

void LayerSetControl(LayerCtrl* l, LAYER_CMD_TYPE cmd, int grade)
{
    LayerContext* lc;
    int i;

    lc = (LayerContext*)l;
	switch(cmd){
		case DISP_CMD_SET_BRIGHTNESS:
		case DISP_CMD_SET_CONTRAST:
		case DISP_CMD_SET_HUE:
		case DISP_CMD_SET_SATURATION:
		case DISP_CMD_SET_VEDIO_ENHANCE_DEFAULT:
			lc->mDispOutPort->setIoctl(lc->mDispOutPort, cmd, grade);
			break;
		default:
			break;
	}
}

int LayerSetSrcRect(LayerCtrl* l, int x, int y, unsigned int width, unsigned int height)
{
	LayerContext* lc;

    lc = (LayerContext*)l;
    logd("Layer set display rect,(%d %d, %dx%d)\n",
            x, y, width, height);

    if(width == 0 && height == 0)
        return -1;

	lc->src_rect.x = x;
	lc->src_rect.x = y;
	lc->src_rect.width = width;
	lc->src_rect.height = height;
	lc->setSrcRectFlag    = 1;
	lc->mDispOutPort->setSrcRect(lc->mDispOutPort, &lc->src_rect);

    return 0;
    }
int LayerSetDisplayRect(LayerCtrl* l, int x, int y, unsigned int width, unsigned int height)
{
	LayerContext* lc;
    lc = (LayerContext*)l;
    logd("Layer set display rect,(%d %d, %dx%d)\n",
            x, y, width, height);
    if(width == 0 && height == 0)
        return -1;
	lc->dst_rect.x = x;
	lc->dst_rect.x = y;
	lc->dst_rect.width = width;
	lc->dst_rect.height = height;
	lc->setDstRectFlag    = 1;
	lc->mDispOutPort->setRect(lc->mDispOutPort, &lc->dst_rect);

    return 0;
}

int LayerDisplayOnoff(LayerCtrl* l, int onoff)
{
	LayerContext* lc;

    lc = (LayerContext*)l;
	if(onoff)
		lc->bLayerShowed = 1;
	else
		lc->bLayerShowed = 0;

	return 0;
}
