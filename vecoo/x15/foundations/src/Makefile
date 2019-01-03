LOCAL_CFLAGS += -I. -DTARGET_SYSTEM=FS_LINUX
LOCAL_LDFLAGS =

SRCS += \
	argparse_c.c \
	list_c.c \
	mempool_c.c \
	kvpair_c.c \
	looper_c.c \
	oswl_android_c.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libInfotmMediafoundations.so

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
