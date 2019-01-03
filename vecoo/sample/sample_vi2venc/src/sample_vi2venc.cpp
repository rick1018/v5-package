/************************************************************************************************/
/* Copyright (C), 2016-2017, Allwinner Tech. Co., Ltd.                                          */
/************************************************************************************************/
/**
 * @file mpp_vi_venc.cpp
 * @brief 该目录是对mpp中的 VI+VENC 通路sample
 * @author id: wangguixing
 * @version v0.1
 * @date 2017-04-08
 */

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

#include "common.h"
#include "mpp_comm.h"

#include "rtsp/MediaStream.h"
#include "rtsp/TinyServer.h"

//#define CONFIG_MULTI_CHAN
/************************************************************************************************/
/*                                     Macros & Typedefs                                        */
/************************************************************************************************/
/* None */


/************************************************************************************************/
/*                                    Structure Declarations                                    */
/************************************************************************************************/
struct vi_venc_param {
    pthread_t       thd_id;
    int             venc_chn;
    int             run_flag;
    PAYLOAD_TYPE_E  venc_type;
    MediaStream    *stream;
};


/************************************************************************************************/
/*                                      Global Variables                                        */
/************************************************************************************************/
static struct vi_venc_param g_param_0;
static struct vi_venc_param g_param_1;

static MediaStream *g_stream_0 = NULL;
static MediaStream *g_stream_1 = NULL;

static unsigned int   g_rc_mode   = 0;          /* 0:CBR 1:VBR 2:FIXQp 3:ABR 4:QPMAP */
static unsigned int   g_profile   = 1;          /* 0: baseline  1:MP  2:HP  3: SVC-T [0,3] */
static PAYLOAD_TYPE_E g_venc_type = PT_H264;    /* PT_H264/PT_H265/PT_MJPEG */
static ROTATE_E       g_rotate    = ROTATE_NONE;

static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_4K_30FPS;
static MPP_COM_VI_TYPE_E g_vi_type_1 = VI_720P_30FPS;

static VENC_CFG_TYPE_E g_venc_type_0 = VENC_4K_TO_4K_20M_30FPS;
static VENC_CFG_TYPE_E g_venc_type_1 = VENC_720P_TO_720P_4M_25FPS;

/************************************************************************************************/
/*                                    Function Declarations                                     */
/************************************************************************************************/
/* None */


/************************************************************************************************/
/*                                     Function Definitions                                     */
/************************************************************************************************/

static ERRORTYPE MPPCallbackFunc(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    DB_PRT(" pChn:%d  event:%d \n", pChn->mModId, (int)event);

    return SUCCESS;
}

static void *SendStreamByRtsp(void *param)
{
    int            ret = 0;
    unsigned char *buf = NULL;
    unsigned int   len = 0;
    uint64_t       pts = 0;
    int            frame_type;
    VencHeaderData head_info;
    struct vi_venc_param *local_param = (struct vi_venc_param *)param;

    while (local_param->run_flag)
    {
        buf        = NULL;
        len        = 0;
        frame_type = -1;
        ret = mpp_comm_venc_get_stream(local_param->venc_chn, local_param->venc_type, -1, &buf, &len, &pts, &frame_type, &head_info);
        if (ret)
        {
            ERR_PRT("Do mpp_comm_venc_get_stream fail! ret:%d\n", ret);
            continue;
        }
        //printf("get stream  buf:%p  len:%d  frame_type:%d \n", buf, len, frame_type);

        if (1 == frame_type)
        {
            /* Get I frame */
            local_param->stream->appendVideoData(head_info.pBuffer, head_info.nLength, pts, MediaStream::FRAME_DATA_TYPE_HEADER);
            local_param->stream->appendVideoData(buf, len, pts, MediaStream::FRAME_DATA_TYPE_I);
        }
        else
        {
            local_param->stream->appendVideoData(buf, len, pts, MediaStream::FRAME_DATA_TYPE_P);
        }
    }

    DB_PRT("Out this function ... ... \n");
    return NULL;
}


