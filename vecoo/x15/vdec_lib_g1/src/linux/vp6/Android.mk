#
# Android.mk for vp6 library of g1 decode.
#
# Author: ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE	= false

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= libg1dec_vp6

LOCAL_SRC_FILES		=  \
			  ../../source/vp6/vp6booldec.c \
			  ../../source/vp6/vp6dec.c \
			  ../../source/vp6/vp6huffdec.c \
			  ../../source/vp6/vp6hwd_api.c \
			  ../../source/vp6/vp6hwd_asic.c \
			  ../../source/vp6/vp6strmbuffer.c \
			  ../../source/vp6/vp6decodemode.c \
			  ../../source/vp6/vp6decodemv.c \
			  ../../source/vp6/vp6scanorder.c \
			  ../../source/vp6/vp6gconst.c \
			  ../../source/vp6/vp6_pp_pipeline.c \
			  ../../source/common/regdrv.c \
			  ../../source/common/refbuffer.c \
			  ../../source/common/tiledref.c \
			  ../../source/common/bqueue.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH) \
			  $(LOCAL_PATH)/../../source/vp6 \
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)/../memalloc \
			  $(LOCAL_PATH)/../ldriver/kernel_26x/ \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config

LOCAL_SHARED_LIBRARIES	= libg1dec_dwl

LOCAL_CFLAGS		= -DDEC_X170_OUTPUT_FORMAT=0 \
			  -DNDEBUG  -DDEC_X170_TIMEOUT_LENGTH=-1


include $(BUILD_SHARED_LIBRARY)

