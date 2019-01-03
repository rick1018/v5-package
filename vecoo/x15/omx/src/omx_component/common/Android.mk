LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	IM_OMX_Basecomponent.c \
	IM_OMX_Baseport.c \
	IM_OMX_Resourcemanager.c


LOCAL_MODULE := libim_basecomponent

LOCAL_CFLAGS :=

LOCAL_STATIC_LIBRARIES := libim_osal
LOCAL_SHARED_LIBRARIES := libcutils libutils

LOCAL_C_INCLUDES := $(IM_OMX_INC)/khronos \
	$(IM_OMX_INC)/infotm \
	$(IM_OMX_TOP)/im_osal/inc

include $(BUILD_STATIC_LIBRARY)

