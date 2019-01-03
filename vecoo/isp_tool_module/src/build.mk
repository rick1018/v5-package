TARGET_PATH :=$(call my-dir)
TOP_PATH := $(TARGET_PATH)

LOCAL_INC := \
	$(TARGET_TOP)/middleware/media/LIBRARY/libisp/include \
	$(TARGET_TOP)/middleware/media/LIBRARY/libisp \
	$(TARGET_TOP)/middleware/media/LIBRARY/libisp/include/V4l2Camera  \
	$(TARGET_TOP)/middleware/media/LIBRARY/libisp/include/V4l2Camera/device

#########################################
# venc and rtsp include path
LOCAL_INC += \
    $(TARGET_TOP)/custom_aw/include \
    $(TARGET_TOP)/system/include \
    $(TARGET_TOP)/middleware/include/utils \
    $(TARGET_TOP)/middleware/include/media \
    $(TARGET_TOP)/middleware/include \
    $(TARGET_TOP)/middleware/media/include \
    $(TARGET_TOP)/middleware/media/include/utils \
    $(TARGET_TOP)/middleware/media/include/component \
    $(TARGET_TOP)/middleware/media/LIBRARY/libisp/include \
    $(TARGET_TOP)/middleware/media/LIBRARY/libisp/include/V4l2Camera \
    $(TARGET_TOP)/middleware/media/LIBRARY/libisp/isp_tuning \
    $(TARGET_TOP)/middleware/media/LIBRARY/include_stream \
    $(TARGET_TOP)/middleware/media/LIBRARY/include_FsWriter \
    $(TARGET_TOP)/middleware/media/LIBRARY/libcedarc/include \
    $(TARGET_TOP)/middleware/media/LIBRARY/libcedarx/libcore/common/iniparser \
    $(TARGET_TOP)/middleware/app_mpp_sample/include \
    $(TARGET_PATH)/libvi_venc_rtsp

#########################################
include $(ENV_CLEAR)

TARGET_SRC := \
		tuning_app/awTuningApp.c \
		tuning_app/log_handle.c \
		tuning_app/socket_protocol.c \
		tuning_app/thread_pool.c \
		tuning_app/server/capture_image.c \
		tuning_app/server/isp_handle.c \
		tuning_app/server/mini_shell.c \
		tuning_app/server/server.c \
		tuning_app/server/server_api.c \
		tuning_app/server/server_core.c \
		tuning_app/server/register_opt.c \
		tuning_app/server/raw_flow_opt.c \

#############################################
# venc and rtsp src file
TARGET_SRC += \
		libvi_venc_rtsp/vi_venc_rtsp.cpp

TARGET_SHARED_LIB := \
    librt \
    libpthread \
    liblog \
    libion \
    libhwdisplay \
    libasound \
    libISP \
    libMemAdapter \
    libcedarxstream \
    libvdecoder \
    libvencoder \
    libcedarx_aencoder \
    libadecoder \
    libnormal_audio \
    libcdx_parser \
    lib_ise_bi \
    lib_ise_mo \
    lib_ise_sti \
    libcutils \
    libcdx_common \
    libcdx_base \
    libsample_confparser \
    libmedia_mpp \
    libmedia_utils \
    libmpp_vi \
    libmpp_isp \
    libisp_ini \
    libmpp_component \
    libTinyServer \

TARGET_STATIC_LIB := \
    libmuxers \
    libmp4_muxer \
    libffavutil \
    libFsWriter \
    libmp3_muxer \
    libaac_muxer \
    libraw_muxer \
    libmpeg2ts_muxer \
    libcedarxdemuxer \

TARGET_CPPFLAGS += -fPIC -Wall -Wno-unused-but-set-variable \
				   -DSUPPORT_VENC_RTSP
TARGET_CFLAGS += -fPIC -Wall -Wno-unused-but-set-variable \
				   -DSUPPORT_VENC_RTSP

############################################

TARGET_INC := $(LOCAL_INC)

TARGET_CFLAGS += -O2 -Wall
TARGET_CPPFLAGS += -O2 -Wall
TARGET_LDFLAGS += $(LOCAL_LDFLAGS)

TARGET_MODULE := awTuningApp
include $(BUILD_BIN)

#########################################
