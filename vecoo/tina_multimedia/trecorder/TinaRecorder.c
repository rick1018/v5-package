#include "TinaRecorder.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <CdxQueue.h>
#include <AwPool.h>
#include <CdxBinary.h>
#include "memoryAdapter.h"
#include "awencoder.h"
#include "RecorderWriter.h"

typedef struct DemoRecoderContext
{
    AwEncoder*       mAwEncoder;
    VideoEncodeConfig videoConfig;
    AudioEncodeConfig audioConfig;
    CdxMuxerT* pMuxer;
    int muxType;
    char mSavePath[1024];
    CdxWriterT* pStream;
    unsigned char* extractDataBuff;
    unsigned int extractDataLength;
    pthread_t muxerThreadId ;
    pthread_t audioDataThreadId ;
    pthread_t videoDataThreadId ;
    AwPoolT *mDataPool;
	CdxQueueT *dataQueue;
    AwPoolT *mTmpDataPool;
	CdxQueueT *mTmpDataQueue;
    int  mRecordDuration;
    int videoEos;
    int audioEos;
    bool muxThreadExitFlag;
	EncDataCallBackOps *ops;
}DemoRecoderContext;

static void NotifyCallbackForAwEncorder(void* pUserData, int msg, void* param)
{
	TinaRecorder *hdlTina = (TinaRecorder *)pUserData;
	if(hdlTina == NULL)
		return;
        VencThumbInfo *pthumbAddr;
        DemoRecoderContext* pDemoRecoder = (DemoRecoderContext*)hdlTina->EncoderContext;
	dataParam dataparam;
	renderBuf rBuf;
	int scaleWidth,scaleHeight;
	memset(&rBuf, 0, sizeof(dataParam));
	memset(&dataparam, 0, sizeof(dataParam));
	static char *addr = NULL;
	VencThumbInfo thumbAddr;
    switch(msg)
    {
        case AWENCODER_VIDEO_ENCODER_NOTIFY_RETURN_BUFFER:
        {
            int id = *((int*)param);
			dataparam.bufferId = id;
			if(hdlTina->dispport.enable && hdlTina->dispport.route == VIDEO_SRC_FROM_VE)
			{
//				sem_wait(&hdlTina->renderFinSem);
				hdlTina->vport.enqueue(&hdlTina->vport, &dataparam);
			}
			else if(hdlTina->outport.enable == 1
				&&(hdlTina->dispport.enable && hdlTina->dispport.route == VIDEO_SRC_FROM_CAM))
			{
				sem_wait(&hdlTina->dispfinSem);
				hdlTina->vport.enqueue(&hdlTina->vport, &hdlTina->param);
			}
			else
				hdlTina->vport.enqueue(&hdlTina->vport, &dataparam);

			break;
        }
		case AWENCODER_VIDEO_ENCODER_NOTIFY_RETURN_THUMBNAIL:
			pthumbAddr = (VencThumbInfo *)param;
			//printf("hah:,i get thumb nail now!\n");
			//printf("thumb addr:%x size:%d\n",thumbAddr->pThumbBuf,thumbAddr->nThumbSize);
			if(pthumbAddr->pThumbBuf != addr)
			{
				addr = pthumbAddr->pThumbBuf;
			}
			sem_post(&hdlTina->renderDataSem);
			break;

		case AWENCODER_VIDEO_ENCODER_NOTIFY_THUMBNAIL_GETBUFFER:
			if(hdlTina->rotateAngel == ROTATION_ANGLE_0)
			{
				hdlTina->dispport.dequeue(&hdlTina->dispport,&rBuf);
				hdlTina->scaleDownBuf = rBuf.vir_addr;
				memcpy(&hdlTina->rBuf, &rBuf, sizeof(renderBuf));
			}
			scaleWidth = ((hdlTina->encodeWidth + 15)/16*16)/hdlTina->scaleDownRatio;
			scaleHeight = ((hdlTina->encodeHeight + 15)/16*16)/hdlTina->scaleDownRatio;
			thumbAddr.pThumbBuf = hdlTina->scaleDownBuf;
			thumbAddr.nThumbSize = scaleWidth*scaleHeight*3/2;

			AwEncoderSetParamete(pDemoRecoder->mAwEncoder,AwEncoder_SetThumbNailAddr,&thumbAddr);
			break;

		case AWENCODER_VIDEO_ENCODER_NOTIFY_ERROR:
			//printf("Notify error!,return:%d hdl:%x\n",dataparam.bufferId,hdlTina);
			//dataparam.bufferId = hdlTina->lastEncodeId;
			//hdlTina->vport.enqueue(&hdlTina->vport,&dataparam);
			break;
        default:
        {
            printf("warning: unknown callback from AwRecorder.\n");
            break;
        }
    }

    return ;
}

static void clearConfigure(TinaRecorder *hdl)
{
	hdl->encodeWidth = 0;
	hdl->encodeHeight = 0;
	hdl->encodeBitrate = 0;
	hdl->encodeFramerate = 0;
	hdl->outputFormat = -1;
	hdl->callback = NULL;
	hdl->encodeVFormat = -1;
	hdl->encodeAFormat = -1;
	hdl->maxRecordTime;
	hdl->EncoderContext = NULL;
#if 0
	memset(&hdl->vport, 0, sizeof(vInPort));
	memset(&hdl->aport, 0, sizeof(aInPort));
	memset(&hdl->dispport, 0, sizeof(dispOutPort));
	memset(&hdl->outport, 0, sizeof(rOutPort));
#endif
}

static int InitJpegExif(TinaRecorder *hdl,EXIFInfo *exifinfo)
{
    if(hdl==NULL || exifinfo==NULL)
        printf("hdl or exifinfo is null,InitJpegExif() fail\n");
    exifinfo->ThumbWidth = hdl->encodeWidth/8;
    exifinfo->ThumbHeight = hdl->encodeHeight/8;

    printf("exifinfo->ThumbWidth = %d\n",exifinfo->ThumbWidth);
    printf("exifinfo->ThumbHeight = %d\n",exifinfo->ThumbHeight);

    strcpy((char*)exifinfo->CameraMake,        "allwinner make test");
    strcpy((char*)exifinfo->CameraModel,        "allwinner model test");
    strcpy((char*)exifinfo->DateTime,         "2014:02:21 10:54:05");
    strcpy((char*)exifinfo->gpsProcessingMethod,  "allwinner gps");

    exifinfo->Orientation = 0;

    exifinfo->ExposureTime.num = 2;
    exifinfo->ExposureTime.den = 1000;

    exifinfo->FNumber.num = 20;
    exifinfo->FNumber.den = 10;
    exifinfo->ISOSpeed = 50;

    exifinfo->ExposureBiasValue.num= -4;
    exifinfo->ExposureBiasValue.den= 1;

    exifinfo->MeteringMode = 1;
    exifinfo->FlashUsed = 0;

    exifinfo->FocalLength.num = 1400;
    exifinfo->FocalLength.den = 100;

    exifinfo->DigitalZoomRatio.num = 4;
    exifinfo->DigitalZoomRatio.den = 1;

    exifinfo->WhiteBalance = 1;
    exifinfo->ExposureMode = 1;

    exifinfo->enableGpsInfo = 1;

    exifinfo->gps_latitude = 23.2368;
    exifinfo->gps_longitude = 24.3244;
    exifinfo->gps_altitude = 1234.5;

    exifinfo->gps_timestamp = (long)time(NULL);

    strcpy((char*)exifinfo->CameraSerialNum,  "123456789");
    strcpy((char*)exifinfo->ImageName,  "exif-name-test");
    strcpy((char*)exifinfo->ImageDescription,  "exif-descriptor-test");
}

static int saveJpeg(char* savePath,VencOutputBuffer* jpegData){
    int ret = 0;
    FILE* savaJpegFd = fopen(savePath, "wb");
    printf("save jpeg path:%s\n",savePath);
    if(savaJpegFd==NULL){
	printf("fopen %s  fail****\n",savePath);
	printf("err str: %s\n",strerror(errno));
        return -1;
    }else{
	fseek(savaJpegFd,0,SEEK_SET);
        fwrite(jpegData->pData0, 1, jpegData->nSize0, savaJpegFd);
        if(jpegData->nSize1)
        {
            fwrite(jpegData->pData1, 1, jpegData->nSize1, savaJpegFd);
        }
	fclose(savaJpegFd);
	savaJpegFd = NULL;
    }
    return ret;
}
static int captureJpegPhoto(TinaRecorder *hdl,char* savePath,dataParam* yuvDataParam){
    if(hdl == NULL || savePath == NULL || yuvDataParam == NULL){
        printf("captureJpegPhoto err:hdl = 0x%p,savePath = 0x%p,yuvDataParam = 0x%p\n",hdl,savePath,yuvDataParam);
        return -1;
    }
    int ret = 0;
    VencBaseConfig baseConfig;
    VencInputBuffer inputBuffer;
    VencOutputBuffer outputBuffer;
    VideoEncoder* pVideoEnc = NULL;
    EXIFInfo exifinfo;
    InitJpegExif(hdl,&exifinfo);
    memset(&baseConfig, 0 ,sizeof(VencBaseConfig));
    memset(&inputBuffer, 0x00, sizeof(VencInputBuffer));
    memset(&outputBuffer, 0x00, sizeof(VencOutputBuffer));
    baseConfig.nInputWidth= hdl->vport.MainWidth;
    baseConfig.nInputHeight = hdl->vport.MainHeight;
    baseConfig.nStride = hdl->vport.MainWidth;

    baseConfig.nDstWidth = hdl->mCaptureConfig.captureWidth;
    baseConfig.nDstHeight = hdl->mCaptureConfig.captureHeight;
    switch(hdl->vport.format)
    {
        case TR_PIXEL_YUV420SP:
                baseConfig.eInputFormat = VENC_PIXEL_YUV420SP;
		break;
        case TR_PIXEL_YVU420SP:
                baseConfig.eInputFormat = VENC_PIXEL_YVU420SP;
		break;
        case TR_PIXEL_YUV420P:
                baseConfig.eInputFormat = VENC_PIXEL_YUV420P;
		break;
        case TR_PIXEL_YVU420P:
                baseConfig.eInputFormat = VENC_PIXEL_YVU420P;
		break;
        case TR_PIXEL_YUV422SP:
                baseConfig.eInputFormat = VENC_PIXEL_YUV422SP;
		break;
        case TR_PIXEL_YVU422SP:
                baseConfig.eInputFormat = VENC_PIXEL_YVU422SP;
		break;
        case TR_PIXEL_YUV422P:
                baseConfig.eInputFormat = VENC_PIXEL_YUV422P;
		break;
        case TR_PIXEL_YVU422P:
                baseConfig.eInputFormat = VENC_PIXEL_YVU422P;
		break;
        case TR_PIXEL_YUYV422:
                baseConfig.eInputFormat = VENC_PIXEL_YUYV422;
		break;
        case TR_PIXEL_UYVY422:
                baseConfig.eInputFormat = VENC_PIXEL_UYVY422;
		break;
        case TR_PIXEL_YVYU422:
                baseConfig.eInputFormat = VENC_PIXEL_YVYU422;
		break;
        case TR_PIXEL_VYUY422:
                baseConfig.eInputFormat = VENC_PIXEL_VYUY422;
		break;
        case TR_PIXEL_ARGB:
                baseConfig.eInputFormat = VENC_PIXEL_ARGB;
		break;
        case TR_PIXEL_RGBA:
                baseConfig.eInputFormat = VENC_PIXEL_RGBA;
		break;
        case TR_PIXEL_ABGR:
                baseConfig.eInputFormat = VENC_PIXEL_ABGR;
		break;
        case TR_PIXEL_BGRA:
                baseConfig.eInputFormat = VENC_PIXEL_BGRA;
		break;
        default:
		printf("input pixel format is not supported!\n");
		break;
    }

    pVideoEnc = VideoEncCreate(VENC_CODEC_JPEG);
    if(pVideoEnc == NULL){
        printf("VideoEncCreate fail\n");
		return -1;
    }

    int quality = 50;
    int jpeg_mode = 0;
    int vbvSize = 750*1024;
    int vbvThreshold = 0;//that means all the vbv buffer can be used
    if(baseConfig.nDstWidth<1920){
        vbvSize = 512*1024;
    }

    VideoEncSetParameter(pVideoEnc, VENC_IndexParamJpegExifInfo, &exifinfo);
    VideoEncSetParameter(pVideoEnc, VENC_IndexParamJpegQuality, &quality);
    VideoEncSetParameter(pVideoEnc, VENC_IndexParamJpegEncMode, &jpeg_mode);
    VideoEncSetParameter(pVideoEnc, VENC_IndexParamSetVbvSize, &vbvSize);
    VideoEncSetParameter(pVideoEnc, VENC_IndexParamSetFrameLenThreshold, &vbvThreshold);

    int vencRet = -1;

    vencRet = VideoEncInit(pVideoEnc, &baseConfig);

    if(vencRet == -1){
        printf("VideoEncInit fail\n");
        goto handle_err;
    }

    inputBuffer.nID = yuvDataParam->bufferId;
    inputBuffer.nPts = yuvDataParam->pts;
    inputBuffer.nFlag = 0;
    inputBuffer.pAddrPhyY = yuvDataParam->MainYPhyAddr;
    inputBuffer.pAddrPhyC = yuvDataParam->MainCPhyAddr;

    inputBuffer.bEnableCorp = 0;
    inputBuffer.sCropInfo.nLeft =  240;
    inputBuffer.sCropInfo.nTop  =  240;
    inputBuffer.sCropInfo.nWidth  =  240;
    inputBuffer.sCropInfo.nHeight =  240;

    vencRet = AddOneInputBuffer(pVideoEnc, &inputBuffer);
    if(vencRet == -1){
        printf("AddOneInputBuffer fail\n");
        goto handle_err;
    }

    vencRet = VideoEncodeOneFrame(pVideoEnc);
    if(vencRet == -1){
        printf("VideoEncodeOneFrame fail\n");
        goto handle_err;
    }

    vencRet = GetOneBitstreamFrame(pVideoEnc, &outputBuffer);
    if(vencRet == -1){
        printf("GetOneBitstreamFrame fail\n");
        goto handle_err;
    }

    saveJpeg(savePath,&outputBuffer);

    vencRet = FreeOneBitStreamFrame(pVideoEnc, &outputBuffer);
    if(vencRet == -1){
        printf("FreeOneBitStreamFrame fail\n");
        goto handle_err;
    }

    if(pVideoEnc)
    {
        VideoEncDestroy(pVideoEnc);
    }

    return ret;
 handle_err:
    if(pVideoEnc)
    {
        VideoEncDestroy(pVideoEnc);
    }

    return -1;
}
static int initStorePktInfo(TinaRecorder *hdlTina){
    if(hdlTina == NULL)
        return -1;
    hdlTina->mTotalAudioPktBufSize = 8*1024;
    hdlTina->mAudioPktBufSizeCount = 0;
    hdlTina->mNeedAudioPktBufSpace = 0;
    pthread_mutex_init(&hdlTina->mAudioPktBufLock, NULL);
    sem_init(&hdlTina->mAudioPktBufSem, 0, 0);

    if(hdlTina->encodeWidth>=1920)
        hdlTina->mTotalVideoPktBufSize = 2*1024*1024;
    else
        hdlTina->mTotalVideoPktBufSize = 1*1024*1024;
    hdlTina->mVideoPktBufSizeCount = 0;
    hdlTina->mNeedVideoPktBufSpace = 0;
    pthread_mutex_init(&hdlTina->mVideoPktBufLock, NULL);
    sem_init(&hdlTina->mVideoPktBufSem, 0, 0);
    return 0;
}

