LOCAL_CFLAGS += -I./include -I./source -DTARGET_SYSTEM=FS_LINUX
LOCAL_LDFLAGS =

SRCS += \
	source/dmmu_lib.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libInfotmMediadmmu_lib.so

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
