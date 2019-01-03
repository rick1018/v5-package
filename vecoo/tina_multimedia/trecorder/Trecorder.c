#define LOG_TAG "TRecorder"
#include "tlog.h"
#include "Trecorder.h"
#include "TinaRecorder.h"
#include "pcm_multi.h"
#include "audioInPort.h"

#define ISP_WIDTH 640
#define ISP_HEIGHT 480

typedef enum
{
    T_RECORD_STATUS_INIT,
    T_RECORD_STATUS_INITIALIZED,
    T_RECORD_STATUS_DATA_SOURCE_CONFIGURED,
    T_RECORD_STATUS_PREPARED,
    T_RECORD_STATUS_RECORDING,
    T_RECORD_STATUS_RELEASED,
}TRecordStatus;

typedef struct TrecoderContext
{
    TinaRecorder*        mTinaRecorder;
    vInPort*                mVideoInPort;
    aInPort*                mAudioInPort;
    dispOutPort*          mDispOutPort;
    rOutPort*              mRecorderOutPort;
    int                         mStatus;
    int                         mFlags;

    //input video args:
    int                         mEnableVideoInput;
    int                         mInputVideoFormat;
    int                         mInputVideoFramerate;
    int                         mInputVideoYuvWidth;
    int                         mInputVideoYuvHeight;

    //input audio args:
    int                         mEnableAudioInput;
    int                         mInputAudioFormat;
    int                         mInputAudioSamplerate;
    int                         mInputAudioChannels;
    int                         mInputAudioBirate;

    //preview args:
    int                         mEnablePreview;
    int                         mPreviewRotate;
	int                         route;
    VoutRect               mPreviewRect;
	VoutRect               mPreviewSrcRect;

    void*                    mUserData;
    TRecorderCallback  mCallback;
}TrecoderContext;

int CallbackFromTinaRecorder(void* pUserData, int msg, void* param)
{
    TrecoderContext *trContext = (TrecoderContext *)pUserData;
    if(trContext == NULL){
        TLOGE("trecorderContext is null");
        return -1;
    }
    int appMsg = -1;
    switch(msg)
    {
        case TINA_RECORD_ONE_FILE_COMPLETE:
        {
            appMsg = T_RECORD_ONE_FILE_COMPLETE;
	    break;
        }
        default:
        {
            TLOGW("warning: unknown callback from tinarecorder");
            break;
        }
    }
    if((appMsg != -1) && (trContext->mCallback != NULL)){
        trContext->mCallback(trContext->mUserData,appMsg,param);
    }
    return 0;
}


TrecorderHandle *CreateTRecorder(){
    TrecoderContext* trecorderContext = (TrecoderContext*)malloc(sizeof(TrecoderContext));
    if(trecorderContext == NULL){
        TLOGE("CreateTRecorder fail");
        return NULL;
    }
    memset(trecorderContext,0,sizeof(TrecoderContext));
    trecorderContext->mTinaRecorder = CreateTinaRecorder();
    if(trecorderContext->mTinaRecorder == NULL){
        TLOGE("CreateTRecorder:CreateTinaRecorder ERR");
        free(trecorderContext);
        return NULL;
    }
    return (TrecorderHandle*)trecorderContext;
}
int TRsetOutput(TrecorderHandle *hdl,char *url){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    int ret = 0;
    //create the outPut
    trContext->mRecorderOutPort = (rOutPort*)malloc(sizeof(rOutPort));
    if(trContext->mRecorderOutPort == NULL){
        TLOGE("TRsetOutput:Create mRecorderOutPort err");
        return -1;
    }
	if(url == NULL)
	{
		free(trContext->mRecorderOutPort);
		trContext->mRecorderOutPort = NULL;
		ret = trContext->mTinaRecorder->setOutput(trContext->mTinaRecorder,NULL);
	}
	else
	{
	strcpy(trContext->mRecorderOutPort->url,url);
		trContext->mRecorderOutPort->enable = 1;
		ret = trContext->mTinaRecorder->setOutput(trContext->mTinaRecorder,trContext->mRecorderOutPort);
	}
    return ret;
}
int TRsetCamera(TrecorderHandle *hdl,int index){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    int ret = 0;
    //create the vInPort
    if(index != T_CAMERA_DISABLE)
    {
	    trContext->mVideoInPort = CreateTinaVinport();
	    if(trContext->mVideoInPort == NULL){
	        TLOGE("TRsetCamera:CreateTinaVinport err");
	        ret = -1;
	    }else{
	        trContext->mVideoInPort->open(trContext->mVideoInPort,index);
			ret = trContext->mTinaRecorder->setVideoInPort(trContext->mTinaRecorder,trContext->mVideoInPort);
	    }
    }
	else
	{
		ret = trContext->mTinaRecorder->setVideoInPort(trContext->mTinaRecorder,NULL);
		trContext->mVideoInPort = NULL;
	}
    return ret;
}