static int destroyStorePktInfo(TinaRecorder *hdlTina){
    if(hdlTina == NULL)
        return -1;
    sem_destroy(&hdlTina->mVideoPktBufSem);
    pthread_mutex_destroy(&hdlTina->mVideoPktBufLock);
    sem_destroy(&hdlTina->mAudioPktBufSem);
    pthread_mutex_destroy(&hdlTina->mAudioPktBufLock);
    return 0;
}

/*
static int initPreMallocMem(TinaRecorder *hdlTina){
    if(hdlTina == NULL)
        return -1;

    hdlTina->mTotalAudioPktNum = 64;
    for(int i = 0 ; i < hdlTina->mTotalAudioPktNum ; i++){
        memset(&(hdlTina->mAudioPktPtr[i]),0,sizeof(CdxMuxerPacketT));
    }
    hdlTina->mAudioPktWriteIndex = 0;
    hdlTina->mAudioPktUnusedCount = hdlTina->mTotalAudioPktNum;

    hdlTina->mTotalAudioPktBufSize = 64*1024;
    if(hdlTina->mAudioBufPtr == NULL){
        hdlTina->mAudioBufPtr = (char*)malloc(hdlTina->mTotalAudioPktBufSize);
    }
    if(hdlTina->mAudioBufPtr == NULL){
        printf("hdlTina->mAudioBufPtr malloc err\n");
        goto fail_release;
    }
    memset(hdlTina->mAudioBufPtr,0,hdlTina->mTotalAudioPktBufSize);
    hdlTina->mAudioBufEndPtr = hdlTina->mAudioBufPtr+hdlTina->mTotalAudioPktBufSize;
    hdlTina->mAudioBufWritePtr = hdlTina->mAudioBufPtr;
    hdlTina->mAudioBufUnusedSize = hdlTina->mTotalAudioPktBufSize;
    pthread_mutex_init(&hdlTina->mAudioPktLock, NULL);

    hdlTina->mTotalVideoPktNum = 64;
    for(int i = 0 ; i < hdlTina->mTotalVideoPktNum ; i++){
        memset(&(hdlTina->mVideoPktPtr[i]),0,sizeof(CdxMuxerPacketT));
    }
    hdlTina->mVideoPktWriteIndex = 0;
    hdlTina->mVideoPktUnusedCount = hdlTina->mTotalVideoPktNum;

    if(hdlTina->encodeWidth>=1920)
        hdlTina->mTotalVideoPktBufSize = 2*1024*1024;
    else
        hdlTina->mTotalVideoPktBufSize = 1*1024*1024;
    if(hdlTina->mVideoBufPtr == NULL){
        hdlTina->mVideoBufPtr = (char*)malloc(hdlTina->mTotalVideoPktBufSize);
    }
    memset(hdlTina->mVideoBufPtr,0,hdlTina->mTotalVideoPktBufSize);
    hdlTina->mVideoBufEndPtr = hdlTina->mVideoBufPtr+hdlTina->mTotalVideoPktBufSize;
    if(hdlTina->mVideoBufPtr == NULL){
        printf("hdlTina->mBufPtr malloc err\n");
        goto fail_release;
    }
    hdlTina->mVideoBufWritePtr = hdlTina->mVideoBufPtr;
    hdlTina->mVideoBufUnusedSize = hdlTina->mTotalVideoPktBufSize;
    pthread_mutex_init(&hdlTina->mVideoPktLock, NULL);
    return 0;
fail_release:
    if(hdlTina->mAudioBufPtr){
        free(hdlTina->mAudioBufPtr);
        hdlTina->mAudioBufPtr = NULL;
    }
    if(hdlTina->mVideoBufPtr){
        free(hdlTina->mVideoBufPtr);
        hdlTina->mVideoBufPtr = NULL;
    }
    pthread_mutex_destroy(&hdlTina->mAudioPktLock);
    pthread_mutex_destroy(&hdlTina->mVideoPktLock);
    return -1;
}

static int destroyPreMallocMem(TinaRecorder *hdlTina){
    if(hdlTina == NULL)
        return -1;
    if(hdlTina->mAudioBufPtr){
        free(hdlTina->mAudioBufPtr);
        hdlTina->mAudioBufPtr = NULL;
    }
    if(hdlTina->mVideoBufPtr){
        free(hdlTina->mVideoBufPtr);
        hdlTina->mVideoBufPtr = NULL;
    }
    pthread_mutex_destroy(&hdlTina->mAudioPktLock);
    pthread_mutex_destroy(&hdlTina->mVideoPktLock);
    return 0;
}
*/
int onVideoDataEnc(void *app,CdxMuxerPacketT *buff)
{
	TinaRecorder *hdlTina = (TinaRecorder *)app;
	if(hdlTina == NULL)
		return -1;
    DemoRecoderContext* pDemoRecoder = (DemoRecoderContext*)hdlTina->EncoderContext;

    CdxMuxerPacketT *packet = NULL;
    if (!buff)
        return 0;
	if(hdlTina->outport.enable)
	{
	    if((hdlTina->mNeedMuxerVideoFlag == 0 && buff->keyFrameFlag==1) || (hdlTina->mNeedMuxerVideoFlag == 1)){
                if(hdlTina->mNeedMuxerVideoFlag == 0 && buff->keyFrameFlag==1){//this is the first buf,we should record the base pts
                    hdlTina->mBaseVideoPts = buff->pts;
                    hdlTina->mNeedMuxerVideoFlag = 1;
                }
                pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                hdlTina->mNeedVideoPktBufSpace = buff->buflen-(hdlTina->mTotalVideoPktBufSize-hdlTina->mVideoPktBufSizeCount);
                if(hdlTina->mNeedVideoPktBufSpace > 0){
                    printf("hdlTina->mNeedVideoPktBufSpace = %d,waiting\n",hdlTina->mNeedVideoPktBufSpace);
                    pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
                    sem_wait(&hdlTina->mVideoPktBufSem);
                }else{
                    pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
                }
                packet = (CdxMuxerPacketT*)malloc(sizeof(CdxMuxerPacketT));
                packet->buflen = buff->buflen;
                packet->length = buff->length;
                packet->buf = malloc(buff->buflen);
                memcpy(packet->buf, buff->buf, packet->buflen);
                packet->pts = buff->pts - hdlTina->mBaseVideoPts;
                packet->type = buff->type;
                packet->streamIndex  = buff->streamIndex;
                packet->duration = buff->duration;
                packet->keyFrameFlag = buff->keyFrameFlag;
                //printf("hdlTina->mCameraIndex:%d video packet pts = %lld\n",hdlTina->mCameraIndex,packet->pts);
                //printf("hdlTina->mCameraIndex:%d video packet duration = %lld\n",hdlTina->mCameraIndex,packet->duration);
                pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                hdlTina->mVideoPktBufSizeCount +=packet->buflen;
                pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
	        CdxQueuePush(pDemoRecoder->dataQueue,packet);
                sem_post(&hdlTina->mQueueDataSem);

                //printf("camIndex %d:video buf len = %d\n", hdlTina->mCameraIndex,buff->buflen);
                /*
                while(hdlTina->outport.enable==1 && ((hdlTina->mVideoPktUnusedCount<=0) || (hdlTina->mVideoBufUnusedSize < buff->buflen)
                    ||( (hdlTina->mVideoBufEndPtr -hdlTina->mVideoBufWritePtr<buff->buflen)&&( hdlTina->mVideoBufUnusedSize-(hdlTina->mVideoBufEndPtr -hdlTina->mVideoBufWritePtr)< buff->buflen)))){
                    printf("camIndex %d:no buf to store the encoded video,hdlTina->mVideoPktUnusedCount = %d,hdlTina->mVideoBufUnusedSize = %d,buff->buflen = %d\n",
                        hdlTina->mCameraIndex,hdlTina->mVideoPktUnusedCount,hdlTina->mVideoBufUnusedSize,buff->buflen);
                    usleep(8*1000);
                    if(hdlTina->outport.enable == 0){
                        printf("onVideoDataEnc:recording has been stopped,quit the while loop\n");
                        return 0;
                    }
                }
                packet = &(hdlTina->mVideoPktPtr[hdlTina->mVideoPktWriteIndex]);
                //printf("camIndex %d:video packet = %p,hdlTina->mVideoPktWriteIndex = %d\n",hdlTina->mCameraIndex,packet,hdlTina->mVideoPktWriteIndex);
                hdlTina->mVideoPktWriteIndex++;
                if(hdlTina->mVideoPktWriteIndex == hdlTina->mTotalVideoPktNum){
                    hdlTina->mVideoPktWriteIndex = 0;
                }
                packet->buflen = buff->buflen;
                packet->length = buff->length;
                packet->pts = buff->pts - hdlTina->mBaseVideoPts;
                packet->type = buff->type;
                packet->streamIndex  = buff->streamIndex;
                packet->duration = buff->duration;
                packet->keyFrameFlag = buff->keyFrameFlag;
                //printf("camIndex %d:video packet->pts = %lld,buff->pts = %lld\n", hdlTina->mCameraIndex,packet->pts,buff->pts);
                if(hdlTina->mVideoBufEndPtr -hdlTina->mVideoBufWritePtr<packet->buflen){
                    hdlTina->mVideoBufWritePtr = hdlTina->mVideoBufPtr;//let the buf point to the begin
                }
                packet->buf = hdlTina->mVideoBufWritePtr;
                memcpy(packet->buf, buff->buf, packet->buflen);
                hdlTina->mVideoBufWritePtr = hdlTina->mVideoBufWritePtr+packet->buflen;
                if(hdlTina->mVideoBufWritePtr == hdlTina->mVideoBufEndPtr){
                   //printf("camIndex %d:hdlTina->mVideoBufWritePtr(%p) is equal to hdlTina->mVideoBufEndPtr(%p)\n",
                   //         hdlTina->mCameraIndex,hdlTina->mVideoBufWritePtr,hdlTina->mVideoBufEndPtr);
                   hdlTina->mVideoBufWritePtr = hdlTina->mVideoBufPtr;
                }
                pthread_mutex_lock(&hdlTina->mVideoPktLock);
                hdlTina->mVideoPktUnusedCount--;
                hdlTina->mVideoBufUnusedSize = hdlTina->mVideoBufUnusedSize - packet->buflen;
                pthread_mutex_unlock(&hdlTina->mVideoPktLock);
                CdxQueuePush(pDemoRecoder->dataQueue,packet);
                */
           }
	}
    return 0;
}

int onAudioDataEnc(void *app,CdxMuxerPacketT *buff)
{
	TinaRecorder *hdlTina = (TinaRecorder *)app;
	if(hdlTina == NULL)
		return -1;
    DemoRecoderContext* pDemoRecoder = (DemoRecoderContext*)hdlTina->EncoderContext;
    CdxMuxerPacketT *packet = NULL;
    if (!buff)
        return 0;
	if(hdlTina->outport.enable)
	{
	    pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
            hdlTina->mNeedAudioPktBufSpace = buff->buflen-(hdlTina->mTotalAudioPktBufSize-hdlTina->mAudioPktBufSizeCount);
            if(hdlTina->mNeedAudioPktBufSpace > 0){
                printf("hdlTina->mNeedAudioPktBufSpace = %d,waiting\n",hdlTina->mNeedAudioPktBufSpace);
                pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
                sem_wait(&hdlTina->mAudioPktBufSem);
            }else{
                pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
            }
	    packet = (CdxMuxerPacketT*)malloc(sizeof(CdxMuxerPacketT));
	    packet->buflen = buff->buflen;
	    packet->length = buff->length;
	    packet->buf = malloc(buff->buflen);
	    memcpy(packet->buf, buff->buf, packet->buflen);
	    packet->pts = buff->pts;
	    packet->type = buff->type;
	    packet->streamIndex = buff->streamIndex;
	    packet->duration = buff->duration;
	    //printf("hdlTina->mCameraIndex:%d audio packet pts = %lld\n",hdlTina->mCameraIndex,packet->pts);
	    //printf("hdlTina->mCameraIndex:%d audio packet duration = %lld\n",hdlTina->mCameraIndex,packet->duration);
	    pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
            hdlTina->mAudioPktBufSizeCount +=packet->buflen;
            pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
	    CdxQueuePush(pDemoRecoder->dataQueue,packet);
            sem_post(&hdlTina->mQueueDataSem);

            //printf("camIndex %d:audio buf len = %d.audio pts = %lld\n", hdlTina->mCameraIndex,buff->buflen,packet->pts);
            /*
            while(hdlTina->outport.enable == 1 && (hdlTina->mAudioPktUnusedCount<=0 || hdlTina->mAudioBufUnusedSize < buff->buflen
                ||( (hdlTina->mAudioBufEndPtr -hdlTina->mAudioBufWritePtr<buff->buflen)&&( hdlTina->mAudioBufUnusedSize-(hdlTina->mAudioBufEndPtr -hdlTina->mAudioBufWritePtr)< buff->buflen)))){
                printf("camIndex %d:no buf to store the encoded audio,hdlTina->mAudioPktUnusedCount = %d,hdlTina->mAudioBufUnusedSize = %d,buff->buflen = %d\n",
                        hdlTina->mCameraIndex,hdlTina->mAudioPktUnusedCount,hdlTina->mAudioBufUnusedSize,buff->buflen);
                usleep(8*1000);
                if(hdlTina->outport.enable == 0){
                    printf("onAudioDataEnc:recording has been stopped,quit the while loop\n");
                    return 0;
                }
            }
            packet = &(hdlTina->mAudioPktPtr[hdlTina->mAudioPktWriteIndex]);
            //printf("camIndex %d:audio packet = %p,hdlTina->mAudioPktWriteIndex = %d\n",hdlTina->mCameraIndex,packet,hdlTina->mAudioPktWriteIndex);
            hdlTina->mAudioPktWriteIndex++;
            if(hdlTina->mAudioPktWriteIndex == hdlTina->mTotalAudioPktNum){
                hdlTina->mAudioPktWriteIndex = 0;
            }
            packet->buflen = buff->buflen;
            packet->length = buff->length;
            packet->pts = buff->pts;
            packet->type = buff->type;
            packet->streamIndex  = buff->streamIndex;
            packet->duration = buff->duration;
            packet->keyFrameFlag = buff->keyFrameFlag;
            //printf("camIndex %d:audio packet->pts = %lld,buff->pts = %lld\n", hdlTina->mCameraIndex,packet->pts,buff->pts);
            if(hdlTina->mAudioBufEndPtr -hdlTina->mAudioBufWritePtr<packet->buflen){
                hdlTina->mAudioBufWritePtr = hdlTina->mAudioBufPtr;//let the buf point to the begin
            }
            packet->buf = hdlTina->mAudioBufWritePtr;
            memcpy(packet->buf, buff->buf, packet->buflen);
            hdlTina->mAudioBufWritePtr = hdlTina->mAudioBufWritePtr+packet->buflen;
            if(hdlTina->mAudioBufWritePtr == hdlTina->mAudioBufEndPtr){
               //printf("camIndex %d:hdlTina->mAudioBufWritePtr(%p) is equal to hdlTina->mAudioBufEndPtr(%p)\n",
               //         hdlTina->mCameraIndex,hdlTina->mAudioBufWritePtr,hdlTina->mAudioBufEndPtr);
               hdlTina->mAudioBufWritePtr = hdlTina->mAudioBufPtr;
            }
            pthread_mutex_lock(&hdlTina->mAudioPktLock);
            hdlTina->mAudioPktUnusedCount--;
            hdlTina->mAudioBufUnusedSize = hdlTina->mAudioBufUnusedSize - packet->buflen;
            pthread_mutex_unlock(&hdlTina->mAudioPktLock);
            CdxQueuePush(pDemoRecoder->dataQueue,packet);
            */
	}

    return 0;
}