static int CreateRtspServer(TinyServer **rtsp)
{
    int ret  = 0;
    int port = 8554;
    char ip[64] = {0};
    std::string ip_addr;

    ret = get_net_dev_ip("eth0", ip);
    if (ret)
    {
        ERR_PRT("Do get_net_dev_ip fail! ret:%d\n", ret);
        return -1;
    }
    DB_PRT("This dev eth0 ip:%s \n", ip);

    ip_addr = ip;
    *rtsp = TinyServer::createServer(ip_addr, port);

    printf("============================================================\n");
    printf(" V1.2  rtsp://%s:%d/ch0  \n", ip_addr.c_str(), port);
    printf("============================================================\n");

    return 0;
}


static int rtsp_start(TinyServer *rtsp)
{
    int ret = 0;

    MediaStream::MediaStreamAttr attr;
    if (PT_H264 == g_venc_type)
    {
        attr.videoType  = MediaStream::MediaStreamAttr::VIDEO_TYPE_H264;
    }
    else if (PT_H265 == g_venc_type)
    {
        attr.videoType  = MediaStream::MediaStreamAttr::VIDEO_TYPE_H265;
    }
    attr.audioType  = MediaStream::MediaStreamAttr::AUDIO_TYPE_AAC;
    attr.streamType = MediaStream::MediaStreamAttr::STREAM_TYPE_UNICAST;

    g_stream_0 = rtsp->createMediaStream("ch0", attr);
    rtsp->runWithNewThread();

    g_param_0.stream    = g_stream_0;
    g_param_0.run_flag  = 1;
    g_param_0.venc_chn  = VENC_CHN_0;
    g_param_0.venc_type = g_venc_type;
    ret = pthread_create(&g_param_0.thd_id, NULL, SendStreamByRtsp, &g_param_0);
    if (ret)
    {
       ERR_PRT("Do pthread_create fail! ret:%d\n", ret);
    }

#ifdef CONFIG_MULTI_CHAN
    g_param_1.stream    = g_stream_1;
    g_param_1.run_flag  = 1;
    g_param_1.venc_chn  = VENC_CHN_1;
    g_param_1.venc_type = g_venc_type;
    ret = pthread_create(&g_param_1.thd_id, NULL, SendStreamByRtsp, &g_param_1);
    if (ret)
    {
       ERR_PRT("Do pthread_create fail! ret:%d\n", ret);
    }
#endif

    return 0;
}


static void rtsp_stop(TinyServer *rtsp)
{
    g_param_0.run_flag = 0;
#ifdef CONFIG_MULTI_CHAN
    g_param_1.run_flag = 0;
#endif

    usleep(10*1000);

    rtsp->stop();
    pthread_join(g_param_0.thd_id, 0);
    delete g_param_0.stream;

#ifdef CONFIG_MULTI_CHAN
    pthread_join(g_param_1.thd_id, 0);
    delete g_param_1.stream;
#endif
}


static int g_yuv_run_flag  = 1;
static int g_yuv_save_flag = 1;
static void *vi_yuv_save_onefile_proc(void *param)
{
    int ret = 0;
    int cnt = 0, i = 0;
    int open_flag = 0;
    char file_name[256] = {0};
    FILE *fd;
    VIDEO_FRAME_INFO_S stFrameInfo;

    while (g_yuv_run_flag)
    {
        if ((ret = AW_MPI_VI_GetFrame(VI_DEV_3, VI_CHN_1, &stFrameInfo, 100)) != 0)
        {
            ERR_PRT("Do AW_MPI_VI_GetFrame fail! ret:%d \n", ret);
            continue ;
        }

        if (g_yuv_save_flag)
        {
            if (0 == open_flag)
            {
                DB_PRT("===================================================\n");
                sprintf(file_name, "/tmp/%dx%d_%d.yuv",
                        stFrameInfo.VFrame.mWidth,
                        stFrameInfo.VFrame.mHeight, i);
                fd = fopen(file_name, "wb+");
                DB_PRT("====== Save yuv file:%s start!  =======\n", file_name);
                open_flag = 1;
                i++;
            }

            cnt++;
            if (cnt < 90)
            {
                fwrite(stFrameInfo.VFrame.mpVirAddr[0],
                       stFrameInfo.VFrame.mWidth * stFrameInfo.VFrame.mHeight,
                       1, fd);
                fwrite(stFrameInfo.VFrame.mpVirAddr[1],
                       stFrameInfo.VFrame.mWidth * stFrameInfo.VFrame.mHeight >> 1,
                       1, fd);
            }
            else
            {
                cnt             = 0;
                open_flag       = 0;
                g_yuv_save_flag = 0;
                DB_PRT("====== Save yuv file:%s end !  =======\n", file_name);
                fclose(fd);
                DB_PRT("=========================================\n", file_name);
            }
        }

        if ((ret = AW_MPI_VI_ReleaseFrame(VI_DEV_3, VI_CHN_1, &stFrameInfo)) != 0)
        {
            ERR_PRT("Do AW_MPI_VI_ReleaseFrame fail! ret:%d \n", ret);
            continue ;
        }
        
    }

    return NULL;
}

