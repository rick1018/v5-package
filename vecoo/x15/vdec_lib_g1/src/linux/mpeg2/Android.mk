#
# Android.mk for mpeg2 library of g1 decode.
#
# Author:ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE	= false

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= libg1dec_mpeg2

LOCAL_SRC_FILES		= ../../source/mpeg2/mpeg2decapi.c \
			  ../../source/mpeg2/mpeg2decapi_internal.c \
			  ../../source/mpeg2/mpeg2hwd_strm.c \
			  ../../source/mpeg2/mpeg2hwd_headers.c \
			  ../../source/mpeg2/mpeg2hwd_utils.c \
			  ../../source/common/regdrv.c \
			  ../../source/common/refbuffer.c \
			  ../../source/common/workaround.c \
			  ../../source/common/tiledref.c  \
			  ../../source/common/bqueue.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH)	\
			  $(LOCAL_PATH)/../../source/mpeg2 \
			  $(LOCAL_PATH)/../../source/dwl \
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config

LOCAL_SHARED_LIBRARIES	= libg1dec_dwl 

LOCAL_CFLAGS		= -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN

include $(BUILD_SHARED_LIBRARY)
