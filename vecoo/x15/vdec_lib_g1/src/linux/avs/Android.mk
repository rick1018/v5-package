# Android.mk for avs library of g1 decode.
#
# Author: ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE	= false

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= libg1dec_avs

LOCAL_SRC_FILES		=  \
			  ../../source/common/tiledref.c \
			  ../../source/avs/avsdecapi_internal.c \
			  ../../source/avs/avsdecapi.c \
			  ../../source/avs/avs_strm.c \
			  ../../source/avs/avs_headers.c \
			  ../../source/avs/avs_utils.c \
			  ../../source/avs/avs_vlc.c \
			  ../../source/common/regdrv.c \
			  ../../source/common/refbuffer.c \
			  ../../source/common/bqueue.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH) \
			  $(LOCAL_PATH)/../../source/avs \
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)/../ldriver/kernel_26x/ \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config

LOCAL_SHARED_LIBRARIES	= libg1dec_dwl
LOCAL_CFALGS = -DNDEBUG
include $(BUILD_SHARED_LIBRARY)

