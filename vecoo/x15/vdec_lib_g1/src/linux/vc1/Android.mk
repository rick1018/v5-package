#
# Android.mk for vc1 library of g1 decode.
#
# Author: ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE	= false

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= libg1dec_vc1

LOCAL_SRC_FILES		= ../../source/vc1/vc1decapi.c \
			  ../../source/vc1/vc1hwd_asic.c \
			  ../../source/vc1/vc1hwd_bitplane.c \
			  ../../source/vc1/vc1hwd_decoder.c \
			  ../../source/vc1/vc1hwd_picture_layer.c \
			  ../../source/vc1/vc1hwd_stream.c \
			  ../../source/vc1/vc1hwd_vlc.c \
			  ../../source/vc1/vc1hwd_headers.c \
			  ../../source/common/regdrv.c \
			  ../../source/common/refbuffer.c \
			  ../../source/common/tiledref.c  \
			  ../../source/common/bqueue.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH) \
			  $(LOCAL_PATH)/../../source/vc1 \
			  $(LOCAL_PATH)/../../source/dwl \
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config

LOCAL_CFLAGS		= -DDEC_X170_OUTPUT_FORMAT=0 

LOCAL_SHARED_LIBRARIES	= libg1dec_dwl

include $(BUILD_SHARED_LIBRARY)
