LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

include $(LOCAL_PATH)/../env.mk
include $(LOCAL_PATH)/../config.mk

ifeq ($(IM_SUPPORT_vdec_lib_g1), true)
LOCAL_CFLAGS += -DVDEC_LIB_G1
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include	\
    $(LOCAL_PATH)/vp6lib/include	\
    $(LOCAL_PLATFORM_PATH)/InfotmMedia/vdec_lib_g1/source/inc \
    $(LOCAL_PLATFORM_PATH)/InfotmMedia/vdec_lib_g1/source/h264high
else
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include	\
    $(LOCAL_PATH)/vp6lib/include	\
    $(LOCAL_PLATFORM_PATH)/9170_4_6/source/inc \
    $(LOCAL_PLATFORM_PATH)/9170_4_6/source/h264high
endif

ifeq ($(IM_SUPPORT_ffmpeg-0.8), true)
LOCAL_C_INCLUDES +=	\
	$(LOCAL_PATH)/../ffmpeg-0.8	\
	$(LOCAL_PATH)/../ffmpeg-0.8/libavformat	\
	$(LOCAL_PATH)/../ffmpeg-0.8/libavcodec	\
	$(LOCAL_PATH)/../ffmpeg-0.8/libavutils  \
	$(LOCAL_PATH)/../ffmpeg-0.8/libswscale
else
LOCAL_C_INCLUDES +=	\
	$(LOCAL_PATH)/../ffmpeg	\
	$(LOCAL_PATH)/../ffmpeg/libavformat	\
	$(LOCAL_PATH)/../ffmpeg/libavcodec	\
	$(LOCAL_PATH)/../ffmpeg/libavutil  \
	$(LOCAL_PATH)/../ffmpeg/libswscale 
endif

LOCAL_SRC_FILES += \
	vdec.cpp       \
	IM_VideoDec.cpp \
	IM_Mp4HwDec.cpp \
	IM_MultiBuffer.cpp \
	IM_H264HwDec.cpp \
	IM_Mpeg2HwDec.cpp \
	IM_RvHwDec.cpp \
	IM_VC1HwDec.cpp \
	IM_MjpegHwDec.cpp \
	IM_SwFFmpegDec.cpp \

ifeq ($(IM_SUPPORT_vdec_lib_g1), true)
LOCAL_SRC_FILES += \
	IM_AvsHwDec.cpp \
	IM_Vp6HwDec.cpp \
	IM_Vp8HwDec.cpp 
endif
LOCAL_CFLAGS += -DIM_DEBUG -D__STDC_CONSTANT_MACROS -DIM_VDEC_DBG_LEVEL=2 -DMOSAIC_PIC_OUT
				
LOCAL_STATIC_LIBRARIES :=	\
	InfotmMediaFoundations  

ifeq ($(IM_SUPPORT_vdec_lib_g1), true)
LOCAL_SHARED_LIBRARIES := \
	libg1dec_mpeg2  \
	libg1dec_mpeg4   \
	libg1dec_h264   \
	libg1dec_rv   \
	libg1dec_vc1  \
	libg1dec_vp6  \
	libg1dec_vp8  \
	libg1dec_avs  \
	libg1dec_pp   \
	libg1dec_dwl   \
	libg1dec_jpeg  
else
LOCAL_SHARED_LIBRARIES := \
	libdecx170m2  \
	libdecx170h   \
	libdecx170m   \
	libdecx170p   \
	libdecx170rv  \
	libdecx170v   \
	libdwlx170    \
	libx170j      
endif

LOCAL_SHARED_LIBRARIES += \
	libcutils	\
	libim_avcodec	\
	libim_avutil	\
	libim_avformat	\
	libim_swscale	\
	libInfotmMediaBuffalloc	\
	libInfotmMediaDevmmu

   
# do not prelink
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := libInfotmMediaVideoDec2
include $(BUILD_SHARED_LIBRARY)

