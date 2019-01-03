/************************************************************************************************/
/*                                      Include Files                                           */
/************************************************************************************************/

#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi_videoformat_conversion.h>

#include "common.h"
#include "mpp_comm.h"

struct vi_venc_param {
    pthread_t       thd_id;
    int             venc_chn;
    int             run_flag;
    PAYLOAD_TYPE_E  venc_type;
};

/************************************************************************************************/
/*                                      Global Variables                                        */
/************************************************************************************************/

static MPPCallbackInfo  g_cb_info;

static struct vi_venc_param g_param_0;
static struct vi_venc_param g_param_1;

static unsigned int   g_rc_mode   = 0;          /* 0:CBR 1:VBR 2:FIXQp 3:ABR 4:QPMAP */
static unsigned int   g_profile   = 1;          /* 0: baseline  1:MP  2:HP  3: SVC-T [0,3] */
static PAYLOAD_TYPE_E g_venc_type = PT_H265;    /* PT_H264/PT_H265/PT_MJPEG */
static ROTATE_E       g_rotate    = ROTATE_NONE;

static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_4K_30FPS;

static VENC_CFG_TYPE_E g_venc_type_0 = VENC_4K_TO_4K_20M_30FPS;

/************************************************************************************************/
/*                                    Function Declarations                                     */
/************************************************************************************************/
/* None */
#define HLAY(chn, lyl) (chn*4+lyl)

/************************************************************************************************/
/*                                     Function Definitions                                     */
/************************************************************************************************/

static ERRORTYPE MPPCallbackFunc(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    DB_PRT(" pChn:%d  event:%d \n", pChn->mModId, (int)event);

    return SUCCESS;
}

static void *SendStream(void *param)
{
    int            ret = 0;
    unsigned char *buf = NULL;
    unsigned int   len = 0;
    uint64_t       pts = 0;
    int            frame_type;
    uint64_t curPts = 0;
    VencHeaderData head_info;
    struct vi_venc_param *local_param = (struct vi_venc_param *)param;
    VIDEO_FRAME_INFO_S       video_frame = {0};
    VDEC_STREAM_S vdec_stream = {0};
    VO_LAYER  vo_layer = 0;
    vo_layer = HLAY(VO_CHN_0, 0);
    printf("%s\n", __func__);
    while (1)
    {
        buf        = NULL;
        len        = 0;
        frame_type = -1;
	memset(&vdec_stream, 0, sizeof(VDEC_STREAM_S));
        ret = mpp_comm_venc_get_stream(local_param->venc_chn, local_param->venc_type, -1, &buf, &len, &pts, &frame_type, &head_info);
        if (ret)
        {
            ERR_PRT("Do mpp_comm_venc_get_stream fail! ret:%d\n", ret);
	}

        if (1 == frame_type)
        {
	    vdec_stream.pAddr = head_info.pBuffer;
	    vdec_stream.mLen = head_info.nLength;
	    vdec_stream.mPTS = pts;
	    vdec_stream.mbEndOfFrame = FALSE;
	    vdec_stream.mbEndOfStream = FALSE;
	    ret = AW_MPI_VDEC_SendStream(VDEC_CHN_0, &vdec_stream, -1);
            if (ret)
            {
                ERR_PRT("Do AW_MPI_VDEC_SendStream fail! ret:%d\n", ret);
	    }
	    vdec_stream.pAddr = buf;
	    vdec_stream.mLen = len;
	    vdec_stream.mPTS = pts;
	    vdec_stream.mbEndOfFrame = TRUE;
	    vdec_stream.mbEndOfStream = FALSE;
	    ret = AW_MPI_VDEC_SendStream(VDEC_CHN_0, &vdec_stream, -1);
            if (ret)
            {
                ERR_PRT("Do AW_MPI_VDEC_SendStream fail! ret:%d\n", ret);
	    }
        }
        else
        {
	    vdec_stream.pAddr = buf;
	    vdec_stream.mLen = len;
	    vdec_stream.mPTS = pts;
	    vdec_stream.mbEndOfFrame = TRUE;
	    vdec_stream.mbEndOfStream = FALSE;
	    ret = AW_MPI_VDEC_SendStream(VDEC_CHN_0, &vdec_stream, -1);
            if (ret)
            {
                ERR_PRT("Do AW_MPI_VDEC_SendStream fail! ret:%d\n", ret);
	    }
        }
    }

    printf("Out this function ... ... \n");
    return NULL;
}

