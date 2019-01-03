LOCAL_CFLAGS += -I. \
			  -I./../../source/avs \
			  -I./../../source/inc \
			  -I./../ldriver/kernel_26x/ \
			  -I./../../source/common \
			  -I./../../source/config

LOCAL_CFALGS = -DNDEBUG

SRCS += \
	../../source/common/tiledref.c \
	../../source/avs/avsdecapi_internal.c \
	../../source/avs/avsdecapi.c \
	../../source/avs/avs_strm.c \
	../../source/avs/avs_headers.c \
	../../source/avs/avs_utils.c \
	../../source/avs/avs_vlc.c \
	../../source/common/regdrv.c \
	../../source/common/refbuffer.c \
	../../source/common/bqueue.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libg1dec_avs.so

all:$(TARGET) install

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -o $@ $^ -fPIC -shared

$(OBJS):%.c.o:%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $^ -o $@ -fPIC -shared

clean:
	-rm -rf $(OBJS) $(TARGET)

install:
	-mkdir -p $(CONFIG_PREFIX)/lib
	-cp $(TARGET) $(CONFIG_PREFIX)/lib/

.PHONY:all clean $(TARGET) install
