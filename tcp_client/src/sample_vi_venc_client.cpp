/************************************************************************************************/
/*                                      Include Files                                           */
/************************************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <linux/tcp.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>

#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "tcp_states.h"
#include "mpp_comm.h"
#include "common.h"

#define MAXNO(__A__, __B__) ((__A__ > __B__) ? __A__ : __B__)
#define HLAY(chn, lyl) (chn*4+lyl)

struct v5_socket_data{
    int frame_type;
    uint64_t pts;
    int endOfFrame;
    int endOfStream;
    unsigned int len;
    int offset;
    int start;
    uint64_t cksum;
    unsigned char data[1000];
};

static unsigned char * tmpFrameBox;
static MPPCallbackInfo  g_cb_info;

static unsigned int   g_rc_mode   = 0;          /* 0:CBR 1:VBR 2:FIXQp 3:ABR 4:QPMAP */
static unsigned int   g_profile   = 1;          /* 0: baseline  1:MP  2:HP  3: SVC-T [0,3] */
static PAYLOAD_TYPE_E g_venc_type = PT_H265;    /* PT_H264/PT_H265/PT_MJPEG */
static ROTATE_E       g_rotate    = ROTATE_NONE;

static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_4K_30FPS;

static VENC_CFG_TYPE_E g_venc_type_0 = VENC_4K_TO_4K_20M_30FPS;

static int serverPid = 0;
static int needRestart = 1;
static unsigned char saveBuff[1300] = {0};
static int saveLen = 0;
static VDEC_STREAM_S vdec_stream = {0};
static unsigned int totalLen = 0, cksum = 0;
static int tryCount = 0;

static ERRORTYPE MPPCallbackFunc(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    DB_PRT(" pChn:%d  event:%d \n", pChn->mModId, (int)event);

    return SUCCESS;
}

int connectServer(int port){
    int sockfd = 0 , flags = 0, ret = 0, error = 0;
    fd_set rset, wset;
    char recvBuff[1024];
    struct sockaddr_in serv_addr,client_addr;
    struct timeval ts;
    socklen_t len = sizeof(error);

    ts.tv_sec = 2;

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error : Could not create socket");
        return -1;
    }

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset;
 
    if( (flags = fcntl(sockfd, F_GETFL, 0 )) < 0 )return -1;

    if(fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)return -1;

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, "192.168.0.101", &serv_addr.sin_addr)<=0)
    {
        printf("inet_pton error occured \n");
        goto error;
    }

    if( (ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        if(errno != EINPROGRESS) {
           printf("Error : Connect Failed");
            return -1;
        }
    }

    if(ret == 0){
        errno = ETIMEDOUT;
        goto error;
    }

    if(FD_ISSET(sockfd,&rset) || FD_ISSET(sockfd, &wset)){
        if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error,&len) < 0)goto error;
    }else goto error;

    if(error){
        errno = error;
        goto error;
    }

    if(fcntl(sockfd,F_SETFL,flags) < 0) goto error;

    return sockfd;
error:
    return -1;
}

unsigned checksum(void *buffer, uint64_t len, unsigned int seed)
{
      unsigned char *buf = (unsigned char *)buffer;
      size_t i;

      for (i = 0; i < len; ++i)
            seed += (unsigned int)(*buf++);

      return seed;
}

