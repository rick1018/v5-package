#
# Android.mk for mpeg4 library of g1 decode.
#
# Author: ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE	= false

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= libg1dec_mpeg4

LOCAL_SRC_FILES		= ../../source/mpeg4/mp4dechwd_error_conceal.c \
			  ../../source/mpeg4/mp4decapi.c \
			  ../../source/mpeg4/mp4decapi_internal.c \
			  ../../source/mpeg4/mp4dechwd_rvlc.c \
			  ../../source/mpeg4/mp4dechwd_strmdec.c \
			  ../../source/mpeg4/mp4dechwd_headers.c \
			  ../../source/mpeg4/mp4dechwd_motiontexture.c \
			  ../../source/mpeg4/mp4dechwd_shortvideo.c \
			  ../../source/mpeg4/mp4dechwd_utils.c \
			  ../../source/mpeg4/mp4dechwd_videopacket.c \
			  ../../source/mpeg4/mp4dechwd_vlc.c \
			  ../../source/mpeg4/mp4dechwd_vop.c \
			  ../../source/mpeg4/mp4dechwd_custom.c \
			  ../../source/common/regdrv.c \
			  ../../source/common/refbuffer.c \
			  ../../source/common/workaround.c \
			  ../../source/common/tiledref.c  \
			  ../../source/common/bqueue.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH) \
			  $(LOCAL_PATH)/../../source/mpeg4 \
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)/../memalloc \
			  $(LOCAL_PATH)/../ldriver/kernel_26x/ \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config

LOCAL_SHARED_LIBRARIES	= libg1dec_dwl libutils

LOCAL_CFLAGS		= -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN \
			  -DNDEBUG -D_MP4_RLC_BUFFER_SIZE=384

include $(BUILD_SHARED_LIBRARY)

