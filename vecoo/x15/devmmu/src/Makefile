LOCAL_CFLAGS += -I. -DTARGET_SYSTEM=FS_LINUX -I./pmm_lib/include
LOCAL_LDFLAGS =

	#pmm_lib/source/pmm_lib.c \
	pmm_lib/source/pmm_lib_linux.c \

SRCS += \
	devmmu.c	\
    dmmu_dwl_android.c

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libInfotmMediaDevmmu.so

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