int TRsetAudioSrc(TrecorderHandle *hdl,int index){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    int ret = 0;
    //create the aInPort
    if(index != T_AUDIO_MIC_DISABLE)
    {
	    trContext->mAudioInPort = CreateAlsaAport(index);
	    if(trContext->mAudioInPort == NULL){
	        TLOGE("TRsetAudioSrc:CreateAlsaAport ERR");
	        return -1;
	    }
		else
		{
			ret = trContext->mTinaRecorder->setAudioInPort(trContext->mTinaRecorder,trContext->mAudioInPort);
		}
    }
	else
	{
		ret = trContext->mTinaRecorder->setAudioInPort(trContext->mTinaRecorder,NULL);
		trContext->mAudioInPort = NULL;
	}
    return ret;
}
int TRsetPreview(TrecorderHandle *hdl,int index){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    int ret = 0;
    //create the dispOutPort
    if(index != T_DISP_LAYER_DISABLE)
    {
	    trContext->mDispOutPort = CreateVideoOutport(index);
	    if(trContext->mDispOutPort == NULL){
	        TLOGE("TRsetPreview: CreateVideoOutport ERR");
	        return -1;
	    }
		else
		{
			ret = trContext->mTinaRecorder->setDispOutPort(trContext->mTinaRecorder,trContext->mDispOutPort);
		}
    }
	else
	{
		ret = trContext->mTinaRecorder->setDispOutPort(trContext->mTinaRecorder,NULL);
		trContext->mDispOutPort = NULL;
	}
    return ret;
}

