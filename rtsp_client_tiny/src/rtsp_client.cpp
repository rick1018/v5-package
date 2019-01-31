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

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
}

#include "rtsp/MediaStream.h"
#include "rtsp/TinyServer.h"

#include <common.h>
using namespace std;

const char *outputName = "rtsp://admin:hk888888@123.51.133.102:554/live.sdp";

static int CreateRtspServer(TinyServer **rtsp)
{
    int ret  = 0;
    int port = 8554;
    char ip[64] = {0};
    std::string ip_addr;

    ret = get_net_dev_ip("eth0", ip);
    if (ret)
    {
        printf("[%s]%s(%d)Do get_net_dev_ip fail! ret:%d\n", __FILE__, __func__, __LINE__, ret);
        return -1;
    }
    printf("[%s]%s(%d)This dev eth0 ip:%s \n", __FILE__, __func__, __LINE__, ip);

    ip_addr = ip;
    *rtsp = TinyServer::createServer(ip_addr, port);

    printf("[%s]%s(%d)============================================================\n", __FILE__, __func__, __LINE__);
    printf("[%s]%s(%d)   rtsp://%s:%d/ch0  \n", __FILE__, __func__, __LINE__, ip_addr.c_str(), port);
    printf("[%s]%s(%d)============================================================\n", __FILE__, __func__, __LINE__);

    return 0;
}

static MediaStream * rtsp_start(TinyServer *rtsp)
{
    int ret = 0;

    MediaStream::MediaStreamAttr attr;
    MediaStream *g_stream_0;

    //attr.videoType  = MediaStream::MediaStreamAttr::VIDEO_TYPE_H265;
    attr.videoType  = MediaStream::MediaStreamAttr::VIDEO_TYPE_H264;
    attr.audioType  = MediaStream::MediaStreamAttr::AUDIO_TYPE_AAC;
    attr.streamType = MediaStream::MediaStreamAttr::STREAM_TYPE_UNICAST;

    g_stream_0 = rtsp->createMediaStream("ch0", attr);
    rtsp->runWithNewThread();

    return g_stream_0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    TinyServer *rtsp;
    AVPacket pkt;
    MediaStream *rtspStream;
    AVFormatContext *ic = NULL;
    AVDictionary *options = NULL;

    avcodec_register_all();
    av_register_all();
    //av_log_set_level(AV_LOG_TRACE);
    av_log_set_level(AV_LOG_ERROR);
    avformat_network_init();

    if((ret = CreateRtspServer(&rtsp)) < 0){
	printf("[%s]%s(%d) Do CreateRtspServer fail! \n", __FILE__, __func__, __LINE__);
    } 

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

    rtspStream = rtsp_start(rtsp);

    while(1){
	if(av_read_frame(ic, &pkt)) {
	    av_free_packet(&pkt);
            printf("[%d] av_read_frame error.[%s][%d]\n",__LINE__, strerror(errno), errno);
	    continue;
	}

	if((pkt.data[4] == 0x61) || (pkt.data[4] == 0x21) || (pkt.data[4] == 0x41)){
	    rtspStream->appendVideoData(pkt.data, pkt.size, pkt.pts, MediaStream::FRAME_DATA_TYPE_P);
        } else {
	    rtspStream->appendVideoData(pkt.data, pkt.size, pkt.pts, MediaStream::FRAME_DATA_TYPE_I);
	}
    }

    return ret;
}

