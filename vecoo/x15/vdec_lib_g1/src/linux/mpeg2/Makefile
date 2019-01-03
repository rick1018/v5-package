LOCAL_CFLAGS		= -DDEBUG \
			  -D_ASSERT_USED \
			  -D_RANGE_CHECK \
			  -D_ERROR_PRINT \
			  -DDEC_MODULE_PATH=\"/dev/imapx800-vdec\" 
LOCAL_CFLAGS		+= -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN

LOCAL_CFLAGS += -I. \
				-I./../../source/mpeg2 \
				-I./../../source/dwl \
				-I./../../source/inc \
				-I./../../source/common \
				-I./../../source/config

SRCS += \
		../../source/mpeg2/mpeg2decapi.c \
		../../source/mpeg2/mpeg2decapi_internal.c \
		../../source/mpeg2/mpeg2hwd_strm.c \
		../../source/mpeg2/mpeg2hwd_headers.c \
		../../source/mpeg2/mpeg2hwd_utils.c \
		../../source/common/regdrv.c \
		../../source/common/refbuffer.c \
		../../source/common/workaround.c \
		../../source/common/tiledref.c  \
		../../source/common/bqueue.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libg1dec_mpeg2.so

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
