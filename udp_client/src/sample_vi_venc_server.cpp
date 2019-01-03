/************************************************************************************************/
/*                                      Include Files                                           */
/************************************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <linux/tcp.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>
#include "common.h"
#include "mpp_comm.h"

#include "tcp_states.h"

struct v5_socket_data{
    int frame_type;
    uint64_t pts;
    int endOfFrame;
    int endOfStream;
    unsigned int len;
    int offset;
    int start;
    uint64_t cksum;
    unsigned char data[1024];
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
int serverPid = -1;
#define HLAY(chn, lyl) (chn*4+lyl)

static struct sockaddr_in servaddr;

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

    tv.tv_sec = 2;
    tv.tv_usec = 0;

    int clientIndex = 0;

    int optval = 1;
    result = accept( server, (struct sockaddr*)&caddr, (socklen_t *)&addr_len);
    if (!strcmp(inet_ntoa(caddr.sin_addr), "0.0.0.0")) return -1;

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

    if(setsockopt( server_pid, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0 ) return -1;
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
uint64_t checksum(void *buffer, unsigned int len, unsigned int seed)
{
      unsigned char *buf = (unsigned char *)buffer;
      size_t i;

      for (i = 0; i < len; ++i)
            seed += (unsigned int)(*buf++);

      return seed;
}

static void sendToSocket(unsigned char *framData, unsigned int framLen, int frame_type, uint64_t pts){

    struct sockaddr_in cliaddr;
    struct v5_socket_data info;
    unsigned int len = framLen;
    int offset = 0;
    unsigned char *data = framData;
    unsigned char buf[1500] = {0};
    unsigned char *ptr = NULL;
    int datalen = 0;
    struct hostent *server;


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
    bzero((char *)&servaddr, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
 
    servaddr.sin_port = htons(12345);
 
        server = gethostbyname("192.168.0.1");
    if (server == 0) {
        fprintf(stderr,"ERROR, no such host as \n");
    }

	bcopy(server->h_addr_list[0],
          (caddr_t)&servaddr.sin_addr, server->h_length);
        data += offset;
        if(len > sizeof(info.data))
            offset = sizeof(info.data);
	else 
	    offset = len;

        memcpy(&info.data, data, offset);
        info.offset = offset;

	ptr = (unsigned char *)&buf;
	//memcpy(ptr, &startFlag, sizeof(startFlag));
	//ptr += sizeof(startFlag);
	memcpy(ptr, &info.frame_type, sizeof(int));
	ptr += sizeof(int);
	//memcpy(ptr, &splitFlag, sizeof(splitFlag));
	//ptr += sizeof(splitFlag);
	memcpy(ptr, &info.pts, sizeof(uint64_t));
	ptr += sizeof(uint64_t);
	//memcpy(ptr, &splitFlag, sizeof(splitFlag));
	//ptr += sizeof(splitFlag);
	memcpy(ptr, &info.endOfFrame, sizeof(int));
	ptr += sizeof(int);
	//memcpy(ptr, &splitFlag, sizeof(splitFlag));
	//ptr += sizeof(splitFlag);
	memcpy(ptr, &info.endOfStream, sizeof(int));
	ptr += sizeof(int);
	//memcpy(ptr, &splitFlag, sizeof(splitFlag));
	//ptr += sizeof(splitFlag);
	memcpy(ptr, &info.len, sizeof(unsigned int));
	ptr += sizeof(unsigned int);
	//memcpy(ptr, &splitFlag, sizeof(splitFlag));
	//ptr += sizeof(splitFlag);
	memcpy(ptr, &info.offset, sizeof(int));
	ptr += sizeof(int);
	//memcpy(ptr, &splitFlag, sizeof(splitFlag));
	//ptr += sizeof(splitFlag);
	memcpy(ptr, &info.start, sizeof(int));
	ptr += sizeof(int);
	//memcpy(ptr, &splitFlag, sizeof(splitFlag));
	//ptr += sizeof(splitFlag);
	memcpy(ptr, &info.cksum, sizeof(uint64_t));
	ptr += sizeof(uint64_t);
	//memcpy(ptr, &splitFlag, sizeof(splitFlag));
	//ptr += sizeof(splitFlag);
	memcpy(ptr, data, offset);

        if((datalen = sendto(serverPid, buf, sizeof(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr))) <= 0){
	    close(clientPid);
	    clientPid = -1;
            break;
        }

        info.start = 0;
        len -= offset;
	usleep(500);
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
    VencHeaderData head_info;

    while (1)
    {
        ret = mpp_comm_venc_get_stream(VENC_CHN_0, g_venc_type, -1, &buf, &len, &pts, &frame_type, &head_info);
	if(ret){
	    printf("Do mpp_comm_venc_get_stream fail! ret:%d\n", ret);
	    continue;
	}
	//if(clientPid > 0 ){
	    if(frame_type == 1){
	        sendToSocket(head_info.pBuffer, head_info.nLength, frame_type, pts);
	    }
	    sendToSocket(buf, len, frame_type, pts);
	    usleep(1000);
	//}
	usleep(10);
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
	struct sockaddr_in myaddr;
struct sockaddr_in servaddr;
int size;
struct hostent *hp;
char data[1024];

    if((serverPid = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
	printf("udp fail \n");
	exit(1);
    }

        bzero((char *)&myaddr, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        myaddr.sin_port = htons(0);

        if (bind(serverPid, (struct sockaddr *)&myaddr,
                                sizeof(myaddr)) <0) {
                perror("bind failed!");
                exit(1);
        }
    ret = mpp_comm_sys_init();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_sys_init fail! ret:%d \n", ret);
        return -1;
    }

    ret = vi_create();
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

    ret = components_start();
    if (ret)
    {
        ERR_PRT("Do components_start fail! ret:%d\n", ret);
    }
/*
        bzero((char *)&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(12345);
        hp = gethostbyname("10.20.70.28");

        if (hp == 0) {
                fprintf(stderr, "could not obtain address of %s\n", argv[2]);
                return (-1);
        }
        bcopy(hp->h_addr_list[0], (caddr_t)&servaddr.sin_addr,
                        hp->h_length);
*/


    thread_start();

    while (1) {
	sleep(1);
    }
    return ret;
}

