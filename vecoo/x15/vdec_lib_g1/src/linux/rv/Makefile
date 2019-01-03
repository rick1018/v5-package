LOCAL_CFLAGS		= -DRV_RAW_STREAM_SUPPORT \
			  -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN 

LOCAL_CFLAGS += -I. \
				-I./../../source/rv	\
				-I./../../source/dwl \
				-I./../../source/inc \
				-I./../../source/common \
				-I./../../source/config

SRCS += \
		../../source/rv/rvdecapi.c \
		../../source/rv/rvdecapi_internal.c \
		../../source/rv/rv_strm.c \
		../../source/rv/rv_headers.c \
		../../source/rv/rv_utils.c \
		../../source/rv/rv_rpr.c \
		../../source/rv/on2rvdecapi.c \
		../../source/common/regdrv.c \
		../../source/common/refbuffer.c \
		../../source/common/tiledref.c  \
		../../source/common/bqueue.c \
		../../source/common/workaround.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libg1dec_rv.so

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
