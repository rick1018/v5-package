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

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <sem_util.h>
#include <file_msg_drv.h>
#include <nalu.h>
#include <stream.h>

const char *outputName = "rtsp://admin:hk888888@123.51.133.102:554/live.sdp";

int main(int argc, char **argv)
{
    int ret = 0;
    int streamType = 0;
    int status = 0;
    int frameType;
    unsigned int timestamp, temporalId = 0;
    struct timeval ts;
    AVPacket pkt;
    STREAM_SET streamSet;
    AVFormatContext *ic = NULL;
    AVDictionary *options = NULL;

    avcodec_register_all();
    av_register_all();
    //av_log_set_level(AV_LOG_TRACE);
    av_log_set_level(AV_LOG_ERROR);
    avformat_network_init();

    memset(&streamSet, 0, sizeof(streamSet));

    //force to set setresolution at inital time.
    streamSet.chn = 1;
    streamSet.liveresolution = streamSet.mpeg42resolution = streamSet.mpeg43resolution = streamSet.mpeg4resolution = -1;
    status = stream_init(stream_get_handle(), &streamSet);

    av_dict_set(&options, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
    av_dict_set(&options,"rtsp_transport", "tcp", AV_DICT_IGNORE_SUFFIX);

    if((ret = avformat_open_input(&ic, outputName, NULL, &options)) < 0){
	printf("[%s]%s(%d) open : \"%s\" Fail!! \n", __FILE__, __func__, __LINE__, outputName);
    }

    av_dict_set(&options, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);

    if((ret = avformat_find_stream_info(ic, NULL)) < 0){
	printf("[%s]%s(%d) get stream info fail!! \n", __FILE__, __func__, __LINE__);
    }

    printf("\n[%s]%s(%d) ****** DUMP IFNO ****** \n", __FILE__, __func__, __LINE__);
    av_dump_format(ic, 0, outputName, 0);
    printf("[%s]%s(%d) ****** DUMP IFNO ****** \n\n", __FILE__, __func__, __LINE__);

    while(1){
	if(av_read_frame(ic, &pkt)) {
	    av_free_packet(&pkt);
            printf("[%d] av_read_frame error.[%s][%d]\n",__LINE__, strerror(errno), errno);
	    continue;
	}

        streamType = -1; //STREAM_MJPG, STREAM_H264_1,STREAM_H264_2;                    
        gettimeofday(&ts, NULL);
        timestamp = (ts.tv_sec*1000) + (ts.tv_usec/1000);

        streamType = STREAM_H264_2;
	if((pkt.data[4] == 0x61) || (pkt.data[4] == 0x21) || (pkt.data[4] == 0x41)) frameType = P_FRAME;
        else frameType = I_FRAME;

        ret = stream_write((void*)pkt.data, pkt.size, frameType, streamType, timestamp, temporalId, stream_get_handle());

        streamType = STREAM_H264_1;

        ret = stream_write((void*)pkt.data, pkt.size, frameType, streamType, timestamp, temporalId, stream_get_handle());

    }

    return ret;
}

