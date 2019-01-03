/************************************************************************************************/
/*                                      Include Files                                           */
/************************************************************************************************/
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

#include "common.h"
#include "mpp_comm.h"

#include "tcp_states.h"

struct v5_socket_data{
    int frame_type;
    uint64_t pts;
    int endOfFrame;
    int endOfStream;
    uint64_t len;
    int offset;
    int start;
    uint64_t cksum;
    unsigned char data[1000];
};

#define MAXNO(__A__, __B__) ((__A__ > __B__) ? __A__ : __B__)

static MPPCallbackInfo  g_cb_info;
static DEMUX_INFO_S     g_demux_info;
static DEMUX_CB_PARAM_S g_demux_param;

static MUX_CB_PARAM_S  g_mux_param;
static MPPCallbackInfo g_mux_cb;

static unsigned int   g_rc_mode   = 0;          /* 0:CBR 1:VBR 2:FIXQp 3:ABR 4:QPMAP */
static unsigned int   g_profile   = 1;          /* 0: baseline  1:MP  2:HP  3: SVC-T [0,3] */
static PAYLOAD_TYPE_E g_venc_type = PT_H265;    /* PT_H264/PT_H265/PT_MJPEG */
static ROTATE_E       g_rotate    = ROTATE_NONE;

//static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_1080P_30FPS;
static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_4K_30FPS;

//static VENC_CFG_TYPE_E g_venc_type_0 = VENC_1080P_TO_1080P_8M_30FPS;
static VENC_CFG_TYPE_E g_venc_type_0 = VENC_4K_TO_4K_20M_30FPS;
int clientPid = -1;
#define HLAY(chn, lyl) (chn*4+lyl)

static ERRORTYPE MPPCallbackFunc(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    DB_PRT(" pChn:%d  event:%d \n", pChn->mModId, (int)event);

    return SUCCESS;
}