int TRstart(TrecorderHandle *hdl,int flags){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    int ret = 0;
	int trFlags;
    CHECK_ERROR_RETURN(trContext);
    CHECK_ERROR_RETURN(trContext->mTinaRecorder);
    CHECK_ERROR_RETURN(trContext->mTinaRecorder->start);
	switch(flags)
	{
		case T_PREVIEW:
			trFlags = FLAGS_PREVIEW;
			break;
		case T_RECORD:
			trFlags = FLAGS_RECORDING;
			break;
		case T_ALL:
			trFlags = FLAGS_ALL;
			break;
		default:
			TLOGE("unknow stop flags!\n");
			return -1;
	}
    ret = trContext->mTinaRecorder->start(trContext->mTinaRecorder,trFlags);
    return ret;
}
int TRstop(TrecorderHandle *hdl,int flags){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    int ret = 0;
	int trFlags = 0;
    CHECK_ERROR_RETURN(trContext);
    CHECK_ERROR_RETURN(trContext->mTinaRecorder);
    CHECK_ERROR_RETURN(trContext->mTinaRecorder->stop);
	switch(flags)
	{
		case T_PREVIEW:
			trFlags = FLAGS_PREVIEW;
			break;
		case T_RECORD:
			trFlags = FLAGS_RECORDING;
			break;
		case T_ALL:
			trFlags = FLAGS_ALL;
			break;
		default:
			TLOGE("unknow stop flags!\n");
			return -1;
	}
	trContext->mFlags = trFlags;

    ret = trContext->mTinaRecorder->stop(trContext->mTinaRecorder,(int)trFlags);

	if(trFlags == FLAGS_ALL || (trFlags == trContext->mTinaRecorder->startFlags))
	{
		printf("deinit aport!\n");
		if(trContext->mAudioInPort)
		{
			TP_CHECK(trContext->mTinaRecorder->aport.deinit);
			trContext->mTinaRecorder->aport.deinit(&trContext->mTinaRecorder->aport);
			DestroyAlsaAport(trContext->mAudioInPort);
			trContext->mAudioInPort = NULL;
		}
		printf("deinit vport!\n");
		if(trContext->mVideoInPort)
		{
			TP_CHECK(trContext->mTinaRecorder->vport.releasebuf);
			TP_CHECK(trContext->mTinaRecorder->vport.close);
			trContext->mTinaRecorder->vport.releasebuf(&trContext->mTinaRecorder->vport,3);
			trContext->mTinaRecorder->vport.WMRelease(&trContext->mTinaRecorder->vport);
			trContext->mTinaRecorder->vport.close(&trContext->mTinaRecorder->vport);
			//a problem here,so we use free,fixme
			free(trContext->mVideoInPort);
			//DestroyTinaVinport(trContext->mVideoInPort);
			trContext->mVideoInPort = NULL;
		}
		printf("deinit disp port!\n");
		if(trContext->mDispOutPort)
		{
			TP_CHECK(trContext->mTinaRecorder->dispport.deinit);
			trContext->mTinaRecorder->dispport.deinit(&trContext->mTinaRecorder->dispport);
			DestroyVideoOutport(trContext->mDispOutPort);
			trContext->mVideoInPort = NULL;
		}
		printf("release tina recorder!\n");
		if(trContext->mTinaRecorder)
		{
			TP_CHECK(trContext->mTinaRecorder->release);
			trContext->mTinaRecorder->release(trContext->mTinaRecorder);
			DestroyTinaRecorder(trContext->mTinaRecorder);
			trContext->mTinaRecorder = NULL;
		}

		if(trContext->mRecorderOutPort){
			free(trContext->mRecorderOutPort);
			trContext->mRecorderOutPort = NULL;
		}
	}
	else if(trFlags == FLAGS_PREVIEW)
	{
		//fixme,add here
	}
	else
	{
		//fixme,add here
	}
    return ret;
}