static void *TRDirdispThread(void *param)
{
	videoParam paramDisp;

	TinaRecorder *hdlTina = (TinaRecorder *)param;
	if(hdlTina == NULL)
		return NULL;

	while(hdlTina->dispport.enable)
	{
		sem_wait(&hdlTina->vport.dqfinSem);
		if(hdlTina->dispport.enable && hdlTina->dispport.route == VIDEO_SRC_FROM_ISP)
		{
			paramDisp.isPhy = 0;
			//fixme,set right width and height here
			paramDisp.srcInfo.w = hdlTina->vport.SubWidth;
			paramDisp.srcInfo.h = hdlTina->vport.SubHeight;
			paramDisp.srcInfo.crop_x = 0;
			paramDisp.srcInfo.crop_y = 0;
			paramDisp.srcInfo.crop_w = hdlTina->vport.SubWidth;
			paramDisp.srcInfo.crop_h = hdlTina->vport.SubHeight;
			paramDisp.srcInfo.format = VIDEO_PIXEL_FORMAT_NV21;
			hdlTina->dispport.setRotateAngel(&hdlTina->dispport,ROTATION_ANGLE_90);
			//fixme,get buffer from dequeue
			if(hdlTina->vport.rot_angle == 0)
				hdlTina->dispport.writeData(&hdlTina->dispport,hdlTina->param.SubYVirAddr,
								hdlTina->param.SubSize,&paramDisp);
			else
				hdlTina->dispport.writeData(&hdlTina->dispport, hdlTina->param.RotYVirAddr,
								hdlTina->param.RotSize,&paramDisp);
		}

		if(hdlTina->dispport.enable && hdlTina->dispport.route == VIDEO_SRC_FROM_CAM)
		{
			paramDisp.isPhy = 0;
			paramDisp.srcInfo.w = hdlTina->vport.MainWidth;
			paramDisp.srcInfo.h = hdlTina->vport.MainHeight;
			paramDisp.srcInfo.crop_x = 0;
			paramDisp.srcInfo.crop_y = 0;
			paramDisp.srcInfo.crop_w = hdlTina->vport.MainWidth;
			paramDisp.srcInfo.crop_h = hdlTina->vport.MainHeight;
			paramDisp.srcInfo.format = VIDEO_PIXEL_FORMAT_NV21;
			hdlTina->dispport.setRotateAngel(&hdlTina->dispport,ROTATION_ANGLE_0);
			hdlTina->dispport.writeData(&hdlTina->dispport, hdlTina->param.MainYVirAddr,
							hdlTina->param.MainSize,&paramDisp);
		}

		sem_post(&hdlTina->dispfinSem);
	}
	sem_destroy(&hdlTina->dispfinSem);
	return NULL;
}
static void *TRRenderThread(void *param)
{
	TinaRecorder *hdlTina = (TinaRecorder *)param;
        if(hdlTina == NULL)
		return NULL;
	videoParam paramDisp;
	int scaleWidth = ((hdlTina->encodeWidth + 15)/16*16)/hdlTina->scaleDownRatio;
	int scaleHeight = ((hdlTina->encodeHeight + 15)/16*16)/hdlTina->scaleDownRatio;
	int bufferSize = scaleWidth*scaleHeight*3/2;

	hdlTina->dispport.setRotateAngel(&hdlTina->dispport,ROTATION_ANGLE_0);
	paramDisp.isPhy = 0;
	paramDisp.srcInfo.w = scaleWidth;
	paramDisp.srcInfo.h = scaleHeight;
	paramDisp.srcInfo.crop_x = 0;
	paramDisp.srcInfo.crop_y = 0;
	paramDisp.srcInfo.crop_w = scaleWidth;
	paramDisp.srcInfo.crop_h = scaleHeight;
	//fixme here,to set correct format
	paramDisp.srcInfo.format = VIDEO_PIXEL_FORMAT_NV12;

	while(hdlTina->dispport.enable)
	{
		if(hdlTina->renderStatus == RENDER_STATUS_LOOP)
		{
				sem_wait(&hdlTina->renderDataSem);
				if(hdlTina->rotateAngel == ROTATION_ANGLE_0)
				{
					hdlTina->dispport.queueToDisplay(&hdlTina->dispport,bufferSize,&paramDisp,&hdlTina->rBuf);
				}
				else
				{
					hdlTina->dispport.writeData(&hdlTina->dispport,hdlTina->scaleDownBuf,bufferSize,&paramDisp);
				}
//				sem_post(&hdlTina->renderFinSem);
		}
		else if(hdlTina->renderStatus == RENDER_STATUS_PAUSE)
		{
			sem_post(&hdlTina->renderSem);
			while(hdlTina->renderStatus != RENDER_STATUS_RECONFIG && hdlTina->renderStatus != RENDER_STATUS_STOP)
			{
				usleep(1000);
			}
		}
		else if(hdlTina->renderStatus == RENDER_STATUS_RECONFIG)
		{
			scaleWidth = ((hdlTina->encodeWidth + 15)/16*16)/hdlTina->scaleDownRatio;
			scaleHeight = ((hdlTina->encodeHeight + 15)/16*16)/hdlTina->scaleDownRatio;
			paramDisp.isPhy = 0;
			paramDisp.srcInfo.w = scaleWidth;
			paramDisp.srcInfo.h = scaleHeight;
			paramDisp.srcInfo.crop_x = 0;
			paramDisp.srcInfo.crop_y = 0;
			paramDisp.srcInfo.crop_w = scaleWidth;
			paramDisp.srcInfo.crop_h = scaleHeight;
			//fixme here,to set correct format
			paramDisp.srcInfo.format = VIDEO_PIXEL_FORMAT_NV12;
			hdlTina->renderStatus = RENDER_STATUS_LOOP;
			bufferSize = scaleWidth*scaleHeight*3/2;
		}
	}
	sem_post(&hdlTina->renderSem);
	sem_destroy(&hdlTina->renderDataSem);
	sem_destroy(&hdlTina->renderFinSem);
	sem_destroy(&hdlTina->renderSem);
	return NULL;
}

static void* MuxerThread(void *param)

