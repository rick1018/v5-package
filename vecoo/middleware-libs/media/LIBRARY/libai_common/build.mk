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
include $(ENV_CLEAR)

include $(TARGET_TOP)/middleware/config/mpp_config.mk

BUILD_MPP_CVE := N

ifeq ($(MPPCFG_MOD),Y)
BUILD_MPP_CVE := Y
endif

ifeq ($(MPPCFG_VLPR),Y)
BUILD_MPP_CVE := Y
endif

ifeq ($(MPPCFG_BDII),Y)
BUILD_MPP_CVE := Y
endif

ifeq ($(MPPCFG_HCNT),Y)
BUILD_MPP_CVE := Y
endif


ifeq ($(BUILD_MPP_CVE),Y)

TARGET_MODULE :=
TARGET_PREBUILT_LIBS := $(wildcard $(TARGET_PATH)/*.so)
include $(BUILD_MULTI_PREBUILT)

endif

