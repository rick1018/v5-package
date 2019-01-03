#
# 1. Set the path and clear environment
# 	TARGET_PATH := $(call my-dir)
# 	include $(ENV_CLEAR)
#
# 2. Set the source files and headers files
#	TARGET_SRC := xxx_1.c xxx_2.c
#	TARGET_INc := xxx_1.h xxx_2.h
#
# 3. Set the output target
#	TARGET_MODULE := xxx
#
# 4. Include the main makefile
#	include $(BUILD_BIN)
#
# Before include the build makefile, you can set the compilaion
# flags, e.g. TARGET_ASFLAGS TARGET_CFLAGS TARGET_CPPFLAGS
#

TARGET_PATH :=$(call my-dir)
TOP_PATH := $(TARGET_PATH)

# ============================================================================
# start gcc sample_vipp2venc.c
#########################################
include $(ENV_CLEAR)
TARGET_SRC := \
	./libvi_venc_rtsp/vi_venc_rtsp.cpp \
	./demo.c

TARGET_INC := \
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
	$(TARGET_PATH)/libvi_venc_rtsp

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
    libmpp_ise \
    libmpp_vo \
    libmpp_component \

TARGET_STATIC_LIB := \
	libTinyServer \
    libmuxers \
    libmp4_muxer \
    libffavutil \
    libFsWriter \
    libmp3_muxer \
    libaac_muxer \
    libraw_muxer \
    libmpeg2ts_muxer \
    libcedarxdemuxer \

TARGET_CPPFLAGS += -fPIC -Wall -Wno-unused-but-set-variable
TARGET_CFLAGS += -fPIC -Wall -Wno-unused-but-set-variable

# TARGET_LDFLAGS += -static

TARGET_MODULE := vi_venc_rtsp_demo

include $(BUILD_BIN)