static int vi_create(void)
{
    int ret = 0;
    VI_ATTR_S vi_attr;

    /**  create vi dev 2  src 1080P **/
    ret = mpp_comm_vi_get_attr(g_vi_type_0, &vi_attr);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_get_attr fail! ret:%d \n", ret);
        return -1;
    }
    vi_attr.use_current_win = 0;
    vi_attr.wdr_mode = 1;
    vi_attr.capturemode = V4L2_MODE_VIDEO; /* V4L2_MODE_VIDEO; V4L2_MODE_IMAGE; V4L2_MODE_PREVIEW */

    ret = mpp_comm_vi_dev_create(VI_DEV_0, &vi_attr);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_dev_create fail! ret:%d  vi_dev:%d\n", ret, VI_DEV_0);
        return -1;
    }

    ret = mpp_comm_vi_chn_create(VI_DEV_0, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_chn_create fail! ret:%d  vi_dev:%d  vi_chn:%d \n", ret, VI_DEV_0, VI_CHN_0);
        return -1;
    }

#ifdef CONFIG_MULTI_CHAN
    /**  create vi dev 3  src 720P **/
    ret = mpp_comm_vi_get_attr(g_vi_type_1, &vi_attr);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_get_attr fail! ret:%d \n", ret);
        return -1;
    }
    vi_attr.use_current_win = 1;
    vi_attr.wdr_mode = 1;
    vi_attr.capturemode = V4L2_MODE_VIDEO; /* V4L2_MODE_VIDEO; V4L2_MODE_IMAGE; V4L2_MODE_PREVIEW */

    ret = mpp_comm_vi_dev_create(VI_DEV_3, &vi_attr);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_dev_create fail! ret:%d  vi_dev:%d\n", ret, VI_DEV_3);
        return -1;
    }
    ret = mpp_comm_vi_chn_create(VI_DEV_3, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_chn_create fail! ret:%d  vi_dev:%d  vi_chn:%d \n", ret, VI_DEV_3, VI_CHN_0);
        return -1;
    }
#endif

    /*  Run isp server */
    //AW_MPI_ISP_Init();
    //AW_MPI_ISP_Run(ISP_DEV_0);

    return 0;
}


static int vi_destroy(void)
{
    int ret = 0;

    /*  Stop isp server */
    //AW_MPI_ISP_Stop(ISP_DEV_0);
    //AW_MPI_ISP_Exit();

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

#ifdef CONFIG_MULTI_CHAN
    /**  destroy vi dev 3  **/
    ret = mpp_comm_vi_chn_destroy(VI_DEV_3, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_chn_destroy fail! ret:%d  vi_dev:%d  vi_chn:%d \n", ret, VI_DEV_3, VI_CHN_0);
    }
    ret = mpp_comm_vi_dev_destroy(VI_DEV_3);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_dev_destroy fail! ret:%d  vi_dev:%d \n", ret, VI_DEV_3);
    }
#endif

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

#ifdef CONFIG_MULTI_CHAN
    ret = mpp_comm_venc_get_cfg(g_venc_type_1, &venc_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_venc_get_cfg fail! ret:%d \n", ret);
        return -1;
    }
    ret = mpp_comm_venc_create(VENC_CHN_1, g_venc_type, g_rc_mode, g_profile, g_rotate, &venc_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_venc_create fail! ret:%d \n", ret);
        return -1;
    }
