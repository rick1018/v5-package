#
# Android.mk for rv library of g1 decode.
#
# Author:ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE	= false

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= libg1dec_rv

LOCAL_SRC_FILES		= ../../source/rv/rvdecapi.c \
			  ../../source/rv/rvdecapi_internal.c \
			  ../../source/rv/rv_strm.c \
			  ../../source/rv/rv_headers.c \
			  ../../source/rv/rv_utils.c \
			  ../../source/rv/rv_rpr.c \
			  ../../source/rv/on2rvdecapi.c \
			  ../../source/common/regdrv.c \
			  ../../source/common/refbuffer.c \
			  ../../source/common/tiledref.c  \
			  ../../source/common/bqueue.c \
			  ../../source/common/workaround.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH)	\
			  $(LOCAL_PATH)/../../source/rv	\
			  $(LOCAL_PATH)/../../source/dwl \
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config

LOCAL_CFLAGS		= -DRV_RAW_STREAM_SUPPORT \
			  -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN 

LOCAL_SHARED_LIBRARIES	= libg1dec_dwl

include $(BUILD_SHARED_LIBRARY)