int user_connect (int server)
{
    struct sockaddr_in cc;
    int addr_len = sizeof(cc);

    int result = -1;
    struct sockaddr_in caddr;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 500;

    int clientIndex = 0;

    int optval = 1;
    result = accept( server, (struct sockaddr*)&caddr, (socklen_t *)&addr_len);
    //if (!strcmp(inet_ntoa(caddr.sin_addr), "0.0.0.0")) return -1;

    if(setsockopt(result, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
        printf("set SO_RCVTIMEO ERROR\n");
    if(setsockopt(result, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
        printf("set SO_SNDTIMEO ERROR\n");

    return result;
}

int socket_server(unsigned short int server_port){
    struct sockaddr_in saddr;
    int server_pid = -1;
    int optval = 1;

    server_pid = socket( AF_INET, SOCK_STREAM, 0 ); 
    if(server_pid < 0)
    {
        printf("TCP Server start Error!!\n");
        return -1;
    }

    memset( &saddr , 0 , sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons( server_port );
    saddr.sin_addr.s_addr = INADDR_ANY;

    //if(setsockopt( server_pid, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0 ) return -1;
    if(setsockopt( server_pid, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval)) < 0 ){
	printf("%s(%d) set socket no delay fail\n", __func__, __LINE__);
    }

    while (bind( server_pid , (struct sockaddr *)&saddr , 16 ) < 0)
    {
        printf("TCP Server set fail\n");
        printf("Tcp Server Port[ %d ] locking.. Waitting... \n", server_port);
        sleep(1);
    }
    if (listen(server_pid , 5) < 0)
    {
        printf("TCP Server listen fail !! \n");
        return -1;
    }
    return server_pid; 
}

unsigned checksum(void *buffer, uint64_t len, unsigned int seed)
{
      unsigned char *buf = (unsigned char *)buffer;
      size_t i;

      for (i = 0; i < len; ++i)
            seed += (unsigned int)(*buf++);

      return seed;
}

static void sendToSocket(unsigned char *framData, uint64_t framLen, int frame_type, uint64_t pts){

    struct v5_socket_data info;
    uint64_t len = framLen;
    unsigned int offset = 0;
    unsigned char *data = framData;

    info.frame_type = frame_type;

    if(frame_type == 1)
	info.endOfFrame = 0;	
    else 
	info.endOfFrame = 1;	

    info.len = framLen;
    info.pts = pts;
    info.endOfStream = 0;
    info.start = 1;
    info.cksum = checksum(framData, framLen, 0);

    do{
        data += offset;
        if(len > sizeof(info.data))
            offset = sizeof(info.data);
	else 
	    offset = len;

        memcpy(&info.data, data, offset);
        info.offset = offset;

        if(send(clientPid, &info, sizeof(struct v5_socket_data), 0) <= 0){
	    close(clientPid);
	    clientPid = -1;
            break;
        }

        info.start = 0;
        len -= offset;
    }while(len > 0);
}

int socket_check(int sock)
{
     if(sock<=0)
         return 0;
     struct tcp_info info;
     int len = sizeof(info);
     getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
     if((info.tcpi_state == TCP_ESTABLISHED)) {
        return 1;
     }else{
        return 0;
     }

    return 1;
}

static void *SendStream_1(void *param)
{
    int            ret = 0;
    unsigned char *buf = NULL;
    unsigned int   len = 0;
    uint64_t       pts = 0;
    int            frame_type = 0;
    uint64_t curPts = 0;
    VencHeaderData head_info;
    VIDEO_FRAME_INFO_S       video_frame = {0};
    VDEC_STREAM_S vdec_stream = {0};
    struct timeval tv;
    uint64_t timeStamp = 0, sleepTime = 0;

    while (1)
    {
        ret = mpp_comm_venc_get_stream(VENC_CHN_0, g_venc_type, -1, &buf, &len, &pts, &frame_type, &head_info);
	if(clientPid > 0){
	    if(frame_type == 1){
	        sendToSocket(head_info.pBuffer, head_info.nLength, frame_type, pts);
	    }
	    sendToSocket(buf, len, frame_type, pts);
	}
	usleep(100);
    }
    printf("Out this function ... ... \n");
    return NULL;
}

static int thread_start()
{
    int ret = 0;
    pthread_t id = 0;

    ret = pthread_create(&id, NULL, SendStream_1, &ret);
    if (ret)
    {
       ERR_PRT("Do pthread_create fail! ret:%d\n", ret);
    }

    return 0;
}

static int g_yuv_run_flag  = 1;
static int g_yuv_save_flag = 1;

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

static int components_bind(void)
{
    int ret = 0;

    ret = mpp_comm_vi_bind_venc(VI_DEV_0, VI_CHN_0, VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_bind_venc fail! ret:%d \n", ret);
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

    ret = AW_MPI_VENC_StartRecvPic(VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VENC_StartRecvPic fail! ret:%d\n", ret);
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    int sockFd = 0;
    int d_val;
    int maxfd = 0;
    fd_set fds;
    printf("\033[2J");
    printf("\n\n\nDo sample vi+venc+rtsp. default:VI_4K@25fps+VENC(4K@25fps+720P@25fps) H264/CBR/MainProfile\n");

    sockFd = socket_server(12345);
    if(sockFd < 0){
	printf("socket fail");
	return -1;
    }

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
    }

    /* Get venc config to create 1080P/720P venc channel */

    ret = venc_create();
    if (ret)
    {
        ERR_PRT("Do venc_create fail! ret:%d\n", ret);
    }

    /* Bind */
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

    thread_start();

    while (1) {
	FD_ZERO(&fds);
        maxfd = 0;
        FD_SET(sockFd, &fds);
        maxfd = MAXNO(sockFd, maxfd);
 	select(maxfd + 1, &fds, NULL, NULL, NULL);
	if(FD_ISSET(sockFd, &fds)){
	    if(clientPid > 0)close(clientPid);
            clientPid = user_connect(sockFd);
        }
    }
    return ret;
}

