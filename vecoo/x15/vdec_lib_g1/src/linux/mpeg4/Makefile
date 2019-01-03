LOCAL_CFLAGS		= -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN \
			  -DNDEBUG -D_MP4_RLC_BUFFER_SIZE=384

LOCAL_CFLAGS += -I. \
				-I./../../source/mpeg4 \
				-I./../../source/inc \
				-I./../memalloc \
				-I./../ldriver/kernel_26x/ \
				-I./../../source/common \
				-I./../../source/config

SRCS += \
		../../source/mpeg4/mp4dechwd_error_conceal.c \
		../../source/mpeg4/mp4decapi.c \
		../../source/mpeg4/mp4decapi_internal.c \
		../../source/mpeg4/mp4dechwd_rvlc.c \
		../../source/mpeg4/mp4dechwd_strmdec.c \
		../../source/mpeg4/mp4dechwd_headers.c \
		../../source/mpeg4/mp4dechwd_motiontexture.c \
		../../source/mpeg4/mp4dechwd_shortvideo.c \
		../../source/mpeg4/mp4dechwd_utils.c \
		../../source/mpeg4/mp4dechwd_videopacket.c \
		../../source/mpeg4/mp4dechwd_vlc.c \
		../../source/mpeg4/mp4dechwd_vop.c \
		../../source/mpeg4/mp4dechwd_custom.c \
		../../source/common/regdrv.c \
		../../source/common/refbuffer.c \
		../../source/common/workaround.c \
		../../source/common/tiledref.c  \
		../../source/common/bqueue.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libg1dec_mpeg4.so

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