int TRrelease(TrecorderHandle *hdl){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	int ret = 0;
    CHECK_ERROR_RETURN(trContext);
    free(trContext);

    return ret;
}
int TRprepare(TrecorderHandle *hdl){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    int ret = 0;
	int ispEnable;
	int rot_angle;
    CHECK_ERROR_RETURN(trContext);
    CHECK_ERROR_RETURN(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->prepare);
	printf("prepare handle:0x%p\n",trContext);
    if(trContext->mAudioInPort)
    {
	    TP_CHECK(trContext->mTinaRecorder->aport.init);
		/*printf("audio config,enable:%d format:%d sr:%d ch:%d bitrate:%d\n",trContext->mEnableAudioInput,trContext->mInputAudioFormat,
			trContext->mInputAudioSamplerate,trContext->mInputAudioChannels,trContext->mInputAudioBirate);*/
		trContext->mTinaRecorder->aport.init(&trContext->mTinaRecorder->aport,trContext->mEnableAudioInput,trContext->mInputAudioFormat,
			trContext->mInputAudioSamplerate,trContext->mInputAudioChannels,trContext->mInputAudioBirate);
    }

    if(trContext->mVideoInPort)
    {
		TP_CHECK(trContext->mTinaRecorder->vport.init);
		TP_CHECK(trContext->mTinaRecorder->vport.requestbuf);
		if(trContext->mDispOutPort && trContext->route == VIDEO_SRC_FROM_ISP)
			ispEnable = 1;
		else
			ispEnable = 0;
		/*printf("vport format:%d framerate:%d width:%d height:%d isp:%d\n",trContext->mInputVideoFormat,trContext->mInputVideoFramerate,
			trContext->mInputVideoYuvWidth,trContext->mInputVideoYuvHeight,ispEnable);*/

		rot_angle = 90;
		trContext->mTinaRecorder->vport.init(&trContext->mTinaRecorder->vport,trContext->mEnableVideoInput,trContext->mInputVideoFormat,
			trContext->mInputVideoFramerate,trContext->mInputVideoYuvWidth,trContext->mInputVideoYuvHeight,ISP_WIDTH,ISP_HEIGHT,ispEnable,
			rot_angle,trContext->mTinaRecorder->use_wm);
		trContext->mTinaRecorder->vport.WMInit(&trContext->mTinaRecorder->vport.WM_info, "/rom/etc/res/wm_540p_");
		trContext->mTinaRecorder->vport.requestbuf(&trContext->mTinaRecorder->vport,3);

	}

    if(trContext->mDispOutPort)
    {
	    TP_CHECK(trContext->mTinaRecorder->dispport.init);
		/*printf("enable:%d rotate:%d rect width:%d height:%d route:%d\n",trContext->mEnablePreview,trContext->mPreviewRotate,trContext->mPreviewRect.width,
			trContext->mPreviewRect.height,trContext->route);*/
		trContext->mTinaRecorder->dispport.init(&trContext->mTinaRecorder->dispport,trContext->mEnablePreview,trContext->mPreviewRotate,&trContext->mPreviewRect);
		trContext->mTinaRecorder->dispport.setRoute(&trContext->mTinaRecorder->dispport,trContext->route);
    }

    ret = trContext->mTinaRecorder->prepare(trContext->mTinaRecorder);
    return ret;
}
int TRreset(TrecorderHandle *hdl){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	TinaRecorder *hdlTina = trContext->mTinaRecorder;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->reset);


    if(trContext->mAudioInPort)
    {
	    TP_CHECK(hdlTina->aport.deinit);
		hdlTina->aport.deinit(&hdlTina->aport);
		DestroyAlsaAport(trContext->mAudioInPort);
		trContext->mAudioInPort = NULL;
    }

    if(trContext->mVideoInPort)
	{
		TP_CHECK(hdlTina->vport.releasebuf);
		TP_CHECK(hdlTina->vport.close);
		hdlTina->vport.releasebuf(&hdlTina->vport,3);
		hdlTina->vport.close(&hdlTina->vport);
		DestroyTinaVinport(trContext->mVideoInPort);
		trContext->mVideoInPort = NULL;
		if (hdlTina->vport.use_wm == 1)
			hdlTina->vport.WMRelease(&hdlTina->vport);
    }

    if(trContext->mDispOutPort)
    {
	    TP_CHECK(hdlTina->dispport.deinit);
		hdlTina->dispport.deinit(&hdlTina->dispport);
		DestroyVideoOutport(trContext->mDispOutPort);
		trContext->mDispOutPort = NULL;
    }

    if(trContext->mRecorderOutPort){
        free(trContext->mRecorderOutPort);
        trContext->mRecorderOutPort = NULL;
    }

    int ret = 0;
    ret = trContext->mTinaRecorder->reset(trContext->mTinaRecorder);
    return ret;
}
int TRresume(TrecorderHandle *hdl){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->resume);
    int ret = 0;
    ret = trContext->mTinaRecorder->resume(trContext->mTinaRecorder,trContext->mFlags);
    return ret;
}

