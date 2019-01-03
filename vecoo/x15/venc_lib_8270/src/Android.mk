# 
# Android.mk for 82700 Encode library.
#
# author: infotm Mid
#

LOCAL_PATH := $(call my-dir)

# Main library
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE	= false

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= lib8270enc


SRC_ENC_COMMON		= source/common
SRC_H264		= source/h264
SRC_JPEG		= source/jpeg
SRC_CAM			= source/camstab
SRC_EWL			= linux_reference/ewl

LOCAL_SRC_FILES		= $(SRC_ENC_COMMON)/encasiccontroller_v2.c \
			  $(SRC_ENC_COMMON)/encasiccontroller.c \
			  $(SRC_ENC_COMMON)/encpreprocess.c \
			  $(SRC_H264)/H264CodeFrame.c \
			  $(SRC_H264)/H264Init.c \
			  $(SRC_H264)/H264NalUnit.c \
			  $(SRC_H264)/H264PictureParameterSet.c \
			  $(SRC_H264)/H264PutBits.c \
			  $(SRC_H264)/H264RateControl.c \
			  $(SRC_H264)/H264SequenceParameterSet.c \
			  $(SRC_H264)/H264Slice.c \
			  $(SRC_H264)/H264EncApi.c \
			  $(SRC_H264)/H264Sei.c \
			  $(SRC_H264)/H264Cabac.c \
			  $(SRC_H264)/h264encapi_ext.c \
              $(SRC_H264)/H264Mad.c \
              $(SRC_H264)/H264TestId.c \
			  $(SRC_JPEG)/EncJpeg.c \
			  $(SRC_JPEG)/EncJpegInit.c \
			  $(SRC_JPEG)/EncJpegCodeFrame.c \
			  $(SRC_JPEG)/EncJpegPutBits.c \
			  $(SRC_JPEG)/JpegEncApi.c \
			  $(SRC_CAM)/vidstabcommon.c \
			  $(SRC_CAM)/vidstabalg.c \
			  $(SRC_CAM)/vidstabapi.c \
			  $(SRC_CAM)/vidstabinternal.c \
			  $(SRC_EWL)/ewl_x280_common.c \
			  $(SRC_EWL)/ewl_x280_irq.c \

LOCAL_C_INCLUDES 	= $(LOCAL_PATH) \
			  $(LOCAL_PATH)/linux_reference/ewl \
			  $(LOCAL_PATH)/linux_reference/debug_trace \
			  $(LOCAL_PATH)/linux_reference/kernel_module \
			  $(LOCAL_PATH)/linux_reference/memalloc \
			  $(LOCAL_PATH)/source/h264 \
			  $(LOCAL_PATH)/source/jpeg \
			  $(LOCAL_PATH)/source/common \
			  $(LOCAL_PATH)/source/camstab \
			  $(LOCAL_PATH)/inc  \
			  $(LOCAL_PATH)/../include	\

LOCAL_SHARED_LIBRARIES := \
			libcutils \
			libutils \
			libInfotmMediaBuffalloc	\
			libInfotmMediaDevmmu \
	
LOCAL_CFLAGS 		= -DNDEBUG \
			  -DEWL_NO_HW_TIMEOUT \
			  -DMPEG4_HW_VLC_MODE_ENABLED \
			  -DMPEG4_HW_RLC_MODE_ENABLED \
			  -DREMOVE_HANTRO_USER_DATA \
			  -DSINGLE_THREAD_PROGRAM  \
			  -DEWL_FILE	

#LOCAL_CFLAGS    += -DIM_DEBUG

include $(LOCAL_PATH)/../env.mk

include $(BUILD_SHARED_LIBRARY)
