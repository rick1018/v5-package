LOCAL_CFLAGS		= -DDEC_X170_OUTPUT_FORMAT=0 

LOCAL_CFLAGS += -I. \
				-I./../../source/vc1 \
				-I./../../source/dwl \
				-I./../../source/inc \
				-I./../../source/common \
				-I./../../source/config

SRCS += \
		../../source/vc1/vc1decapi.c \
		../../source/vc1/vc1hwd_asic.c \
		../../source/vc1/vc1hwd_bitplane.c \
		../../source/vc1/vc1hwd_decoder.c \
		../../source/vc1/vc1hwd_picture_layer.c \
		../../source/vc1/vc1hwd_stream.c \
		../../source/vc1/vc1hwd_vlc.c \
		../../source/vc1/vc1hwd_headers.c \
		../../source/common/regdrv.c \
		../../source/common/refbuffer.c \
		../../source/common/tiledref.c  \
		../../source/common/bqueue.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libg1dec_vc1.so

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
