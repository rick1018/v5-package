#
# Android.mk for jpeg library of g1 decode.
#
# Author: ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= libg1dec_jpeg
LOCAL_PRELINK_MODULE	= false

LOCAL_SRC_FILES		= ../../source/common/regdrv.c \
			  ../../source/jpeg/jpegdecapi.c \
			  ../../source/jpeg/jpegdechdrs.c \
			  ../../source/jpeg/jpegdecinternal.c \
			  ../../source/jpeg/jpegdecscan.c \
			  ../../source/jpeg/jpegdecutils.c \
			  jpegasicdbgtrace.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH) \
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)/../../source/dwl \
			  $(LOCAL_PATH)/../../source/jpeg \
			  $(LOCAL_PATH)/../memalloc \
			  $(LOCAL_PATH)/../ldriver/kernel_26x \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config

LOCAL_SHARED_LIBRARIES	= libg1dec_dwl

LOCAL_CFLAGS		= -DDEBUG \
			  -D_ASSERT_USED \
			  -D_RANGE_CHECK \
			  -D_ERROR_PRINT \
			  -DDEC_MODULE_PATH=\"/dev/imapx800-vdec\" 

include $(BUILD_SHARED_LIBRARY)

