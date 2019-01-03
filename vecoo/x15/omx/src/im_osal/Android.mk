LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
FIXED_ARCH_TYPE   ?= android

LOCAL_SRC_FILES := \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Event.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Queue.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_ETC.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Mutex.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Thread.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Memory.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Semaphore.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Library.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Trace.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_List.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Buffer.cpp


LOCAL_MODULE := libim_osal
LOCAL_CFLAGS :=	-D_ANDROID_
LOCAL_CFLAGS += -DTARGET_SYSTEM=FS_ANDROID -DTS_VER_MAJOR=4 -DTS_VER_MINOR=3 -DTS_VER_PATCH=0

LOCAL_STATIC_LIBRARIES :=

LOCAL_SHARED_LIBRARIES := libcutils libutils \
                          libui \
                          libhardware \
                          libandroid_runtime \
                          libbinder \
                          libmedia 

ifeq ($(VDEC_LIB_G1), true)
LLOCAL_SHARED_LIBRARIES += libInfotmMediaBuffalloc
LOCAL_CFLAGS +=	-DVDEC_LIB_G1
else
LOCAL_SHARED_LIBRARIES += libmm_dma
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(FIXED_ARCH_TYPE)/ \
	$(IM_OMX_INC)/khronos \
	$(IM_OMX_INC)/infotm \
	$(IM_OMX_TOP)/im_osal/inc \
	$(IM_OMX_COMPONENT)/common \
	device/infotm/$(TARGET_BOARD_PLATFORM)/InfotmMedia/gralloc_mali400/ \
	frameworks/native/include/media/hardware

ifeq ($(VDEC_LIB_G1), true)
LOCAL_C_INCLUDES += device/infotm/$(TARGET_BOARD_PLATFORM)/InfotmMedia/include
LOCAL_C_INCLUDES += device/infotm/$(TARGET_BOARD_PLATFORM)/InfotmMedia/g3d_mali400/src/ump/include
else
LOCAL_C_INCLUDES +=  hardware/infotm/imap210/mm_dma 
endif

include $(BUILD_STATIC_LIBRARY)
