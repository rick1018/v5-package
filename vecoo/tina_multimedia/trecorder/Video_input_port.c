/*
 * Copyright (C) 2017 Allwinnertech
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "video_input_port"
#include "tlog.h"
#include "Video_input_port.h"

long long GetNowUs()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}
int VportOpen(void *hdl, int index)
{
	vInPort *handle = (vInPort *) hdl;
	char dev_name[30];

	if (handle->state != CAMERA_STATE_NONE) {
		printf("[%s] Open device error", __func__);
		return -1;
	}
         handle->mCameraIndex = index;
	snprintf(dev_name, sizeof(dev_name), "/dev/video%d", index);
	handle->fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
	if (handle->fd < 0) {
		printf("[%s] Open device error", __func__);
		return -1;
	}

	handle->state = CAMERA_STATE_CREATED;
	return 0;
}

int VportClose(void *hdl)
{
	vInPort *handle = (vInPort *) hdl;
	int ret;
	printf("enter Vport close,state:%d fd:%d handle:%p",handle->state,handle->fd,hdl);
	if (handle->state != CAMERA_STATE_CREATED) {
		printf("[%s] Camera state error", __func__);
		return -1;
	}

	if (ret = close(handle->fd) != 0) {
		printf("[%s] Close device error", __func__);
		return ret;
	}

	handle->state = CAMERA_STATE_NONE;
	return 0;
}

int GetCaptureFormat(int format)
{
	switch(format)
	{
		case TR_PIXEL_YUV420SP:
			return V4L2_PIX_FMT_NV12;
		case TR_PIXEL_YVU420SP:
			return V4L2_PIX_FMT_NV21;
		case TR_PIXEL_YUV420P:
			return V4L2_PIX_FMT_YUV420;
		case TR_PIXEL_YVU420P:
			return V4L2_PIX_FMT_YVU420;
		case TR_PIXEL_YUV422SP:
			return V4L2_PIX_FMT_NV16;
		case TR_PIXEL_YVU422SP:
			return V4L2_PIX_FMT_NV61;
		case TR_PIXEL_YUV422P:
			return V4L2_PIX_FMT_YUV422P;
		case TR_PIXEL_YUYV422:
			return V4L2_PIX_FMT_YUYV;
		case TR_PIXEL_UYVY422:
			return V4L2_PIX_FMT_UYVY;
		case TR_PIXEL_YVYU422:
			return V4L2_PIX_FMT_YVYU;
		case TR_PIXEL_VYUY422:
			return V4L2_PIX_FMT_VYUY;
		default:
			printf("[%s] Capture pixel format is not supported", __func__);
			return -1;
	}
}
int VportInit(void *hdl, int enable, int format, int framerate, int width,
	      int height, int subwidth, int subheight,int ispEnable, int rot_angle, int use_wm)
{
	vInPort *handle = (vInPort *) hdl;
	int ret;
	struct v4l2_input inp;
	struct v4l2_streamparm parms;
	struct v4l2_format fmt;
	struct v4l2_pix_format sub_fmt;

	if (handle->state != CAMERA_STATE_CREATED) {
		printf("[%s] Camera state error.state:%d\n", __func__,handle->state);
		return -1;
	}

	handle->enable = enable;
	handle->format = format;
	handle->use_isp = ispEnable;
	handle->use_wm = use_wm;
	handle->framerate = framerate;
	handle->rot_angle = rot_angle;
	handle->MainWidth = width;
	handle->MainHeight = height;
	handle->SubWidth = subwidth;
	handle->SubHeight = subheight;

	/* set capture input */
	inp.index = 0;
	inp.type = V4L2_INPUT_TYPE_CAMERA;
	if (ret = ioctl(handle->fd, VIDIOC_S_INPUT, &inp) < 0) {
		printf("[%s] Set csi input error", __func__);
		return ret;
	}

	/* set capture parms */
	parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parms.parm.capture.timeperframe.numerator = 1;
	parms.parm.capture.timeperframe.denominator = handle->framerate;
	parms.parm.capture.capturemode = V4L2_MODE_VIDEO;
	if (ret = ioctl(handle->fd, VIDIOC_S_PARM, &parms) < 0) {
		printf("[%s] Set csi input error", __func__);
		return ret;
	}

	/* set mainchannel format */
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = handle->MainWidth;
	fmt.fmt.pix.height = handle->MainHeight;
	fmt.fmt.pix.pixelformat = GetCaptureFormat(handle->format);
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
	/* if use isp set subchannel format */
	if (handle->use_isp == 1) {
		memset(&sub_fmt, 0, sizeof(struct v4l2_pix_format));
		fmt.fmt.pix.subchannel = &sub_fmt;
		sub_fmt.width = handle->SubWidth;
		sub_fmt.height = handle->SubHeight;
		sub_fmt.pixelformat = V4L2_PIX_FMT_NV21;
		sub_fmt.field = V4L2_FIELD_NONE;
		sub_fmt.rot_angle = handle->rot_angle;
	}
	if (ret = ioctl(handle->fd, VIDIOC_S_FMT, &fmt) < 0) {
		printf("[%s] Set csi resolution and format error", __func__);
		return ret;
	}

	/* init sem_dq */
	sem_init(&handle->dqfinSem, 0, 0);

	/* init memops */
	handle->pMemops = GetMemAdapterOpsS();
	SunxiMemOpen(handle->pMemops);

	return 0;
}

