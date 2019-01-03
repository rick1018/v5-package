#
# Android.mk for vp8 library of g1 decode.
#
# Author: ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE	= false

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= libg1dec_vp8

LOCAL_SRC_FILES		= ../../source/vp8/vp8decapi.c \
			  ../../source/vp8/vp8hwd_bool.c \
			  ../../source/vp8/vp8hwd_probs.c \
			  ../../source/vp8/vp8hwd_headers.c \
			  ../../source/vp8/vp8hwd_decoder.c \
			  ../../source/vp8/vp8hwd_pp_pipeline.c \
			  ../../source/vp8/vp8hwd_asic.c \
			  ../../source/common/regdrv.c \
			  ../../source/common/refbuffer.c \
			  ../../source/common/tiledref.c  \
			  ../../source/common/bqueue.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH) \
			  $(LOCAL_PATH)/../../source/vp8 \
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)/../memalloc \
			  $(LOCAL_PATH)/../ldriver/kernel_26x/ \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config

LOCAL_SHARED_LIBRARIES	= libg1dec_dwl

LOCAL_CFLAGS		= -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN \
			  -DNDEBUG 

include $(BUILD_SHARED_LIBRARY)

