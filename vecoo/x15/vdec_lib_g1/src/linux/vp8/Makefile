LOCAL_CFLAGS		= -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN \
			  -DNDEBUG 

LOCAL_CFLAGS += -I. \
			  -I./../../source/vp8 \
			  -I./../../source/inc \
			  -I./../memalloc \
			  -I./../ldriver/kernel_26x/ \
			  -I./../../source/common \
			  -I./../../source/config

SRCS += \
		../../source/vp8/vp8decapi.c \
		../../source/vp8/vp8hwd_bool.c \
		../../source/vp8/vp8hwd_probs.c \
		../../source/vp8/vp8hwd_headers.c \
		../../source/vp8/vp8hwd_decoder.c \
		../../source/vp8/vp8hwd_pp_pipeline.c \
		../../source/vp8/vp8hwd_asic.c \
		../../source/common/regdrv.c \
		../../source/common/refbuffer.c \
		../../source/common/tiledref.c  \
		../../source/common/bqueue.c

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

.PHONY:all clean $(TARGET) install
