/*
**
** Copyright (C) 2008, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#include <log/log_wrapper.h>
//#include <cutils/properties.h>
#include <string.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <vo/hwdisplay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/fb.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <errno.h>

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "HwDisplay"
#endif

static pthread_mutex_t disp_mgr_lock;
static int mInitialized = 0;
static int mDisp_fd = -1;
static unsigned int mLayerStatus[CHN_NUM][LYL_NUM];
static int mScreen;
static int mCurLayerCnt;

int hw_display_init()
{
    LOGD("");
    pthread_mutex_init(&disp_mgr_lock, NULL);
    memset(mLayerStatus, 0, sizeof(mLayerStatus));
    return hwd_init();
}

int hw_display_deinit()
{
    if (mInitialized) {
        hwd_exit();
        mInitialized = 0;
        pthread_mutex_destroy(&disp_mgr_lock);
    }
    return 0;
}


int layer_cmd2(int screen, unsigned int hlay)
{
    return 0;
}

int layer_cmd(unsigned int hlay)    //finish
{
    return layer_cmd2(mScreen, hlay);
}

int layer_request(int *pCh, int *pId)   //finish
{
    LOGD("");
    int ch;
    int id;

    pthread_mutex_lock(&disp_mgr_lock);
    for(id=0; id<LYL_NUM; id++) {
        for (ch=0; ch<CHN_NUM; ch++) {
            if (!(mLayerStatus[ch][id] & HWD_STATUS_REQUESTED)) {
                mLayerStatus[ch][id] |= HWD_STATUS_REQUESTED;
                goto out;
            }
        }
    }
out:
    if ((ch==CHN_NUM) && (id==LYL_NUM)) {
        LOGE("all layer used.");
		pthread_mutex_unlock(&disp_mgr_lock);
        return RET_FAIL;
    }
    *pCh = ch;
    *pId = id;
    mCurLayerCnt++;
	pthread_mutex_unlock(&disp_mgr_lock);
    LOGD("requested: ch:%d, id:%d", ch, id);
    return HLAY(ch, id);
}

int layer_release(int hlay)
{
    return hwd_layer_close(hlay);
}

int layer_config(__DISP_t cmd, disp_layer_config *pinfo)
{
    unsigned long args[4] = {0};
    int ret = RET_OK;
    args[0] = mScreen;
    args[1] = (unsigned long)pinfo;
    args[2] = 1;
    ret = ioctl(mDisp_fd, cmd, args);
    if(ret < 0) {
        LOGE("ioctl failed: %s", strerror(errno));
        ret = RET_FAIL;
    }
    return ret;
}

int layer_get_para(disp_layer_config *pinfo)
{
    int ret = layer_config(DISP_LAYER_GET_CONFIG, pinfo);
    if (ret < 0)
        LOGE("get para failed");

    return ret;
}

int layer_set_para(disp_layer_config *pinfo)
{
    int ret = layer_config(DISP_LAYER_SET_CONFIG, pinfo);
    if (ret < 0)
        LOGE("set para failed");

    return ret;
}

#define ALIGN_16B(x) (((x) + (15)) & ~(15))


int hwd_layer_set_src(unsigned int hlay, struct src_info *src)
{
    LOGD("");
	pthread_mutex_lock(&disp_mgr_lock);
    unsigned long args[4] = {0};
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    layer_get_para(&config);

    config.info.fb.crop.x = src->crop_x;
    config.info.fb.crop.y = src->crop_y;
    config.info.fb.crop.width  = (src->crop_w);
    config.info.fb.crop.height = (src->crop_h);

    LOGD("width: 0x%llx, height: 0x%llx", config.info.fb.crop.width, config.info.fb.crop.height);

    config.info.fb.crop.x = config.info.fb.crop.x << 32;
    config.info.fb.crop.y = config.info.fb.crop.y << 32;
    config.info.fb.crop.width  = config.info.fb.crop.width << 32;
    config.info.fb.crop.height = config.info.fb.crop.height << 32;

    LOGD("width: 0x%llx, height: 0x%llx", config.info.fb.crop.width, config.info.fb.crop.height);

    config.info.fb.size[0].width  = src->w;
    config.info.fb.size[0].height = src->h;
    config.info.fb.color_space = (disp_color_space)src->color_space;
    switch(src->format) {
        case HWC_FORMAT_YUV420PLANAR: // YU12
            config.info.fb.format = DISP_FORMAT_YUV420_P;
            /*
            config.info.fb.addr[0] = (int)(config.info.fb.addr[0] );
            config.info.fb.addr[1] = (int)(config.info.fb.addr[0] + fb_width*fb_height);
            config.info.fb.addr[2] = (int)(config.info.fb.addr[0] + fb_width*fb_height*5/4);
            */
            config.info.fb.size[1].width    = config.info.fb.size[0].width / 2;
            config.info.fb.size[1].height   = config.info.fb.size[0].height / 2;
            config.info.fb.size[2].width    = config.info.fb.size[0].width / 2;
            config.info.fb.size[2].height   = config.info.fb.size[0].height / 2;
            break;
        case HWC_FORMAT_YUV420UVC:
            config.info.fb.format = DISP_FORMAT_YUV420_SP_UVUV;
            config.info.fb.size[1].width    = config.info.fb.size[0].width / 2;
            config.info.fb.size[1].height   = config.info.fb.size[0].height / 2;
            //config.info.fb.size[2].width    = config.info.fb.size[0].width / 2;
            //config.info.fb.size[2].height   = config.info.fb.size[0].height / 2;
            break;
        case HWC_FORMAT_YUV420VUC: // NV21
            config.info.fb.format = DISP_FORMAT_YUV420_SP_VUVU;
            config.info.fb.size[1].width  = config.info.fb.size[0].width/2;
            config.info.fb.size[1].height = config.info.fb.size[0].height/2;
            //config.info.fb.color_space = DISP_YCC;
            //config.info.fb.size[2].width  = config.info.fb.size[0].width/2;
            //config.info.fb.size[2].height = config.info.fb.size[0].height/2;
            break;
        case HWC_FORMAT_YUV444PLANAR:
            config.info.fb.format = DISP_FORMAT_YUV444_P;
            /*
            config.info.fb.addr[0] = (int)(config.info.fb.addr[0]);
            config.info.fb.addr[1] = (int)(config.info.fb.addr[0] + fb_width*fb_height);
            config.info.fb.addr[2] = (int)(config.info.fb.addr[0] + fb_width*fb_height*2);
            */
            config.info.fb.size[1].width    = config.info.fb.size[0].width;
            config.info.fb.size[1].height   = config.info.fb.size[0].height;
            config.info.fb.size[2].width    = config.info.fb.size[0].width;
            config.info.fb.size[2].height   = config.info.fb.size[0].height;
            break;
        case HWC_FORMAT_YUV422PLANAR:
            config.info.fb.format = DISP_FORMAT_YUV422_P;
            /*
            config.info.fb.addr[0] = (int)(config.info.fb.addr[0] );
            config.info.fb.addr[1] = (int)(config.info.fb.addr[0] + fb_width*fb_height);
            config.info.fb.addr[2] = (int)(config.info.fb.addr[0] + fb_width*fb_height*3/2);
            */
            config.info.fb.size[1].width    = config.info.fb.size[0].width / 2;
            config.info.fb.size[1].height   = config.info.fb.size[0].height;
            config.info.fb.size[2].width    = config.info.fb.size[0].width / 2;
            config.info.fb.size[2].height   = config.info.fb.size[0].height;
            break;
        case HWC_FORMAT_YUV422UVC:
            config.info.fb.format = DISP_FORMAT_YUV422_SP_UVUV;
            config.info.fb.size[1].width    = config.info.fb.size[0].width / 2;
            config.info.fb.size[1].height   = config.info.fb.size[0].height;
            config.info.fb.size[2].width    = config.info.fb.size[0].width / 2;
            config.info.fb.size[2].height   = config.info.fb.size[0].height;
            break;
        case HWC_FORMAT_YUV422VUC:
            config.info.fb.format = DISP_FORMAT_YUV422_SP_VUVU;
            config.info.fb.size[1].width    = config.info.fb.size[0].width / 2;
            config.info.fb.size[1].height   = config.info.fb.size[0].height;
            config.info.fb.size[2].width    = config.info.fb.size[0].width / 2;
            config.info.fb.size[2].height   = config.info.fb.size[0].height;
            break;
        case HWC_FORMAT_YUV411PLANAR:
            config.info.fb.format = DISP_FORMAT_YUV411_P;
            /*
            config.info.fb.addr[0] = (int)(config.info.fb.addr[0] );
            config.info.fb.addr[1] = (int)(config.info.fb.addr[0] + fb_width*fb_height);
            config.info.fb.addr[2] = (int)(config.info.fb.addr[0] + fb_width*fb_height*5/4);
            */
            config.info.fb.size[1].width    = config.info.fb.size[0].width / 4;
            config.info.fb.size[1].height   = config.info.fb.size[0].height;
            config.info.fb.size[2].width    = config.info.fb.size[0].width / 4;
            config.info.fb.size[2].height   = config.info.fb.size[0].height;
            break;
        case HWC_FORMAT_YUV411UVC:
            config.info.fb.format = DISP_FORMAT_YUV411_SP_UVUV;
            config.info.fb.size[1].width    = config.info.fb.size[0].width / 4;
            config.info.fb.size[1].height   = config.info.fb.size[0].height;
            config.info.fb.size[2].width    = config.info.fb.size[0].width / 4;
            config.info.fb.size[2].height   = config.info.fb.size[0].height;
            break;
        case HWC_FORMAT_YUV411VUC:
            config.info.fb.format = DISP_FORMAT_YUV411_SP_VUVU;
            config.info.fb.size[1].width    = config.info.fb.size[0].width / 4;
            config.info.fb.size[1].height   = config.info.fb.size[0].height;
            config.info.fb.size[2].width    = config.info.fb.size[0].width / 4;
            config.info.fb.size[2].height   = config.info.fb.size[0].height;
            break;
        default:
            /* INTERLEAVED */
            /*
            config.info.fb.addr[0] = (int)(config.info.fb.addr[0] + fb_width*fb_height/3*0);
            config.info.fb.addr[1] = (int)(config.info.fb.addr[0] + fb_width*fb_height/3*1);
            config.info.fb.addr[2] = (int)(config.info.fb.addr[0] + fb_width*fb_height/3*2);
            */
            config.info.fb.format = DISP_FORMAT_ARGB_8888;
            config.info.fb.size[1].width    = config.info.fb.size[0].width;
            config.info.fb.size[1].height   = config.info.fb.size[0].height;
            config.info.fb.size[2].width    = config.info.fb.size[0].width;
            config.info.fb.size[2].height   = config.info.fb.size[0].height;
            break;
    }
    LOGD("set fb.format %d %d, color_space %d end, size0[%dx%d], size1[%dx%d]", src->format, config.info.fb.format, config.info.fb.color_space,
        config.info.fb.size[0].width, config.info.fb.size[0].height, config.info.fb.size[1].width, config.info.fb.size[1].height);
	pthread_mutex_unlock(&disp_mgr_lock);
	return layer_set_para(&config);
}


