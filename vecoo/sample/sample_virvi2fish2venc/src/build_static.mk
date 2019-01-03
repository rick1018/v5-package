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
# start gcc sample_virvi2fish2venc.c
#########################################
include $(ENV_CLEAR)
TARGET_SRC := sample_virvi2fish2venc.c 
				
TARGET_INC := \
		$(TARGET_PATH)/include \
		$(TARGET_PATH)/../include/utils \
		$(TARGET_PATH)/../include \
		$(TARGET_TOP)/middleware/media \
		$(TARGET_TOP)/middleware/media/LIBRARY/libisp/include/V4l2Camera \
		$(TARGET_TOP)/middleware/media/LIBRARY \
		$(TARGET_TOP)/middleware/media/LIBRARY/libisp/include \
		$(TARGET_TOP)/middleware/media/LIBRARY/libisp/isp_tuning \
		$(TARGET_TOP)/middleware/media/LIBRARY/libISE \
		$(TARGET_TOP)/middleware/media/LIBRARY/include_FsWriter \
		$(TARGET_TOP)/middleware/media/LIBRARY/include_stream \
		$(TARGET_TOP)/middleware/media/LIBRARY/libcedarc \
		$(TARGET_TOP)/middleware/media/LIBRARY/libcedarc/include \
		$(TARGET_TOP)/middleware/media/LIBRARY/audioEncLib/include \
		$(TARGET_TOP)/middleware/media/LIBRARY/audioDecLib/include \
		$(TARGET_TOP)/middleware/media/include \
		$(TARGET_TOP)/middleware/media/include/utils \
		$(TARGET_TOP)/middleware/media/include/component \
		$(TARGET_TOP)/middleware/include/media \
		$(TARGET_TOP)/middleware/include \
		$(TARGET_TOP)/middleware/include/utils \
		$(TARGET_TOP)/middleware/media/LIBRARY/libcedarx/libcore/common/iniparser \
		$(TARGET_TOP)/middleware/sample/configfileparser \

TARGET_SHARED_LIB := \
    librt \
    libpthread \
    liblog \
    libasound \
    libcdx_parser \
    libcdx_common \
    libcdx_base \
	libsample_confparser

TARGET_STATIC_LIB := \
	libaw_mpp \
    lib_hal \
    libMemAdapter \
    libmedia_utils \
    libhwdisplay \
    libvdecoder \
    libvencoder \
    libadecoder \
	libcdc_base \
	libvideoengine \
	libcedarx_aencoder \
	libaacenc \
	libadpcmenc \
	libg711enc \
	libg726enc \
	libmp3enc \
	libawh264 \
	libawh265 \
	libawmjpegplus \
	libVE \
    libion \
    libISP \
    libisp_ae \
    libisp_af \
    libisp_afs \
    libisp_awb \
    libisp_base \
    libisp_gtm \
    libisp_iso \
    libisp_math \
    libisp_md \
    libisp_pltm \
    libisp_rolloff \
    libmatrix \
    libiniparser \
    libisp_ini \
    libisp_dev \
    libmuxers \
    libmp4_muxer \
    libffavutil \
    libFsWriter \
    libmp3_muxer \
    libaac_muxer \
    libraw_muxer \
    libcutils \
    libmpeg2ts_muxer \
    libcedarxdemuxer \
    libcedarxstream \
    lib_ise_mo \


TARGET_CFLAGS += -fPIC -Wall -Wno-unused-but-set-variable -ldl

#TARGET_LDFLAGS += -static

TARGET_MODULE := sample_virvi2fish2venc

include $(BUILD_BIN)