int VportRequestbuf(void *hdl, int count)
{
	vInPort *handle = (vInPort *) hdl;
	int ret;
	int i;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;

	printf("\n\n\n requeset buf:%d\n",handle->state);

	if (handle->state != CAMERA_STATE_CREATED) {
		printf("[%s] Camera state error", __func__);
		return -1;
	}

	/* reqbufs */
	memset(&reqbuf, 0, sizeof(struct v4l2_requestbuffers));
	reqbuf.count = count;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	if (ret = ioctl(handle->fd, VIDIOC_REQBUFS, &reqbuf) < 0) {
		printf("[%s] Request buffer error %d", __func__, ret);
		return ret;
	}

	/* querybuf and mmap */
	handle->buffers = calloc(count, sizeof(buffer));
	for (i = 0; i < count; i++) {
		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (ret = ioctl(handle->fd, VIDIOC_QUERYBUF, &buf) < 0) {
			printf("[%s] QUERYBUF error", __func__);
			return ret;
		}
		handle->buffers[i].length = buf.length;
		handle->buffers[i].start = mmap(NULL, buf.length,
						PROT_READ | PROT_WRITE,
						MAP_SHARED,
						handle->fd, buf.m.offset);
		if (handle->buffers[i].start == MAP_FAILED) {
			printf("[%s] MMAP error", __func__);
			return -1;
		}
		printf("map buffer index: %d, start: %p, len: %x, offset: %d\n",
			i, handle->buffers[i].start, buf.length, buf.m.offset);
	}

	/* enqbuf for ready */
	for (i = 0; i < count; i++) {
		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (ret = ioctl(handle->fd, VIDIOC_QBUF, &buf) < 0) {
			printf("[%s] QBUF error", __func__);
			return ret;
		}

	}

	handle->state = CAMERA_STATE_PREPARED;
	return 0;
}

int VportReleasebuf(void *hdl, int count)
{
	vInPort *handle = (vInPort *) hdl;
	int i;

	if (handle->state != CAMERA_STATE_PREPARED) {
		printf("[%s] Camera state error", __func__);
		return -1;
	}

	for (i = 0; i < count; i++)
		munmap(handle->buffers[i].start, handle->buffers[i].length);

	free(handle->buffers);

	/* close memops */
	SunxiMemClose(handle->pMemops);

	handle->state = CAMERA_STATE_CREATED;
	return 0;
}

int VportStartcapture(void *hdl)
{
	vInPort *handle = (vInPort *) hdl;
	enum v4l2_buf_type type;
	int ret;

	if (handle->state != CAMERA_STATE_PREPARED) {
		printf("[%s] Camera state error", __func__);
		return -1;
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ret = ioctl(handle->fd, VIDIOC_STREAMON, &type) < 0) {
		printf("[%s] Streamon error", __func__);
		return ret;
	}

	handle->state = CAMERA_STATE_CAPTURING;
	return 0;
}