int hwd_layer_set_rect(unsigned int hlay, struct view_info *view)
{


    pthread_mutex_lock(&disp_mgr_lock);
    LOGD("(%s %d)", __FUNCTION__, __LINE__);
    disp_layer_config config;

    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    layer_get_para(&config);

    config.info.screen_win.x      = view->x;
    config.info.screen_win.y      = view->y;
    config.info.screen_win.width  = view->w;
    config.info.screen_win.height = view->h;
	pthread_mutex_unlock(&disp_mgr_lock);
    LOGD("(%s %d) end", __FUNCTION__, __LINE__);
    return layer_set_para(&config);
}

int hwd_layer_top(unsigned int hlay)
{
    LOGD("(%s %d) set hlay to top", __FUNCTION__, __LINE__);
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    layer_get_para(&config);

    config.info.zorder = ZORDER_MAX;
    return layer_set_para(&config);
}

int hwd_layer_bottom(unsigned int hlay)
{
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    layer_get_para(&config);

    config.info.zorder = ZORDER_MIN;
    return layer_set_para(&config);
}

int hwd_layer_set_zorder(unsigned int hlay, unsigned char zorder)
{
    if(zorder < ZORDER_MIN || zorder > ZORDER_MAX)
    {
        LOGW("zorder[%d] is wrong!", zorder);
        return RET_FAIL;
    }
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    layer_get_para(&config);

    config.info.zorder = zorder;
    return layer_set_para(&config);
}