int TRsetVideoEncodeSize(TrecorderHandle *hdl,int width,int height){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->setVideoEncodeSize);
    int ret = 0;
    ret = trContext->mTinaRecorder->setVideoEncodeSize(trContext->mTinaRecorder,width,height);
    return ret;
}
int TRsetOutputFormat(TrecorderHandle *hdl,int format){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	int trFormat;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->setOutputFormat);
    int ret = 0;

	switch(format)
	{
		case T_OUTPUT_TS:
			trFormat = TR_OUTPUT_TS;
			break;
		case T_OUTPUT_MOV:
			trFormat = TR_OUTPUT_MOV;
			break;
		case T_OUTPUT_JPG:
			trFormat = TR_OUTPUT_JPG;
			break;
		case T_OUTPUT_AAC:
			trFormat = TR_OUTPUT_AAC;
			break;
		case T_OUTPUT_MP3:
			trFormat = TR_OUTPUT_MP3;
			break;
		default:
			return -1;
	}
    ret = trContext->mTinaRecorder->setOutputFormat(trContext->mTinaRecorder,trFormat);
    return ret;
}
int TRsetRecorderCallback(TrecorderHandle *hdl,TRecorderCallback callback,void* pUserData){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->setCallback);
    int ret = 0;
    trContext->mCallback = callback;
    trContext->mUserData = pUserData;
    ret = trContext->mTinaRecorder->setCallback(trContext->mTinaRecorder,CallbackFromTinaRecorder,(void*)trContext);
    return ret;
}
int TRsetMaxRecordTimeMs(TrecorderHandle *hdl,int ms){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->setMaxRecordTime);
    int ret = 0;
    ret = trContext->mTinaRecorder->setMaxRecordTime(trContext->mTinaRecorder,ms);
    return ret;
}
int TRsetVideoEncoderFormat(TrecorderHandle *hdl,int VFormat){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	int trFormat;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->setEncodeVideoFormat);
    int ret = 0;
	switch(VFormat)
	{
		case T_VIDEO_H264:
			trFormat = TR_VIDEO_H264;
			break;
		case T_VIDEO_MJPEG:
			trFormat = TR_VIDEO_MJPEG;
			break;
		default:
			return -1;
	}
    ret = trContext->mTinaRecorder->setEncodeVideoFormat(trContext->mTinaRecorder,trFormat);
    return ret;
}
int TRsetAudioEncoderFormat(TrecorderHandle *hdl,int AFormat){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	int trFormat;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->setEncodeAudioFormat);
    int ret = 0;

	switch(AFormat)
	{
		case T_AUDIO_PCM:
			trFormat = TR_AUDIO_PCM;
			break;
		case T_AUDIO_AAC:
			trFormat = TR_AUDIO_AAC;
			break;
		case T_AUDIO_MP3:
			trFormat = TR_AUDIO_MP3;
			break;
		case T_AUDIO_LPCM:
			trFormat = TR_AUDIO_LPCM;
			break;
		default:
			return -1;
	}
    ret = trContext->mTinaRecorder->setEncodeAudioFormat(trContext->mTinaRecorder,trFormat);
    return ret;
}
int TRsetEncoderBitRate(TrecorderHandle *hdl,int bitrate){//only for video
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->setVideoEncodeBitRate);
    int ret = 0;
    ret = trContext->mTinaRecorder->setVideoEncodeBitRate(trContext->mTinaRecorder,bitrate);
    return ret;
}
int TRsetRecorderEnable(TrecorderHandle *hdl,int enable){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
	if(enable)
	{
	TP_CHECK(trContext->mRecorderOutPort);
	}

    int ret = 0;
	if(trContext->mRecorderOutPort)
	{
	    ret = trContext->mRecorderOutPort->enable = enable;
	}
    return ret;
}
int TRsetEncodeFramerate(TrecorderHandle *hdl,int framerate){//only for video
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->setEncodeFramerate);
    int ret = 0;
    ret = trContext->mTinaRecorder->setEncodeFramerate(trContext->mTinaRecorder,framerate);
    return ret;
}
int TRgetRecordTime(TrecorderHandle *hdl){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->getRecorderTime);
    int recordtime = 0;
    recordtime = trContext->mTinaRecorder->getRecorderTime(trContext->mTinaRecorder);
    return recordtime;
}
int TRCaptureCurrent(TrecorderHandle *hdl,TCaptureConfig *config){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(config);
    TP_CHECK(trContext->mTinaRecorder->captureCurrent);
    int ret = 0;
    captureConfig capConfig;
	switch(config->captureFormat)
	{
		case T_CAPTURE_JPG:
			capConfig.capFormat = TR_CAPTURE_JPG;
			break;
		default:
			return -1;
	}
    strcpy(capConfig.capturePath,config->capturePath);
    capConfig.captureWidth = config->captureWidth;
    capConfig.captureHeight = config->captureHeight;
    ret = trContext->mTinaRecorder->captureCurrent(trContext->mTinaRecorder,&capConfig);
    return ret;
}

