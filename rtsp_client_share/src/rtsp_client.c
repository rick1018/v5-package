#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <linux/tcp.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include <mpi_venc.h>
#include <mpi_vdec.h>
#include <mpi_sys.h>
#include <mm_comm_sys.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <sem_util.h>
#include <file_msg_drv.h>
#include <nalu.h>
#include <stream.h>

const char *outputName = "rtsp://admin:hk888888@123.51.133.102:554/live.sdp";
static PAYLOAD_TYPE_E g_venc_type = PT_H264;
static ROTATE_E       g_rotate    = ROTATE_NONE;
static unsigned int   g_rc_mode   = 0; /* 0:CBR 1:VBR 2:FIXQp 3:ABR 4:QPMAP */
static unsigned int   g_profile   = 1;          /* 0: baseline  1:MP  2:HP  3: SVC-T [0,3] */
static MPPCallbackInfo  g_cb_info;

#define VDEC_CHN_0    0
#define VENC_CHN_0    0

typedef struct tag_VENC_CFG_S {
    unsigned int src_width;
    unsigned int src_height;
    unsigned int dst_width;
    unsigned int dst_height;

    unsigned int src_fps;
    unsigned int dst_fps;
    unsigned int bitrate;
    unsigned int gop;

    unsigned int max_qp;
    unsigned int min_qp;

    int            is_by_frame;
    VIDEO_FIELD_E  field;
    PIXEL_FORMAT_E pixel_format;
}VENC_CFG_S;

static ERRORTYPE MPPCallbackFunc(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    //printf(" pChn:%d  event:%d \n", pChn->mModId, (int)event);

    return SUCCESS;
}


void v5SdkInit(){
    ERRORTYPE ret = 0;
    MPP_SYS_CONF_S mSysConf;

    memset(&mSysConf, 0, sizeof(MPP_SYS_CONF_S));
    mSysConf.nAlignWidth = 32;
    ret = AW_MPI_SYS_SetConf(&mSysConf);
    if (SUCCESS != ret)
    {
        printf("Do AW_MPI_SYS_SetConf fail! ret:%d \n", ret);
        return;
    }

    ret = AW_MPI_SYS_Init();
    if (SUCCESS != ret)
    {
        printf("Do AW_MPI_SYS_Init fail! ret:%d \n", ret);
        return;
    }

    printf("Do sys init success!\n");
}

void ffmpegInit(){
    avcodec_register_all();
    av_register_all();
    //av_log_set_level(AV_LOG_TRACE);
    av_log_set_level(AV_LOG_ERROR);
    avformat_network_init();
}

void shmInit(){
    int status = 0;
    STREAM_SET streamSet;

    memset(&streamSet, 0, sizeof(streamSet));

    //force to set setresolution at inital time.
    streamSet.chn = 1;
    streamSet.liveresolution = streamSet.mpeg42resolution = streamSet.mpeg43resolution = streamSet.mpeg4resolution = -1;
    status = stream_init(stream_get_handle(), &streamSet);
}

static int vdec_create(void){

    int ret = 0;
    VDEC_CHN_ATTR_S vdec_attr;

    memset(&vdec_attr, 0, sizeof(VDEC_CHN_ATTR_S));
    vdec_attr.mType                         = g_venc_type;
    vdec_attr.mPicWidth                     = 1280;
    vdec_attr.mPicHeight                    = 720;
    vdec_attr.mInitRotation                 = ROTATE_NONE;
    vdec_attr.mOutputPixelFormat            = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vdec_attr.mVdecVideoAttr.mMode          = VIDEO_MODE_FRAME;
    vdec_attr.mVdecVideoAttr.mSupportBFrame = 0;

    ret = AW_MPI_VDEC_CreateChn(VDEC_CHN_0, &vdec_attr);
    if (ERR_VDEC_EXIST == ret)
    {
        printf("===============================================\n");
        printf("Vdec chn[%d] is exist, find next!\n", VDEC_CHN_0);
        printf("===============================================\n");
        return -1;
    }
    else if (SUCCESS != ret)
    {
        printf("===============================================\n");
        printf("Do AW_MPI_VDEC_CreateChn vdec_chn:%d fail! ret:0x%x\n", VDEC_CHN_0, ret);
        printf("===============================================\n");
        return -1;
    }

    memset(&g_cb_info,    0, sizeof(MPPCallbackInfo));
    g_cb_info.cookie   = NULL;
    g_cb_info.callback = NULL;

    ret = AW_MPI_VDEC_RegisterCallback(VDEC_CHN_0, &g_cb_info);
    if (SUCCESS != ret)
    {
        printf("===============================================\n");
        printf("Do AW_MPI_DEMUX_RegisterCallback vdec_chn:%d fail! ret:0x%x\n", VDEC_CHN_0, ret);
        printf("===============================================\n");
        return -1;
    }

    ret = AW_MPI_VDEC_StartRecvStream(VDEC_CHN_0);
    if (ret)
    {
        printf("Do AW_MPI_VDEC_StartRecvStream fail! vdec_chn:%d ret:%d\n", VDEC_CHN_0, ret);
        return -1;
    }

    return 0;
}