int hwd_layer_open(unsigned int hlay)
{
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    layer_get_para(&config);

    config.enable = 1;
    return layer_set_para(&config);
}

int hwd_layer_close(unsigned int hlay)
{
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    layer_get_para(&config);

    config.enable = 0;
    return layer_set_para(&config);
}

int hwd_layer_ck_on(unsigned int hlay)
{
    return 0;
}

int hwd_layer_ck_off(unsigned int hlay)
{
    return 0;
}

int hwd_layer_ck_value(unsigned int hlay, unsigned int color)
{

    return 0;
}

int layer_set_normal(unsigned int hlay)
{
    return 0;
}

int hwd_set_rot(int p_chn, int p_layer, int rot)
{
    int ret = 0;
    unsigned long args[4]={0};

	/* Check input parameter */
    if (mDisp_fd < 0) {
        LOGW("The hwd is not Init!");
        return -1;
    }

    args[1] = p_chn;  //channel id
    args[2] = p_layer;  //layer id
    args[3] = rot;   //rotate degree

    ret = ioctl(mDisp_fd, DISP_ROTATION_SW_SET_ROT, args);
    if (ret) {
        LOGW("Do cmd:DISP_ROTATION_SW_SET_ROT error! ret:%d ", ret);
    }

    return ret;
}

void openHdmi(int p_chn, int p_layer, disp_output_type p_disp_type, disp_tv_mode p_tv_mode)
{
    int ret = 0;
    unsigned long args[4]={0};

    if( p_disp_type == DISP_OUTPUT_TYPE_HDMI) {
        args[1] = DISP_OUTPUT_TYPE_NONE;
        ret = ioctl(mDisp_fd, DISP_DEVICE_SWITCH, args);
        if (ret) {
            LOGE("disp switch failed, disp_type: [%d], tv_mode: [%d]", p_disp_type, p_tv_mode);
        }

        hwd_set_rot(p_chn,p_layer, ROTATION_SW_0);
    } else {
        hwd_set_rot(p_chn,p_layer, ROTATION_SW_90);
    }

    args[0] = p_layer;
    args[1] = p_disp_type;
    args[2] = p_tv_mode;
    ret = ioctl(mDisp_fd, DISP_DEVICE_SWITCH, args);
    if (ret) {
        LOGE("disp switch failed, disp_type: [%d], tv_mode: [%d]", p_disp_type, p_tv_mode);
    }
}

int hwd_layer_other_screen(int arg, unsigned int hlay1, unsigned int hlay2,disp_output_type disp_type, disp_tv_mode tv_mode)
{
    pthread_mutex_lock(&disp_mgr_lock);
    LOGD("hwd_layer_other_screen");
    disp_layer_config ui_config, csi_config;
    csi_config.channel = 0;
    csi_config.layer_id = 0;
    layer_get_para(&csi_config);
    if (arg == SCREEN_1) {  //change the ui from (2,0) to (1, 0), so that ui can be scaled
        ui_config.channel = 2;
        ui_config.layer_id = 0;
        layer_get_para(&ui_config);
        openHdmi(1, SCREEN_0, disp_type,tv_mode);
        ui_config.enable = 0;
        layer_set_para(&ui_config);
        ui_config.info.screen_win.width  = 1280;
        ui_config.info.screen_win.height = 720;
        ui_config.channel = 1;
        ui_config.enable = 1;
        ui_config.info.zorder = 1;
        layer_set_para(&ui_config);
        csi_config.info.zorder = 2; //set the csi layer to the top to avoid be covered by ui
        layer_set_para(&csi_config);
    } else {
        ui_config.channel = 1;
        ui_config.layer_id = 0;
        layer_get_para(&ui_config);
        ui_config.enable = 0;
        layer_set_para(&ui_config);
        ui_config.info.screen_win.width  =  320;
        ui_config.info.screen_win.height =  240;
        ui_config.channel = 2;
        ui_config.enable = 1;
        ui_config.info.zorder = 8;
        layer_set_para(&ui_config); //restore ui to (1,0)
        csi_config.info.zorder = 0; //set the csi to the bottom, because ui can do alpha with it
        layer_set_para(&csi_config);
        openHdmi(2, SCREEN_0, disp_type,tv_mode);
    }
	pthread_mutex_unlock(&disp_mgr_lock);
    LOGD("hwd_layer_other_screen end");
    return 0;
}

