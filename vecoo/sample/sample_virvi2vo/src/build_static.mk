TARGET_PATH :=$(call my-dir)
TOP_PATH := $(TARGET_PATH)

#########################################
include $(ENV_CLEAR)

TARGET_SRC := \
    sample_virvi2vo.c

TARGET_INC := \
    $(TARGET_TOP)/system/public/include \
    $(TARGET_TOP)/middleware/include \
    $(TARGET_TOP)/middleware/include/utils \
    $(TARGET_TOP)/middleware/include/media \
    $(TARGET_TOP)/middleware/media/include \
    $(TARGET_TOP)/middleware/media/include/utils \
    $(TARGET_TOP)/middleware/media/include/component \
    $(TARGET_TOP)/middleware/media/LIBRARY/libisp/include \
    $(TARGET_TOP)/middleware/media/LIBRARY/libisp/include/V4l2Camera \
    $(TARGET_TOP)/middleware/media/LIBRARY/libisp/isp_tuning \
    $(TARGET_TOP)/middleware/media/LIBRARY/libAIE_Vda/include \
    $(TARGET_TOP)/middleware/media/LIBRARY/include_stream \
    $(TARGET_TOP)/middleware/media/LIBRARY/include_FsWriter \
    $(TARGET_TOP)/middleware/media/LIBRARY/libcedarc/include \
    $(TARGET_TOP)/middleware/media/LIBRARY/libcedarx/libcore/common/iniparser \
    $(TARGET_TOP)/middleware/sample/configfileparser \
    # $(TARGET_TOP)/middleware/media/LIBRARY/libisp/isp_tuning \
    # $(TARGET_TOP)/middleware/media/LIBRARY/libcedarc/include \

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

TARGET_STATIC_LIB := \
    libsample_confparser \
    libaw_mpp \
    libmedia_utils \
    libmuxers \
    libmp4_muxer \
    libffavutil \
    libFsWriter \
    libmp3_muxer \
    libaac_muxer \
    libraw_muxer \
    libmpeg2ts_muxer \
    libcedarxdemuxer \

TARGET_CFLAGS += -fPIC -Wall -Wno-unused-but-set-variable -ldl

TARGET_MODULE := sample_virvi2vo

include $(BUILD_BIN)
#########################################