{
	TinaRecorder *hdlTina = (TinaRecorder *)param;
	if(hdlTina == NULL)
		return NULL;
    DemoRecoderContext* p = (DemoRecoderContext*)hdlTina->EncoderContext;
    CdxMuxerPacketT *mPacket = NULL;
    RecoderWriterT *rw;
    char rm_cmd[MAX_URL_LEN + 4];
    CdxMuxerMediaInfoT mediainfo;
    int fs_cache_size = 0;
    int reopenFlag;
    long long baseAudioPts = -1;
    long long baseVideoPts = -1;
    long long audioDurationCount = 0;
    long long videoDurationCount = 0;
    unsigned int keyFramExitFlag;

    int file_count = 0;
    long long each_file_duration = hdlTina->maxRecordTime;//ms
    printf("each_file_duration = %lld ms\n",each_file_duration);
    if(each_file_duration == 0)
        each_file_duration = 60*1000;//if not set the record time,set it to 1 min
#if FS_WRITER
        //for FSWRITEMODE_CACHETHREAD mode
        /*
        CdxFsCacheMemInfo fs_cache_mem;
        memset(&fs_cache_mem, 0, sizeof(CdxFsCacheMemInfo));
        fs_cache_mem.m_cache_size = 2*1024*1024;
        fs_cache_mem.mp_cache = (cdx_int8*)malloc(fs_cache_mem.m_cache_size);
        if (fs_cache_mem.mp_cache == NULL)
        {
            printf("fs_cache_mem.mp_cache malloc failed\n");
            return NULL;
        }
        */
#endif

    REOPEN:
        reopenFlag = 0;
        keyFramExitFlag = 0;
        //first:we should callback to app,which can let the app set the output path
        if(hdlTina->callback){
            hdlTina->callback(hdlTina->pUserData,TINA_RECORD_ONE_FILE_COMPLETE,NULL);
            printf("hdlTina->callback finish\n");
        }
        if(hdlTina->mOutputPath)
        {
            if ((p->pStream = CdxWriterCreat(p->mSavePath)) == NULL)
            {
                printf("CdxWriterCreat() failed\n");
                /*
                if(fs_cache_mem.mp_cache)
                {
                    free(fs_cache_mem.mp_cache);
                    fs_cache_mem.mp_cache = NULL;
                }
                */
                return 0;
            }
            rw = (RecoderWriterT*)p->pStream;
            rw->file_mode = FP_FILE_MODE;

            strcpy(rw->file_path, hdlTina->mOutputPath);
            printf("rw->file_path =%s\n",rw->file_path);
            RWOpen(p->pStream);
            p->pMuxer = CdxMuxerCreate(p->muxType, p->pStream);
            if(p->pMuxer == NULL)
            {
                printf("CdxMuxerCreate failed\n");
                if(p->pStream)
                {
                    RWClose(p->pStream);
                    CdxWriterDestroy(p->pStream);
                    p->pStream = NULL;
                    rw = NULL;
                }
                /*
                if(fs_cache_mem.mp_cache)
                {
                    free(fs_cache_mem.mp_cache);
                    fs_cache_mem.mp_cache = NULL;
                }
                */
                return 0;
            }
        }
        file_count++;
        memset(&mediainfo, 0, sizeof(CdxMuxerMediaInfoT));
        if(hdlTina->aport.enable)
	{
		switch (p->audioConfig.nType)
		{
            case AUDIO_ENCODE_PCM_TYPE:
				mediainfo.audio.eCodecFormat = AUDIO_ENCODER_PCM_TYPE;
				break;
            case AUDIO_ENCODE_AAC_TYPE:
				mediainfo.audio.eCodecFormat = AUDIO_ENCODER_AAC_TYPE;
				break;
            case AUDIO_ENCODE_MP3_TYPE:
				mediainfo.audio.eCodecFormat = AUDIO_ENCODER_MP3_TYPE;
				break;
            case AUDIO_ENCODE_LPCM_TYPE:
				mediainfo.audio.eCodecFormat = AUDIO_ENCODER_LPCM_TYPE;
				break;
            default:
				printf("unlown audio type(%d)\n", p->audioConfig.nType);
				break;
		}
	    mediainfo.audioNum = 1;
		mediainfo.audio.nAvgBitrate = p->audioConfig.nBitrate;
		mediainfo.audio.nBitsPerSample = p->audioConfig.nSamplerBits;
		mediainfo.audio.nChannelNum = p->audioConfig.nOutChan;
		mediainfo.audio.nMaxBitRate = p->audioConfig.nBitrate;
		mediainfo.audio.nSampleRate = p->audioConfig.nOutSamplerate;
		mediainfo.audio.nSampleCntPerFrame = 1024; // 固定的,一帧数据的采样点数目
    }
    if(hdlTina->vport.enable && (p->muxType == CDX_MUXER_MOV || p->muxType == CDX_MUXER_TS))
	{
		mediainfo.videoNum = 1;
		if(p->videoConfig.nType == VIDEO_ENCODE_H264)
		    mediainfo.video.eCodeType = VENC_CODEC_H264;
		else if(p->videoConfig.nType == VIDEO_ENCODE_JPEG)
		    mediainfo.video.eCodeType = VENC_CODEC_JPEG;
		else
		{
	            printf("warnning:cannot suppot this video type,use the default:h264\n");
	            mediainfo.video.eCodeType = VENC_CODEC_H264;
		}
		mediainfo.video.nWidth    =  p->videoConfig.nOutWidth;
		mediainfo.video.nHeight   =  p->videoConfig.nOutHeight;
		mediainfo.video.nFrameRate = p->videoConfig.nFrameRate;
    }
    printf("******************* mux mediainfo *****************************\n");
    printf("videoNum                   : %d\n", mediainfo.videoNum);
    printf("videoTYpe                  : %d\n", mediainfo.video.eCodeType);
    printf("framerate                  : %d\n", mediainfo.video.nFrameRate);
    printf("width                      : %d\n", mediainfo.video.nWidth);
    printf("height                     : %d\n", mediainfo.video.nHeight);
    printf("audioNum                   : %d\n", mediainfo.audioNum);
    printf("audioFormat                : %d\n", mediainfo.audio.eCodecFormat);
    printf("audioChannelNum            : %d\n", mediainfo.audio.nChannelNum);
    printf("audioSmpleRate             : %d\n", mediainfo.audio.nSampleRate);
    printf("audioBitsPerSample         : %d\n", mediainfo.audio.nBitsPerSample);
    printf("**************************************************************\n");

    if(p->pMuxer)
    {
		CdxMuxerSetMediaInfo(p->pMuxer, &mediainfo);
#if FS_WRITER
        //for FSWRITEMODE_CACHETHREAD mode
        /*
        if(fs_cache_mem.mp_cache != NULL){
            memset(fs_cache_mem.mp_cache,0,fs_cache_mem.m_cache_size);
        }
        int fs_mode = FSWRITEMODE_CACHETHREAD;
        CdxMuxerControl(p->pMuxer, SET_CACHE_MEM, &fs_cache_mem);
        CdxMuxerControl(p->pMuxer, SET_FS_WRITE_MODE, &fs_mode);
        */

        //for FSWRITEMODE_SIMPLECACHE mode

        fs_cache_size = 8 * 1024;
        CdxMuxerControl(p->pMuxer, SET_FS_SIMPLE_CACHE_SIZE, &fs_cache_size);
        int fs_mode = FSWRITEMODE_SIMPLECACHE;
        CdxMuxerControl(p->pMuxer, SET_FS_WRITE_MODE, &fs_mode);

#endif
    }

    if(p->extractDataLength > 0 && p->pMuxer)
    {
        if(p->pMuxer)
		{
            CdxMuxerWriteExtraData(p->pMuxer, p->extractDataBuff, p->extractDataLength, 0);
		}
    }

    if(p->pMuxer)
    {
		CdxMuxerWriteHeader(p->pMuxer);
    }

	while(hdlTina->outport.enable && reopenFlag == 0)
	{
        //if the mTmpDataQueue has data,we should pop this queue data first
        int tmpdataQueueAudioCout = 0;
        int tmpdataQueueVideoCount = 0;
        while (!CdxQueueEmpty(p->mTmpDataQueue))
		{
		        //printf("camIndex%d:CdxQueueEmpty(p->mTmpDataQueue) is not empty\n",hdlTina->mCameraIndex);
			mPacket = CdxQueuePop(p->mTmpDataQueue);
                        if(mPacket->type == CDX_MEDIA_AUDIO){
                            tmpdataQueueAudioCout++;
                        }
                        if(mPacket->type == CDX_MEDIA_VIDEO){
                            tmpdataQueueVideoCount++;
                        }
                        //if the audio duration or video duration is larger than record time,push the packet to tmp data queue
                        if(audioDurationCount>=file_count*each_file_duration && mPacket->type == CDX_MEDIA_AUDIO){
                            //CdxQueuePush(p->mTmpDataQueue,mPacket);
                            //continue;
                            printf("hdlTina->mCameraIndex%d:err:the audioDurationCount in tmp dataqueue data is larger than one file duration\n",hdlTina->mCameraIndex);
                            printf("hdlTina->mCameraIndex%d:tmpdataQueueAudioCout = %d,mPacket->pts = %lld,baseAudioPts = %lld\n",
                                            hdlTina->mCameraIndex,tmpdataQueueAudioCout,mPacket->pts,baseAudioPts);
                            assert(0);
                        }
                        if(videoDurationCount>=file_count*each_file_duration && mPacket->type == CDX_MEDIA_VIDEO){
                            //CdxQueuePush(p->mTmpDataQueue,mPacket);
                            //continue;
                            printf("hdlTina->mCameraIndex%d:err:the videoDurationCount in tmp dataqueue data is larger than one file duration\n",hdlTina->mCameraIndex);
                            printf("hdlTina->mCameraIndex%d:tmpdataQueueVideoCount = %d,mPacket->pts = %lld,baseVideoPts = %lld\n",
                                        hdlTina->mCameraIndex,tmpdataQueueVideoCount,mPacket->pts,baseVideoPts);
                            assert(0);
                        }
                        //the other case,only need to count the duration and write the packet
                        if(mPacket->type == CDX_MEDIA_AUDIO){
                            if(baseAudioPts == -1)
                                baseAudioPts = mPacket->pts;
                            audioDurationCount = mPacket->pts -baseAudioPts;
                        }
                        if(mPacket->type == CDX_MEDIA_VIDEO){
                            if(baseVideoPts == -1)
                                baseVideoPts = mPacket->pts;
                            videoDurationCount = mPacket->pts -baseVideoPts;

                        }

			if(p->pMuxer)
			{
				if(CdxMuxerWritePacket(p->pMuxer, mPacket) < 0)
				{
					loge("+++++++ CdxMuxerWritePacket failed");
				}
			}
                        /*
                        if(mPacket->type == CDX_MEDIA_AUDIO){
                            pthread_mutex_lock(&hdlTina->mAudioPktLock);
                            hdlTina->mAudioPktUnusedCount++;
                            hdlTina->mAudioBufUnusedSize = hdlTina->mAudioBufUnusedSize + mPacket->buflen;
                            pthread_mutex_unlock(&hdlTina->mAudioPktLock);
                        }
                        if(mPacket->type == CDX_MEDIA_VIDEO){
                            pthread_mutex_lock(&hdlTina->mVideoPktLock);
                            hdlTina->mVideoPktUnusedCount++;
                            hdlTina->mVideoBufUnusedSize = hdlTina->mVideoBufUnusedSize + mPacket->buflen;
                            pthread_mutex_unlock(&hdlTina->mVideoPktLock);
                        }
                        */
                        free(mPacket->buf);
                        if(mPacket->type == CDX_MEDIA_AUDIO){
                            pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
                            if(hdlTina->mNeedAudioPktBufSpace>0){
                                hdlTina->mNeedAudioPktBufSpace -= mPacket->buflen;
                                if(hdlTina->mNeedAudioPktBufSpace <= 0){
                                    //printf("sem_post(&hdlTina->mAudioPktBufSem)\n");
                                    sem_post(&hdlTina->mAudioPktBufSem);
                                }
                            }
                            hdlTina->mAudioPktBufSizeCount -=mPacket->buflen;
                            pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
                        }
                        if(mPacket->type == CDX_MEDIA_VIDEO){
                            pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                            if(hdlTina->mNeedVideoPktBufSpace>0){
                                hdlTina->mNeedVideoPktBufSpace -= mPacket->buflen;
                                if(hdlTina->mNeedVideoPktBufSpace <= 0){
                                    //printf("sem_post(&hdlTina->mVideoPktBufSem)\n");
                                    sem_post(&hdlTina->mVideoPktBufSem);
                                }
                            }
                            hdlTina->mVideoPktBufSizeCount -=mPacket->buflen;
                            pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
                        }
                        free(mPacket);
                        //printf("hdlTina->mCameraIndex%d : tmpdataqueue:audioDurationCount = %d \n",hdlTina->mCameraIndex,audioDurationCount);
                        //printf("hdlTina->mCameraIndex%d : tmpdataqueue:videoDurationCount = %d \n",hdlTina->mCameraIndex,videoDurationCount);
		}
                int pushInTmpdataQueueAudioCount = 0;
                int pushInTmpdataQueueVideoCount = 0;
		while (reopenFlag == 0 && hdlTina->outport.enable)
		{
		        if(audioDurationCount>=file_count*each_file_duration && videoDurationCount>=file_count*each_file_duration && keyFramExitFlag==1){
                            printf("hdlTina->mCameraIndex%d:in tmpdata queue:pushInTmpdataQueueAudioCount = %d,pushInTmpdataQueueVideoCount = %d\n",
                                hdlTina->mCameraIndex,pushInTmpdataQueueAudioCount,pushInTmpdataQueueVideoCount);
                            reopenFlag = 1;
                            break;
                        }
                        sem_wait(&hdlTina->mQueueDataSem);
		        if(!CdxQueueEmpty(p->dataQueue)){
                            //printf("camIndex%d:CdxQueueEmpty(p->dataQueue) is not empty\n",hdlTina->mCameraIndex);
                            mPacket = CdxQueuePop(p->dataQueue);
                            //if the audio duration or video duration is larger than record time,push the packet to tmp data queue
                            if(audioDurationCount>=file_count*each_file_duration && mPacket->type == CDX_MEDIA_AUDIO){
                                CdxQueuePush(p->mTmpDataQueue,mPacket);
                                pushInTmpdataQueueAudioCount++;
                                continue;
                            }
                            if(videoDurationCount>=file_count*each_file_duration && mPacket->type == CDX_MEDIA_VIDEO && (mPacket->keyFrameFlag == 1 || keyFramExitFlag==1)){
                                keyFramExitFlag = 1;
                                CdxQueuePush(p->mTmpDataQueue,mPacket);
                                pushInTmpdataQueueVideoCount++;
                                continue;
                            }

                            //the other case,only need to count the duration and write the packet
                            if(mPacket->type == CDX_MEDIA_AUDIO){
                                if(baseAudioPts == -1)
                                    baseAudioPts = mPacket->pts;
                                audioDurationCount = mPacket->pts -baseAudioPts;
                                //printf("hdlTina->mCameraIndex%d:audioDurationCount = %lld\n",hdlTina->mCameraIndex ,audioDurationCount);
                            }
                            if(mPacket->type == CDX_MEDIA_VIDEO){
                                if(baseVideoPts == -1)
                                    baseVideoPts = mPacket->pts;
                                videoDurationCount = mPacket->pts -baseVideoPts;
                                //printf("hdlTina->mCameraIndex%d:videoDurationCount = %lld\n",hdlTina->mCameraIndex,videoDurationCount);
                            }
                            if(p->pMuxer)
                            {
                                if(CdxMuxerWritePacket(p->pMuxer, mPacket) < 0)
                                {
                                    loge("+++++++ CdxMuxerWritePacket failed");
                                }
                            }
                            /*
                            if(mPacket->type == CDX_MEDIA_AUDIO){
                                pthread_mutex_lock(&hdlTina->mAudioPktLock);
                                hdlTina->mAudioPktUnusedCount++;
                                hdlTina->mAudioBufUnusedSize = hdlTina->mAudioBufUnusedSize + mPacket->buflen;
                                pthread_mutex_unlock(&hdlTina->mAudioPktLock);
                            }
                            if(mPacket->type == CDX_MEDIA_VIDEO){
                                pthread_mutex_lock(&hdlTina->mVideoPktLock);
                                hdlTina->mVideoPktUnusedCount++;
                                hdlTina->mVideoBufUnusedSize = hdlTina->mVideoBufUnusedSize + mPacket->buflen;
                                pthread_mutex_unlock(&hdlTina->mVideoPktLock);
                            }
                            */
                            free(mPacket->buf);
                            if(mPacket->type == CDX_MEDIA_AUDIO){
                                pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
                                if(hdlTina->mNeedAudioPktBufSpace>0){
                                    hdlTina->mNeedAudioPktBufSpace -= mPacket->buflen;
                                    if(hdlTina->mNeedAudioPktBufSpace <= 0){
                                        //printf("sem_post(&hdlTina->mAudioPktBufSem)\n");
                                        sem_post(&hdlTina->mAudioPktBufSem);
                                    }
                                }
                                hdlTina->mAudioPktBufSizeCount -=mPacket->buflen;
                                pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
                            }
                            if(mPacket->type == CDX_MEDIA_VIDEO){
                                pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                                if(hdlTina->mNeedVideoPktBufSpace>0){
                                    hdlTina->mNeedVideoPktBufSpace -= mPacket->buflen;
                                    if(hdlTina->mNeedVideoPktBufSpace <= 0){
                                        //printf("sem_post(&hdlTina->mVideoPktBufSem)\n");
                                        sem_post(&hdlTina->mVideoPktBufSem);
                                    }
                                }
                                hdlTina->mVideoPktBufSizeCount -=mPacket->buflen;
                                pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
                            }
                            free(mPacket);
                            //printf("hdlTina->mCameraIndex%d : dataqueue:audioDurationCount = %d \n",hdlTina->mCameraIndex,audioDurationCount);
                            //printf("hdlTina->mCameraIndex%d : dataqueue:videoDurationCount = %d \n",hdlTina->mCameraIndex,videoDurationCount);
                        }
		        //usleep(1*1000);
		}
                printf("hdlTina->mCameraIndex%d:exit the while (!CdxQueueEmpty(p->dataQueue)),reopenFlag = %d\n",hdlTina->mCameraIndex,reopenFlag);
	}

        //this means the app stop record,so we should write all packet to file
        printf("hdlTina->outport.enable = %d\n",hdlTina->outport.enable);
        if(!hdlTina->outport.enable){
            while (!CdxQueueEmpty(p->mTmpDataQueue))
            {
                mPacket = CdxQueuePop(p->mTmpDataQueue);
                if(p->pMuxer)
                {
			if(CdxMuxerWritePacket(p->pMuxer, mPacket) < 0)
			{
				loge("+++++++ CdxMuxerWritePacket failed");
			}
                }
                free(mPacket->buf);
                if(mPacket->type == CDX_MEDIA_AUDIO){
                    pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
                    if(hdlTina->mNeedAudioPktBufSpace>0){
                        hdlTina->mNeedAudioPktBufSpace -= mPacket->buflen;
                        if(hdlTina->mNeedAudioPktBufSpace <= 0){
                            //printf("sem_post(&hdlTina->mAudioPktBufSem)\n");
                            sem_post(&hdlTina->mAudioPktBufSem);
                        }
                    }
                    hdlTina->mAudioPktBufSizeCount -=mPacket->buflen;
                    pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
                }
                if(mPacket->type == CDX_MEDIA_VIDEO){
                    pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                    if(hdlTina->mNeedVideoPktBufSpace>0){
                        hdlTina->mNeedVideoPktBufSpace -= mPacket->buflen;
                        if(hdlTina->mNeedVideoPktBufSpace <= 0){
                            //printf("sem_post(&hdlTina->mVideoPktBufSem)\n");
                            sem_post(&hdlTina->mVideoPktBufSem);
                        }
                    }
                    hdlTina->mVideoPktBufSizeCount -=mPacket->buflen;
                    pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
                }
                free(mPacket);
                /*
                if(mPacket->type == CDX_MEDIA_AUDIO){
                    pthread_mutex_lock(&hdlTina->mAudioPktLock);
                    hdlTina->mAudioPktUnusedCount++;
                    hdlTina->mAudioBufUnusedSize = hdlTina->mAudioBufUnusedSize + mPacket->buflen;
                    printf("cameraIndex(%d):has been stoped,write all mTmpDataQueue data to file,hdlTina->mAudioPktUnusedCount = %d\n",hdlTina->mCameraIndex,hdlTina->mAudioPktUnusedCount);
                    pthread_mutex_unlock(&hdlTina->mAudioPktLock);
                }
                if(mPacket->type == CDX_MEDIA_VIDEO){
                    pthread_mutex_lock(&hdlTina->mVideoPktLock);
                    hdlTina->mVideoPktUnusedCount++;
                    hdlTina->mVideoBufUnusedSize = hdlTina->mVideoBufUnusedSize + mPacket->buflen;
                    printf("cameraIndex(%d):has been stoped,write all mTmpDataQueue data to file,hdlTina->mVideoPktUnusedCount = %d\n",hdlTina->mCameraIndex,hdlTina->mVideoPktUnusedCount);
                    pthread_mutex_unlock(&hdlTina->mVideoPktLock);
                }
                */
            }
            while (!CdxQueueEmpty(p->dataQueue))
            {
                mPacket = CdxQueuePop(p->dataQueue);
                if(p->pMuxer)
                {
			if(CdxMuxerWritePacket(p->pMuxer, mPacket) < 0)
			{
				loge("+++++++ CdxMuxerWritePacket failed");
			}
                }
                free(mPacket->buf);
                if(mPacket->type == CDX_MEDIA_AUDIO){
                    pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
                    if(hdlTina->mNeedAudioPktBufSpace>0){
                        hdlTina->mNeedAudioPktBufSpace -= mPacket->buflen;
                        if(hdlTina->mNeedAudioPktBufSpace <= 0){
                            //printf("sem_post(&hdlTina->mAudioPktBufSem)\n");
                            sem_post(&hdlTina->mAudioPktBufSem);
                        }
                    }
                    hdlTina->mAudioPktBufSizeCount -=mPacket->buflen;
                    pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
                }
                if(mPacket->type == CDX_MEDIA_VIDEO){
                    pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                    if(hdlTina->mNeedVideoPktBufSpace>0){
                        hdlTina->mNeedVideoPktBufSpace -= mPacket->buflen;
                        if(hdlTina->mNeedVideoPktBufSpace <= 0){
                            //printf("sem_post(&hdlTina->mVideoPktBufSem)\n");
                            sem_post(&hdlTina->mVideoPktBufSem);
                        }
                    }
                    hdlTina->mVideoPktBufSizeCount -=mPacket->buflen;
                    pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
                }
                free(mPacket);
                /*
                if(mPacket->type == CDX_MEDIA_AUDIO){
                    pthread_mutex_lock(&hdlTina->mAudioPktLock);
                    hdlTina->mAudioPktUnusedCount++;
                    hdlTina->mAudioBufUnusedSize = hdlTina->mAudioBufUnusedSize + mPacket->buflen;
                    printf("cameraIndex(%d):has been stoped,write all dataQueue data to file,hdlTina->mAudioPktUnusedCount = %d\n",hdlTina->mCameraIndex,hdlTina->mAudioPktUnusedCount);
                    pthread_mutex_unlock(&hdlTina->mAudioPktLock);
                }
                if(mPacket->type == CDX_MEDIA_VIDEO){
                    pthread_mutex_lock(&hdlTina->mVideoPktLock);
                    hdlTina->mVideoPktUnusedCount++;
                    hdlTina->mVideoBufUnusedSize = hdlTina->mVideoBufUnusedSize + mPacket->buflen;
                    printf("cameraIndex(%d):has been stoped,write all dataQueue data to file,hdlTina->mVideoPktUnusedCount = %d\n",hdlTina->mCameraIndex,hdlTina->mVideoPktUnusedCount);
                    pthread_mutex_unlock(&hdlTina->mVideoPktLock);
                }
                */
            }
        }

        if(p->pMuxer)
        {
            CdxMuxerWriteTrailer(p->pMuxer);
            printf("cameraIndex(%d):CdxMuxerWriteTrailer() finish,file_count = %d\n",hdlTina->mCameraIndex,file_count);
        }

        if(p->pMuxer)
        {
		CdxMuxerClose(p->pMuxer);
            p->pMuxer = NULL;
        }
        if(p->pStream)
        {
            RWClose(p->pStream);
            CdxWriterDestroy(p->pStream);
            p->pStream = NULL;
            rw = NULL;
        }
        if(reopenFlag == 1){
            printf("hdlTina->mCameraIndex%d:file_count = %d,need to reopen another file to save the mp4\n",hdlTina->mCameraIndex,file_count);
            printf("hdlTina->mCameraIndex%d:audioDurationCount = %lld,videoDurationCount = %lld\n",hdlTina->mCameraIndex,audioDurationCount,videoDurationCount);
            goto REOPEN;
        }
 #if FS_WRITER
        //for FSWRITEMODE_CACHETHREAD mode
        /*
        if(fs_cache_mem.mp_cache)
        {
            free(fs_cache_mem.mp_cache);
            fs_cache_mem.mp_cache = NULL;
        }
        */
#endif
        p->muxThreadExitFlag = true;
        printf("muxer thread exit normal\n");
        return 0;
}