/*int hwd_layer_clear(unsigned int hlay)
{
    int channel = HD2CHN(hlay);
    int layer_id = HD2LYL(hlay);
    hwd_layer_close(hlay);
    mLayerStatus[channel][layer_id] &= ~HWD_STATUS_OPENED;
    return 0;
}*/

int hwd_layer_render(unsigned int hlay, libhwclayerpara_t *picture)
{
    int ret;
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel  = HD2CHN(hlay);
    config.layer_id = HD2LYL(hlay);
    layer_get_para(&config);
    config.info.fb.addr[0] = picture->top_y;
    config.info.fb.addr[1] = picture->top_c;
    config.info.fb.addr[2] = picture->bottom_y;
    ret = layer_set_para(&config);
//    if(!(mLayerStatus[config.channel][config.layer_id] & HWD_STATUS_OPENED)) {
//        LOGD("(%s %d) %d", __FUNCTION__, __LINE__, hlay);
//        hwd_layer_open(hlay); //to avoid that the first frame is not initialized
//        mLayerStatus[config.channel][config.layer_id] |= HWD_STATUS_OPENED;
//    }
    return ret;
}

int hwd_layer_exchange(unsigned int hlay1, unsigned int hlay2, int otherOnTop)
{
    disp_layer_config config1;
    disp_layer_config config2;
    disp_rect rect_tmp;
    unsigned char zorder_tmp;
    memset(&config1, 0, sizeof(disp_layer_config));
    memset(&config2, 0, sizeof(disp_layer_config));
    config1.channel = HD2CHN(hlay1);
    config1.layer_id = HD2LYL(hlay1);
    config2.channel = HD2CHN(hlay2);
    config2.layer_id = HD2LYL(hlay2);
    layer_get_para(&config1);
    layer_get_para(&config2);

    rect_tmp = config1.info.screen_win;
    config1.info.screen_win = config2.info.screen_win;
    config2.info.screen_win = rect_tmp;

    zorder_tmp = config1.info.zorder;
    config1.info.zorder = config2.info.zorder;
    config2.info.zorder = zorder_tmp;

    layer_set_para(&config1);
    layer_set_para(&config2);
    return RET_OK;
}

int hwd_layer_exchange_zorder(unsigned int hlay1, unsigned int hlay2)
{
    disp_layer_config config1;
    disp_layer_config config2;
    unsigned char zorder_tmp;
    memset(&config1, 0, sizeof(disp_layer_config));
    memset(&config2, 0, sizeof(disp_layer_config));
    config1.channel = HD2CHN(hlay1);
    config1.layer_id = HD2LYL(hlay1);
    config2.channel = HD2CHN(hlay2);
    config2.layer_id = HD2LYL(hlay2);
    layer_get_para(&config1);
    layer_get_para(&config2);

    zorder_tmp = config1.info.zorder;
    config1.info.zorder = config2.info.zorder;
    config2.info.zorder = zorder_tmp;

    layer_set_para(&config1);
    layer_set_para(&config2);
    return RET_OK;
}

int hwd_layer_switch(unsigned int hlay, int bOpen)
{
    if (1 == bOpen) {
        return hwd_layer_open(hlay);
    } else if (0 == bOpen) {
        return hwd_layer_close(hlay);
    }
    return RET_FAIL;
}

int hwd_layer_release(unsigned int hlay)
{

    int chn = HD2CHN(hlay);
    int lyl = HD2LYL(hlay);
    int ret = layer_release(hlay);
    if (RET_OK == ret) {
        pthread_mutex_lock(&disp_mgr_lock);
        if (chn >=0 && lyl >=0 && mLayerStatus[chn][lyl]) {
            mLayerStatus[chn][lyl] = 0;
            mCurLayerCnt--;
        }
		pthread_mutex_unlock(&disp_mgr_lock);
    }
    return ret;

}

int hwd_screen_capture_start(int screen, disp_capture_info cap_info)
{
    unsigned long args[4]={0};
    args[0] = screen;
    args[1] = (unsigned long)&cap_info;
    args[2] = 0;
    args[3] = 0;
    if(ioctl(mDisp_fd,DISP_CAPTURE_START,(void*)args) < 0) {
        LOGD("DISP_CAPTURE_START fail");
        return -1;
    }
    return 0;
}


int hwd_screen_capture_commit(int screen, disp_capture_info cap_info)
{
    unsigned long args[4]={0};
    args[0] = screen;
    args[1] = (unsigned long)&cap_info;
    args[2] = 0;
    args[3] = 0;
    if(ioctl(mDisp_fd,DISP_CAPTURE_COMMIT,(void*)args) < 0) {
        LOGD("DISP_CAPTURE_COMMIT fail");
        return -1;
    }
    return 0;
}

int hwd_screen_capture_stop(int screen, disp_capture_info cap_info)
{
    unsigned long args[4]={0};
    args[0] = screen;
    args[1] = (unsigned long)&cap_info;
    args[2] = 0;
    args[3] = 0;
    if(ioctl(mDisp_fd,DISP_CAPTURE_STOP,(void*)args) < 0) {
        LOGD("DISP_CAPTURE_STOP fail");
        return -1;
    }
    return 0;
}

