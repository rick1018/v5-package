#
# Android.mk for h264 library of g1 decode.
#
# Author:ayakashi
#

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE		= libg1dec_h264
LOCAL_PRELINK_MODULE	= false

LOCAL_SRC_FILES		= ../../source/h264high/h264decapi.c \
			  ../../source/h264high/h264hwd_asic.c \
			  ../../source/h264high/h264hwd_intra_prediction.c \
			  ../../source/h264high/h264hwd_inter_prediction.c \
			  ../../source/h264high/legacy/h264hwd_util.c \
			  ../../source/h264high/legacy/h264hwd_byte_stream.c \
			  ../../source/h264high/legacy/h264hwd_seq_param_set.c \
			  ../../source/h264high/legacy/h264hwd_pic_param_set.c \
			  ../../source/h264high/legacy/h264hwd_slice_header.c \
			  ../../source/h264high/h264hwd_slice_data.c \
			  ../../source/h264high/h264hwd_macroblock_layer.c \
			  ../../source/h264high/legacy/h264hwd_stream.c \
			  ../../source/h264high/legacy/h264hwd_vlc.c \
			  ../../source/h264high/h264hwd_cavlc.c \
			  ../../source/h264high/legacy/h264hwd_nal_unit.c \
			  ../../source/h264high/legacy/h264hwd_neighbour.c \
			  ../../source/h264high/h264hwd_storage.c \
			  ../../source/h264high/legacy/h264hwd_slice_group_map.c \
			  ../../source/h264high/h264hwd_dpb.c \
			  ../../source/h264high/legacy/h264hwd_vui.c \
			  ../../source/h264high/legacy/h264hwd_pic_order_cnt.c \
			  ../../source/h264high/h264hwd_decoder.c \
			  ../../source/h264high/h264hwd_conceal.c \
			  ../../source/h264high/h264_pp_pipeline.c \
			  ../../source/h264high/h264hwd_cabac.c \
			  ../../source/h264high/../common/regdrv.c \
			  ../../source/h264high/../common/refbuffer.c \
			  ../../source/h264high/../common/tiledref.c  \
			  ../../source/h264high/h264decapi_e.c

LOCAL_C_INCLUDES	= $(LOCAL_PATH)	\
			  $(LOCAL_PATH)/../../source/h264high \
			  $(LOCAL_PATH)/../../source/h264high/legacy \
			  $(LOCAL_PATH)/legacy	\
			  $(LOCAL_PATH)/../../source/dwl \
			  $(LOCAL_PATH)/../../source/inc \
			  $(LOCAL_PATH)../memalloc \
			  $(LOCAL_PATH)../ldriver/kernel_26x/ \
			  $(LOCAL_PATH)/../../source/common \
			  $(LOCAL_PATH)/../../source/config

LOCAL_CFLAGS  		= -DDEC_X170_OUTPUT_FORMAT=0 \
			  -DDEC_X170_TIMEOUT_LENGTH=-1 


LOCAL_SHARED_LIBRARIES	= libg1dec_dwl 

include $(BUILD_SHARED_LIBRARY)