int TRsetCameraInputFormat(TrecorderHandle *hdl,int format){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	int trFormat;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mVideoInPort);
    TP_CHECK(trContext->mVideoInPort->setFormat);
    int ret = 0;

	switch(format)
	{
		case T_CAMERA_YUV420SP:
			trFormat = TR_PIXEL_YUV420SP;
			break;
		case T_CAMERA_YVU420SP:
			trFormat = TR_PIXEL_YVU420SP;
			break;
		case T_CAMERA_YUV420P:
			trFormat = TR_PIXEL_YUV420P;
			break;
		case T_CAMERA_YVU420P:
			trFormat = TR_PIXEL_YVU420P;
			break;
		case T_CAMERA_YUV422SP:
			trFormat = TR_PIXEL_YUV422SP;
			break;
		case T_CAMERA_YVU422SP:
			trFormat = TR_PIXEL_YVU422SP;
			break;
		case T_CAMERA_YUV422P:
			trFormat = TR_PIXEL_YUV422P;
			break;
		case T_CAMERA_YVU422P:
			trFormat = TR_PIXEL_YVU422P;
			break;
		case T_CAMERA_YUYV422:
			trFormat = TR_PIXEL_YUYV422;
			break;
		case T_CAMERA_UYVY422:
			trFormat = TR_PIXEL_UYVY422;
			break;
		case T_CAMERA_YVYU422:
			trFormat = TR_PIXEL_YVYU422;
			break;
		case T_CAMERA_VYUY422:
			trFormat = TR_PIXEL_VYUY422;
			break;
		case T_CAMERA_ARGB:
			trFormat = TR_PIXEL_ARGB;
			break;
		case T_CAMERA_RGBA:
			trFormat = TR_PIXEL_RGBA;
			break;
		case T_CAMERA_ABGR:
			trFormat = TR_PIXEL_ABGR;
			break;
		case T_CAMERA_BGRA:
			trFormat = TR_PIXEL_BGRA;
			break;
		case T_CAMERA_TILE_32X32:
			trFormat = TR_PIXEL_TILE_32X32;
			break;
		case T_CAMERA_TILE_128X32:
			trFormat = TR_PIXEL_TILE_128X32;
			break;
		default:
			return -1;
	}

    trContext->mInputVideoFormat = trFormat;
    //ret = trContext->mVideoInPort->setFormat(trContext->mVideoInPort,format);
    return ret;
}
int TRsetCameraFramerate(TrecorderHandle *hdl,int framerate){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mVideoInPort);
    TP_CHECK(trContext->mVideoInPort->setFrameRate);
    int ret = 0;
    trContext->mInputVideoFramerate = framerate;
    //ret = trContext->mVideoInPort->setFrameRate(trContext->mVideoInPort,framerate);
    return ret;
}
int TRsetCameraCaptureSize(TrecorderHandle *hdl,int width,int height){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mVideoInPort);
    TP_CHECK(trContext->mVideoInPort->setSize);
    int ret = 0;
    trContext->mInputVideoYuvWidth = width;
    trContext->mInputVideoYuvHeight = height;
    //ret = trContext->mVideoInPort->setSize(trContext->mVideoInPort,width,height);
    return ret;
}
int TRsetCameraEnableWM(TrecorderHandle *hdl,int width,int height,int enable){
		TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    int ret = 0;
    trContext->mTinaRecorder->use_wm = enable;
    trContext->mTinaRecorder->wm_width = width;
    trContext->mTinaRecorder->wm_height = height;
    return ret;
}
int TRsetCameraDiscardRatio(TrecorderHandle *hdl,int ratio){//do this in tinarecorder?
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    int ret = 0;
    return ret;
}
int TRsetCameraWaterMarkIndex(TrecorderHandle *hdl,int id){//do this in tinarecorder?
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    int ret = 0;
    return ret;
}

int TRsetCameraEnable(TrecorderHandle *hdl,int enable){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mVideoInPort);
    TP_CHECK(trContext->mVideoInPort->setEnable);
    int ret = 0;
    trContext->mEnableVideoInput = enable;
    //ret = trContext->mVideoInPort->setEnable(trContext->mVideoInPort,enable);
    return ret;
}