void hwd_set_outputtype(int screen, int val)
{
    LOGD("+++++=(%s %d %d)", __FUNCTION__, __LINE__,screen);
    unsigned long args[4]={0};
    args[0] = screen;
    args[1] = val;
    ioctl(mDisp_fd, DISP_DEVICE_SWITCH, args);
    LOGE("(%s %d)", __FUNCTION__, __LINE__);
    return ;
}

void hwd_get_outputtype(int screen, int *val)
{
    LOGD("+++++=(%s %d %d)", __FUNCTION__, __LINE__,screen);
    if(val == NULL){
        LOGD("val addr NULL. (%s %d)", __FUNCTION__, __LINE__);
        return;
    }
    disp_output_type output_type;
    unsigned int arg[3];

    arg[0] = screen;
    output_type = (disp_output_type)ioctl(mDisp_fd, DISP_GET_OUTPUT_TYPE, (void*)arg);
    *val = output_type;
    return ;
}

int hwd_layer_request(struct view_info* surface)
{
    int hlay;
    int ch, id;
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    hlay = layer_request(&ch, &id);
    config.channel = ch;
    config.layer_id = id;
    config.enable = 0;
    config.info.screen_win.x = surface->x;
    config.info.screen_win.y = surface->y;
    config.info.screen_win.width    = surface->w;
    config.info.screen_win.height   = surface->h;
    config.info.mode = LAYER_MODE_BUFFER;
    config.info.alpha_mode = 0;
    config.info.alpha_value = 128;
    config.info.fb.flags = DISP_BF_NORMAL;
    config.info.fb.scan = DISP_SCAN_PROGRESSIVE;
    config.info.fb.color_space = (surface->h<720)?DISP_BT601:DISP_BT709;
    config.info.zorder = HLAY(ch, id);
    LOGI("hlay:%d, zorder=%d, cnt:%d", hlay, config.info.zorder, mCurLayerCnt);
    layer_set_para(&config);
    return hlay;
}

int hw_layer_request(unsigned char channel_id, unsigned char layer_id,
                     unsigned char zorder, struct view_info *surface)
{

    int hlay;
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));

    if (mLayerStatus[channel_id][layer_id] & HWD_STATUS_REQUESTED) {
        LOGE("channel id: %d, layer id: %d has been requested", channel_id, layer_id);
        return -1;
    }

    config.channel  = channel_id;
    config.layer_id = layer_id;
    config.enable = 0;
    config.info.screen_win.x = surface->x;
    config.info.screen_win.y = surface->y;
    config.info.screen_win.width    = surface->w;
    config.info.screen_win.height   = surface->h;
    config.info.mode = LAYER_MODE_BUFFER;
    config.info.alpha_mode = 0;
    config.info.alpha_value = 128;
    config.info.fb.flags = DISP_BF_NORMAL;
    config.info.fb.scan = DISP_SCAN_PROGRESSIVE;
    config.info.fb.color_space = (surface->h<720)?DISP_BT601:DISP_BT709;
    config.info.zorder = zorder;

    layer_set_para(&config);
    mCurLayerCnt++;
    mLayerStatus[channel_id][layer_id] |= HWD_STATUS_REQUESTED;
    hlay = HLAY(channel_id, layer_id);
    LOGI("hlay:%d, zorder=%d, cnt:%d", hlay, config.info.zorder, mCurLayerCnt);
    return hlay;
}


int hwd_init()
{
    LOGD("(%s %d)", __FUNCTION__, __LINE__);
    int ret = RET_OK;
    mScreen = SCREEN_0;
    if (mInitialized) {
        return RET_FAIL;
    }

    mDisp_fd = open(DISP_DEV, O_RDWR);
    if (mDisp_fd < 0)
    {
        LOGE("Failed to open disp device, ret:%d, errno: %d", mDisp_fd, errno);
        return  RET_FAIL;
    }


    mLayerStatus[2][0] = HWD_STATUS_REQUESTED;  //used by fb0

    //display resume
    int args[4]={0};
    args[0] = 0;
    args[1] = 0;
    ioctl(mDisp_fd, DISP_BLANK, args);

    mInitialized = 1;
    mCurLayerCnt = 1;
    return ret;
}

int hwd_exit(void)
{
    LOGD("(%s %d)", __FUNCTION__, __LINE__);
    int ret = RET_OK;

    close(mDisp_fd);

    return ret;
}


int layer_request_v40(int *pCh, int *pId, int ui_video)   //finish
{
    LOGD("(%s %d)", __FUNCTION__, __LINE__);
    int ch = 0;
    int id = 0;

	if(ui_video == 0)//for ui
    {
        pthread_mutex_lock(&disp_mgr_lock);
        for(id=0; id<LYL_NUM; id++) {
            for (ch=0; ch<CHN_NUM; ch++) {
                if (!(mLayerStatus[ch][id] & HWD_STATUS_REQUESTED)) {
                    mLayerStatus[ch][id] |= HWD_STATUS_REQUESTED;
                    goto out;
                }
            }
        }
    }else if(ui_video == 1)//for video
    {
        pthread_mutex_lock(&disp_mgr_lock);
        for (ch=0; ch<CHN_NUM; ch++) {
            for(id=0; id<LYL_NUM; id++) {
                if (!(mLayerStatus[ch][id] & HWD_STATUS_REQUESTED)) {
                    mLayerStatus[ch][id] |= HWD_STATUS_REQUESTED;
                    goto out;
                }
            }
        }
    }
out:
    if ((ch==CHN_NUM) && (id==LYL_NUM)) {
        LOGE("all layer used.");
        return RET_FAIL;
    }
    *pCh = ch;
    *pId = id;
    mCurLayerCnt++;
    pthread_mutex_unlock(&disp_mgr_lock);
    LOGD("requested: ch:%d, id:%d", ch, id);
    return HLAY(ch, id);
}

