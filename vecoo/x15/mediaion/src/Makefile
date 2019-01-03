LOCAL_CFLAGS += -I./include
LOCAL_LDFLAGS =

SRCS += \
		source/ion.c
OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = libInfotmMediaIon.so

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
	-cp include/*.h $(CONFIG_PREFIX)/include/

.PHONY:all clean $(TARGET) install
