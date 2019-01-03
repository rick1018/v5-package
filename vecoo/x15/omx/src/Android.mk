
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

IM_OMX_TOP := $(LOCAL_PATH)

IM_OMX_INC := $(IM_OMX_TOP)/include/

ifeq ($(TARGET_BOARD_PLATFORM), imapx800)
VDEC_LIB_G1 := true
endif

ifeq ($(TARGET_BOARD_PLATFORM), imapx900)
VDEC_LIB_G1 := true
endif
IM_OMX_COMPONENT := $(IM_OMX_TOP)/omx_component

include $(IM_OMX_TOP)/im_osal/Android.mk
include $(IM_OMX_TOP)/omx_core/Android.mk

include $(IM_OMX_COMPONENT)/common/Android.mk
include $(IM_OMX_COMPONENT)/vdec/Android.mk
include $(IM_OMX_COMPONENT)/venc/Android.mk
include $(IM_OMX_COMPONENT)/adec/Android.mk