int hwd_layer_request_v40(struct view_info* surface, int ui_video)
{
    int hlay;
    int ch, id;
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    hlay = layer_request_v40(&ch, &id, ui_video);
    config.channel = ch;
    config.layer_id = id;
    config.enable = 0;
    config.info.screen_win.x = surface->x;
    config.info.screen_win.y = surface->y;
    config.info.screen_win.width    = surface->w;
    config.info.screen_win.height   = surface->h;
    config.info.mode = LAYER_MODE_BUFFER;
    config.info.alpha_mode = 0;
    config.info.alpha_value = 128;
    config.info.fb.flags = DISP_BF_NORMAL;
    config.info.fb.scan = DISP_SCAN_PROGRESSIVE;
    config.info.fb.color_space = (surface->h<720)?DISP_BT601:DISP_BT709;
    config.info.zorder = HLAY(ch, id);
    LOGI("hlay:%d, zorder=%d, cnt:%d", hlay, config.info.zorder, mCurLayerCnt);
    layer_set_para(&config);
    return hlay;
}

int hwd_layer_request_hlay(int hlay)
{
    int ch = HD2CHN(hlay);
    int id = HD2LYL(hlay);
    pthread_mutex_lock(&disp_mgr_lock);
    if (mLayerStatus[ch][id] & HWD_STATUS_REQUESTED)
    {
        pthread_mutex_unlock(&disp_mgr_lock);
        return -1;
    }
    mLayerStatus[ch][id] |= HWD_STATUS_REQUESTED;
    mCurLayerCnt++;
    pthread_mutex_unlock(&disp_mgr_lock);
    disp_layer_config config;
    memset(&config, 0, sizeof(disp_layer_config));
    config.channel = ch;
    config.layer_id = id;
    config.enable = 0;
    config.info.screen_win.x = 0;
    config.info.screen_win.y = 0;
    config.info.screen_win.width    = 320;
    config.info.screen_win.height   = 240;
    config.info.mode = LAYER_MODE_BUFFER;
    config.info.alpha_mode = 0;
    config.info.alpha_value = 128;
    config.info.fb.flags = DISP_BF_NORMAL;
    config.info.fb.scan = DISP_SCAN_PROGRESSIVE;
    config.info.fb.color_space = DISP_BT601;
    config.info.zorder = HLAY(ch, id);
    LOGD("hlay:%d, zorder=%d, cnt:%d", hlay, config.info.zorder, mCurLayerCnt);
    layer_set_para(&config);
    return 0;
}

int hwd_layer_check_status(int hlay)
{
    int ch, id;
    int layerStatus;
    pthread_mutex_lock(&disp_mgr_lock);
    ch = HD2CHN(hlay);
    id = HD2LYL(hlay);
    layerStatus = mLayerStatus[ch][id];
    pthread_mutex_unlock(&disp_mgr_lock);
    return layerStatus;
}

int hwd_is_support_tv_mode(disp_output_type disp_type, disp_tv_mode tv_mode)
{
    int ret = -1;
    switch (disp_type) {
        case DISP_OUTPUT_TYPE_LCD:
            switch (tv_mode) {
                /* tv_mode for LCD is fixed in sys_config.fex.
                 * the configuration here is just to stay compatible with apps.
                 */
                case DISP_TV_MOD_480I:
                case DISP_TV_MOD_576I:
                case DISP_TV_MOD_720P_50HZ:
                case DISP_TV_MOD_720P_60HZ:
                case DISP_TV_MOD_1080P_24HZ:
                case DISP_TV_MOD_1080P_25HZ:
                case DISP_TV_MOD_1080P_30HZ:
                case DISP_TV_MOD_1080P_50HZ:
                case DISP_TV_MOD_1080P_60HZ:
                case DISP_TV_MOD_1080I_50HZ:
                case DISP_TV_MOD_1080I_60HZ:
                    ret = 0;
                    break;
                default:
                    ret = 1;
                    break;
            }
            break;

        case DISP_OUTPUT_TYPE_TV:
            switch (tv_mode) {
                case DISP_TV_MOD_PAL:
                case DISP_TV_MOD_PAL_SVIDEO:
                case DISP_TV_MOD_NTSC:
                case DISP_TV_MOD_NTSC_SVIDEO:
                case DISP_TV_MOD_PAL_M:
                case DISP_TV_MOD_PAL_M_SVIDEO:
                case DISP_TV_MOD_PAL_NC:
                case DISP_TV_MOD_PAL_NC_SVIDEO:
                case DISP_TV_MOD_480I:
                case DISP_TV_MOD_576I:
                    ret = 0;
                    break;
                default:
                    ret = 1;
                    break;
            }
            break;

        case DISP_OUTPUT_TYPE_HDMI:
            switch (tv_mode) {
                case DISP_TV_MOD_720P_50HZ:
                case DISP_TV_MOD_720P_60HZ:
                case DISP_TV_MOD_1080I_50HZ:
                case DISP_TV_MOD_1080I_60HZ:
                case DISP_TV_MOD_1080P_24HZ:
                case DISP_TV_MOD_1080P_50HZ:
                case DISP_TV_MOD_1080P_60HZ:
                case DISP_TV_MOD_1080P_24HZ_3D_FP:
                case DISP_TV_MOD_720P_50HZ_3D_FP:
                case DISP_TV_MOD_720P_60HZ_3D_FP:
                case DISP_TV_MOD_1080P_25HZ:
                case DISP_TV_MOD_1080P_30HZ:
                case DISP_TV_MOD_3840_2160P_30HZ:
                case DISP_TV_MOD_3840_2160P_25HZ:
                case DISP_TV_MOD_3840_2160P_24HZ:
                    ret = 0;
                    break;
                default:
                    ret = 1;
                    break;
            }
            break;

        case DISP_OUTPUT_TYPE_VGA:
            ret = 1;
            break;

        default:
            LOGW("Input disp_type:%d error!", disp_type);
            ret = -1;
            break;
    }

    return ret;
}

