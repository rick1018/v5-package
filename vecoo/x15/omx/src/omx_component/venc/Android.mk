LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	IM_OMX_Venc.c \
	IM_OMX_H264Enc.c \
	library_register.c


LOCAL_MODULE := libOMX.Infotm.Video.Encoder
LOCAL_ARM_MODE := arm
LOCAL_MODULE_TAGS := optional
LOCAL_STATIC_LIBRARIES := libim_basecomponent libim_osal
LOCAL_SHARED_LIBRARIES := \
			libc \
			libdl \
			libcutils \
			libutils  \
			libui \
			libhardware \
			lib8270enc \
			libInfotmMediaDevmmu \
			libInfotmMediaBuffalloc

LOCAL_CFLAGS += -DTARGET_SYSTEM=FS_ANDROID -DTS_VER_MAJOR=4 -DTS_VER_MINOR=3 -DTS_VER_PATCH=0

LOCAL_CFLAGS += -DIM_DEBUG

ifeq ($(TARGET_BOARD_PLATFORM), imapx900)
LOCAL_CFLAGS += -DVENC_MMU
endif

LOCAL_C_INCLUDES := $(IM_OMX_INC)/khronos \
	$(IM_OMX_INC)/infotm \
	$(IM_OMX_TOP)/im_osal/inc \
	$(IM_OMX_TOP)/omx_core \
	$(IM_OMX_COMPONENT)/common \
	$(IM_OMX_COMPONENT)/venc \
	device/infotm/$(TARGET_BOARD_PLATFORM)/InfotmMedia/include \
	device/infotm/$(TARGET_BOARD_PLATFORM)/InfotmMedia/venc_lib_8270/inc/ 

include $(BUILD_SHARED_LIBRARY)