static int thread_start()
{
    int ret = 0;

    g_param_0.run_flag  = 1;
    g_param_0.venc_chn  = VENC_CHN_0;
    g_param_0.venc_type = g_venc_type;
    ret = pthread_create(&g_param_0.thd_id, NULL, SendStream, &g_param_0);
    if (ret)
    {
       ERR_PRT("Do pthread_create fail! ret:%d\n", ret);
    }

    return 0;
}

static int vo_create(void)
{
    int ret = 0;
    int mVoLayer = 0;
    int hlay0 = 0;
    AW_MPI_VO_Enable(0);
    AW_MPI_VO_AddOutsideVideoLayer(HLAY(2, 0));
    AW_MPI_VO_CloseVideoLayer(HLAY(2, 0)); /* close ui layer. */

    while(hlay0 < VO_MAX_LAYER_NUM)
    {
        if(SUCCESS == AW_MPI_VO_EnableVideoLayer(hlay0))
        {
            break;
        }
        hlay0++;
    }
    if(hlay0 >= VO_MAX_LAYER_NUM)
    {
        printf("fatal error! enable video layer fail!");
    }

    VO_PUB_ATTR_S spPubAttr;
    AW_MPI_VO_GetPubAttr(0, &spPubAttr);
    spPubAttr.enIntfType = VO_INTF_HDMI;
    spPubAttr.enIntfSync = VO_OUTPUT_3840x2160_30;
    AW_MPI_VO_SetPubAttr(0, &spPubAttr);

   VO_VIDEO_LAYER_ATTR_S mLayerAttr;
    mVoLayer = hlay0;
    AW_MPI_VO_GetVideoLayerAttr(mVoLayer, &mLayerAttr);
    mLayerAttr.stDispRect.X = 0;
    mLayerAttr.stDispRect.Y = 0;
    mLayerAttr.stDispRect.Width = 3840;
    mLayerAttr.stDispRect.Height = 2160;
    AW_MPI_VO_SetVideoLayerAttr(mVoLayer, &mLayerAttr);

    ret = AW_MPI_VO_EnableChn(mVoLayer, 0);
    if(SUCCESS == ret){
        printf("create vo channel[0] success!");
    }
    else if(ERR_VO_CHN_NOT_DISABLE == ret){
        printf("vo channel[0] is exist, find next!");
    } else {
        printf("fatal error! create vo channel[0] ret[0x%x]!", ret);
    }

    MPPCallbackInfo cbInfo;
    cbInfo.cookie = NULL;
    cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackFunc;
    AW_MPI_VO_RegisterCallback(mVoLayer, 0, &cbInfo);
    AW_MPI_VO_SetChnDispBufNum(mVoLayer, 0, 2);
    return 0;
}

