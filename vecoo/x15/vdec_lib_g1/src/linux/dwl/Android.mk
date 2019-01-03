#
# Android.mk for dwl library of vdec g1 decode.
#
# by ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional

include $(LOCAL_PATH)/../../../env.mk
LOCAL_MODULE		= libg1dec_dwl
LOCAL_PRELINK_MODULE	= false

LOCAL_SRC_FILES		= dwl_linux.c \
			  dwl_x170_linux_irq.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH)	\
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)/../ldriver/kernel_26x/ \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config \
			  $(LOCAL_PLATFORM_PATH)/InfotmMedia/include	\
			  $(LOCAL_PLATFORM_PATH)/items

LOCAL_STATIC_LIBRARIES :=\
#	InfotmMediaFoundations_c	

LOCAL_SHARED_LIBRARIES	= \
			libcutils \
			libInfotmMediaBuffalloc	\
			libBoardItems

LOCAL_CFLAGS += -DDEC_MODULE_PATH=\"/dev/vdec\" \
			-DDVFS_SUPPORT	\

include $(BUILD_SHARED_LIBRARY)


