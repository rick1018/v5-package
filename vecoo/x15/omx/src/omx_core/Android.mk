LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := IM_OMX_Component_Register.c \
	IM_OMX_Core.c


LOCAL_MODULE := libIM_OMX_Core

LOCAL_CFLAGS :=

LOCAL_ARM_MODE := arm

LOCAL_STATIC_LIBRARIES := libim_osal libim_basecomponent
LOCAL_SHARED_LIBRARIES := libc libdl libcutils libutils 

LOCAL_C_INCLUDES := $(IM_OMX_INC)/khronos \
	$(IM_OMX_INC)/infotm \
	$(IM_OMX_TOP)/im_osal/inc \
	$(IM_OMX_TOP)/omx_component/common

include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)

#TARGET_REGISTRY_DIR := $(TARGET_OUT)/etc

#copy_from := \
	Infotmomxregistry \

#copy_to   := $(addprefix $(TARGET_REGISTRY_DIR)/,$(copy_from))
#copy_from := $(addprefix $(LOCAL_PATH)/,$(copy_from))

#$(copy_to) : $(TARGET_REGISTRY_DIR)/% : $(LOCAL_PATH)/% | $(ACP)
#	$(transform-prebuilt-to-target)

#ALL_PREBUILT += $(copy_to)

