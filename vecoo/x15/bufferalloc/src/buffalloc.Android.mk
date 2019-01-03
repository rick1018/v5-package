# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


LOCAL_PATH := $(call my-dir)

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_MODULE_TAGS:= optional

include $(LOCAL_PATH)/../env.mk

LOCAL_CFLAGS += -DBUFFALLOC_EXPORTS

LOCAL_SHARED_LIBRARIES := \
	libcutils	\
	liblog  \
	libInfotmMediaIon

LOCAL_STATIC_LIBRARIES :=\
	InfotmMediaFoundations_c

LOCAL_SRC_FILES :=	\
	buffalloc_c.c   \
	alc_dwl_android_ion.c

LOCAL_C_INCLUDES := 	\
	$(LOCAL_PATH)/../include	\
	$(LOCAL_PATH)/../buffalloc  \
    $(LOCAL_PATH)/../external/filesystem/android_4_4/project/ion_lib/include	\
	$(LOCAL_PATH)/../dmmu_lib/include

LOCAL_CFLAGS += -DIM_DEBUG

LOCAL_MODULE := libInfotmMediaBuffalloc
include $(BUILD_SHARED_LIBRARY)

#
# build alc_test
#
#include $(LOCAL_PATH)/test/Android.mk