int TRsetMICInputFormat(TrecorderHandle *hdl,int format){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mAudioInPort);
    TP_CHECK(trContext->mAudioInPort->setFormat);
    int ret = 0;
	//we only support PCM now
	if(format != T_AUDIO_PCM)
		return -1;
	//fixme,use switch case here
    trContext->mInputAudioFormat = format;
    //ret = trContext->mAudioInPort->setFormat(trContext->mAudioInPort,format);
    return ret;
}

int TRsetMICSampleRate(TrecorderHandle *hdl,int sampleRate){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mAudioInPort);
    TP_CHECK(trContext->mAudioInPort->setAudioSampleRate);
    int ret = 0;
    trContext->mInputAudioSamplerate = sampleRate;
    //ret = trContext->mAudioInPort->setAudioSampleRate(trContext->mAudioInPort,sampleRate);
    return ret;
}
int TRsetMICChannels(TrecorderHandle *hdl,int channels){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mAudioInPort);
    TP_CHECK(trContext->mAudioInPort->setAudioChannels);
    int ret = 0;
    trContext->mInputAudioChannels = channels;
    //ret = trContext->mAudioInPort->setAudioChannels(trContext->mAudioInPort,channels);
    return ret;
}
int TRsetMICBitrate(TrecorderHandle *hdl,int bitrate){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mAudioInPort);
    TP_CHECK(trContext->mAudioInPort->setAudioBitRate);
    int ret = 0;
    trContext->mInputAudioBirate = bitrate;
    //ret = trContext->mAudioInPort->setAudioBitRate(trContext->mAudioInPort,bitrate);//only support 16
    return ret;
}
int TRsetMICEnable(TrecorderHandle *hdl,int enable){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mAudioInPort);
    TP_CHECK(trContext->mAudioInPort->setEnable);
    int ret = 0;
    trContext->mEnableAudioInput = enable;
    //ret = trContext->mAudioInPort->setEnable(trContext->mAudioInPort,enable);
    return ret;
}

int TRsetPreviewRect(TrecorderHandle *hdl,TdispRect *rect){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	TinaRecorder *hdlTina = trContext->mTinaRecorder;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mDispOutPort->setRect);
    TP_CHECK(rect);
    int ret = 0;
    trContext->mPreviewRect.x = rect->x;
    trContext->mPreviewRect.y = rect->y;
    trContext->mPreviewRect.width = rect->width;
    trContext->mPreviewRect.height= rect->height;

	if(hdlTina->status == TINA_RECORD_STATUS_RECORDING)
	{
	    ret = hdlTina->dispport.setRect(&hdlTina->dispport,&trContext->mPreviewRect);
	}
    return ret;
}
int TRsetPreviewRotate(TrecorderHandle *hdl,int angle){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	TinaRecorder *hdlTina = trContext->mTinaRecorder;
	int trAngel;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mDispOutPort);
    TP_CHECK(trContext->mDispOutPort->setRotateAngel);
    int ret = 0;
	switch(angle)
	{
		case T_ROTATION_ANGLE_0:
			trAngel = ROTATION_ANGLE_0;
			break;
		case T_ROTATION_ANGLE_90:
			trAngel = ROTATION_ANGLE_90;
			break;

		case T_ROTATION_ANGLE_180:
			trAngel = ROTATION_ANGLE_180;
			break;
		case T_ROTATION_ANGLE_270:
			trAngel = ROTATION_ANGLE_270;
			break;
		default:
			return -1;
	}
    trContext->mPreviewRotate = trAngel;
	if(hdlTina->status == TINA_RECORD_STATUS_RECORDING)
	{
	    ret = hdlTina->dispport.setRotateAngel(&hdlTina->dispport,trAngel);
	}
    return ret;
}