static int venc_create(void)
{
    int ret = 0;
    BOOL nSuccessFlag = FALSE;
    VENC_CHN_ATTR_S mVencChnAttr;

    VENC_CFG_S venc_cfg  = {
        .src_width      = 1280,
        .src_height     = 720,
        .dst_width      = 1280,
        .dst_height     = 720,
        .src_fps        = 30,
        .dst_fps        = 15,
        .bitrate        = 500*1000,
        .gop            = 30,
        .max_qp         = 0,
        .min_qp         = 0,
        .is_by_frame    = 1,
        .field          = VIDEO_FIELD_FRAME,
        .pixel_format   = MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420
    };

    memset(&mVencChnAttr, 0, sizeof(VENC_CHN_ATTR_S));

    mVencChnAttr.VeAttr.Type           = g_venc_type;
    mVencChnAttr.VeAttr.Rotate         = g_rotate;
    mVencChnAttr.VeAttr.SrcPicWidth    = venc_cfg.src_width;
    mVencChnAttr.VeAttr.SrcPicHeight   = venc_cfg.src_height;
    mVencChnAttr.VeAttr.PixelFormat    = venc_cfg.pixel_format;
    mVencChnAttr.VeAttr.Field          = venc_cfg.field;
    mVencChnAttr.VeAttr.MaxKeyInterval = venc_cfg.dst_fps * 2;

    if (PT_H264 == g_venc_type){
        mVencChnAttr.VeAttr.AttrH264e.bByFrame       = venc_cfg.is_by_frame;
        mVencChnAttr.VeAttr.AttrH264e.PicWidth       = venc_cfg.dst_width;
        mVencChnAttr.VeAttr.AttrH264e.PicHeight      = venc_cfg.dst_height;
        mVencChnAttr.VeAttr.AttrH264e.Profile        = g_profile;
        mVencChnAttr.VeAttr.AttrH264e.mbPIntraEnable = TRUE;

        switch(g_rc_mode){
            case 0: /* CBR mode */
                mVencChnAttr.RcAttr.mAttrH264Cbr.mGop           = venc_cfg.gop;
                mVencChnAttr.RcAttr.mAttrH264Cbr.mSrcFrmRate    = venc_cfg.src_fps;
                mVencChnAttr.RcAttr.mAttrH264Cbr.fr32DstFrmRate = venc_cfg.dst_fps;
                mVencChnAttr.RcAttr.mAttrH264Cbr.mBitRate       = venc_cfg.bitrate;
                mVencChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H264CBR;
                break;
            case 1: /* VBR mode */
                mVencChnAttr.RcAttr.mAttrH264Vbr.mGop           = venc_cfg.gop;
                mVencChnAttr.RcAttr.mAttrH264Vbr.mSrcFrmRate    = venc_cfg.src_fps;
                mVencChnAttr.RcAttr.mAttrH264Vbr.fr32DstFrmRate = venc_cfg.dst_fps;
                mVencChnAttr.RcAttr.mAttrH264Vbr.mMaxBitRate    = venc_cfg.bitrate;
                mVencChnAttr.RcAttr.mAttrH264Vbr.mMaxQp         = venc_cfg.max_qp;
                mVencChnAttr.RcAttr.mAttrH264Vbr.mMinQp         = venc_cfg.min_qp;
                mVencChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H264VBR;
                break;
            case 2: /* FIXQp mode */
                mVencChnAttr.RcAttr.mAttrH264FixQp.mGop           = venc_cfg.gop;
                mVencChnAttr.RcAttr.mAttrH264FixQp.mSrcFrmRate    = venc_cfg.src_fps;
                mVencChnAttr.RcAttr.mAttrH264FixQp.fr32DstFrmRate = venc_cfg.dst_fps;
                mVencChnAttr.RcAttr.mAttrH264FixQp.mIQp           = (venc_cfg.max_qp + venc_cfg.min_qp) / 2;
                mVencChnAttr.RcAttr.mAttrH264FixQp.mPQp           = (venc_cfg.max_qp + venc_cfg.min_qp) / 2;
                mVencChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H264FIXQP;
                break;
            case 3: /* ABR mode */
                mVencChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H264ABR;
                break;
            default:
                mVencChnAttr.RcAttr.mRcMode = VENC_RC_MODE_BUTT;
                printf("H264 Don't support this rc_mode:%d !\n", g_rc_mode);
                return -1;
        }
    } else if (PT_H265 == g_venc_type) {
        mVencChnAttr.VeAttr.AttrH265e.mbByFrame      = venc_cfg.is_by_frame;
        mVencChnAttr.VeAttr.AttrH265e.mPicWidth      = venc_cfg.dst_width;
        mVencChnAttr.VeAttr.AttrH265e.mPicHeight     = venc_cfg.dst_height;
        mVencChnAttr.VeAttr.AttrH265e.mProfile       = g_profile;
        mVencChnAttr.VeAttr.AttrH265e.mbPIntraEnable = TRUE;

        switch(g_rc_mode)
        {
            case 0: /* CBR mode */
                mVencChnAttr.RcAttr.mAttrH265Cbr.mGop           = venc_cfg.gop;
                mVencChnAttr.RcAttr.mAttrH265Cbr.mSrcFrmRate    = venc_cfg.src_fps;
                mVencChnAttr.RcAttr.mAttrH265Cbr.fr32DstFrmRate = venc_cfg.dst_fps;
                mVencChnAttr.RcAttr.mAttrH265Cbr.mBitRate       = venc_cfg.bitrate;
                mVencChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H265CBR;
                break;
            case 1: /* VBR mode */
                mVencChnAttr.RcAttr.mAttrH265Vbr.mGop           = venc_cfg.gop;
                mVencChnAttr.RcAttr.mAttrH265Vbr.mSrcFrmRate    = venc_cfg.src_fps;
                mVencChnAttr.RcAttr.mAttrH265Vbr.fr32DstFrmRate = venc_cfg.dst_fps;
                mVencChnAttr.RcAttr.mAttrH265Vbr.mMaxBitRate    = venc_cfg.bitrate;
                mVencChnAttr.RcAttr.mAttrH265Vbr.mMaxQp         = venc_cfg.max_qp;
                mVencChnAttr.RcAttr.mAttrH265Vbr.mMinQp         = venc_cfg.min_qp;
                mVencChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H265VBR;
                break;
            case 2: /* FIXQp mode */
                mVencChnAttr.RcAttr.mAttrH265FixQp.mGop           = venc_cfg.gop;
                mVencChnAttr.RcAttr.mAttrH265FixQp.mSrcFrmRate    = venc_cfg.src_fps;
                mVencChnAttr.RcAttr.mAttrH265FixQp.fr32DstFrmRate = venc_cfg.dst_fps;
                mVencChnAttr.RcAttr.mAttrH265FixQp.mIQp           = (venc_cfg.max_qp + venc_cfg.min_qp) / 2;
                mVencChnAttr.RcAttr.mAttrH265FixQp.mPQp           = (venc_cfg.max_qp + venc_cfg.min_qp) / 2;
                mVencChnAttr.RcAttr.mRcMode = VENC_RC_MODE_H265FIXQP;
                break;
            default:
                mVencChnAttr.RcAttr.mRcMode = VENC_RC_MODE_BUTT;
                printf("H265 Don't support this rc_mode:%d !\n", g_rc_mode);
                return -1;
        }
    }

    ret = AW_MPI_VENC_CreateChn(VENC_CHN_0, &mVencChnAttr);
    if (SUCCESS == ret)
    {
        nSuccessFlag = TRUE;
        printf("create venc channel[%d] success!\n", VENC_CHN_0);
    }
    else if (ERR_VENC_EXIST == ret)
    {
        printf("venc channel[%d] is exist, find next!\n", VENC_CHN_0);
    }
    else
    {
        printf("create venc channel[%d] ret[0x%x], find next!\n", VENC_CHN_0, ret);
    }

    if (nSuccessFlag == FALSE)
    {
        printf("fatal error! create venc channel fail!\n");
        return FAILURE;
    }

    VENC_FRAME_RATE_S stFrameRate;
    stFrameRate.SrcFrmRate = stFrameRate.DstFrmRate = venc_cfg.dst_fps;
    AW_MPI_VENC_SetFrameRate(VENC_CHN_0, &stFrameRate);

    /* Register Venc call back function */
    MPPCallbackInfo venc_cb;
    venc_cb.cookie   = NULL;
    venc_cb.callback = (MPPCallbackFuncType)&MPPCallbackFunc;

    AW_MPI_VENC_RegisterCallback(VENC_CHN_0, &venc_cb);

    ret = AW_MPI_VENC_StartRecvPic(VENC_CHN_0);
    if (ret)
    {
        printf("Do AW_MPI_VENC_StartRecvPic fail! ret:%d\n", ret);
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    int streamType = 0;
    int frameType;
    unsigned int timestamp, temporalId = 0;
    struct timeval ts;
    AVPacket pkt;
    AVFormatContext *ic = NULL;
    VENC_PACK_S h264_pack;
    VENC_STREAM_S *eStream = malloc(sizeof(VENC_STREAM_S));
    VDEC_STREAM_S *pStream = malloc(sizeof(VDEC_STREAM_S));
    VIDEO_FRAME_INFO_S *tVideoFrameInfo = malloc(sizeof(VIDEO_FRAME_INFO_S));
    AVDictionary *options = NULL;
    VencHeaderData *head_info = malloc(sizeof(VencHeaderData));
    unsigned char *buf = malloc(1920 * 1080 * 3 * sizeof(unsigned char));

    v5SdkInit();

    ffmpegInit();

    shmInit();

    av_dict_set(&options, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
    av_dict_set(&options,"rtsp_transport", "tcp", AV_DICT_IGNORE_SUFFIX);

    if((ret = avformat_open_input(&ic, outputName, NULL, &options)) < 0){
	printf("[%s]%s(%d) open : \"%s\" Fail!! \n", __FILE__, __func__, __LINE__, outputName);
    }

    av_dict_set(&options, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);

    if((ret = avformat_find_stream_info(ic, NULL)) < 0){
	printf("[%s]%s(%d) get stream info fail!! \n", __FILE__, __func__, __LINE__);
    }

    av_dump_format(ic, 0, outputName, 0);

    vdec_create();

    venc_create();

    while(1){
	if(av_read_frame(ic, &pkt)) {
	    //av_free_packet(&pkt);
	    av_packet_unref(&pkt);
            printf("[%d] av_read_frame error.[%s][%d]\n",__LINE__, strerror(errno), errno);
	    continue;
	}
	/* ---- decode start ----*/
	memset(pStream, 0, sizeof(pStream));
	pStream->pAddr = pkt.data;
	pStream->mLen = pkt.size;
	pStream->mPTS = pkt.pts;
	pStream->mbEndOfFrame = TRUE;
	pStream->mbEndOfStream = FALSE;

	ret = AW_MPI_VDEC_SendStream(VDEC_CHN_0, pStream, -1);

	if(SUCCESS == ret)
        {
	}else {
	    printf("vdec send fail\n");
	}

	ERRORTYPE ret = AW_MPI_VDEC_GetImage(VDEC_CHN_0, tVideoFrameInfo, -1);
        if(SUCCESS == ret)
        {
/*
	    printf("MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420 : %d\n",MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420);
	    printf("SUCCESS mId : %ld  width: %ld height: %ld\n", tVideoFrameInfo->mId, tVideoFrameInfo->VFrame.mWidth, tVideoFrameInfo->VFrame.mHeight);
	    printf("SUCCESS PIXEL_FORMAT_E : %d mVideoFormat: %d \n", tVideoFrameInfo->VFrame.mPixelFormat, tVideoFrameInfo->VFrame.mVideoFormat);
	    printf("SUCCESS [0] : 0x%x  [1]: 0x%x [2]: 0x%x\n", tVideoFrameInfo->VFrame.mPhyAddr[0], tVideoFrameInfo->VFrame.mPhyAddr[1], tVideoFrameInfo->VFrame.mPhyAddr[2]);
*/
	    ret = AW_MPI_VENC_SendFrame(VENC_CHN_0, tVideoFrameInfo, -1);
	    if(ret){
	    	printf("Do AW_MPI_VENC_SendFrame fail! [0x%x]\n", ret);
	    }

	    memset(eStream, 0, sizeof(eStream));
	    memset(&h264_pack, 0, sizeof(VENC_PACK_S));
	    memset(buf, 0, sizeof(1920 * 1080 * 3 * sizeof(unsigned char)));
	    eStream->mPackCount = 1;
	    eStream->mpPack = &h264_pack;
	    ret = AW_MPI_VENC_GetStream(VENC_CHN_0, eStream, -1);
	    if(SUCCESS == ret && eStream->mpPack != NULL && eStream->mpPack->mLen0 > 0)
    	    {
		unsigned char *tBuf = buf;
		unsigned int tLength = 0;
        	streamType = -1; //STREAM_MJPG, STREAM_H264_1,STREAM_H264_2;                    
        	gettimeofday(&ts, NULL);
        	timestamp = (ts.tv_sec*1000) + (ts.tv_usec/1000);

 	        if (H264E_NALU_ISLICE == eStream->mpPack->mDataType.enH264EType){
		    ret = AW_MPI_VENC_GetH264SpsPpsInfo(VENC_CHN_0, head_info);
		    if (SUCCESS != ret) {
                        printf("Do AW_MPI_VENC_GetH264SpsPpsInfo fail! ret:%d \n", ret);
                    }

		    memcpy(tBuf, head_info->pBuffer, head_info->nLength);
		    tBuf += head_info->nLength;
		    tLength += head_info->nLength;
		    frameType = I_FRAME;
		
		} else if (H264E_NALU_PSLICE == eStream->mpPack->mDataType.enH264EType){
		    frameType = P_FRAME;
		}
		memcpy(tBuf, eStream->mpPack->mpAddr0, eStream->mpPack->mLen0);
		tLength += eStream->mpPack->mLen0;

        	streamType = STREAM_H264_2;
        	ret = stream_write((void*)buf, tLength, frameType, streamType, timestamp, temporalId, stream_get_handle());

        	streamType = STREAM_H264_1;
        	ret = stream_write((void*)buf, tLength, frameType, streamType, timestamp, temporalId, stream_get_handle());

                if(AW_MPI_VENC_ReleaseStream(VENC_CHN_0, eStream) != SUCCESS)
                {
            		printf("jpeg stream data return fail!\n");
                }
    	    } else {
	    	printf("AW_MPI_VENC_GetStream FAIL [0x%x]\n",ret);
	    }
            AW_MPI_VDEC_ReleaseImage(VDEC_CHN_0, tVideoFrameInfo);
        } else {
	    printf("why not get frame from vdec, maybe EOF [0x%x]\n", ret);
	    break;
	}
	/* ---- decode end ----*/

	/* ---- encode start ----*/
/*
	ret = AW_MPI_VENC_SendFrame(VENC_CHN_0, videoFrameInfo, -1);
	if(ret){
	    printf("Do AW_MPI_VO_SendFrame fail!\n");
	}

	memset(pStream, 0, sizeof(pStream));

	if(SUCCESS == AW_MPI_VENC_GetStream(VENC_CHN_0, pStream, -1))
    	{
	    pkt.data = pStream->pAddr;
	    pkt.size = pStream->mLen;
	    pkt.pts = pStream->mPTS;

            if(AW_MPI_VENC_ReleaseStream(VENC_CHN_0, pStream) != SUCCESS)
            {
            	printf("jpeg stream data return fail!\n");
            }
    	}
*/
	/* ---- encode end ----*/
/*
        streamType = -1; //STREAM_MJPG, STREAM_H264_1,STREAM_H264_2;                    
        gettimeofday(&ts, NULL);
        timestamp = (ts.tv_sec*1000) + (ts.tv_usec/1000);

        streamType = STREAM_H264_2;
	if((pkt.data[4] == 0x61) || (pkt.data[4] == 0x21) || (pkt.data[4] == 0x41)) frameType = P_FRAME;
        else frameType = I_FRAME;

        ret = stream_write((void*)pkt.data, pkt.size, frameType, streamType, timestamp, temporalId, stream_get_handle());

        streamType = STREAM_H264_1;

        ret = stream_write((void*)pkt.data, pkt.size, frameType, streamType, timestamp, temporalId, stream_get_handle());
*/
    }

    return ret;
}

