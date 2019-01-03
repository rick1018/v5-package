#ifndef _TINA_RECORDER_H_
#define _TINA_RECORDER_H_

#define MAX_URL_LEN 1024

#include <pthread.h>
#include <semaphore.h>
#include <videoOutPort.h>
#include "CdxMuxer.h"
#include "Video_input_port.h"

typedef enum
{
	TR_OUTPUT_TS,
	TR_OUTPUT_MOV,
	TR_OUTPUT_JPG,
	TR_OUTPUT_AAC,
	TR_OUTPUT_MP3,
	TR_OUTPUT_END
}outputFormat;

typedef enum
{
	TR_VIDEO_H264,
	TR_VIDEO_MJPEG,
	TR_VIDEO_END
}videoEncodeFormat;

typedef enum
{
	TR_AUDIO_PCM,
	TR_AUDIO_AAC,
	TR_AUDIO_MP3,
	TR_AUDIO_LPCM,
	TR_AUDIO_END
}audioEncodeFormat;

typedef enum
{
	TR_CAPTURE_JPG,
	TR_CAPTURE_END
}captureFormat;

#if 0
typedef enum
{
    TR_PIXEL_YUV420SP,//NV12
    TR_PIXEL_YVU420SP,//NV21
    TR_PIXEL_YUV420P,//YU12
    TR_PIXEL_YVU420P,//YV12
    TR_PIXEL_YUV422SP,
    TR_PIXEL_YVU422SP,
    TR_PIXEL_YUV422P,
    TR_PIXEL_YVU422P,
    TR_PIXEL_YUYV422,
    TR_PIXEL_UYVY422,
    TR_PIXEL_YVYU422,
    TR_PIXEL_VYUY422,
    TR_PIXEL_ARGB,
    TR_PIXEL_RGBA,
    TR_PIXEL_ABGR,
    TR_PIXEL_BGRA,
    TR_PIXEL_TILE_32X32,//MB32_420
    TR_PIXEL_TILE_128X32,
}pixelFormat;

typedef struct
{
	int isPhy;
	int bufferId;
	long long pts;
	int sourceIndex;
}dataParam;
#endif

typedef struct
{
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
}dispRect;

typedef struct
{
    char capturePath[128];
    captureFormat capFormat ;
    int captureWidth;
    int captureHeight;
}captureConfig;


typedef enum
{
	FLAGS_PREVIEW,
	FLAGS_RECORDING,
	FLAGS_ALL
}TinaFlags;

typedef enum
{
	TINA_RECORD_STATUS_INIT,
	TINA_RECORD_STATUS_INITIALIZED,
	TINA_RECORD_STATUS_DATA_SOURCE_CONFIGURED,
	TINA_RECORD_STATUS_PREPARED,
	TINA_RECORD_STATUS_RECORDING,
	TINA_RECORD_STATUS_RELEASED
}tinaRecordStatus;


#if 0
typedef struct
{
	int (*init)(void *hdl,int enable,int format,int framerate,int width,int height);
	int (*readData)(void *hdl,void *data,int size);
	int (*dequeue)(void *hdl,void **data,int *size,dataParam *param);
	int (*queue)(void *hdl,dataParam *param);
	int (*getEnable)(void *hdl);
	int (*getFrameRate)(void *hdl);
	int (*getFormat)(void *hdl);
	int (*getSize)(void *hdl,int *width,int *height);

	int (*setEnable)(void *hdl,int enable);
	int (*setFrameRate)(void *hdl,int framerate);
	int (*setFormat)(void *hdl,int format);
	int (*setSize)(void *hdl,int width,int height);

	int (*setparameter)(void *hdl,int cmd,int param);

	int enable;
	int framerate;
	int width,height;
	pixelFormat format;
}vInPort;
#endif

typedef struct
{
	int (*init)(void *hdl,int enable,int format,int samplerate,int channels,int bitrate);
	int (*deinit)(void *hdl);
	int (*readData)(void *hdl,void *data,int size);

	int (*clear)(void *hdl);
	int (*getEnable)(void *hdl);
	int (*getFormat)(void *hdl);
	int (*getAudioSampleRate)(void *hdl);
	int (*getAudioChannels)(void *hdl);
	int (*getAudioBitRate)(void *hdl);

	int (*setEnable)(void *hdl,int enable);
	int (*setFormat)(void *hdl,int format);
	int (*setAudioSampleRate)(void *hdl,int samplerate);
	int (*setAudioChannels)(void *hdl,int channels);
	int (*setAudioBitRate)(void *hdl,int bitrate);

	int (*setparameter)(void *hdl,int cmd,int param);
	long long (*getpts)(void *hdl);

	int enable;
	audioEncodeFormat format;
	int samplerate;
	int channels;
	int bitRate;
	long long pts;
	void *dupHdl;
}aInPort;

typedef struct
{
	int (*init)(void *hdl,int enable,char *url);
	int (*writeData)(void *hdl,void **data,int size,dataParam *para);
	int (*getUrl)(void *hdl,char *url);
	int (*setUrl)(void *hdl,char *url);
	int (*setEnable)(void *hdl,int enable);
	int (*getEnable)(void *hdl);

	int enable;
	char url[MAX_URL_LEN];
}rOutPort;

