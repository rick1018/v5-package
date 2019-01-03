LOCAL_CFLAGS		= -DDEC_X170_OUTPUT_FORMAT=DEC_X170_OUTPUT_FORMAT_RASTER_SCAN \
			  -DNDEBUG -D_MP4_RLC_BUFFER_SIZE=384

LOCAL_CFLAGS += -I. \
			  -I./../../source/mpeg2 \
			  -I./../../source/mpeg4 \
			  -I./../../source/jpeg \
			  -I./../../source/vc1 \
			  -I./../../source/vp6 \
			  -I./../../source/vp8 \
			  -I./../../source/avs \
			  -I./../../source/pp	\
			  -I./../../source/h264high \
			  -I./../../source/rv	\
			  -I./../../source/dwl \
			  -I./../../source/inc \
			  -I./../../source/common \
			  -I./../../source/config


SRCS += \
		../../source/pp/ppapi.c \
		../../source/pp/ppinternal.c \
		../../source/common/regdrv.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libg1dec_pp.so

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
