/************************************************************************************************/
/*                                      Include Files                                           */
/************************************************************************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <mpi_videoformat_conversion.h>

#include <linux/tcp.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "mpp_comm.h"
#include "tcp_states.h"

#include "libavformat/avformat.h"

#define _DEBUG_MSG(e,...) fprintf(stderr, "[%s:%d] " e "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HLAY(chn, lyl) (chn*4+lyl)

static MPPCallbackInfo  g_cb_info;

static unsigned int   g_rc_mode   = 0;          /* 0:CBR 1:VBR 2:FIXQp 3:ABR 4:QPMAP */
static unsigned int   g_profile   = 1;          /* 0: baseline  1:MP  2:HP  3: SVC-T [0,3] */
static PAYLOAD_TYPE_E g_venc_type = PT_H264;    /* PT_H264/PT_H265/PT_MJPEG */
static ROTATE_E       g_rotate    = ROTATE_NONE;

static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_4K_30FPS;

static VENC_CFG_TYPE_E g_venc_type_0 = VENC_4K_TO_4K_20M_30FPS;
int clientPid = -1;
int videoindex = -1;
AVFormatContext *ifmt_ctx = NULL;

const char *in_filename = "rtsp://127.0.0.1";

static ERRORTYPE MPPCallbackFunc(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    DB_PRT(" pChn:%d  event:%d \n", pChn->mModId, (int)event);

    return SUCCESS;
}

static int vo_create(void){
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
        _DEBUG_MSG("fatal error! enable video layer fail!");
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
        _DEBUG_MSG("create vo channel[0] success!");
    }
    else if(ERR_VO_CHN_NOT_DISABLE == ret){
        _DEBUG_MSG("vo channel[0] is exist, find next!");
    } else {
        _DEBUG_MSG("fatal error! create vo channel[0] ret[0x%x]!", ret);
    }
 
    MPPCallbackInfo cbInfo;
    cbInfo.cookie = NULL;
    cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackFunc;
    AW_MPI_VO_RegisterCallback(mVoLayer, 0, &cbInfo);
    AW_MPI_VO_SetChnDispBufNum(mVoLayer, 0, 2);
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

    return 0;
}

static int components_bind(void)
{
    int ret = 0;

    ret = mpp_comm_vdec_bind_vo(VDEC_CHN_0, VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vdec_bind_vo fail! ret:%d vdec_chn:%d vo_chn:%d\n", ret, VDEC_CHN_0, VO_CHN_0);
        return -1;
    }

    return 0;
}

static int components_start(void)
{
    int ret = 0;
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
static void initV5(void){
    int ret = 0;
    ret = mpp_comm_sys_init();
    if (ret)
    {
        printf("Do mpp_comm_sys_init fail! ret:%d \n", ret);
        return;
    }

    ret = vdec_create();
    if (ret)
    {
        printf("Do vi_create fail! ret:%d\n", ret);
    }

    ret = vo_create();
    if (ret)
    {
        printf("Do vo_create fail! ret:%d\n", ret);
    }

    ret = components_bind();
    if (ret)
    {
        printf("Do components_bind fail! ret:%d\n", ret);
    }
}

static void initFFmpeg(void){
    int ret = 0;

    av_register_all();

    avformat_network_init();

    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
		printf( "Could not open input file.");
    }


    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		printf( "Failed to retrieve input stream information");
    }

    av_dump_format(ifmt_ctx, 0, in_filename, 0);
}

int main(int argc, char **argv)
{
    int ret = 0;
    int d_val;
    AVFormatContext* octx = NULL;
    VDEC_STREAM_S vdec_stream = {0};
    FAVPacket pkt;

    initV5();

    initFFmpeg();

    ret = components_start();
    if (ret)
    {
        printf("Do components_start fail! ret:%d\n", ret);
    }

    while (1) {
	if(av_read_frame(ifmt_ctx, &pkt) < 0)break;

	vdec_stream.mPTS = pkt.pts;
	vdec_stream.mLen = pkt.size;
	vdec_stream.mbEndOfStream = FALSE;
	vdec_stream.mbEndOfFrame = TRUE;
	vdec_stream.pAddr = pkt.data;
	ret = AW_MPI_VDEC_SendStream(VDEC_CHN_0, &vdec_stream, -1);
        if (ret){
            printf("Do AW_MPI_VDEC_SendStream fail! ret:%d\n", ret);
        }

    }
    return ret;
}