static int vi_create(void)
{
    int ret = 0;
    VI_ATTR_S stAttr;

    ret = AW_MPI_VI_CreateVipp(VI_DEV_0);
    if (ret != SUCCESS){
        printf("fatal error! AW_MPI_VI CreateVipp failed");
    }

    ret = AW_MPI_VI_GetVippAttr(VI_DEV_0, &stAttr);
    if (ret != SUCCESS){
        printf("fatal error! AW_MPI_VI GetVippAttr failed");
    }

    memset(&stAttr, 0, sizeof(VI_ATTR_S));
    stAttr.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    stAttr.memtype = V4L2_MEMORY_MMAP;
    stAttr.format.pixelformat = map_PIXEL_FORMAT_E_to_V4L2_PIX_FMT(MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420);
    stAttr.format.field = V4L2_FIELD_NONE;
    stAttr.format.width = 3840;
    stAttr.format.height = 2160;
    stAttr.nbufs = 5;
    stAttr.nplanes = 2;
    stAttr.use_current_win = 0;
    stAttr.fps = 30;
    ret = AW_MPI_VI_SetVippAttr(VI_DEV_0, &stAttr);
    if (ret != SUCCESS){
        printf("fatal error! AW_MPI_VI SetVippAttr failed");
    }

    ret = AW_MPI_VI_EnableVipp(VI_DEV_0);
    if(ret != SUCCESS){
        printf("fatal error! enableVipp fail!");
    }

    ret = AW_MPI_VI_CreateVirChn(VI_DEV_0, VI_CHN_0, NULL);
    if(ret != SUCCESS){
        printf("fatal error! createVirChn[%d] fail!", VI_CHN_0);
    }

    return ret;
}


static int vi_destroy(void)
{
    int ret = 0;

    /**  destroy vi dev 2  **/
    ret = mpp_comm_vi_chn_destroy(VI_DEV_0, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_chn_destroy fail! ret:%d  vi_dev:%d  vi_chn:%d \n", ret, VI_DEV_0, VI_CHN_0);
    }
    ret = mpp_comm_vi_dev_destroy(VI_DEV_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_dev_destroy fail! ret:%d  vi_dev:%d \n", ret, VI_DEV_0);
    }

    return 0;
}

static int venc_create_new(void)
{
    int ret = 0;
    BOOL nSuccessFlag = FALSE;
    VENC_CHN_ATTR_S mVencChnAttr;

    VENC_CFG_S venc_cfg  = {
        .src_width      = 3840,
        .src_height     = 2160,
        .dst_width      = 3840,
        .dst_height     = 2160,
        .src_fps        = 30,
        .dst_fps        = 30,
        .bitrate        = 20*1000*1000,
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
                ERR_PRT("H264 Don't support this rc_mode:%d !\n", rc_mode);
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
                ERR_PRT("H265 Don't support this rc_mode:%d !\n", rc_mode);
                return -1;
        }
    }
    ret = AW_MPI_VENC_CreateChn(VENC_CHN_0, &mVencChnAttr);
    if (SUCCESS == ret)
    {
        nSuccessFlag = TRUE;
        DB_PRT("create venc channel[%d] success!\n", venc_chn);
    }
    else if (ERR_VENC_EXIST == ret)
    {
        ERR_PRT("venc channel[%d] is exist, find next!\n", venc_chn);
    }
    else
    {
        ERR_PRT("create venc channel[%d] ret[0x%x], find next!\n", venc_chn, ret);
    }

    if (nSuccessFlag == FALSE)
    {
        ERR_PRT("fatal error! create venc channel fail!\n");
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

    return 0;

}

static int venc_create(void)
{
    int ret = 0;

    VENC_CFG_S venc_cfg  = {0};

    ret = mpp_comm_venc_get_cfg(g_venc_type_0, &venc_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_venc_get_cfg fail! ret:%d \n", ret);
        return -1;
    }
    ret = mpp_comm_venc_create(VENC_CHN_0, g_venc_type, g_rc_mode, g_profile, g_rotate, &venc_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_venc_create fail! ret:%d \n", ret);
        return -1;
    }


    /* Register Venc call back function */
    MPPCallbackInfo venc_cb;
    venc_cb.cookie   = NULL;
    venc_cb.callback = (MPPCallbackFuncType)&MPPCallbackFunc;

    AW_MPI_VENC_RegisterCallback(VENC_CHN_0, &venc_cb);

    return 0;

}

static int venc_destroy(void)
{
    int ret = 0;

    ret = mpp_comm_venc_destroy(VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_venc_destroy fail! ret:%d  venc_chn:%d \n", ret, VENC_CHN_0);
    }

    return 0;
}


