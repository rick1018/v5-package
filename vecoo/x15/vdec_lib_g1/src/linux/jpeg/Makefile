LOCAL_CFLAGS		= -DDEBUG \
			  -D_ASSERT_USED \
			  -D_RANGE_CHECK \
			  -D_ERROR_PRINT \
			  -DDEC_MODULE_PATH=\"/dev/imapx800-vdec\" 

LOCAL_CFLAGS += -I. \
			  -I./../../source/inc \
			  -I./../../source/dwl \
			  -I./../../source/jpeg \
			  -I./../memalloc \
			  -I./../ldriver/kernel_26x \
			  -I./../../source/common \
			  -I./../../source/config

SRCS += \
		../../source/common/regdrv.c \
		../../source/jpeg/jpegdecapi.c \
		../../source/jpeg/jpegdechdrs.c \
		../../source/jpeg/jpegdecinternal.c \
		../../source/jpeg/jpegdecscan.c \
		../../source/jpeg/jpegdecutils.c \
		jpegasicdbgtrace.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libg1dec_jpeg.so

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