static int getDataToVDEC(){

    int len = 0, ret = 0, index = 0;
    unsigned char buff[1300] = {0};
    unsigned char *box = tmpFrameBox;
    unsigned char *saveBox = (unsigned char *)&saveBuff;
    unsigned char *ptr = NULL;
    struct v5_socket_data info;
    int cc[] = {1};
    static int trycount = 0;

    if((len = recv( serverPid, buff, sizeof(buff), 0)) <= 0){
	printf("server disconnect!!!! [%s][%d]\n\n", strerror(errno), errno);
	    if(serverPid > 0){
	    	printf("%s(%d)\n", __func__, __LINE__);
    	    	close(serverPid);
    	    	serverPid = -1;
	    	memset(&info, 0, sizeof(v5_socket_data));
	    	memset(&vdec_stream, 0, sizeof(VDEC_STREAM_S));
	    	needRestart = 1;
	    	totalLen = 0;
	    	saveLen = 0;
    	    }
    	return len;
    }

    if(len != sizeof(buff)) printf("%s(%d) %d\n", __func__, __LINE__,len);

    setsockopt(serverPid, IPPROTO_TCP, TCP_QUICKACK, cc, sizeof(int));

    saveBox += saveLen;

    if((index = sizeof(buff) - saveLen) < len ){
        memcpy(saveBox, &buff, index);
        saveLen += index;
    } else {
        memcpy(saveBox, &buff, len);
        saveLen += len;
    }

    if(saveLen < sizeof(buff) || saveLen != sizeof(buff)){
	printf("%s(%d) %d < %d %d\n",__func__, __LINE__, saveLen, sizeof(buff), len);
	return len;
    }

    ptr = (unsigned char *)&saveBuff;
    memcpy(&info.frame_type, ptr, sizeof(int));
    ptr += sizeof(int);

    memcpy(&info.pts, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);

    memcpy(&info.endOfFrame, ptr, sizeof(int));
    ptr += sizeof(int);

    memcpy(&info.endOfStream, ptr, sizeof(int));
    ptr += sizeof(int);

    memcpy(&info.len, ptr, sizeof(unsigned int));
    ptr += sizeof(unsigned int);

    memcpy(&info.offset, ptr, sizeof(int));
    ptr += sizeof(int);

    memcpy(&info.start, ptr, sizeof(int));
    ptr += sizeof(int);

    memcpy(&info.cksum, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);

    if(info.offset > sizeof(info.data)){
        printf("%s(%d) overflow 3 %u > %d\n", __func__, __LINE__, info.offset, sizeof(info.data));
    } else{
        memcpy(&info.data, ptr, info.offset);
    }

    if(index > 0 && (len - index) > 0){
    	if((len - index) > sizeof(saveBuff)){
	        printf("%s(%d) overflow 4 %d > %d\n", __func__, __LINE__, (len - index), sizeof(saveBuff));
		saveLen = 0;
                goto overfolow;
    	}
	unsigned char *tmpBuff = (unsigned char *)&buff;
	tmpBuff += index;
        memcpy(&saveBuff, tmpBuff, (len - index));
        saveLen = (len - index);
    } else {
	saveLen = 0;
    }

    /*bad package check*/

    if(info.len <= 0 || info.pts <= 0 || info.endOfFrame < 0 || info.endOfFrame > 1 || info.offset > sizeof(info.data)){
        printf("%s(%d) get 0\n", __func__, __LINE__);

overfolow:
	if(serverPid > 0 && trycount > 5){
           printf("%s(%d)\n", __func__, __LINE__);
           close(serverPid);
           serverPid = -1;
           memset(&info, 0, sizeof(v5_socket_data));
           memset(&vdec_stream, 0, sizeof(VDEC_STREAM_S));
           needRestart = 1;
           totalLen = 0;
           saveLen = 0;
	   trycount = 0;
        } else {
		trycount++;
	}
/*
       printf("%s(%d) -------------BAD--write Frame----------------------\n", __func__, __LINE__); 
        printf("%s(%d) -------------- vdec_stream.start : %d--------------\n", __func__, __LINE__, info.start); 
        printf("%s(%d) -------------- vdec_stream.frameType : %d--------------\n", __func__, __LINE__, info.frame_type); 
        printf("%s(%d) -------------- vdec_stream.mPTS : %lld--------------\n", __func__, __LINE__, info.pts); 
        printf("%s(%d) ---------------vdec_stream.mLen : %u----------------------\n", __func__, __LINE__, info.len); 
        printf("%s(%d) ---------------endOfFrame : %d offset : %d----------------------\n\n", __func__, __LINE__, info.endOfFrame, info.offset); 
        printf("%s(%d) ---------------totalLen : %u----------------------\n", __func__, __LINE__, totalLen); 
	
	if(++tryCount > 5) {
    	    printf("%s(%d)\n", __func__, __LINE__);
	    close(serverPid);
	    serverPid = -1;
	    tryCount = 0;
	}
	memset(&info, 0, sizeof(v5_socket_data));
	memset(&vdec_stream, 0, sizeof(VDEC_STREAM_S));
	needRestart = 1;
	totalLen = 0;
	saveLen = 0;
*/
	return len;
    }

    if(needRestart == 1 && info.start != 1){
        printf("%s(%d) skip package\n", __func__, __LINE__);
	return len;
    }
	
    if(info.start == 1){
	if( vdec_stream.mLen > 0 && needRestart != 1){
	    if(totalLen == vdec_stream.mLen){
	        vdec_stream.pAddr = tmpFrameBox;

	        ret = AW_MPI_VDEC_SendStream(VDEC_CHN_0, &vdec_stream, -1);
                if (ret){
		    printf("Do AW_MPI_VDEC_SendStream fail! ret:%d\n", ret);
		    ERR_PRT("Do AW_MPI_VDEC_SendStream fail! ret:%d\n", ret);
	        }
	    } else {
		printf("%s(%d) bad package [%d],[%d]\n", __func__, __LINE__,totalLen ,vdec_stream.mLen);
	    }
	}
        needRestart = 0;
	tryCount = 0;
	vdec_stream.mPTS = info.pts;
	vdec_stream.mLen = info.len;
	vdec_stream.mbEndOfStream = FALSE;
	cksum = info.cksum;

	if(info.endOfFrame == 1){
            vdec_stream.mbEndOfFrame = TRUE;
        } else {
            vdec_stream.mbEndOfFrame = FALSE;
        }
        totalLen = 0;
    } 

    if(totalLen > info.len){
    	printf("%s(%d) totalLen > info.len %d > %d\n", __func__, __LINE__, totalLen, info.len);
        totalLen = info.offset;
    }
    if(info.start != 0 && info.start != 1){
    	printf("%s(%d)\n", __func__, __LINE__);
	return len;
    }
    box += totalLen;
    totalLen += info.offset;
    memcpy(box, (unsigned char *)&info.data, info.offset);

    return len;
}