typedef enum
{
	RENDER_STATUS_LOOP,
	RENDER_STATUS_PAUSE,
	RENDER_STATUS_RECONFIG,
	RENDER_STATUS_STOP
}renderStatus;

typedef enum TinaRecorderCallbackMsg
{
    TINA_RECORD_ONE_FILE_COMPLETE			          = 0,
}TinaRecorderCallbackMsg;

typedef int (*TinaRecorderCallback)(void* pUserData,int msgType,void* param);

typedef struct
{
	int (*setVideoInPort)(void *hdl,vInPort *vPort);
	int (*setAudioInPort)(void *hdl,aInPort *aPort);
	int (*setDispOutPort)(void *hdl,dispOutPort *dispPort);
	int (*setOutput)(void *hdl,rOutPort *output);

	int (*start)(void *hdl,int flags);
	int (*stop)(void *hdl,int flags);
	int (*release)(void *hdl);
	int (*prepare)(void *hdl);
	int (*reset)(void *hdl);
	int (*resume)(void *hdl,int flags);

	int (*setVideoEncodeSize)(void *hdl,int width,int height);
	int (*setVideoEncodeBitRate)(void *hdl,int bitrate);
	int (*setOutputFormat)(void *hdl,int format);
	int (*setCallback)(void *hdl,TinaRecorderCallback callback,void* pUserData);
	int (*setEncodeVideoFormat)(void *hdl,int format);
	int (*setEncodeAudioFormat)(void *hdl,int format);
	int (*setEncodeFramerate)(void *hdl,int framerate);
	int (*setVEScaleDownRatio)(void *hdl,int ratio);

	int (*getRecorderTime)(void *hdl);
	int (*captureCurrent)(void *hdl,captureConfig *capConfig);
	int (*setMaxRecordTime)(void *hdl,int ms);
	int (*setParameter)(void *hdl,int cmd,int parameter);

        int (*changeOutputPath)(void *hdl,char* path);
        int (*setAudioMute)(void *hdl,int muteFlag);

	int encodeWidth;
	int encodeHeight;
	int encodeBitrate;
	int encodeFramerate;
	outputFormat outputFormat;
	TinaRecorderCallback callback;
        void* pUserData;
	videoEncodeFormat encodeVFormat;
	audioEncodeFormat encodeAFormat;
	int maxRecordTime;
	vInPort vport;
	aInPort aport;
	dispOutPort dispport;
	rOutPort outport;
	void *EncoderContext;
	int status;
	char *scaleDownBuf;
	int scaleDownRatio;

	int vinset;
	int ainset;
	int routset;
	int dispoutset;
	int lastEncodeId;
	pthread_t renderId;
	int renderStarted;
	int renderStatus;
	sem_t renderSem;
	sem_t renderFinSem;
	sem_t renderDataSem;

	int renderQueue;

	sem_t dispfinSem;
	pthread_t dirdispId;

	dataParam param;
	int mCaptureFlag;
	char mOutputPath[256];
	int startFlags;
        int mCameraIndex;
        int mNeedMuxerVideoFlag;
        long long mBaseVideoPts;
        int mAudioMuteFlag;
        captureConfig mCaptureConfig;
        /*
        CdxMuxerPacketT mAudioPktPtr[64];
        int mAudioPktWriteIndex;
        int mAudioPktUnusedCount;
        int mTotalAudioPktNum;

        char* mAudioBufPtr;
        char* mAudioBufEndPtr;
        char* mAudioBufWritePtr;
        int mAudioBufUnusedSize;
        int mTotalAudioPktBufSize;
        pthread_mutex_t mAudioPktLock;

        CdxMuxerPacketT mVideoPktPtr[64];
        int mVideoPktWriteIndex;
        int mVideoPktUnusedCount;
        int mTotalVideoPktNum;

        char* mVideoBufPtr;
        char* mVideoBufEndPtr;
        char* mVideoBufWritePtr;
        int mVideoBufUnusedSize;
        int mTotalVideoPktBufSize;
        pthread_mutex_t mVideoPktLock;
        */
        sem_t mCaptureFinishSem;

        int mTotalAudioPktBufSize;
        int mAudioPktBufSizeCount;
        int mNeedAudioPktBufSpace;
        pthread_mutex_t mAudioPktBufLock;
        sem_t mAudioPktBufSem;

        int mTotalVideoPktBufSize;
        int mVideoPktBufSizeCount;
        int mNeedVideoPktBufSpace;
        pthread_mutex_t mVideoPktBufLock;
        sem_t mVideoPktBufSem;
		int rotateAngel;
		renderBuf rBuf;
		int use_wm;
		int wm_width;
		int wm_height;
        sem_t mQueueDataSem;
}TinaRecorder;

TinaRecorder *CreateTinaRecorder();
int DestroyTinaRecorder(TinaRecorder *hdl);
#endif
