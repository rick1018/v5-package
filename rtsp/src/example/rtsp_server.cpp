// PHZ
// RTSP服务器Demo

#include "RtspServer.h"
#include "xop.h"
#include "xop/NetInterface.h"
#include <thread>
#include <memory>
#include <iostream>
#include <string>

#include <mpi_videoformat_conversion.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "mpp_comm.h"

//using namespace xop;

static MPPCallbackInfo  g_cb_info;

static unsigned int   g_rc_mode   = 0;          /* 0:CBR 1:VBR 2:FIXQp 3:ABR 4:QPMAP */
static unsigned int   g_profile   = 1;          /* 0: baseline  1:MP  2:HP  3: SVC-T [0,3] */
static PAYLOAD_TYPE_E g_venc_type = PT_H264;    /* PT_H264/PT_H265/PT_MJPEG */
static ROTATE_E       g_rotate    = ROTATE_NONE;

static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_4K_30FPS;

static VENC_CFG_TYPE_E g_venc_type_0 = VENC_4K_TO_4K_20M_30FPS;

static ERRORTYPE MPPCallbackFunc(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    DB_PRT(" pChn:%d  event:%d \n", pChn->mModId, (int)event);

    return SUCCESS;
}

static int vi_create_new(void){
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
    stAttr.nbufs = 10;
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

    ret = AW_MPI_VI_CreateVirChn(VI_DEV_0, VI_CHN_1, NULL);
    if(ret != SUCCESS){
        printf("fatal error! createVirChn[%d] fail!", VI_CHN_0);
    }
    return ret;
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

static int components_bind(void)
{
    int ret = 0;

    ret = mpp_comm_vi_bind_venc(VI_DEV_0, VI_CHN_0, VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_bind_venc fail! ret:%d \n", ret);
        return -1;
    }

    ret = mpp_comm_vi_bind_vo(VI_DEV_0, VI_CHN_1, VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_chn_create fail! ret:%d  vi_dev:%d  vi_chn:%d \n", ret, VI_DEV_0, VI_CHN_1);
        return -1;
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

    ret = AW_MPI_VI_EnableVirChn(VI_DEV_0, VI_CHN_1);
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
    return 0;
}

// 负责音视频数据转发的线程函数
void snedFrame(xop::RtspServer* rtspServer, xop::MediaSessionId sessionId, int& clients);

int main(int agrc, char **argv)
{	
    XOP_Init(); //WSAStartup

    int clients = 0, ret = 0; // 记录当前客户端数量
    std::string ip = xop::NetInterface::getLocalIPAddress(); //获取网卡ip地址
    std::string rtspUrl;
   
    std::shared_ptr<xop::EventLoop> eventLoop(new xop::EventLoop());  
    xop::RtspServer server(eventLoop.get(), ip, 554);  //创建一个RTSP服务器

    xop::MediaSession *session = xop::MediaSession::createNew("live"); //创建一个媒体会话, url: rtsp://ip/live
    rtspUrl = "rtsp://" + ip + "/" + session->getRtspUrlSuffix();
    
    // 添加音视频流到媒体会话, track0:h264, track1:aac
    session->addMediaSource(xop::channel_0, xop::H264Source::createNew()); 
    //session->addMediaSource(xop::channel_1, xop::AACSource::createNew(44100,2));
    // session->startMulticast(); // 开启组播(ip,端口随机生成), 默认使用 RTP_OVER_UDP, RTP_OVER_RTSP

    // 设置通知回调函数。 在当前会话中, 客户端连接或断开会通过回调函数发起通知
    session->setNotifyCallback([&clients, &rtspUrl](xop::MediaSessionId sessionId, uint32_t numClients) {
        clients = numClients; //获取当前媒体会话客户端数量
        std::cout << "[" << rtspUrl << "]" << " Online: " << clients << std::endl;
    });

    ret = mpp_comm_sys_init();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_sys_init fail! ret:%d \n", ret);
        return -1;
    }

    ret = vi_create_new();
    if (ret)
    {
        ERR_PRT("Do vi_create fail! ret:%d\n", ret);
    }

    ret = venc_create();
    if (ret)
    {
        ERR_PRT("Do venc_create fail! ret:%d\n", ret);
    }

    ret = components_bind();
    if (ret)
    {
        ERR_PRT("Do components_bind fail! ret:%d\n", ret);
    }

    /* Start */
    ret = components_start();
    if (ret)
    {
        ERR_PRT("Do components_start fail! ret:%d\n", ret);
    }
        
    xop::MediaSessionId sessionId = server.addMeidaSession(session); //添加session到RtspServer后, session会失效
    //server.removeMeidaSession(sessionId); //取消会话, 接口线程安全
         
    std::thread t1(snedFrame, &server, sessionId, std::ref(clients)); //开启负责音视频数据转发的线程
    t1.detach(); 
   
    eventLoop->loop(); //主线程运行 RtspServer 

    getchar();
    return 0;
}

// 负责音视频数据转发的线程函数
void snedFrame(xop::RtspServer* rtspServer, xop::MediaSessionId sessionId, int& clients)
{       
    int            ret = 0;
    unsigned char *buf = NULL;
    unsigned int   len = 0;
    uint64_t       pts = 0;
    int            frame_type = 0;
    VencHeaderData head_info;

    while(1)
    {
	ret = mpp_comm_venc_get_stream(VENC_CHN_0, g_venc_type, -1, &buf, &len, &pts, &frame_type, &head_info);
        if(clients > 0) // 媒体会话有客户端在线, 发送音视频数据
        {
	    printf("SEND\n");
            xop::AVFrame videoFrame = {0};
	    if(frame_type == 1){
                videoFrame.size = len + head_info.nLength;  // 视频帧大小 
                videoFrame.timestamp = pts; // 时间戳, 建议使用编码器提供的时间戳
                videoFrame.buffer.reset(new char[videoFrame.size]);
                memcpy(videoFrame.buffer.get(), head_info.pBuffer, head_info.nLength);					
                memcpy(videoFrame.buffer.get() + head_info.nLength, &buf, len);					
	    } else {
                videoFrame.size = len;  // 视频帧大小 
                videoFrame.timestamp = pts; // 时间戳, 建议使用编码器提供的时间戳
                videoFrame.buffer.reset(new char[videoFrame.size]);
                memcpy(videoFrame.buffer.get(), &buf, videoFrame.size);					
            }
            rtspServer->pushFrame(sessionId, xop::channel_0, videoFrame); //送到服务器进行转发, 接口线程安全
        }
        xop::Timer::sleep(1); // 实际使用需要根据帧率计算延时!
    }
}

