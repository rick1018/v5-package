#
# 1. Set the path and clear environment
# 	TARGET_PATH := $(call my-dir)
# 	include $(ENV_CLEAR)
#
# 2. Set the source files and headers files
#	TARGET_SRC := xxx_1.c xxx_2.c
#	TARGET_INc := xxx_1.h xxx_2.h
#
# 3. Set the output target
#	TARGET_MODULE := xxx
#
# 4. Include the main makefile
#	include $(BUILD_BIN)
#
# Before include the build makefile, you can set the compilaion
# flags, e.g. TARGET_ASFLAGS TARGET_CFLAGS TARGET_CPPFLAGS
#

TARGET_PATH :=$(call my-dir)

#########################################
include $(ENV_CLEAR)
include $(TARGET_TOP)/middleware/config/mpp_config.mk

ifeq ($(MPPCFG_USE_KFC), Y)

TARGET_MODULE := libkfc

TARGET_PREBUILT_LIBS := \
    $(TARGET_PATH)/libip_loader_soft.so \
    $(TARGET_PATH)/libkfc_server_enc.so \
    $(TARGET_PATH)/libkfcapi_enc.so \
    $(TARGET_PATH)/lib_hal.a

include $(BUILD_MULTI_PREBUILT)

$(call copy-one-file, \
     $(TARGET_PATH)/iploader_server, \
     $(TARGET_OUT)/target/usr/bin/iploader_server \
)

$(call copy-one-file, \
     $(TARGET_PATH)/kfc_server_pack, \
     $(TARGET_OUT)/target/usr/bin/kfc_server_pack \
)

TARGET_CLEAN += $(TARGET_OUT)/target/usr/bin/iploader_server
TARGET_CLEAN += $(TARGET_OUT)/target/usr/bin/kfc_server_pack

endif