void* AudioInputThread(void *param)
{
	TinaRecorder *hdlTina = (TinaRecorder *)param;
	if(hdlTina == NULL)
		return NULL;
        int ret = -1;
        DemoRecoderContext* p = (DemoRecoderContext*)hdlTina->EncoderContext;

	int readOnceSize = 1024*hdlTina->aport.getAudioChannels(&hdlTina->aport)*2;
        int readActual = 0;

    AudioInputBuffer audioInputBuffer;
    memset(&audioInputBuffer, 0x00, sizeof(AudioInputBuffer));
	audioInputBuffer.nPts = -1;
    audioInputBuffer.nLen = readOnceSize;
    audioInputBuffer.pData = (char *)malloc(audioInputBuffer.nLen);
    int timeOf1024Frames = readOnceSize*1000/(hdlTina->aport.getAudioSampleRate(&hdlTina->aport)*hdlTina->aport.getAudioChannels(&hdlTina->aport)*2);
    printf("timeOf1024Frames = %d\n",timeOf1024Frames);
    struct timeval time1, time2, time3;
    while(hdlTina->aport.enable)
    {
		ret = -1;

	    readActual = hdlTina->aport.readData(&hdlTina->aport,audioInputBuffer.pData,readOnceSize);
		audioInputBuffer.nLen = readActual;

			if(readActual != 0)
			{
			        if(readActual != readOnceSize){
                                    printf("warnning:readActual != readOnceSize,readActual = %d,readOnceSize = %d\n",readActual,readOnceSize);
                                }
			        if(audioInputBuffer.nPts == -1){
                                    audioInputBuffer.nPts = 0;//the first audio input buffer,we should set the pts to 0
                                }else{
                                    audioInputBuffer.nPts = hdlTina->aport.getpts(&hdlTina->aport);
                                }
                                //printf("hdlTina->mCameraIndex%d:audioInputBuffer.nPts = %lld\n",hdlTina->mCameraIndex,audioInputBuffer.nPts);
                                if(hdlTina->mAudioMuteFlag == 1){//if the app set audio Mute,we should set the data 0
                                    memset(audioInputBuffer.pData,0,readActual);
                                }
	            while(ret < 0)
	            {
					ret = AwEncoderWritePCMdata(p->mAwEncoder,&audioInputBuffer);
					if(ret < 0){
                                            printf("hdlTina->mCameraIndex%d:AwEncoderWritePCMdata fail\n",hdlTina->mCameraIndex);
					    usleep(5*1000);
                                      }
			        }
			}

            usleep((timeOf1024Frames-5)*1000);
    }
    if(audioInputBuffer.pData){
        free(audioInputBuffer.pData);
        audioInputBuffer.pData == NULL;
    }
	hdlTina->aport.clear(&hdlTina->aport);
    return 0;
}

void* VideoInputThread(void *param)
{
	TinaRecorder *hdlTina = (TinaRecorder *)param;
	if(hdlTina == NULL)
		return NULL;
    DemoRecoderContext* p = (DemoRecoderContext*)hdlTina->EncoderContext;
    VideoInputBuffer videoInputBuffer;
	int bufSize = 0;
	dataParam paramData;
	unsigned char *displayBuffer;
    int ret = -1;
	static int times = 0;
	videoParam paramDisp;
	char *bufIsp;
	int sizeIsp;
	char *mainPhyAddr;
	char *subVirAddr;
	int videoFrameCount = 0;
    while(hdlTina->vport.enable)
    {
		ret = -1;
		if(!p->videoConfig.bUsePhyBuf)
			goto VIDEO_INPUT_ERROR;

		if(hdlTina->vport.dequeue(&hdlTina->vport,&hdlTina->param) != 0)
		{
			printf("dequeue error!\n");
			usleep(10*1000);
			continue;
		}

		if(hdlTina->mCaptureFlag == 1){
			captureJpegPhoto(hdlTina,hdlTina->mCaptureConfig.capturePath,&hdlTina->param);
			hdlTina->mCaptureFlag = 0;
                        sem_post(&hdlTina->mCaptureFinishSem);
		}
		videoInputBuffer.pAddrPhyY = hdlTina->param.MainYPhyAddr;
		videoInputBuffer.pAddrPhyC = hdlTina->param.MainCPhyAddr;
		videoInputBuffer.nID = hdlTina->param.bufferId;
		videoInputBuffer.nPts = hdlTina->param.pts;
		videoInputBuffer.nLen = hdlTina->param.MainSize;
		//printf("\nmainy:%x mainc:%x id:%d pts:%lld,main size:%d\n",
			//paramData.MainYPhyAddr,paramData.MainCPhyAddr,paramData.bufferId,paramData.pts,paramData.MainSize);
		//add water mark
		if(hdlTina->use_wm)
		{
			hdlTina->vport.addWM(&hdlTina->vport, hdlTina->vport.MainWidth, hdlTina->vport.MainHeight,
														hdlTina->param.MainYVirAddr, hdlTina->param.MainCVirAddr,
														hdlTina->wm_width, hdlTina->wm_height, NULL);
		}

                sem_post(&hdlTina->vport.dqfinSem);
		times++;
#if 0
		if(times % 20 == 0)
		{
			printf("encode %d frames,pts:%lld!\n",times,paramData.pts);
		}
#endif

		if(hdlTina->outport.enable || (hdlTina->dispport.enable
			&& hdlTina->dispport.route == VIDEO_SRC_FROM_VE))
		{
			while(ret < 0)
			{
				ret = AwEncoderWriteYUVdata(p->mAwEncoder,&videoInputBuffer);
				if(ret < 0)
				{
					printf("write yuv data retry!\n");
                                        usleep(5*1000);
				}
				hdlTina->lastEncodeId = hdlTina->param.bufferId;
			}
		}
		else if(hdlTina->outport.enable == 0
				&&(hdlTina->dispport.enable && hdlTina->dispport.route == VIDEO_SRC_FROM_CAM))

		{
			sem_wait(&hdlTina->dispfinSem);
			hdlTina->vport.enqueue(&hdlTina->vport, &hdlTina->param);
		}
    }
VIDEO_INPUT_ERROR:
	printf("video input thread end!\n");
    return 0;
}

int DestroyTinaRecorder(TinaRecorder *hdl)
{
	if(hdl == NULL)
		return -1;
        sem_destroy(&hdl->mCaptureFinishSem);
	if(hdl->status == TINA_RECORD_STATUS_RELEASED)
	{
		free(hdl);
	}
	else
		return -1;
	return 0;
}

int TinasetVideoInPort(void *hdl,vInPort *vPort)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;
	if(hdlTina->status == TINA_RECORD_STATUS_INIT || hdlTina->status == TINA_RECORD_STATUS_INITIALIZED)
	{
		if(vPort != NULL)
			memcpy(&hdlTina->vport, vPort, sizeof(vInPort));
		else
			hdlTina->vport.enable = 0;
		hdlTina->vinset = 1;
		if(hdlTina->ainset && hdlTina->vinset)
			hdlTina->status = TINA_RECORD_STATUS_INITIALIZED;
	}
	else
		return -1;

	return 0;
}

int TinasetAudioInPort(void *hdl,aInPort *aPort)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;

	if(hdlTina->status == TINA_RECORD_STATUS_INIT || hdlTina->status == TINA_RECORD_STATUS_INITIALIZED)
	{
		if(aPort != NULL)
			memcpy(&hdlTina->aport, aPort, sizeof(aInPort));
		else
			hdlTina->aport.enable = 0;
		hdlTina->ainset = 1;
		if(hdlTina->ainset && hdlTina->vinset)
			hdlTina->status = TINA_RECORD_STATUS_INITIALIZED;
	}
	else
		return -1;

	return 0;
}

int TinasetDispOutPort(void *hdl,dispOutPort *dispPort)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;

	if(hdlTina->status == TINA_RECORD_STATUS_INITIALIZED)
	{
		if(dispPort != NULL)
			memcpy(&hdlTina->dispport,dispPort,sizeof(dispOutPort));
		else
			hdlTina->dispport.enable = 0;
		hdlTina->dispoutset = 1;
		if(hdlTina->dispoutset && hdlTina->routset)
			hdlTina->status = TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED;
	}
	else
		return -1;

	return 0;
}

int TinasetOutput(void *hdl,rOutPort *output)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;

	if(hdlTina->status == TINA_RECORD_STATUS_INITIALIZED)
	{
		if(output != NULL)
			memcpy(&hdlTina->outport,output,sizeof(rOutPort));
		else
			hdlTina->outport.enable = 0;
		hdlTina->routset = 1;
		if(hdlTina->dispoutset && hdlTina->routset)
			hdlTina->status = TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED;
	}
	else
		return -1;

	return 0;
}

int TinasetEncodeSize(void *hdl,int width,int height)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;
	if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
	{
		hdlTina->encodeWidth = width;
		hdlTina->encodeHeight = height;
	}
	else
		return -1;
	return 0;
}

int TinasetEncodeBitRate(void *hdl,int bitrate)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;
	if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
		hdlTina->encodeBitrate = bitrate;
	else
		return -1;
	return 0;
}

int TinasetOutputFormat(void *hdl,int format)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;
	if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
		hdlTina->outputFormat = format;
	else
		return -1;
	return 0;
}
int TinasetCallback(void *hdl,TinaRecorderCallback callback,void* pUserData)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;
        printf("TinasetCallback:hdlTina->status = %d\n",hdlTina->status);
	if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED){
		hdlTina->callback = callback;
                hdlTina->pUserData = pUserData;
       }
	else
		return -1;
	return 0;
}


int TinasetEncodeVideoFormat(void *hdl,int format)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;
	if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
		hdlTina->encodeVFormat = format;
	else
		return -1;
	return 0;
}

int TinasetEncodeAudioFormat(void *hdl,int format)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;
	if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
		hdlTina->encodeAFormat = format;
	else
		return -1;
	return 0;
}

int TinagetRecorderTime(void *hdl)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;

	if(hdlTina->status == TINA_RECORD_STATUS_RECORDING)
	{
		//fixme here
		return 111;
	}
	else
		return -1;
}

int TinacaptureCurrent(void *hdl,captureConfig *capConfig)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;

	if(hdlTina->status == TINA_RECORD_STATUS_RECORDING)
	{
		//fixme here,do something
                strcpy(hdlTina->mCaptureConfig.capturePath,capConfig->capturePath);
                hdlTina->mCaptureConfig.capFormat = capConfig->capFormat;
                if(capConfig->captureWidth>8*hdlTina->vport.MainWidth){
                    printf("warnning:the capture width(%d) is 8 times larger than source width(%d),set it to%d\n",
                        capConfig->captureWidth,hdlTina->vport.MainWidth,hdlTina->vport.MainWidth*8);
                    capConfig->captureWidth = hdlTina->vport.MainWidth*8;
                }
                if(capConfig->captureHeight>8*hdlTina->vport.MainHeight){
                    printf("warnning:the capture height(%d) is 8 times larger than source height(%d),set it to%d\n",
                        capConfig->captureHeight,hdlTina->vport.MainHeight,hdlTina->vport.MainHeight*8);
                    capConfig->captureHeight = hdlTina->vport.MainHeight*8;
                }
                hdlTina->mCaptureConfig.captureWidth= capConfig->captureWidth;
                hdlTina->mCaptureConfig.captureHeight= capConfig->captureHeight;
                hdlTina->mCaptureFlag = 1;
                sem_wait(&hdlTina->mCaptureFinishSem);
                printf("capture photo finish\n");
		return 0;
	}
	else
		return -1;
}

int TinasetMaxRecordTime(void *hdl,int ms)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;
	if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
		hdlTina->maxRecordTime = ms;
	else
		return -1;
	return 0;
}

int TinasetParameter(void *hdl,int cmd,int parameter)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;
	if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
	{
		//fixme,do something here
	}
	else
		return -1;
	return 0;
}

