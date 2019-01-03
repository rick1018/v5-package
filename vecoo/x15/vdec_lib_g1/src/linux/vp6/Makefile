LOCAL_CFLAGS		= -DDEC_X170_OUTPUT_FORMAT=0 \
			  -DNDEBUG  -DDEC_X170_TIMEOUT_LENGTH=-1

LOCAL_CFLAGS += -I. \
			  -I./../../source/vp6 \
			  -I./../../source/inc \
			  -I./../memalloc \
			  -I./../ldriver/kernel_26x/ \
			  -I./../../source/common \
			  -I./../../source/config

SRCS += \
		../../source/vp6/vp6booldec.c \
		../../source/vp6/vp6dec.c \
		../../source/vp6/vp6huffdec.c \
		../../source/vp6/vp6hwd_api.c \
		../../source/vp6/vp6hwd_asic.c \
		../../source/vp6/vp6strmbuffer.c \
		../../source/vp6/vp6decodemode.c \
		../../source/vp6/vp6decodemv.c \
		../../source/vp6/vp6scanorder.c \
		../../source/vp6/vp6gconst.c \
		../../source/vp6/vp6_pp_pipeline.c \
		../../source/common/regdrv.c \
		../../source/common/refbuffer.c \
		../../source/common/tiledref.c \
		../../source/common/bqueue.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libg1dec_vp6.so

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