int TRsetPreviewEnable(TrecorderHandle *hdl,int enable){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	TinaRecorder *hdlTina = trContext->mTinaRecorder;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mDispOutPort);
    TP_CHECK(trContext->mDispOutPort->setEnable);
    int ret = 0;
    trContext->mEnablePreview = enable;

	if(hdlTina->status == TINA_RECORD_STATUS_RECORDING)
	{
	    ret = hdlTina->dispport.setEnable(&hdlTina->dispport,enable);
	}
    return ret;
}

int TRsetPreviewRoute(TrecorderHandle *hdl,int route)
{
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	TinaRecorder *hdlTina = trContext->mTinaRecorder;
	int trRoute;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mDispOutPort);
    TP_CHECK(trContext->mDispOutPort->setEnable);
    int ret = 0;
	switch(route)
	{
		case T_ROUTE_ISP:
			trRoute = VIDEO_SRC_FROM_ISP;
			break;
		case T_ROUTE_VE:
			trRoute = VIDEO_SRC_FROM_VE;
			break;
		case T_ROUTE_CAMERA:
			trRoute = VIDEO_SRC_FROM_CAM;
			break;
		default:
			return -1;
			break;
	}
    trContext->route = trRoute;
	return ret;
}

int TRsetVEScaleDownRatio(TrecorderHandle *hdl,int ratio)
{
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->setVEScaleDownRatio);
    int ret = 0;
    ret = trContext->mTinaRecorder->setVEScaleDownRatio(trContext->mTinaRecorder,ratio);
    return ret;
}

int TRchangeOutputPath(TrecorderHandle *hdl,char* path){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->changeOutputPath);
    int ret = 0;
    ret = trContext->mTinaRecorder->changeOutputPath(trContext->mTinaRecorder,path);
    return ret;
}

int TRsetAudioMute(TrecorderHandle *hdl,int muteFlag){
    TrecoderContext* trContext = (TrecoderContext*)hdl;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mTinaRecorder);
    TP_CHECK(trContext->mTinaRecorder->setAudioMute);
    int ret = 0;
    ret = trContext->mTinaRecorder->setAudioMute(trContext->mTinaRecorder,muteFlag);
    return ret;
}


int TRsetPreviewZorder(TrecorderHandle *hdl,int zorder)
{
	TrecoderContext* trContext = (TrecoderContext*)hdl;
	TinaRecorder *hdlTina = trContext->mTinaRecorder;
	int trZorder;
	int ret;
	int trRoute;
	CHECK_ERROR_RETURN(trContext);
	TP_CHECK(trContext->mDispOutPort);
	TP_CHECK(trContext->mDispOutPort->setEnable);

	switch(zorder)
	{
		case T_PREVIEW_ZORDER_TOP:
			trZorder = VIDEO_ZORDER_TOP;
			break;
		case T_PREVIEW_ZORDER_MIDDLE:
			trZorder = VIDEO_ZORDER_MIDDLE;
			break;
		case T_PREVIEW_ZORDER_BOTTOM:
			trZorder = VIDEO_ZORDER_BOTTOM;
			break;
		default:
			return -1;
	}

	if(hdlTina->status == TINA_RECORD_STATUS_RECORDING)
	{
		ret = hdlTina->dispport.SetZorder(&hdlTina->dispport,trZorder);
	}
	return ret;
}

int TRsetPreviewSrcRect(TrecorderHandle *hdl,TdispRect *SrcRect)
{
    TrecoderContext* trContext = (TrecoderContext*)hdl;
	TinaRecorder *hdlTina = trContext->mTinaRecorder;
    CHECK_ERROR_RETURN(trContext);
    TP_CHECK(trContext->mDispOutPort->setSrcRect);
    TP_CHECK(SrcRect);
    int ret = 0;
    trContext->mPreviewSrcRect.x = SrcRect->x;
    trContext->mPreviewSrcRect.y = SrcRect->y;
    trContext->mPreviewSrcRect.width = SrcRect->width;
    trContext->mPreviewSrcRect.height= SrcRect->height;

	if(hdlTina->status == TINA_RECORD_STATUS_RECORDING)
	{
	    ret = hdlTina->dispport.setSrcRect(&hdlTina->dispport,&trContext->mPreviewSrcRect);
	}
    return ret;
}