int Tinastart(void *hdl,int flags)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	int scaleWidth,scaleHeight;
	VencThumbInfo addr;
	videoParam param;
	int enable = 1;
	int ratio;
	if(hdlTina == NULL)
		return -1;
	DemoRecoderContext *demoRecoder = (DemoRecoderContext *)hdlTina->EncoderContext;
        hdlTina->vport.getCameraIndex(&hdlTina->vport,&hdlTina->mCameraIndex);
        printf("Tinastart:hdlTina->mCameraIndex = %d\n",hdlTina->mCameraIndex);
	if(hdlTina->status == TINA_RECORD_STATUS_PREPARED)
	{
		if(!hdlTina->vport.enable)
			return -1;
		if(flags == FLAGS_RECORDING)
		{
			if(hdlTina->outport.enable == 0)
				return -1;
			hdlTina->dispport.enable = 0;
			hdlTina->startFlags = FLAGS_RECORDING;
		}
		else if(flags == FLAGS_PREVIEW)
		{
			hdlTina->outport.enable = 0;
			hdlTina->aport.enable = 0;
			hdlTina->startFlags = FLAGS_PREVIEW;
		}
		else
		{
			hdlTina->startFlags = FLAGS_ALL;
		}
		if(hdlTina->dispport.enable)
		{
			if(hdlTina->dispport.route == VIDEO_SRC_FROM_VE)
			{
				scaleWidth = ((hdlTina->encodeWidth + 15)/16*16)/hdlTina->scaleDownRatio;
				scaleHeight = ((hdlTina->encodeHeight + 15)/16*16)/hdlTina->scaleDownRatio;
				if(hdlTina->rotateAngel != ROTATION_ANGLE_0)
				{
					hdlTina->scaleDownBuf = (char *)malloc(scaleWidth*scaleHeight*3/2);
					if(hdlTina->scaleDownBuf == NULL)
					{
						printf("malloc scaledown buf fail!\n");
						return -1;
					}
					memset(hdlTina->scaleDownBuf,0,scaleWidth*scaleHeight*3/2);
				}
				else
				{
					hdlTina->scaleDownBuf = NULL;
				}

				sem_init(&hdlTina->renderSem,0,0);

				sem_init(&hdlTina->renderDataSem,0,0);
				sem_init(&hdlTina->renderFinSem,0,0);

				param.srcInfo.w = scaleWidth;
				param.srcInfo.h = scaleHeight;
				hdlTina->dispport.allocateVideoMem(&hdlTina->dispport,&param);
				hdlTina->renderStarted = 1;
				hdlTina->renderQueue = 0;
				hdlTina->renderStatus = RENDER_STATUS_LOOP;
				pthread_create(&hdlTina->renderId, NULL, TRRenderThread, hdl);
			}
			else if(hdlTina->dispport.route == VIDEO_SRC_FROM_ISP)
			{
				//fixme here,should get ISP scaled width and height
				scaleWidth = hdlTina->vport.SubWidth;
				scaleHeight = hdlTina->vport.SubHeight;
				param.srcInfo.w = scaleWidth;
				param.srcInfo.h = scaleHeight;
				hdlTina->dispport.allocateVideoMem(&hdlTina->dispport,&param);
			}
			else if(hdlTina->dispport.route == VIDEO_SRC_FROM_CAM)
			{
				sem_init(&hdlTina->dispfinSem,0,0);

				scaleWidth = hdlTina->vport.MainWidth;
				scaleHeight = hdlTina->vport.MainHeight;
				param.srcInfo.w = scaleWidth;
				param.srcInfo.h = scaleHeight;
				hdlTina->dispport.allocateVideoMem(&hdlTina->dispport,&param);
				pthread_create(&hdlTina->dirdispId, NULL, TRDirdispThread, hdl);
			}
			else
				return -1;


		}
		if(hdlTina->vport.enable)
			hdlTina->vport.startcapture(&hdlTina->vport);

		AwEncoderStart(demoRecoder->mAwEncoder);
		AwEncoderGetExtradata(demoRecoder->mAwEncoder,&demoRecoder->extractDataBuff,&demoRecoder->extractDataLength);

		if(hdlTina->dispport.enable && hdlTina->dispport.route == VIDEO_SRC_FROM_VE)
		{
			addr.pThumbBuf = hdlTina->scaleDownBuf;
			addr.nThumbSize = scaleWidth*scaleHeight*3/2;

			printf("thumb size start:%d\n",addr.nThumbSize);
			AwEncoderSetParamete(demoRecoder->mAwEncoder,AwEncoder_SetThumbNailAddr,&addr);
		}

		if(hdlTina->outport.enable)
		{
		    /*
		    int initRet = initPreMallocMem(hdlTina);
                    if(initRet == -1)
                        printf("initPreMallocMem() err\n");
                    else
                        printf("initPreMallocMem() success\n");
                    */
                    sem_init(&hdlTina->mQueueDataSem, 0, 0);
                    int initRet = initStorePktInfo(hdlTina);
                    if(initRet == -1)
                        printf("initStorePktInfo() err\n");
                    else
                        printf("initStorePktInfo() success\n");
		    pthread_create(&demoRecoder->muxerThreadId, NULL, MuxerThread, hdl);
		}

	    if(hdlTina->aport.enable)
		{
	        pthread_create(&demoRecoder->audioDataThreadId, NULL, AudioInputThread, hdl);
	    }

	    if(hdlTina->vport.enable && (demoRecoder->muxType == CDX_MUXER_MOV || demoRecoder->muxType == CDX_MUXER_TS))
	    {
	        pthread_create(&demoRecoder->videoDataThreadId, NULL, VideoInputThread, hdl);
	    }

		hdlTina->status = TINA_RECORD_STATUS_RECORDING;
		return 0;
	}
	else if(hdlTina->status == TINA_RECORD_STATUS_RECORDING)
	{
		if(hdlTina->startFlags == FLAGS_ALL || flags == hdlTina->startFlags)
		{
			printf("return do nothing!\n");
			return 0;
		}
		if(flags == FLAGS_ALL && hdlTina->startFlags == FLAGS_PREVIEW)
			flags = FLAGS_RECORDING;
		else if(flags == FLAGS_ALL && hdlTina->startFlags == FLAGS_RECORDING)
			flags = FLAGS_PREVIEW;
		printf("need to start:%d\n",flags);
		if(flags == FLAGS_PREVIEW)
		{
			if(hdlTina->dispport.route == VIDEO_SRC_FROM_VE)
			{
				scaleWidth = ((hdlTina->encodeWidth + 15)/16*16)/hdlTina->scaleDownRatio;
				scaleHeight = ((hdlTina->encodeHeight + 15)/16*16)/hdlTina->scaleDownRatio;
				if(hdlTina->rotateAngel != ROTATION_ANGLE_0)
				{
					hdlTina->scaleDownBuf = (char *)malloc(scaleWidth*scaleHeight*3/2);
					if(hdlTina->scaleDownBuf == NULL)
					{
						printf("malloc scaledown buf fail!\n");
						return -1;
					}
					memset(hdlTina->scaleDownBuf,0,scaleWidth*scaleHeight*3/2);
				}
				else
				{
					hdlTina->scaleDownBuf = NULL;
				}
				addr.pThumbBuf = hdlTina->scaleDownBuf;
				addr.nThumbSize = scaleWidth*scaleHeight*3/2;
				printf("thumb size start:%d\n",addr.nThumbSize);
				AwEncoderSetParamete(demoRecoder->mAwEncoder,AwEncoder_SetThumbNailAddr,&addr);
				if(hdlTina->scaleDownRatio == 2)
					ratio = VENC_ISP_SCALER_HALF;
				else if(hdlTina->scaleDownRatio == 4)
					ratio = VENC_ISP_SCALER_QUARTER;
				else if(hdlTina->scaleDownRatio == 8)
					ratio = VENC_ISP_SCALER_EIGHTH;
				else
					ratio = VENC_ISP_SCALER_0;
				AwEncoderSetParamete(demoRecoder->mAwEncoder,AwEncoder_SetScaleDownRatio,&ratio);
				enable = 1;
				sem_init(&hdlTina->renderSem,0,0);

				sem_init(&hdlTina->renderDataSem,0,0);
				sem_init(&hdlTina->renderFinSem,0,0);

				param.srcInfo.w = scaleWidth;
				param.srcInfo.h = scaleHeight;
				hdlTina->dispport.allocateVideoMem(&hdlTina->dispport,&param);
				hdlTina->renderStarted = 1;
				hdlTina->renderQueue = 0;
				hdlTina->renderStatus = RENDER_STATUS_LOOP;
				pthread_create(&hdlTina->renderId, NULL, TRRenderThread, hdl);
				AwEncoderSetParamete(demoRecoder->mAwEncoder,AwEncoder_SetThumbNailEnable,&enable);
			}
			else if(hdlTina->dispport.route == VIDEO_SRC_FROM_ISP)
			{
				scaleWidth = hdlTina->vport.SubWidth;
				scaleHeight = hdlTina->vport.SubHeight;
				param.srcInfo.w = scaleWidth;
				param.srcInfo.h = scaleHeight;
				hdlTina->dispport.allocateVideoMem(&hdlTina->dispport,&param);
			}
			else if(hdlTina->dispport.route == VIDEO_SRC_FROM_CAM)
			{
				scaleWidth = hdlTina->vport.MainWidth;
				scaleHeight = hdlTina->vport.MainHeight;
				param.srcInfo.w = scaleWidth;
				param.srcInfo.h = scaleHeight;
				hdlTina->dispport.allocateVideoMem(&hdlTina->dispport,&param);
			}
			else
				return -1;
			hdlTina->dispport.enable = 1;
			hdlTina->startFlags = FLAGS_ALL;
		}
		else if(flags == FLAGS_RECORDING)
		{
		        /*
		        int initRet = initPreMallocMem(hdlTina);
                        if(initRet == -1)
                            printf("initPreMallocMem() err\n");
                        else
                            printf("initPreMallocMem() success\n");
                        */
                        sem_init(&hdlTina->mQueueDataSem, 0, 0);
                        int initRet = initStorePktInfo(hdlTina);
                        if(initRet == -1)
                            printf("initStorePktInfo() err\n");
                        else
                            printf("initStorePktInfo() success\n");
			hdlTina->outport.enable = 1;
			hdlTina->aport.enable = 1;
                        hdlTina->mNeedMuxerVideoFlag = 0;
			if(hdlTina->outport.enable)
			{
			    pthread_create(&demoRecoder->muxerThreadId, NULL, MuxerThread, hdl);
			}
		    if(hdlTina->aport.enable)
			{
		        pthread_create(&demoRecoder->audioDataThreadId, NULL, AudioInputThread, hdl);
		    }
			hdlTina->startFlags = FLAGS_ALL;
	}
	}
		return -1;
}

int Tinarelease(void *hdl)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;
	if(hdlTina->status == TINA_RECORD_STATUS_INIT)
		hdlTina->status = TINA_RECORD_STATUS_RELEASED;
	else
		return -1;

	return 0;
}

int Tinaprepare(void *hdl)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdlTina == NULL)
		return -1;

    EncDataCallBackOps *mEncDataCallBackOps = (EncDataCallBackOps *)malloc(sizeof(EncDataCallBackOps));
    mEncDataCallBackOps->onAudioDataEnc = onAudioDataEnc;
    mEncDataCallBackOps->onVideoDataEnc = onVideoDataEnc;
	hdlTina->rotateAngel = ROTATION_ANGLE_0;
	if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
	{
	    hdlTina->mNeedMuxerVideoFlag=0;
	    //* create the demoRecoder.
	    DemoRecoderContext *demoRecoder = (DemoRecoderContext *)malloc(sizeof(DemoRecoderContext));
		if(demoRecoder == NULL)
			goto error_release;
	    memset(demoRecoder, 0, sizeof(DemoRecoderContext));
		demoRecoder->ops = mEncDataCallBackOps;
		hdlTina->EncoderContext = (void *)demoRecoder;

	    //create the queue to store the encoded video data and encoded audio data
	    demoRecoder->mDataPool = AwPoolCreate(NULL);
		demoRecoder->dataQueue = CdxQueueCreate(demoRecoder->mDataPool);

            //create the queue to store the encoded video data and encoded audio data,which is get from dataQueue
	    demoRecoder->mTmpDataPool = AwPoolCreate(NULL);
		demoRecoder->mTmpDataQueue = CdxQueueCreate(demoRecoder->mTmpDataPool);

	    //the total record duration
	    demoRecoder->mRecordDuration = hdlTina->maxRecordTime/1000;

		switch(hdlTina->outputFormat)
		{
			case TR_OUTPUT_TS:
				demoRecoder->muxType       = CDX_MUXER_TS;
				break;
			case TR_OUTPUT_AAC:
				if(hdlTina->vport.enable != 0)
				{
					printf("vport not disable when record AAC!\n");
					goto error_release;
				}
				demoRecoder->muxType       = CDX_MUXER_AAC;
				break;
			case TR_OUTPUT_MP3:
				if(hdlTina->vport.enable != 0)
				{
					printf("vport not disable when record MP3!\n");
					goto error_release;
				}
				demoRecoder->muxType       = CDX_MUXER_MP3;
				break;
			case TR_OUTPUT_MOV:
	            demoRecoder->muxType       = CDX_MUXER_MOV;
				break;
			case TR_OUTPUT_JPG:
				if(hdlTina->aport.enable != 0)
				{
					printf("aport not disable when taking picture!\n");
					goto error_release;
				}
				break;
			default:
				printf("output format not supported!\n");
				goto error_release;
				break;
		}

	    //config VideoEncodeConfig
	    memset(&demoRecoder->videoConfig, 0x00, sizeof(VideoEncodeConfig));
	    if(hdlTina->vport.enable && (demoRecoder->muxType == CDX_MUXER_MOV || demoRecoder->muxType == CDX_MUXER_TS))
		{
			switch(hdlTina->encodeVFormat)
			{
				case TR_VIDEO_H264:
					demoRecoder->videoConfig.nType	   = VIDEO_ENCODE_H264;
					break;
				case TR_VIDEO_MJPEG:
					demoRecoder->videoConfig.nType	   = VIDEO_ENCODE_JPEG;
					break;
				default:
					printf("video encoder format not support!\n");
					goto error_release;
					break;
			}

			switch(hdlTina->vport.format)
			{
				case TR_PIXEL_YUV420SP:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YUV420_NV12;
					break;
				case TR_PIXEL_YVU420SP:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YUV420_NV21;
					break;
				case TR_PIXEL_YUV420P:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YUV420_YU12;
					break;
				case TR_PIXEL_YVU420P:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YVU420_YV12;
					break;
				case TR_PIXEL_YUV422SP:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YUV422SP;
					break;
				case TR_PIXEL_YVU422SP:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YVU422SP;
					break;
				case TR_PIXEL_YUV422P:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YUV422P;
					break;
				case TR_PIXEL_YVU422P:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YVU422P;
					break;
				case TR_PIXEL_YUYV422:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YUYV422;
					break;
				case TR_PIXEL_UYVY422:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_UYVY422;
					break;
				case TR_PIXEL_YVYU422:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YVYU422;
					break;
				case TR_PIXEL_VYUY422:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_VYUY422;
					break;
				case TR_PIXEL_ARGB:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_ARGB;
					break;
				case TR_PIXEL_RGBA:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_RGBA;
					break;
				case TR_PIXEL_ABGR:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_ABGR;
					break;
				case TR_PIXEL_BGRA:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_BGRA;
					break;
				case TR_PIXEL_TILE_32X32:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_YUV420_MB32;
					break;
				case TR_PIXEL_TILE_128X32:
					demoRecoder->videoConfig.nInputYuvFormat = VIDEO_PIXEL_TILE_128X32;
					break;
				default:
					printf("input pixel format is not supported!\n");
					goto error_release;
					break;
			}

			hdlTina->vport.getSize(&hdlTina->vport,&demoRecoder->videoConfig.nSrcWidth,&demoRecoder->videoConfig.nSrcHeight);
			demoRecoder->videoConfig.nOutHeight  = hdlTina->encodeHeight;
			demoRecoder->videoConfig.nOutWidth   = hdlTina->encodeWidth;
			demoRecoder->videoConfig.nBitRate  = hdlTina->encodeBitrate;
			demoRecoder->videoConfig.nFrameRate = hdlTina->encodeFramerate;
			demoRecoder->videoConfig.bUsePhyBuf  = 1;
			if(hdlTina->dispport.enable && hdlTina->dispport.route == VIDEO_SRC_FROM_VE)
			{
				if(hdlTina->scaleDownRatio == 2)
					demoRecoder->videoConfig.ratio = VENC_ISP_SCALER_HALF;
				else if(hdlTina->scaleDownRatio == 4)
					demoRecoder->videoConfig.ratio = VENC_ISP_SCALER_QUARTER;
				else if(hdlTina->scaleDownRatio == 8)
					demoRecoder->videoConfig.ratio = VENC_ISP_SCALER_EIGHTH;
				else
					demoRecoder->videoConfig.ratio = VENC_ISP_SCALER_0;
			}
	    }
		if(hdlTina->aport.enable)
		{
	        //config AudioEncodeConfig
	        memset(&demoRecoder->audioConfig, 0x00, sizeof(AudioEncodeConfig));
			switch(hdlTina->encodeAFormat)
			{
				case TR_AUDIO_PCM:
					demoRecoder->audioConfig.nType		= AUDIO_ENCODE_PCM_TYPE;
					break;
				case TR_AUDIO_AAC:
					demoRecoder->audioConfig.nType		= AUDIO_ENCODE_AAC_TYPE;
					break;
				case TR_AUDIO_MP3:
					demoRecoder->audioConfig.nType		= AUDIO_ENCODE_MP3_TYPE;
					break;
				case TR_AUDIO_LPCM:
					demoRecoder->audioConfig.nType		= AUDIO_ENCODE_LPCM_TYPE;
					break;
				default:
					printf("audio format not supported!\n");
					goto error_release;
					break;
			}

	        demoRecoder->audioConfig.nInChan = hdlTina->aport.getAudioChannels(&hdlTina->aport);
	        demoRecoder->audioConfig.nInSamplerate = hdlTina->aport.getAudioSampleRate(&hdlTina->aport);
	        demoRecoder->audioConfig.nOutChan = demoRecoder->audioConfig.nInChan;
	        demoRecoder->audioConfig.nOutSamplerate = hdlTina->aport.getAudioSampleRate(&hdlTina->aport);
	        demoRecoder->audioConfig.nSamplerBits = 16;

	        if(demoRecoder->muxType == CDX_MUXER_TS && demoRecoder->audioConfig.nType == AUDIO_ENCODE_PCM_TYPE)
	        {
	            demoRecoder->audioConfig.nFrameStyle = 2;
	        }

	        if(demoRecoder->muxType == CDX_MUXER_TS && demoRecoder->audioConfig.nType == AUDIO_ENCODE_AAC_TYPE)
	        {
	            demoRecoder->audioConfig.nFrameStyle = 1;//not add head when encode aac,because use ts muxer
	        }

	        if(demoRecoder->muxType == CDX_MUXER_AAC)
	        {
	            demoRecoder->audioConfig.nType = AUDIO_ENCODE_AAC_TYPE;
	            demoRecoder->audioConfig.nFrameStyle = 0;//add head when encode aac
	        }

	        if(demoRecoder->muxType == CDX_MUXER_MP3)
	        {
	            demoRecoder->audioConfig.nType = AUDIO_ENCODE_MP3_TYPE;
	        }
		}

		if(hdlTina->outport.enable)
		{
		    //store the muxer file path
		    strcpy(hdlTina->mOutputPath,hdlTina->outport.url);
		}

		demoRecoder->mAwEncoder = AwEncoderCreate(hdl);

		if(demoRecoder->mAwEncoder == NULL)
		{
			printf("can not create AwRecorder, quit.\n");
			goto error_release;
		}

		//* set callback to recoder,if the encoder has used the buf ,it will callback to app
		AwEncoderSetNotifyCallback(demoRecoder->mAwEncoder,NotifyCallbackForAwEncorder,hdl);

		if(hdlTina->aport.enable && (demoRecoder->muxType == CDX_MUXER_AAC || demoRecoder->muxType == CDX_MUXER_MP3))
		{
			AwEncoderInit(demoRecoder->mAwEncoder, NULL, &demoRecoder->audioConfig,mEncDataCallBackOps);
		}
		else if(hdlTina->vport.enable && !hdlTina->aport.enable && (demoRecoder->muxType == CDX_MUXER_MOV || demoRecoder->muxType == CDX_MUXER_TS))
		{
			AwEncoderInit(demoRecoder->mAwEncoder, &demoRecoder->videoConfig, NULL,mEncDataCallBackOps);
		}
		else if(hdlTina->vport.enable && hdlTina->aport.enable && (demoRecoder->muxType == CDX_MUXER_MOV || demoRecoder->muxType == CDX_MUXER_TS))
		{
			AwEncoderInit(demoRecoder->mAwEncoder, &demoRecoder->videoConfig, &demoRecoder->audioConfig,mEncDataCallBackOps);
		}
	}
	else{
                if(mEncDataCallBackOps){
                    free(mEncDataCallBackOps);
                    mEncDataCallBackOps = NULL;
                }
		return -1;
       }