static int vo_create_new(void){
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
}

static int vo_create(void)
{
    int ret = 0;
    VO_DEV_TYPE_E vo_type;
    VO_DEV_CFG_S  vo_cfg;
    vo_type           = VO_DEV_HDMI;
    vo_cfg.res_width  = 3840;
    vo_cfg.res_height = 2160;
    ret = mpp_comm_vo_dev_create(vo_type, &vo_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_dev_create fail! ret:%d  vo_type:%d\n", ret, vo_type);
        return -1;
    }

    VO_CHN_CFG_S vo_chn_cfg;
    vo_chn_cfg.top        = 0;
    vo_chn_cfg.left       = 0;
    vo_chn_cfg.width      = 3840;
    vo_chn_cfg.height     = 2160;
    vo_chn_cfg.vo_buf_num = 0;
    ret = mpp_comm_vo_chn_create(VO_CHN_0, &vo_chn_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_chn_create fail! ret:%d  vo_chn:%d\n", ret, VO_CHN_0);
        return -1;
    }
    /* Create Clock for vo mode */
/*
    ret = mpp_comm_vo_clock_create(CLOCK_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_clock_create fail! ret:%d  clock_chn:%d\n", ret, CLOCK_CHN_0);
        return -1;
    }
*/
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
/*
    ret = mpp_comm_vo_bind_clock(CLOCK_CHN_0, VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_bind_clock fail! ret:%d clock_chn:%d vo_chn:%d\n", ret, CLOCK_CHN_0, VO_CHN_0);
        return -1;
    }
*/
    return 0;
}

static int components_start(void)
{
    int ret = 0;
/*
    ret = AW_MPI_CLOCK_Start(CLOCK_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_CLOCK_Start fail! clock_chn:%d ret:%d\n", CLOCK_CHN_0, ret);
        return -1;
    }
*/
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


int main(int argc, char **argv)
{
    int ret = 0, maxfd = 0;
    fd_set fds;
    struct timeval timeout;
    unsigned char buff[1300] = {0};
    int cc[] = {1}, len = 0;

    ret = mpp_comm_sys_init();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_sys_init fail! ret:%d \n", ret);
        return -1;
    }

#if 1
    ret = vdec_create();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vdec_create fail!  vdec_chn:%d  ret:%d\n", VDEC_CHN_0, ret);
    }

#else
    memset(&g_cb_info,    0, sizeof(MPPCallbackInfo));
    g_cb_info.cookie   = NULL;
    g_cb_info.callback = mpp_comm_demux_callbcak;
    VDEC_CFG_S vdec_cfg;

    vdec_cfg.width     = 3840;
    vdec_cfg.height     = 2160;
    vdec_cfg.codec_type = g_venc_type;
    vdec_cfg.rotation   = g_rotate;
    
    ret = mpp_comm_vdec_create(VDEC_CHN_0, &vdec_cfg, &g_cb_info);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vdec_create fail!  vdec_chn:%d  ret:%d\n", VDEC_CHN_0, ret);
    }
#endif
    ret = vo_create_new();
    if (ret){
        ERR_PRT("Do vo_create fail! ret:%d\n", ret);
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

    if(tmpFrameBox == NULL){
    	tmpFrameBox = (unsigned char *)malloc(4096 * 2160 * 3);
    	if(tmpFrameBox == NULL) {
            printf("craete memory fail\n");
    	    return -1;
    	}
    }
	
    while((serverPid = connectServer(12345)) < 0){
	printf("socket fail, wait try....");
	sleep(1);
    }  
	
    while (1){
	maxfd = 0;
	timeout.tv_sec = 5;
        timeout.tv_usec = 0;

	if(serverPid <= 0){
	    serverPid = connectServer(12345);
	}

	if(serverPid <= 0){
	    sleep(1);
	    continue;
	}

        FD_ZERO(&fds);
        FD_SET(serverPid, &fds);

	maxfd = MAXNO(serverPid, maxfd);
        if(select(maxfd + 1, &fds, NULL, NULL, &timeout) <= 0){
	    printf("timeout 5s\n");
	    close(serverPid);
	    serverPid = -1;
	    continue;
	}

	if(FD_ISSET(serverPid, &fds)){	
	    //while((ret = getDataToVDEC()) > 0){}
	    getDataToVDEC();
	}
    }

    return ret;
}