int hwd_get_disp_type(int *disp_type, int *tv_mode)
{
    int               ret     = 0;
    unsigned long     args[4] ={0};
    disp_output       para;

    args[0] = mScreen;
    args[1] = (unsigned long)(&para);
    ret = ioctl(mDisp_fd, DISP_GET_OUTPUT, args);
    if (ret < 0) {
        LOGW("Do cmd:DISP_GET_OUTPUT error! ret:%d", ret);
        return ret;
    }

    LOGI("Current the  disp_type:0x%x  tv_mode:0x%x", para.type, para.mode);
    *disp_type = para.type;
    *tv_mode = para.mode;

    return ret;
}

int hwd_get_hdmi_hw_mode(disp_tv_mode *disp_mode)
{
    disp_tv_mode disp_type_table[] = {
        DISP_TV_MOD_3840_2160P_30HZ,
        DISP_TV_MOD_3840_2160P_25HZ,
        DISP_TV_MOD_3840_2160P_24HZ,

        DISP_TV_MOD_1080P_60HZ,
        DISP_TV_MOD_1080P_50HZ,
        DISP_TV_MOD_1080P_30HZ,
        DISP_TV_MOD_1080P_25HZ,
        DISP_TV_MOD_1080P_24HZ,
        DISP_TV_MOD_720P_60HZ,
        DISP_TV_MOD_720P_50HZ,

        DISP_TV_MOD_1080P_24HZ_3D_FP,
        DISP_TV_MOD_720P_60HZ_3D_FP,
        DISP_TV_MOD_720P_50HZ_3D_FP,

        DISP_TV_MOD_1080I_60HZ,
        DISP_TV_MOD_1080I_50HZ,
    };

    int ret = 0;
    int i = 0;
    int find_mode = 0;
    unsigned long args[4] ={0};
    args[0] = mScreen;

    // TODO: should provide hdmi device detect ioctl.
    /* now, use this temporary solution. */
    char state[20];
    FILE *fd;
    fd = fopen("/sys/class/switch/hdmi/state", "r");
    if (NULL == fd) {
        return -1;
    }
    fgets(state, sizeof(state), fd);
    if (!strncmp(state, "HDMI=0", 6)) {
        LOGE("no hdmi device detected!!\n");
        ret = -1;
        goto no_hdmi_dev;
    }

    for (i = 0; i < sizeof(disp_type_table)/sizeof(int); i++) {
        args[1] = (unsigned long)disp_type_table[i];
        ret = ioctl(mDisp_fd, DISP_HDMI_SUPPORT_MODE, args);
        if (ret == 1) {
            LOGD("find hdmi hardware supported type[%d]\n", disp_type_table[i]);
            *disp_mode = disp_type_table[i];
            find_mode = 1;
            break;
        }
    }

    if (find_mode) {
        ret = 0; // return SUCCESS
    } else {
        ret = 1;
    }

no_hdmi_dev:
    fclose(fd);
    return ret;
}

#ifdef SDV_PRODUCT

void getScreenInfo(int *swidth, int *sheight)
{
	char *env = NULL;
	char mode[32]={0};
	if ((env=getenv("SCREEN_INFO")) != NULL) {
		strncpy(mode, env, strlen(env)+1);
	}else{
        *swidth = 240;
        *sheight = 320;
        return;
    }
	char *pW = NULL;
	char *pH = NULL;
	pW = strstr(mode, "x");
	*pW = '\0';
	*swidth = atoi(mode);
	*pW = 'x';
	pH = strstr(mode, "-");
	*pW = '\0';
	*sheight = atoi(pW+1);
	*pW = '-';

}


