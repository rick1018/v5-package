LOCAL_CFLAGS  = -DDEC_X170_OUTPUT_FORMAT=0 \
				-DDEC_X170_TIMEOUT_LENGTH=-1 

LOCAL_CFLAGS += -I. \
				-I./../../source/h264high \
				-I./../../source/h264high/legacy \
				-I./legacy	\
				-I./../../source/dwl \
				-I./../../source/inc \
				-I./../memalloc \
				-I./../ldriver/kernel_26x/ \
				-I./../../source/common \
				-I./../../source/config

SRCS += \
		../../source/h264high/h264decapi.c \
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

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libg1dec_h264.so

all:$(TARGET) install

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -o $@ $^ -fPIC -shared

$(OBJS):%.c.o:%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $^ -o $@ -fPIC -shared

clean:
	-rm -rf $(OBJS) $(TARGET)

install:
	-mkdir -p $(CONFIG_PREFIX)/lib
	-mkdir -p $(CONFIG_PREFIX)/include
	-cp $(TARGET) $(CONFIG_PREFIX)/lib/
	-cp *.h $(CONFIG_PREFIX)/include/
	-cp ../../source/h264high/*.h $(CONFIG_PREFIX)/include/

.PHONY:all clean $(TARGET) install
