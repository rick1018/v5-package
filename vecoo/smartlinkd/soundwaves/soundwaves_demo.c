#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <time.h>
#include "adt.h"

#include "include/smartlink_soundwaves.h"

#ifdef __cplusplus
	extern "C" {
#endif
const int gsample_rate = 44100;

/*wav Audio head format */
typedef struct _wave_pcm_hdr
{
    char        riff[4];                // = "RIFF"
    int         size_8;                 // = FileSize - 8
    char        wave[4];                // = "WAVE"
    char        fmt[4];                 // = "fmt "
    int         fmt_size;               // = The size of the next structure : 16

    short int   format_tag;             // = PCM : 1
    short int   channels;               // = channels num : 1
    int         samples_per_sec;        // = Sampling Rate : 6000 | 8000 | 11025 | 16000
    int         avg_bytes_per_sec;      // = The number if bytes per second : samples_per_sec * bits_per_sample / 8
    short int   block_align;            // = Number if bytes per sample point : wBitsPerSample / 8
    short int   bits_per_sample;        // = Quantization bit number: 8 | 16

    char        data[4];                // = "data";
    int         data_size;              // = Pure data length : FileSize - 44
} wave_pcm_hdr;

/* default wav audio head data */
wave_pcm_hdr default_wav_hdr =
{
    { 'R', 'I', 'F', 'F' },
    0,
    {'W', 'A', 'V', 'E'},
    {'f', 'm', 't', ' '},
    16,
    1,
    1,
    16000,
    32000,
    2,
    16,
    {'d', 'a', 't', 'a'},
    0
};

snd_pcm_t* alsa_init(char *device, int sample_rate){
    int err;

    LOGD("device: %s sample_rate is %d\n", device, sample_rate);

    snd_pcm_t *capture_handle = NULL;
    snd_pcm_hw_params_t *hw_params = NULL;

    if((err = snd_pcm_open(&capture_handle, device, SND_PCM_STREAM_CAPTURE, 0)) < 0){
        LOGE("cannot open audio device (%s)\n",  snd_strerror(err));
        return NULL;
    }

    if((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        LOGE("cannot allocate hardware parameter structure (%s)\n",snd_strerror(err));
        goto fail;
    }

    if((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
        LOGE("cannot initialize hardware parameter structure (%s)\n", snd_strerror(err));
        goto fail;
    }

    if((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        LOGE("cannot allocate hardware parameter structure (%s)\n",snd_strerror(err));
        goto fail;
    }

    if((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        LOGE("cannot allocate hardware parameter structure (%s)\n",snd_strerror(err));
        goto fail;
    }

    if((err = snd_pcm_hw_params_set_rate(capture_handle, hw_params, sample_rate, 0)) < 0) {
        LOGE("cannot set sample rate (%s)\n", snd_strerror(err));
        goto fail;
    }

    if((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, 1)) <0) {
        LOGE("cannot set channel count (%s)\n", snd_strerror(err));
        goto fail;
    }

    if((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
        LOGE("cannot set parameters (%s)\n", snd_strerror(err));
        goto fail;
    }

    snd_pcm_hw_params_free(hw_params);

    return capture_handle;
fail:
    LOGE("close dev\n");
    snd_pcm_close(capture_handle);
    LOGE("Fail\n");
    return NULL;
}
void alsa_release(snd_pcm_t* capture_handle){
    snd_pcm_close(capture_handle);
}
int alsa_record(snd_pcm_t* capture_handle, char *buf, int frames)
{
    int r;
    r = snd_pcm_readi(capture_handle, buf, frames);
    if (r == -EPIPE) {
        LOGE("overrun occurred\n");
        snd_pcm_prepare(capture_handle);
    } else if (r < 0) {
        LOGE("error from read: %s\n", snd_strerror(r));
    } else if (r > 0) {
        // 2 bytes/sample, 1 or 2 channels, r: frames
    }
    return r;
}

int main(int argc,char** argv){
	int timeouts = 60;
	int ret_dec;
	int items, bsize, i;
	void* decoder;
	short* buffer;
	char out[ADT_STR];
	char device[20];
	int rate = 44100;
    int fd = open("/test.pcm", O_RDWR);

	strncpy(device,"default",sizeof(device));

//	struct _cmd c;
//	struct _info *info = &c.info;
//	memset(info,0,sizeof(struct _info));

	/* ADT param */
	ret_dec = RET_DEC_NORMAL;
	config_decoder_t config_decoder;
	config_decoder.max_strlen = ADT_STR - 1;
	config_decoder.freq_type = FREQ_TYPE_LOW;
	config_decoder.sample_rate = rate;
	config_decoder.group_symbol_num = 10;
	config_decoder.error_correct = 1;
	config_decoder.error_correct_num = 4;

	/* Create decoder handle */
	decoder  = decoder_create(& config_decoder);
	if(decoder == NULL){
		LOGE("allocate handle error ! create decoder failed.\n");
		return -1;
	}
	/* get buffer size and allocate buffer */
	bsize = decoder_getbsize(decoder);
	buffer = (short*)malloc(sizeof(short)*bsize);
	if(buffer == NULL){
		LOGE("allocate buffer error !\n");
		//goto finish;
            return -1;
	}
	LOGD("bsize: %d\n",bsize);
	snd_pcm_t *capture_handle = alsa_init(device, config_decoder.sample_rate);
	if(capture_handle == NULL){
		//goto finish;
            return -1;
        }

	timeouts = timeouts * config_decoder.sample_rate/bsize;

	/* decoding loop */
	while(ret_dec == RET_DEC_NORMAL && timeouts--){

		/*samples get from ADC */

        items = alsa_record(capture_handle,(char*)buffer,bsize);
        /*items = read(fd, buffer, bsize);*/
		if(items < 0 && items != -EPIPE){

		}

		/*padding with zeros if items is less than bsize samples*/
		for(i = items; i< bsize; i++)
		{
			buffer[i] = 0;
		}
		/* input the pcm data to decoder */
		ret_dec = decoder_fedpcm(decoder, buffer);
	}
	/* check if we can get the output string */
	if(ret_dec != RET_DEC_ERROR)
	{
		/* get the decoder output */
		ret_dec = decoder_getstr(decoder, out);
		if(ret_dec == RET_DEC_NORMAL){
			/* this is the final decoding output */
			LOGD("recv outchar: %s \n", out);
			//info->protocol = AW_SMARTLINKD_PROTO_ADT;
			//strncpy(info->adt_str,out,sizeof(info->adt_str));
		}else{
			/* decoding have not done, so nothing output */
			LOGD("decoder output nothing! \n");
		}
	}else{
		LOGE("decoder error \n");
	}

	alsa_release(capture_handle);
	sleep(5);
}


#ifdef __cplusplus
	}
#endif