int hwd_switch_vo_device(disp_output_type disp_type, disp_tv_mode tv_mode)
{
    int               ret     = 0;
    unsigned long     args[4] ={0};
    disp_output       para;
    disp_layer_config layerinfo;

    memset(&para,      0, sizeof(disp_output));
    memset(&layerinfo, 0, sizeof(disp_layer_config));

    /* Check input parameter */
    if (mDisp_fd < 0) {
        LOGW("The hwd is not Init!");
        return -1;
    }

    int screen_w, screen_h;
    getScreenInfo(&screen_w,&screen_h);
    LOGI("screen size: [%dx%d]", screen_w, screen_h);

    ret = hwd_is_support_tv_mode(disp_type, tv_mode);
    if (ret < 0) {
        LOGW("Inout disp_type:%d error!", disp_type);
        return -1;
    }
    else if(ret > 0) {
        LOGW("This disp_type:%d is not support tv_mode:%d", disp_type, tv_mode);
        return -1;
    }

    args[0] = mScreen;
    args[1] = (unsigned long)(&para);
    ret = ioctl(mDisp_fd, DISP_GET_OUTPUT, args);
    if (ret < 0) {
        LOGW("Do cmd:DISP_GET_OUTPUT error! ret:%d", ret);
        return ret;
    }

    LOGI("Current the  disp_type:0x%x  tv_mode:0x%x", para.type, para.mode);

    if (disp_type == para.type) {
        LOGI("Current the disp_type:%d is not change!", disp_type);
        return 0;
    }

    if (disp_type != DISP_OUTPUT_TYPE_LCD)
    {
        LOGI("======= SWITCH HDMI ===== ");
        openHdmi(2,mScreen,disp_type,tv_mode); //ui display chn rotate
        usleep(30 * 1000);
    }

    int ch, id;
    layerinfo.channel  = 2;
    layerinfo.layer_id = 0;
    ret = layer_get_para(&layerinfo);
    if (ret) {
        LOGW("Do layer_get_para error! ret:%d", ret);
        return -1;
    }

    layerinfo.enable = 0;
    layer_set_para(&layerinfo);

    switch (disp_type) {
        case DISP_OUTPUT_TYPE_TV:
            layerinfo.info.screen_win.x      = 0;
            layerinfo.info.screen_win.y      = 0;
            layerinfo.info.screen_win.width  = 720;
            layerinfo.info.screen_win.height = 576;
            break;
        case DISP_OUTPUT_TYPE_LCD:
            layerinfo.info.screen_win.x      = 0;
            layerinfo.info.screen_win.y      = 0;
            layerinfo.info.screen_win.width  = screen_w;
            layerinfo.info.screen_win.height = screen_h;
            break;
        case DISP_OUTPUT_TYPE_HDMI:
            layerinfo.info.screen_win.x      = 0;
            layerinfo.info.screen_win.y      = 0;
            layerinfo.info.screen_win.width  = 1920;
            layerinfo.info.screen_win.height = 1080;
            break;
        default:
            break;
    }
    layerinfo.enable = 1;
    ret = layer_set_para(&layerinfo);
    if (ret) {
        LOGW("Do layer_set_para error! ret:%d", ret);
        return -1;
    }

    if (disp_type == DISP_OUTPUT_TYPE_LCD)
    {
        usleep(10 * 1000);
        LOGI("======= SWITCH LCD ===== ");
        openHdmi(2,mScreen,disp_type,tv_mode); //Just for fox_V7 2.19LCD switch
        usleep(20 * 1000);
    }

    return 0;
}
#else
int hwd_switch_vo_device(disp_output_type disp_type, disp_tv_mode tv_mode)
{
    int               ret     = 0;
    unsigned long     args[4] ={0};
    disp_output       para;
    disp_layer_config layerinfo;

    memset(&para,      0, sizeof(disp_output));
    memset(&layerinfo, 0, sizeof(disp_layer_config));

    /* Check input parameter */
    if (mDisp_fd < 0) {
        LOGW("The hwd is not Init!");
        return -1;
    }

    ret = hwd_is_support_tv_mode(disp_type, tv_mode);
    if (ret < 0) {
        LOGW("Inout disp_type:%d error!", disp_type);
        return -1;
    }
    else if(ret > 0) {
        LOGW("This disp_type:%d is not support tv_mode:%d", disp_type, tv_mode);
        return -1;
    }

    args[0] = mScreen;
    args[1] = (unsigned long)(&para);
    ret = ioctl(mDisp_fd, DISP_GET_OUTPUT, args);
    if (ret < 0) {
        LOGW("Do cmd:DISP_GET_OUTPUT error! ret:%d", ret);
        return ret;
    }

    LOGI("Current the  disp_type:0x%x  tv_mode:0x%x", para.type, para.mode);

    if (disp_type == para.type) {
        LOGI("Current the disp_type:%d is not change!", disp_type);
        return 0;
    }

    args[0] = mScreen;
    args[1] = disp_type;
    args[2] = tv_mode;
    ret = ioctl(mDisp_fd, DISP_DEVICE_SWITCH, args);
    if (ret) {
        LOGW("Do cmd:DISP_DEVICE_SWITCH error! ret:%d, message:%s", ret, strerror(errno));
        return ret;
    }

    usleep(30 * 1000);
    int ch, id;
    layerinfo.channel  = 2;
    layerinfo.layer_id = 0;
    layerinfo.enable   = 1;
    ret = layer_get_para(&layerinfo);
    if (ret) {
        LOGW("Do layer_get_para error! ret:%d", ret);
        return -1;
    }
    switch (disp_type) {
        case DISP_OUTPUT_TYPE_TV:
            layerinfo.info.screen_win.x      = 0;
            layerinfo.info.screen_win.y      = 0;
            layerinfo.info.screen_win.width  = 720;
            layerinfo.info.screen_win.height = 576;
            break;
        case DISP_OUTPUT_TYPE_LCD:
            layerinfo.info.screen_win.x      = 0;
            layerinfo.info.screen_win.y      = 0;
            layerinfo.info.screen_win.width  = 720;
            layerinfo.info.screen_win.height = 1280;
            break;
        case DISP_OUTPUT_TYPE_HDMI:
            layerinfo.info.screen_win.x      = 0;
            layerinfo.info.screen_win.y      = 0;
            layerinfo.info.screen_win.width  = 1920;
            layerinfo.info.screen_win.height = 1080;
            break;
        default:
            break;
    }
    ret = layer_set_para(&layerinfo);
    if (ret) {
        LOGW("Do layer_set_para error! ret:%d", ret);
        return -1;
    }

    return 0;
}
#endif
