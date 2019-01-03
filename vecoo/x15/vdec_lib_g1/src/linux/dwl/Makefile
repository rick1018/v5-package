LOCAL_CFLAGS += -I. \
				-DTARGET_SYSTEM=FS_LINUX \
				-I./../../source/inc \
				-I./../../source/common \
				-I./../../source/config \
				-I./../../linux/ldriver/kernel_26x/

LOCAL_CFLAGS += -DDEC_MODULE_PATH=\"/dev/vdec\" \
			-DDVFS_SUPPORT	\

SRCS += \
	dwl_linux.c \
	dwl_x170_linux_irq.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libg1dec_dwl.so

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