#endif

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
#ifdef CONFIG_MULTI_CHAN
    ret = mpp_comm_venc_destroy(VENC_CHN_1);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_venc_destroy fail! ret:%d  venc_chn:%d \n", ret, VENC_CHN_1);
    }
#endif

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
#ifdef CONFIG_MULTI_CHAN
    ret = mpp_comm_vi_bind_venc(VI_DEV_3, VI_CHN_0, VENC_CHN_1);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_bind_venc fail! ret:%d \n", ret);
        return -1;
    }
#endif

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
#ifdef CONFIG_MULTI_CHAN
    ret = mpp_comm_vi_unbind_venc(VI_DEV_3, VI_CHN_0, VENC_CHN_1);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_unbind_venc fail! ret:%d\n", ret);
    }
#endif

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
#ifdef CONFIG_MULTI_CHAN
    ret = AW_MPI_VI_EnableVirChn(VI_DEV_3, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VI_EnableVirChn fail! ret:%d\n", ret);
        return -1;
    }
#endif

    ret = AW_MPI_VENC_StartRecvPic(VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VENC_StartRecvPic fail! ret:%d\n", ret);
        return -1;
    }
#ifdef CONFIG_MULTI_CHAN
    ret = AW_MPI_VENC_StartRecvPic(VENC_CHN_1);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VENC_StartRecvPic fail! ret:%d\n", ret);
        return -1;
    }
#endif

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
#ifdef CONFIG_MULTI_CHAN
    ret = AW_MPI_VENC_StopRecvPic(VENC_CHN_1);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VENC_StopRecvPic fail! ret:%d  venc_chn:%d \n", ret, VENC_CHN_1);
    }
#endif

    ret = AW_MPI_VI_DisableVirChn(VI_DEV_0, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VI_DisableVirChn fail! ret:%d  venc_chn:%d \n", ret, VENC_CHN_0);
    }
#ifdef CONFIG_MULTI_CHAN
    ret = AW_MPI_VI_DisableVirChn(VI_DEV_3, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VI_DisableVirChn fail! ret:%d  venc_chn:%d \n", ret, VENC_CHN_0);
    }
#endif

    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;

    printf("\033[2J");
    printf("\n\n\nDo sample vi+venc+rtsp. default:VI_4K@25fps+VENC(4K@25fps+720P@25fps) H264/CBR/MainProfile\n");
    //RunMenuCtrl(g_menu_guide);

    //ParseMenuParam();

    /* Setup 0. Create rtsp */
    TinyServer *rtsp;
    ret = CreateRtspServer(&rtsp);
    if (ret)
    {
        ERR_PRT("Do CreateRtspServer fail! ret:%d \n", ret);
        return -1;
    }

    /* Step 1. Init mpp system */
    ret = mpp_comm_sys_init();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_sys_init fail! ret:%d \n", ret);
        return -1;
    }

    /* Step 2. Get vi config to create 1080P/720P vi channel */
    ret = vi_create();
    if (ret)
    {
        ERR_PRT("Do vi_create fail! ret:%d\n", ret);
        goto _exit_1;
    }

    /* Step 3. Get venc config to create 1080P/720P venc channel */
    ret = venc_create();
    if (ret)
    {
        ERR_PRT("Do venc_create fail! ret:%d\n", ret);
        goto _exit_2;
    }

    /* Setup 4. Bind vi_0 to venc_0  and vi_1 to venc_1 */
    ret = components_bind();
    if (ret)
    {
        ERR_PRT("Do components_bind fail! ret:%d\n", ret);
        goto _exit_3;
    }

    /* Setup 5. Start vi and venc */
    ret = components_start();
    if (ret)
    {
        ERR_PRT("Do components_start fail! ret:%d\n", ret);
        goto _exit_4;
    }

    /* Setup 6. Create thread, and get stream send to rtsp */
    rtsp_start(rtsp);

    int d_val;
    while (1) {
	    sleep(1);
    }

    /* Setup 8. Do this block menu control function */

    rtsp_stop(rtsp);
    delete rtsp;

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