int VportStopcapture(void *hdl)
{
	vInPort *handle = (vInPort *) hdl;
	enum v4l2_buf_type type;
	int ret;

	if (handle->state != CAMERA_STATE_CAPTURING) {
		printf("[%s] Camera state error", __func__);
		return -1;
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ret = ioctl(handle->fd, VIDIOC_STREAMOFF, &type) < 0) {
		printf("[%s] Streamoff error", __func__);
		return ret;
	}

	handle->state = CAMERA_STATE_PREPARED;
	return 0;
}

int VportDequeue(void *hdl, dataParam *param)
{
	int ret;
	unsigned int MainYSize;
	unsigned int SubYSize;
	fd_set fds;
	struct timeval tv;
	struct v4l2_buffer buf;
	vInPort *handle = (vInPort *) hdl;

	if (handle->state != CAMERA_STATE_CAPTURING) {
		printf("[%s] Camera state error,state:%d\n", __func__,handle->state);
		return -1;
	}

	FD_ZERO(&fds);
	FD_SET(handle->fd, &fds);

	tv.tv_sec = CSI_SELECT_FRM_TIMEOUT;
	tv.tv_usec = 0;
	if (ret = select(handle->fd + 1, &fds, NULL, NULL, &tv) < 0) {
		printf("[%s] Select csi input error", __func__);
		return ret;
	}

	memset(&buf, 0, sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(handle->fd, VIDIOC_DQBUF, &buf);
	if (ret != 0) {
		printf("[%s] ioctl v4l2 buf dequeue error", __func__);
		return ret;
	}

	handle->SaveTime = GetNowUs();
	//printf("*******CAMERA[%d] Dequeue time is %lldus\n", handle->mCameraIndex, handle->SaveTime);

	MainYSize = ALIGN_16B(handle->MainWidth) * ALIGN_16B(handle->MainHeight);
	SubYSize = ALIGN_16B(handle->SubWidth) * ALIGN_16B(handle->SubHeight);
	param->MainSize = ALIGN_4K(ALIGN_16B(handle->MainWidth) *
			  ALIGN_16B(handle->MainHeight)*3 >> 1);
	param->SubSize = ALIGN_4K(ALIGN_16B(handle->SubWidth) *
			 ALIGN_16B(handle->SubHeight)*3 >> 1);
	param->RotSize = param->SubSize;

	param->bufferId = buf.index;
	param->pts = (long long)(buf.timestamp.tv_sec) * 1000 +
	    (long long)(buf.timestamp.tv_usec) / 1000;

	param->MainYVirAddr = handle->buffers[buf.index].start;
	param->MainYPhyAddr = (void *)buf.m.offset;
	param->MainCVirAddr = param->MainYVirAddr + MainYSize;
	param->MainCPhyAddr = param->MainYPhyAddr + MainYSize;

	param->SubYVirAddr = param->MainYVirAddr + param->MainSize;
	param->SubYPhyAddr = param->MainYPhyAddr + param->MainSize;
	param->SubCVirAddr = param->SubYVirAddr + SubYSize;
	param->SubCPhyAddr = param->SubYPhyAddr + SubYSize;

	if (handle->rot_angle) {
		param->RotYVirAddr = param->SubYVirAddr + param->SubSize;
		param->RotYPhyAddr = param->SubYPhyAddr + param->SubSize;
		param->RotCVirAddr = param->RotYVirAddr + SubYSize;
		param->RotCPhyAddr = param->RotYPhyAddr + SubYSize;
	}

	return 0;
}

int VportEnqueue(void *hdl, dataParam *param)
{
	int ret;
	struct v4l2_buffer buf;
	long long interval_time;
	vInPort *handle = (vInPort *) hdl;

	if (handle->state != CAMERA_STATE_CAPTURING) {
		printf("[%s] Camera state error", __func__);
		return -1;
	}
	interval_time = GetNowUs() - handle->SaveTime;
	//printf("-------CAMERA[%d] Between DQ and Q is %lldus\n", handle->mCameraIndex, interval_time);
	//printf("*******CAMERA[%d] Enqueue time is %lldus\n", handle->mCameraIndex, interval_time + handle->SaveTime);

	memset(&buf, 0, sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = param->bufferId;
	ret = ioctl(handle->fd, VIDIOC_QBUF, &buf);
	if (ret != 0) {
		printf("[%s] ioctl v4l2 buf enqueue error", __func__);
		return ret;
	}

	return 0;
}

int VportgetEnable(void *hdl)
{
	vInPort *handle = (vInPort *) hdl;

	return handle->enable;
}

int VportsetEnable(void *hdl, int enable)
{
	vInPort *handle = (vInPort *) hdl;

	handle->enable = enable;
	return 0;
}

int VportgetFrameRate(void *hdl)
{
	vInPort *handle = (vInPort *) hdl;

	return handle->framerate;
}

int VportsetFrameRate(void *hdl, int framerate)
{
	vInPort *handle = (vInPort *) hdl;
	struct v4l2_streamparm parms;
	int ret;

	if (handle->state != CAMERA_STATE_CREATED) {
		printf("[%s] Camera state error", __func__);
		return -1;
	}

	parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parms.parm.capture.timeperframe.numerator = 1;
	parms.parm.capture.timeperframe.denominator = handle->framerate;
	if (ret = ioctl(handle->fd, VIDIOC_S_PARM, &parms) < 0) {
		printf("[%s] Set csi input framerate error", __func__);
		return ret;
	}

	return 0;
}

int VportgetFormat(void *hdl)
{
	vInPort *handle = (vInPort *) hdl;

	return handle->format;
}

int VportsetFormat(void *hdl, int format)
{
	vInPort *handle = (vInPort *) hdl;
	struct v4l2_format fmt;
	struct v4l2_pix_format sub_fmt;
	int ret;

	if (handle->state != CAMERA_STATE_CREATED) {
		printf("[%s] Camera state error", __func__);
		return -1;
	}

	handle->format = format;
	/* set mainchannel format */
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = handle->MainWidth;
	fmt.fmt.pix.height = handle->MainHeight;
	fmt.fmt.pix.pixelformat = GetCaptureFormat(handle->format);
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
	/* set subchannel format */
	if (handle->use_isp == 1) {

		memset(&sub_fmt, 0, sizeof(struct v4l2_pix_format));
		fmt.fmt.pix.subchannel = &sub_fmt;
		fmt.fmt.pix.subchannel->width = handle->SubWidth;
		fmt.fmt.pix.subchannel->height = handle->SubHeight;
		fmt.fmt.pix.subchannel->pixelformat = V4L2_PIX_FMT_NV21;
		fmt.fmt.pix.subchannel->field = V4L2_FIELD_NONE;
	}

	if (ret = ioctl(handle->fd, VIDIOC_S_FMT, &fmt) < 0) {
		printf("[%s]  VIDIOC_S_FMT error", __func__);
		return ret;
	}

	return 0;
}

int VportgetSize(void *hdl, int *width, int *height)
{
	vInPort *handle = (vInPort *) hdl;
	if (handle->MainWidth == 0 || handle->MainHeight == 0) {
		printf("[%s] width and height is NULL", __func__);
		return -1;
	}

	*width = handle->MainWidth;
	*height = handle->MainHeight;

	return 0;
}

int VportsetSize(void *hdl, int width, int height)
{
	vInPort *handle = (vInPort *) hdl;
	struct v4l2_format fmt;
	struct v4l2_pix_format sub_fmt;
	int ret;

	if (handle->state != CAMERA_STATE_CREATED) {
		printf("[%s] Camera state error", __func__);
		return -1;
	}

	handle->format = width;
	handle->MainHeight = height;
	/* set mainchannel format */
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = handle->MainWidth;
	fmt.fmt.pix.height = handle->MainHeight;
	fmt.fmt.pix.pixelformat = GetCaptureFormat(handle->format);
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
	/* set subchannel format */
	if (handle->use_isp == 1) {

		memset(&sub_fmt, 0, sizeof(struct v4l2_pix_format));
		fmt.fmt.pix.subchannel = &sub_fmt;
		fmt.fmt.pix.subchannel->width = handle->SubWidth;
		fmt.fmt.pix.subchannel->height = handle->SubHeight;
		fmt.fmt.pix.subchannel->pixelformat = V4L2_PIX_FMT_NV21;
		fmt.fmt.pix.subchannel->field = V4L2_FIELD_NONE;
	}

	if (ret = ioctl(handle->fd, VIDIOC_S_FMT, &fmt) < 0) {
		printf("[%s]  VIDIOC_S_FMT error", __func__);
		return ret;
	}

	return 0;
}

int VportGetIndex(void *hdl, int *index){
    vInPort *handle = (vInPort *) hdl;
    *index = handle->mCameraIndex;
    return 1;
}

int VportGetContrast(void *hdl)
{
	struct v4l2_control control;
	vInPort *handle = (vInPort *) hdl;

	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_CONTRAST;
	if (0 == ioctl (handle->fd, VIDIOC_G_CTRL, &control)) {
		printf("Contrast value = %d\n", control.value);
	} else {
		printf("VIDIOC_G_CTRL(CONTRAST) error!\n");
		return -1;
	}
	return control.value;
}

int VportQueryContrast(void *hdl)
{
	struct v4l2_queryctrl queryctrl;
	vInPort *handle = (vInPort *) hdl;

	memset (&queryctrl, 0, sizeof (queryctrl));
	queryctrl.id = V4L2_CID_CONTRAST;
	if (0 == ioctl (handle->fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		printf("Contrast Range [%d]~[%d] and step = %d\n", queryctrl.minimum,
						queryctrl.maximum, queryctrl.step);
		return 0;
	} else {
		printf("VIDIOC_QUERYCTRL(CONTRAST) error\n");
		return -1;
	}
}

int VportSetContrast(void *hdl, int value)
{
	struct v4l2_control control;
	vInPort *handle = (vInPort *) hdl;

	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_CONTRAST;
	control.value = value;
	if (-1 == ioctl (handle->fd, VIDIOC_S_CTRL, &control))
	{
		printf("VIDIOC_S_CTRL(CONTRAST) error!\n");
		return -1;
	}
	return 0;
}

int VportGetWhiteBalance(void *hdl)
{
	struct v4l2_control control;
	vInPort *handle = (vInPort *) hdl;

	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE;
	if (0 == ioctl (handle->fd, VIDIOC_G_CTRL, &control)) {
		printf("WhiteBalance value = %d\n", control.value);
	} else {
		printf("VIDIOC_G_CTRL(WHITE_BALACNE) error!\n");
		return -1;
	}
	return control.value;
}

int VportSetWhiteBalance(void *hdl, enum v4l2_auto_n_preset_white_balance value)
{
	struct v4l2_control control;
	vInPort *handle = (vInPort *) hdl;

	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_AUTO_N_PRESET_WHITE_BALANCE;
	control.value = value;

	if (-1 == ioctl (handle->fd, VIDIOC_S_CTRL, &control)) {
		printf("VIDIOC_S_CTRL(WHITE_BALANCE) error!\n");
		return -1;
	}
	return 0;
}

int VportQueryExposureCompensation(void *hdl)
{
	struct v4l2_queryctrl queryctrl;
	vInPort *handle = (vInPort *) hdl;

	memset (&queryctrl, 0, sizeof (queryctrl));
	queryctrl.id = V4L2_CID_AUTO_EXPOSURE_BIAS;
	if (0 == ioctl (handle->fd, VIDIOC_QUERYCTRL, &queryctrl))
	{
		printf("ExposureCompensation Range [%d]~[%d] and step = %d\n", queryctrl.minimum,
						queryctrl.maximum, queryctrl.step);
		return 0;
	} else {
		printf("VIDIOC_QUERYCTRL(ExposureCompensation) error\n");
		return -1;
	}
}

int VportGetExposureCompensation(void *hdl)
{
	struct v4l2_control control;
	vInPort *handle = (vInPort *) hdl;

	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_CONTRAST;
	if (0 == ioctl (handle->fd, VIDIOC_G_CTRL, &control)) {
		printf("ExposureCompensation value = %d\n", control.value);
	} else {
		printf("VIDIOC_G_CTRL(ExposureCompensation) error!\n");
		return -1;
	}
	return control.value;
}

int VportSetExposureCompensation(void *hdl, int value)
{
	struct v4l2_control control;
	vInPort *handle = (vInPort *) hdl;

	memset (&control, 0, sizeof (control));
	control.id = V4L2_CID_AUTO_EXPOSURE_BIAS;
	control.value = value;

	if (-1 == ioctl (handle->fd, VIDIOC_S_CTRL, &control)) {
		printf("VIDIOC_S_CTRL(EXPOSURE_BIAS) error!\n");
		return -1;
	}
	return 0;
}

int VportWMInit(WaterMarkInfo *WM_info, char WMPath[30])
{
		int i;
		int watermark_pic_num = 13;
		char filename[64];
		FILE *icon_hdle = NULL;
		unsigned char *tmp_argb = NULL;
		int width = 0;
		int height = 0;

		/* init watermark pic info */
		for (i = 0; i < watermark_pic_num; i++) {
				sprintf(filename, "%s%d.bmp", WMPath, i);
				icon_hdle = fopen(filename, "r");
				if (icon_hdle == NULL) {
						printf("get wartermark %s error\n", filename);
						return -1;
				}
				/* get watermark picture size */
				fseek(icon_hdle, 18, SEEK_SET);
				fread(&width, 1, 4, icon_hdle);
				fread(&height, 1, 4, icon_hdle);
				fseek(icon_hdle, 54, SEEK_SET);

				if (WM_info->width == 0) {
						WM_info->width = width;
						WM_info->height = height * (-1);
				}

				WM_info->single_pic[i].id = i;
				WM_info->single_pic[i].y = (unsigned char *)malloc(WM_info->width * WM_info->height * 5 / 2);
				WM_info->single_pic[i].alph = WM_info->single_pic[i].y + WM_info->width * WM_info->height;
				WM_info->single_pic[i].c = WM_info->single_pic[i].alph + WM_info->width * WM_info->height;

				if (tmp_argb == NULL)
						tmp_argb = (unsigned char *)malloc(WM_info->width * WM_info->height * 4);

				fread(tmp_argb, WM_info->width * WM_info->height * 4, 1, icon_hdle);
				argb2yuv420sp(tmp_argb, WM_info->single_pic[i].alph, WM_info->width, WM_info->height,
								WM_info->single_pic[i].y, WM_info->single_pic[i].c);

						fclose(icon_hdle);
						icon_hdle = NULL;
		}
		WM_info->picture_number = i;

		if (tmp_argb != NULL)
			free(tmp_argb);

		return 0;
}

int VportWMRelease(void *hdl)
{
	WaterMarkInfo WM_info;
	int watermark_pic_num = 13;
	int i;

	vInPort *handle = (vInPort *) hdl;

	for (i = 0; i < watermark_pic_num; i++) {
		if (handle->WM_info.single_pic[i].y) {
			free(handle->WM_info.single_pic[i].y);
			handle->WM_info.single_pic[i].y = NULL;
		}
	}

	return 0;
}

int VportAddWM (void *hdl, unsigned int bg_width, unsigned int bg_height, void *bg_y_vir,
		void *bg_c_vir, unsigned int wm_pos_x, unsigned int wm_pos_y, struct tm *time_data)
{
	time_t rawtime;
	struct tm *time_info;
	long long time1 = 0;
	long long time2 = 0;
	BackGroudLayerInfo BG_info;
	ShowWaterMarkParam WM_Param;

	vInPort *handle = (vInPort *) hdl;
	time1 = GetNowUs();
	memset(&BG_info, 0, sizeof(BackGroudLayerInfo));
	/* init backgroud info */
	BG_info.width		= bg_width;
	BG_info.height		= bg_height;
	BG_info.y		= (unsigned char *)bg_y_vir;
	BG_info.c		= (unsigned char *)bg_c_vir;

	/* init watermark show para */
	WM_Param.pos.x = wm_pos_x;
	WM_Param.pos.y = wm_pos_y;

	if (time_data == NULL)
	{
		time(&rawtime);
		time_info = gmtime(&rawtime);
	}
	else
	{
		time_info = time_data;
		time_info->tm_year = time_info->tm_year - 1900;
	}

	WM_Param.id_list[0] = (time_info->tm_year + 1900) / 1000;
	WM_Param.id_list[1] = ((time_info->tm_year + 1900) / 100) % 10;
	WM_Param.id_list[2] = ((time_info->tm_year + 1900) / 10) % 10;
	WM_Param.id_list[3] = (time_info->tm_year + 1900) % 10;
	WM_Param.id_list[4] = 11;
	WM_Param.id_list[5] = (1 + time_info->tm_mon) / 10 ;
	WM_Param.id_list[6] = (1 + time_info->tm_mon) % 10;
	WM_Param.id_list[7] = 11;
	WM_Param.id_list[8] = time_info->tm_mday / 10;
	WM_Param.id_list[9] = time_info->tm_mday % 10;
	WM_Param.id_list[10] = 10;
	WM_Param.id_list[11] = time_info->tm_hour / 10;
	WM_Param.id_list[12] = time_info->tm_hour % 10;
	WM_Param.id_list[13] = 12;
	WM_Param.id_list[14] = time_info->tm_min / 10;
	WM_Param.id_list[15] = time_info->tm_min % 10;
	WM_Param.id_list[16] = 12;
	WM_Param.id_list[17] = time_info->tm_sec / 10;
	WM_Param.id_list[18] = time_info->tm_sec % 10;
	WM_Param.number = 19;

	watermark_blending(&BG_info, &handle->WM_info, &WM_Param);
	time2 = GetNowUs();
	//printf("water_mark_blending time: %lld(us)\n", time2 - time1);

	SunxiMemFlushCache(handle->pMemops, BG_info.y + wm_pos_y * bg_width,
				handle->WM_info.height * bg_width);
	return 0;
}

vInPort *CreateTinaVinport()
{
	vInPort *tinaVin = (vInPort *)malloc(sizeof(vInPort));
	if (tinaVin == NULL)
		return NULL;

	memset(tinaVin, 0, sizeof(vInPort));
	tinaVin->open = VportOpen;
	tinaVin->close = VportClose;
	tinaVin->init = VportInit;
	tinaVin->requestbuf = VportRequestbuf;
	tinaVin->releasebuf = VportReleasebuf;
	tinaVin->getEnable = VportgetEnable;
	tinaVin->setEnable = VportsetEnable;
	tinaVin->getFrameRate = VportgetFrameRate;
	tinaVin->setFrameRate = VportsetFrameRate;
	tinaVin->getFormat = VportgetFormat;
	tinaVin->setFormat = VportsetFormat;
	tinaVin->getSize = VportgetSize;
	tinaVin->setSize = VportsetSize;
	tinaVin->startcapture = VportStartcapture;
	tinaVin->stopcapture = VportStopcapture;
	tinaVin->dequeue = VportDequeue;
	tinaVin->enqueue = VportEnqueue;
	tinaVin->getCameraIndex = VportGetIndex;
	tinaVin->queryContrast = VportQueryContrast;
	tinaVin->getContrast = VportGetContrast;
	tinaVin->setContrast = VportSetContrast;
	tinaVin->setWhiteBalance = VportSetWhiteBalance;
	tinaVin->getWhiteBalance = VportGetWhiteBalance;
	tinaVin->setExposureCompensation = VportSetExposureCompensation;
	tinaVin->getExposureCompensation = VportGetExposureCompensation;
	tinaVin->queryExposureCompensation = VportQueryExposureCompensation;
	tinaVin->WMInit = VportWMInit;
	tinaVin->WMRelease = VportWMRelease;
	tinaVin->addWM = VportAddWM;
	tinaVin->state = CAMERA_STATE_NONE;
	return tinaVin;
}

int DestroyTinaVinport(vInPort *hdl)
{
	vInPort *handle = (vInPort *)hdl;

	if (handle->state != CAMERA_STATE_NONE) {
		printf("[%s] Camera state error,status:%d", __func__,handle->state);
		return -1;
	}

	sem_destroy(&handle->dqfinSem);

	free(handle);

	return 0;
}