static int components_bind(void)
{
    int ret = 0;

    ret = mpp_comm_vi_bind_venc(VI_DEV_0, VI_CHN_0, VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_bind_venc fail! ret:%d \n", ret);
        return -1;
    }

    ret = mpp_comm_vdec_bind_vo(VDEC_CHN_0, VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vdec_bind_vo fail! ret:%d vdec_chn:%d vo_chn:%d\n", ret, VDEC_CHN_0, VO_CHN_0);
        return -1;
    }

    return 0;
}


static int components_unbind(void)
{
    int ret = 0;

    ret = mpp_comm_vi_unbind_venc(VI_DEV_0, VI_CHN_0, VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_unbind_venc fail! ret:%d\n", ret);
    }

    return 0;
}


static int components_start(void)
{
    int ret = 0;

    ret = AW_MPI_VI_EnableVirChn(VI_DEV_0, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VI_EnableVirChn fail! ret:%d\n", ret);
        return -1;
    }

    ret = AW_MPI_VENC_StartRecvPic(VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VENC_StartRecvPic fail! ret:%d\n", ret);
        return -1;
    }

    ret = AW_MPI_VDEC_StartRecvStream(VDEC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VDEC_StartRecvStream fail! vdec_chn:%d ret:%d\n", VDEC_CHN_0, ret);
        return -1;
    }

    ret = mpp_comm_vo_chn_start(VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_chn_start vo_chn:%d fail! ret:%d\n", VO_CHN_0, ret);
        return -1;
    }

    return 0;
}

static int components_stop(void)
{
    int ret = 0;

    ret = AW_MPI_VENC_StopRecvPic(VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VENC_StopRecvPic fail! ret:%d  venc_chn:%d \n", ret, VENC_CHN_0);
    }

    ret = AW_MPI_VI_DisableVirChn(VI_DEV_0, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VI_DisableVirChn fail! ret:%d  venc_chn:%d \n", ret, VENC_CHN_0);
    }

    return 0;
}

static int vdec_create(void){

    int ret = 0;
    VDEC_CHN_ATTR_S vdec_attr;

    memset(&vdec_attr, 0, sizeof(VDEC_CHN_ATTR_S));
    vdec_attr.mType                         = g_venc_type;
    vdec_attr.mPicWidth                     = 3840;
    vdec_attr.mPicHeight                    = 2160;
    vdec_attr.mInitRotation                 = g_rotate;
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

    return 1;
}


int main(int argc, char **argv)
{
    int ret = 0;
    int sockFd = 0;

    /* Init mpp system */
    ret = mpp_comm_sys_init();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_sys_init fail! ret:%d \n", ret);
        return -1;
    }

    /* Get vi config to create 1080P/720P vi channel */
    ret = vi_create();
    if (ret)
    {
        ERR_PRT("Do vi_create fail! ret:%d\n", ret);
        goto _exit_1;
    }

    /* Create vo device and layer */
    ret = vo_create();
    if (ret)
    {
        ERR_PRT("Do vo_create fail! ret:%d\n", ret);
        goto _exit_1;
    }

    /* Get venc config to create 1080P/720P venc channel */

    ret = venc_create_new();
    if (ret)
    {
        ERR_PRT("Do venc_create fail! ret:%d\n", ret);
    }

    ret = vdec_create();
    if (ret)
    {
        printf("Do vdec_create fail! ret:%d\n", ret);
    }

    /* Bind */
    ret = components_bind();
    if (ret)
    {
        ERR_PRT("Do components_bind fail! ret:%d\n", ret);
        goto _exit_3;
    }

    /* Start */
    ret = components_start();
    if (ret)
    {
        ERR_PRT("Do components_start fail! ret:%d\n", ret);
        goto _exit_4;
    }

    thread_start();

    while (1) {
	sleep(1);
    }

_exit_4:
    components_stop();

_exit_3:
    components_unbind();

_exit_2:
    venc_destroy();

_exit_1:
    vi_destroy();

    ret = mpp_comm_sys_exit();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_sys_exit fail! ret:%d \n", ret);
    }

    return ret;
}

