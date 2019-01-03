#
# Android.mk for pp library of g1 decode.
#
# Author:ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE	= false

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= libg1dec_pp

LOCAL_SRC_FILES		= ../../source/pp/ppapi.c \
			  ../../source/pp/ppinternal.c \
			  ../../source/common/regdrv.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH)	\
			  $(LOCAL_PATH)/../../source/mpeg2 \
			  $(LOCAL_PATH)/../../source/mpeg4 \
			  $(LOCAL_PATH)/../../source/jpeg \
			  $(LOCAL_PATH)/../../source/vc1 \
			  $(LOCAL_PATH)/../../source/vp6 \
			  $(LOCAL_PATH)/../../source/vp8 \
			  $(LOCAL_PATH)/../../source/avs \
			  $(LOCAL_PATH)/../../source/pp	\
			  $(LOCAL_PATH)/../../source/h264high \
			  $(LOCAL_PATH)/../../source/rv	\
			  $(LOCAL_PATH)/../../source/dwl \
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config

LOCAL_SHARED_LIBRARIES	= libg1dec_dwl \
			  libg1dec_h264 \
			  libg1dec_mpeg4 \
			  libg1dec_mpeg2 \
			  libg1dec_jpeg \
			  libg1dec_rv \
			  libg1dec_vc1 \
			  libg1dec_vp6 \
			  libg1dec_vp8 \
			  libg1dec_avs \

#LOCAL_CFLAGS		= -DPP_H264DEC_PIPELINE_SUPPORT \
			  -DPP_MPEG4DEC_PIPELINE_SUPPORT \
			  -DPP_JPEGDEC_PIPELINE_SUPPORT \
			  -DPP_VC1DEC_PIPELINE_SUPPORT \
			  -DPP_MPEG2DEC_PIPELINE_SUPPORT \
			  -DPP_RVDEC_PIPELINE_SUPPORT \
			  -DPP_AVSDEC_PIPELINE_SUPPORT \
			  -DPP_VP6DEC_PIPELINE_SUPPORT \
			  -DPP_VP8DEC_PIPELINE_SUPPORT

include $(BUILD_SHARED_LIBRARY)