prepare_success:
	hdlTina->status = TINA_RECORD_STATUS_PREPARED;
	return 0;

error_release:
	if(hdlTina->EncoderContext != NULL)
	{
	    DemoRecoderContext *p = (DemoRecoderContext *)hdlTina->EncoderContext;
	    CdxQueueDestroy(p->dataQueue);
	    AwPoolDestroy(p->mDataPool);
            CdxQueueDestroy(p->mTmpDataQueue);
	    AwPoolDestroy(p->mTmpDataPool);
		free(p->ops);
		free(p);
	}
	hdlTina->EncoderContext = NULL;
	return -1;
}

int Tinareset(void *hdl)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	int needStopCapture;
	if(hdlTina == NULL)
		return -1;
	DemoRecoderContext *demoRecoder = (DemoRecoderContext *)hdlTina->EncoderContext;
	AwEncoder *w;
	if(demoRecoder != NULL)
	{
		w = demoRecoder->mAwEncoder;
	}

	CdxMuxerPacketT *mPacket = NULL;
	if(hdlTina->status == TINA_RECORD_STATUS_INITIALIZED)
	{
		memset(&hdlTina->vport, 0, sizeof(vInPort));
		memset(&hdlTina->aport, 0, sizeof(aInPort));
	}
	else if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
	{
		clearConfigure(hdlTina);
	}
	else if(hdlTina->status == TINA_RECORD_STATUS_PREPARED)
	{
		if(w != NULL)
			AwEncoderDestory(w);
		CdxQueueDestroy(demoRecoder->dataQueue);
		AwPoolDestroy(demoRecoder->mDataPool);
                CdxQueueDestroy(demoRecoder->mTmpDataQueue);
		AwPoolDestroy(demoRecoder->mTmpDataPool);
		free(demoRecoder->ops);
		free(demoRecoder);
		hdlTina->EncoderContext = NULL;
		clearConfigure(hdlTina);
	}
	else if(hdlTina->status == TINA_RECORD_STATUS_RECORDING)
	{
		if(hdlTina->vport.enable)
		{
			needStopCapture = 1;
		}
		else
		{
			needStopCapture = 0;
		}

		hdlTina->vport.enable = 0;
		hdlTina->aport.enable = 0;
        if(demoRecoder->audioDataThreadId)
            pthread_join(demoRecoder->audioDataThreadId, NULL);
        if(demoRecoder->videoDataThreadId)
            pthread_join(demoRecoder->videoDataThreadId, NULL);

		if(needStopCapture)
			hdlTina->vport.stopcapture(&hdlTina->vport);

		hdlTina->outport.enable = 0;
        sem_post(&hdlTina->mQueueDataSem);
        if(demoRecoder->muxerThreadId)
            pthread_join(demoRecoder->muxerThreadId, NULL);
        while (!CdxQueueEmpty(demoRecoder->dataQueue))
        {
            mPacket = CdxQueuePop(demoRecoder->dataQueue);
            free(mPacket->buf);
            if(mPacket->type == CDX_MEDIA_AUDIO){
                pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
                if(hdlTina->mNeedAudioPktBufSpace>0){
                    hdlTina->mNeedAudioPktBufSpace -= mPacket->buflen;
                    if(hdlTina->mNeedAudioPktBufSpace <= 0){
                        //printf("sem_post(&hdlTina->mAudioPktBufSem)\n");
                        sem_post(&hdlTina->mAudioPktBufSem);
                    }
                }
                hdlTina->mAudioPktBufSizeCount -=mPacket->buflen;
                pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
            }
            if(mPacket->type == CDX_MEDIA_VIDEO){
                pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                if(hdlTina->mNeedVideoPktBufSpace>0){
                    hdlTina->mNeedVideoPktBufSpace -= mPacket->buflen;
                    if(hdlTina->mNeedVideoPktBufSpace <= 0){
                        //printf("sem_post(&hdlTina->mVideoPktBufSem)\n");
                        sem_post(&hdlTina->mVideoPktBufSem);
                    }
                }
                hdlTina->mVideoPktBufSizeCount -=mPacket->buflen;
                pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
            }
            free(mPacket);
        }
        while (!CdxQueueEmpty(demoRecoder->mTmpDataQueue))
        {
            mPacket = CdxQueuePop(demoRecoder->mTmpDataQueue);
            free(mPacket->buf);
            if(mPacket->type == CDX_MEDIA_AUDIO){
                pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
                if(hdlTina->mNeedAudioPktBufSpace>0){
                    hdlTina->mNeedAudioPktBufSpace -= mPacket->buflen;
                    if(hdlTina->mNeedAudioPktBufSpace <= 0){
                        //printf("sem_post(&hdlTina->mAudioPktBufSem)\n");
                        sem_post(&hdlTina->mAudioPktBufSem);
                    }
                }
                hdlTina->mAudioPktBufSizeCount -=mPacket->buflen;
                pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
            }
            if(mPacket->type == CDX_MEDIA_VIDEO){
                pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                if(hdlTina->mNeedVideoPktBufSpace>0){
                    hdlTina->mNeedVideoPktBufSpace -= mPacket->buflen;
                    if(hdlTina->mNeedVideoPktBufSpace <= 0){
                        //printf("sem_post(&hdlTina->mVideoPktBufSem)\n");
                        sem_post(&hdlTina->mVideoPktBufSem);
                    }
                }
                hdlTina->mVideoPktBufSizeCount -=mPacket->buflen;
                pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
            }
            free(mPacket);
        }
		if(w != NULL)
		{
			AwEncoderStop(w);
			AwEncoderDestory(w);
		}
		CdxQueueDestroy(demoRecoder->dataQueue);
		AwPoolDestroy(demoRecoder->mDataPool);
                CdxQueueDestroy(demoRecoder->mTmpDataQueue);
		AwPoolDestroy(demoRecoder->mTmpDataPool);
		free(demoRecoder->ops);
		free(demoRecoder);
		hdlTina->EncoderContext = NULL;
                /*
                int destroyRet = destroyPreMallocMem(hdlTina);
                if(destroyRet == 0)
                    printf("destroyPreMallocMem() success\n");
                else
                    printf("destroyPreMallocMem() err\n");
                */
                int destroyRet = destroyStorePktInfo(hdlTina);
                if(destroyRet == 0)
                    printf("destroyStorePktInfo() success\n");
                else
                    printf("destroyStorePktInfo() err\n");
                sem_destroy(&hdlTina->mQueueDataSem);
		if(hdlTina->dispport.enable)
		{
			hdlTina->dispport.enable = 0;
			if(hdlTina->dispport.route == VIDEO_SRC_FROM_VE)
			{
				sem_post(&hdlTina->renderDataSem);
				sem_wait(&hdlTina->renderSem);
				if(hdlTina->rotateAngel != ROTATION_ANGLE_0)
				{
					free(hdlTina->scaleDownBuf);
				}
				hdlTina->scaleDownBuf = NULL;
			}
			else
			{
				usleep(10*1000);
			}
			hdlTina->dispport.freeVideoMem(&hdlTina->dispport);
		}
		clearConfigure(hdlTina);
	}

	hdlTina->status = TINA_RECORD_STATUS_INIT;
	return 0;
}

static void cleanPreviewStatus(TinaRecorder *hdlTina)
{
	DemoRecoderContext *demoRecoder = (DemoRecoderContext *)hdlTina->EncoderContext;
	CdxMuxerPacketT *mPacket = NULL;
	int needStopCapture = 0;
	if(hdlTina->vport.enable)
		needStopCapture = 1;
	hdlTina->vport.enable = 0;
	if(demoRecoder->videoDataThreadId)
		pthread_join(demoRecoder->videoDataThreadId, NULL);

	if(needStopCapture)
		hdlTina->vport.stopcapture(&hdlTina->vport);

	if(demoRecoder->mAwEncoder != NULL)
	{
		AwEncoderStop(demoRecoder->mAwEncoder);
		AwEncoderDestory(demoRecoder->mAwEncoder);
	}

	if(hdlTina->dispport.enable)
	{
		hdlTina->dispport.enable = 0;
		if(hdlTina->dispport.route == VIDEO_SRC_FROM_VE)
		{
			sem_post(&hdlTina->renderDataSem);
			sem_wait(&hdlTina->renderSem);
			if(hdlTina->rotateAngel != ROTATION_ANGLE_0)
			{
				free(hdlTina->scaleDownBuf);
			}
			hdlTina->scaleDownBuf = NULL;
		}
		else
		{
			usleep(10*1000);
		}
		hdlTina->dispport.freeVideoMem(&hdlTina->dispport);
	}

	CdxQueueDestroy(demoRecoder->dataQueue);
	AwPoolDestroy(demoRecoder->mDataPool);
	CdxQueueDestroy(demoRecoder->mTmpDataQueue);
	AwPoolDestroy(demoRecoder->mTmpDataPool);
	free(demoRecoder->ops);
	free(demoRecoder);
	hdlTina->EncoderContext = NULL;
	clearConfigure(hdlTina);

	hdlTina->status = TINA_RECORD_STATUS_INIT;
}

static void cleanRecordStatus(TinaRecorder *hdlTina)
{
	DemoRecoderContext *demoRecoder = (DemoRecoderContext *)hdlTina->EncoderContext;
	CdxMuxerPacketT *mPacket = NULL;
	int needStopCapture = 0;

	if(hdlTina->vport.enable)
		needStopCapture = 1;

	hdlTina->vport.enable = 0;
	hdlTina->aport.enable = 0;
	if(demoRecoder->audioDataThreadId)
		pthread_join(demoRecoder->audioDataThreadId, NULL);
	if(demoRecoder->videoDataThreadId)
		pthread_join(demoRecoder->videoDataThreadId, NULL);

	if(needStopCapture)
		hdlTina->vport.stopcapture(&hdlTina->vport);

	hdlTina->outport.enable = 0;
        sem_post(&hdlTina->mQueueDataSem);
	if(demoRecoder->muxerThreadId)
		pthread_join(demoRecoder->muxerThreadId, NULL);
	while (!CdxQueueEmpty(demoRecoder->dataQueue))
	{
		mPacket = CdxQueuePop(demoRecoder->dataQueue);
                free(mPacket->buf);
                if(mPacket->type == CDX_MEDIA_AUDIO){
                    pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
                    if(hdlTina->mNeedAudioPktBufSpace>0){
                        hdlTina->mNeedAudioPktBufSpace -= mPacket->buflen;
                        if(hdlTina->mNeedAudioPktBufSpace <= 0){
                            //printf("sem_post(&hdlTina->mAudioPktBufSem)\n");
                            sem_post(&hdlTina->mAudioPktBufSem);
                        }
                    }
                    hdlTina->mAudioPktBufSizeCount -=mPacket->buflen;
                    pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
                }
                if(mPacket->type == CDX_MEDIA_VIDEO){
                    pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                    if(hdlTina->mNeedVideoPktBufSpace>0){
                        hdlTina->mNeedVideoPktBufSpace -= mPacket->buflen;
                        if(hdlTina->mNeedVideoPktBufSpace <= 0){
                            //printf("sem_post(&hdlTina->mVideoPktBufSem)\n");
                            sem_post(&hdlTina->mVideoPktBufSem);
                        }
                    }
                    hdlTina->mVideoPktBufSizeCount -=mPacket->buflen;
                    pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
                }
                free(mPacket);
	}
	while (!CdxQueueEmpty(demoRecoder->mTmpDataQueue))
	{
		mPacket = CdxQueuePop(demoRecoder->mTmpDataQueue);
                free(mPacket->buf);
                if(mPacket->type == CDX_MEDIA_AUDIO){
                    pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
                    if(hdlTina->mNeedAudioPktBufSpace>0){
                        hdlTina->mNeedAudioPktBufSpace -= mPacket->buflen;
                        if(hdlTina->mNeedAudioPktBufSpace <= 0){
                            //printf("sem_post(&hdlTina->mAudioPktBufSem)\n");
                            sem_post(&hdlTina->mAudioPktBufSem);
                        }
                    }
                    hdlTina->mAudioPktBufSizeCount -=mPacket->buflen;
                    pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
                }
                if(mPacket->type == CDX_MEDIA_VIDEO){
                    pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                    if(hdlTina->mNeedVideoPktBufSpace>0){
                        hdlTina->mNeedVideoPktBufSpace -= mPacket->buflen;
                        if(hdlTina->mNeedVideoPktBufSpace <= 0){
                            //printf("sem_post(&hdlTina->mVideoPktBufSem)\n");
                            sem_post(&hdlTina->mVideoPktBufSem);
                        }
                    }
                    hdlTina->mVideoPktBufSizeCount -=mPacket->buflen;
                    pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
                }
                free(mPacket);
	}
	if(demoRecoder->mAwEncoder != NULL)
	{
		AwEncoderStop(demoRecoder->mAwEncoder);
		AwEncoderDestory(demoRecoder->mAwEncoder);
	}
	CdxQueueDestroy(demoRecoder->dataQueue);
	AwPoolDestroy(demoRecoder->mDataPool);
	CdxQueueDestroy(demoRecoder->mTmpDataQueue);
	AwPoolDestroy(demoRecoder->mTmpDataPool);
	free(demoRecoder->ops);
	free(demoRecoder);
	hdlTina->EncoderContext = NULL;
        /*
        int destroyRet = destroyPreMallocMem(hdlTina);
        if(destroyRet == 0)
            printf("destroyPreMallocMem() success\n");
        else
            printf("destroyPreMallocMem() err\n");
        */
        int destroyRet = destroyStorePktInfo(hdlTina);
        if(destroyRet == 0)
            printf("destroyStorePktInfo() success\n");
        else
            printf("destroyStorePktInfo() err\n");
        sem_destroy(&hdlTina->mQueueDataSem);
	clearConfigure(hdlTina);

	hdlTina->status = TINA_RECORD_STATUS_INIT;
}

int Tinastop(void *hdl,int flags)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
        if(hdlTina == NULL)
		return -1;
	int enable;
	DemoRecoderContext *demoRecoder = (DemoRecoderContext *)hdlTina->EncoderContext;
	CdxMuxerPacketT *mPacket = NULL;
	if(hdlTina->status != TINA_RECORD_STATUS_RECORDING)
		return -1;
	else
	{
		if(flags == FLAGS_ALL && hdlTina->startFlags == FLAGS_ALL)
		{
			printf("reset all route!\n");
			Tinareset(hdl);
			return 0;
		}else if(flags == FLAGS_ALL && hdlTina->startFlags == FLAGS_PREVIEW)
		{
			cleanPreviewStatus(hdlTina);
			return 0;
		}
		else if(flags == FLAGS_ALL && hdlTina->startFlags == FLAGS_RECORDING)
		{
			cleanRecordStatus(hdlTina);
			return 0;
		}
		else if(flags == FLAGS_PREVIEW && hdlTina->startFlags == FLAGS_PREVIEW)
		{
			cleanPreviewStatus(hdlTina);
			return 0;
		}
		else if(flags == FLAGS_PREVIEW && hdlTina->startFlags == FLAGS_ALL)
		{
			flags = FLAGS_PREVIEW;
		}
		else if(flags == FLAGS_RECORDING && hdlTina->startFlags == FLAGS_ALL)
		{
			flags = FLAGS_RECORDING;
		}
		else if(flags == FLAGS_RECORDING && hdlTina->startFlags == FLAGS_RECORDING)
		{
			cleanRecordStatus(hdlTina);
			return 0;
		}
		else
			return -1;

		printf("need to stop:%d\n",flags);
		if(flags == FLAGS_PREVIEW)
		{
			if(hdlTina->dispport.route == VIDEO_SRC_FROM_VE)
			{
				//close VE thumbnail
				enable = 0;
				AwEncoderSetParamete(demoRecoder->mAwEncoder,AwEncoder_SetThumbNailEnable,&enable);

				//close render thread
				hdlTina->dispport.enable = 0;
				sem_wait(&hdlTina->renderSem);

				//release DE memory
				hdlTina->dispport.freeVideoMem(&hdlTina->dispport);

				//free scale down buffer

				if(hdlTina->rotateAngel != ROTATION_ANGLE_0)
					free(hdlTina->scaleDownBuf);
			}
			else if(hdlTina->dispport.route == VIDEO_SRC_FROM_ISP)
			{
				hdlTina->dispport.enable = 0;
				//wait a moment till when the display is showing
				usleep(10*1000);
				//free DE memory
				hdlTina->dispport.freeVideoMem(&hdlTina->dispport);
			}
			else if(hdlTina->dispport.route == VIDEO_SRC_FROM_CAM)
			{
				hdlTina->dispport.enable = 0;
				usleep(10*1000);
				hdlTina->dispport.freeVideoMem(&hdlTina->dispport);
			}
			else
				return -1;
			hdlTina->startFlags = FLAGS_RECORDING;
		}
		else if(flags == FLAGS_RECORDING)
		{
			//close muxer thread
			hdlTina->outport.enable = 0;
                        sem_post(&hdlTina->mQueueDataSem);
			if(demoRecoder->muxerThreadId)
				pthread_join(demoRecoder->muxerThreadId, NULL);

			//close audio thread
			hdlTina->aport.enable = 0;
			if(demoRecoder->audioDataThreadId)
				pthread_join(demoRecoder->audioDataThreadId, NULL);

			//clear muxer related resources
			while (!CdxQueueEmpty(demoRecoder->dataQueue))
			{
				mPacket = CdxQueuePop(demoRecoder->dataQueue);
                                free(mPacket->buf);
                                if(mPacket->type == CDX_MEDIA_AUDIO){
                                    pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
                                    if(hdlTina->mNeedAudioPktBufSpace>0){
                                        hdlTina->mNeedAudioPktBufSpace -= mPacket->buflen;
                                        if(hdlTina->mNeedAudioPktBufSpace <= 0){
                                            //printf("sem_post(&hdlTina->mAudioPktBufSem)\n");
                                            sem_post(&hdlTina->mAudioPktBufSem);
                                        }
                                    }
                                    hdlTina->mAudioPktBufSizeCount -=mPacket->buflen;
                                    pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
                                }
                                if(mPacket->type == CDX_MEDIA_VIDEO){
                                    pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                                    if(hdlTina->mNeedVideoPktBufSpace>0){
                                        hdlTina->mNeedVideoPktBufSpace -= mPacket->buflen;
                                        if(hdlTina->mNeedVideoPktBufSpace <= 0){
                                            //printf("sem_post(&hdlTina->mVideoPktBufSem)\n");
                                            sem_post(&hdlTina->mVideoPktBufSem);
                                        }
                                    }
                                    hdlTina->mVideoPktBufSizeCount -=mPacket->buflen;
                                    pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
                                }
                                free(mPacket);
			}

			while (!CdxQueueEmpty(demoRecoder->mTmpDataQueue))
			{
				mPacket = CdxQueuePop(demoRecoder->mTmpDataQueue);
                                free(mPacket->buf);
                                if(mPacket->type == CDX_MEDIA_AUDIO){
                                    pthread_mutex_lock(&hdlTina->mAudioPktBufLock);
                                    if(hdlTina->mNeedAudioPktBufSpace>0){
                                        hdlTina->mNeedAudioPktBufSpace -= mPacket->buflen;
                                        if(hdlTina->mNeedAudioPktBufSpace <= 0){
                                            //printf("sem_post(&hdlTina->mAudioPktBufSem)\n");
                                            sem_post(&hdlTina->mAudioPktBufSem);
                                        }
                                    }
                                    hdlTina->mAudioPktBufSizeCount -=mPacket->buflen;
                                    pthread_mutex_unlock(&hdlTina->mAudioPktBufLock);
                                }
                                if(mPacket->type == CDX_MEDIA_VIDEO){
                                    pthread_mutex_lock(&hdlTina->mVideoPktBufLock);
                                    if(hdlTina->mNeedVideoPktBufSpace>0){
                                        hdlTina->mNeedVideoPktBufSpace -= mPacket->buflen;
                                        if(hdlTina->mNeedVideoPktBufSpace <= 0){
                                            //printf("sem_post(&hdlTina->mVideoPktBufSem)\n");
                                            sem_post(&hdlTina->mVideoPktBufSem);
                                        }
                                    }
                                    hdlTina->mVideoPktBufSizeCount -=mPacket->buflen;
                                    pthread_mutex_unlock(&hdlTina->mVideoPktBufLock);
                                }
                                free(mPacket);
			}
                        /*
                        int destroyRet = destroyPreMallocMem(hdlTina);
                        if(destroyRet == 0)
                            printf("destroyPreMallocMem() success\n");
                        else
                            printf("destroyPreMallocMem() err\n");
                        */
                        int destroyRet = destroyStorePktInfo(hdlTina);
                        if(destroyRet == 0)
                            printf("destroyStorePktInfo() success\n");
                        else
                            printf("destroyStorePktInfo() err\n");
                        sem_destroy(&hdlTina->mQueueDataSem);
			hdlTina->startFlags = FLAGS_PREVIEW;
		}
	}
	return 0;
}


int TinasetEncodeFramerate(void *hdl,int framerate)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
	if(hdl == NULL)
		return -1;
	if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
		hdlTina->encodeFramerate = framerate;
	else
		return -1;
	return 0;
}

int TinasetVEScaleDownRatio(void *hdl,int ratio)
{
	TinaRecorder *hdlTina = (TinaRecorder *)hdl;
        if(hdlTina == NULL)
                return -1;
        DemoRecoderContext* p = (DemoRecoderContext*)hdlTina->EncoderContext;
	int scaleWidth,scaleHeight;
	videoParam param;
	VencThumbInfo addr;
	int enable = 0;
	int newratio = 0;
		if(hdlTina->status == TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED)
	{
		hdlTina->scaleDownRatio = ratio;
	}
	else if(hdlTina->status == TINA_RECORD_STATUS_RECORDING)
	{
		if(hdlTina->dispport.enable && hdlTina->dispport.route == VIDEO_SRC_FROM_VE)
		{
			//get new ratio from app
			hdlTina->scaleDownRatio = ratio;
			if(ratio != 2 && ratio != 4 && ratio != 8)
			{
				printf("unsupported scale down ratio!\n");
				return -1;
			}
			//close data copy from VE
			//fixme,to do here
			enable = 0;
			AwEncoderSetParamete(p->mAwEncoder,AwEncoder_SetThumbNailEnable,&enable);
			usleep(30*1000);
			//close data render from TR scale down buffer
			hdlTina->renderStatus = RENDER_STATUS_PAUSE;
			sem_wait(&hdlTina->renderSem);
			//reconfig scaledown buffer and display buffer
			hdlTina->dispport.freeVideoMem(&hdlTina->dispport);

			if(hdlTina->rotateAngel != ROTATION_ANGLE_0)
				free(hdlTina->scaleDownBuf);
			scaleWidth = ((hdlTina->encodeWidth + 15)/16*16)/hdlTina->scaleDownRatio;
			scaleHeight = ((hdlTina->encodeHeight + 15)/16*16)/hdlTina->scaleDownRatio;

			if(hdlTina->rotateAngel != ROTATION_ANGLE_0)
			{
				hdlTina->scaleDownBuf = (char *)malloc(scaleWidth*scaleHeight*3/2);
				if(hdlTina->scaleDownBuf == NULL)
				{
					hdlTina->renderStatus = RENDER_STATUS_STOP;
					hdlTina->renderStarted = 0;
					printf("malloc scaledown buf fail!\n");
					return -1;
				}
				memset(hdlTina->scaleDownBuf,0,scaleWidth*scaleHeight*3/2);
			}
			else
			{
				hdlTina->scaleDownBuf = NULL;
			}
			param.srcInfo.w = scaleWidth;
			param.srcInfo.h = scaleHeight;
			hdlTina->dispport.allocateVideoMem(&hdlTina->dispport,&param);

			addr.pThumbBuf = hdlTina->scaleDownBuf;
			addr.nThumbSize = scaleWidth*scaleHeight*3/2;

			AwEncoderSetParamete(p->mAwEncoder,AwEncoder_SetThumbNailAddr,&addr);
			//set new scale down ratio to VE
			//fixme,to do here
			if(hdlTina->scaleDownRatio == 2)
				newratio = VENC_ISP_SCALER_HALF;
			else if(hdlTina->scaleDownRatio == 4)
				newratio = VENC_ISP_SCALER_QUARTER;
			else if(hdlTina->scaleDownRatio == 8)
				newratio = VENC_ISP_SCALER_EIGHTH;
			else
				newratio = VENC_ISP_SCALER_0;

			AwEncoderSetParamete(p->mAwEncoder,AwEncoder_SetScaleDownRatio,&newratio);

			usleep(60*1000);
			//start render new buffer
			hdlTina->renderStatus = RENDER_STATUS_RECONFIG;
			//start copy ve buffer to scaledown buffer
			//fixme,to do here
			enable = 1;
			AwEncoderSetParamete(p->mAwEncoder,AwEncoder_SetThumbNailEnable,&enable);
		}
		else
			return -1;
	}
	else
		return -1;
	return 0;
}

int TinaChangeOutputPath(void *hdl,char* path){
    TinaRecorder *hdlTina = (TinaRecorder *)hdl;
    if(hdlTina == NULL){
        printf("TinaChangeOutputPath fail\n");
        return -1;
    }
    strcpy(hdlTina->mOutputPath,path);
    return 0;
}

int TinaSetAudioMute(void *hdl,int muteFlag){
    TinaRecorder *hdlTina = (TinaRecorder *)hdl;
    if(hdlTina == NULL){
        printf("TinaSetAudioMute() fail\n");
        return -1;
    }
    hdlTina->mAudioMuteFlag = muteFlag;
    return 0;
}


TinaRecorder *CreateTinaRecorder()
{
	TinaRecorder *tinaRec = (TinaRecorder *)malloc(sizeof(TinaRecorder));
	if(tinaRec == NULL)
		return NULL;
	memset(tinaRec, 0, sizeof(TinaRecorder));
	tinaRec->setVideoInPort			=	TinasetVideoInPort;
	tinaRec->setAudioInPort			=	TinasetAudioInPort;
	tinaRec->setDispOutPort			=	TinasetDispOutPort;
	tinaRec->setOutput			=	TinasetOutput;
	tinaRec->start				=	Tinastart;
	tinaRec->stop				=	Tinastop;
	tinaRec->release			=	Tinarelease;
	tinaRec->reset				=	Tinareset;
	tinaRec->prepare            =   Tinaprepare;
	tinaRec->setVideoEncodeSize			=	TinasetEncodeSize;
	tinaRec->setVideoEncodeBitRate		=	TinasetEncodeBitRate;
	tinaRec->setOutputFormat		=	TinasetOutputFormat;
	tinaRec->setCallback			=	TinasetCallback;
	tinaRec->setEncodeVideoFormat	=   TinasetEncodeVideoFormat;
	tinaRec->setEncodeAudioFormat   =   TinasetEncodeAudioFormat;
	tinaRec->getRecorderTime        =   TinagetRecorderTime;
	tinaRec->captureCurrent         =   TinacaptureCurrent;
	tinaRec->setMaxRecordTime       =   TinasetMaxRecordTime;
	tinaRec->setParameter           =   TinasetParameter;
	tinaRec->setEncodeFramerate     =   TinasetEncodeFramerate;
	tinaRec->setVEScaleDownRatio    =   TinasetVEScaleDownRatio;
        tinaRec->changeOutputPath = TinaChangeOutputPath;
        tinaRec->setAudioMute = TinaSetAudioMute;
        sem_init(&tinaRec->mCaptureFinishSem, 0, 0);
	return tinaRec;
}
