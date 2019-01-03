LOCAL_CFLAGS += -I.
LOCAL_LDFLAGS =

SRCS += \
	path.c \
	imgdecode.c \
	main.c
OBJS = $(patsubst %.c, %.c.o, $(SRCS))

TARGET = imgdecode

all:$(TARGET) install

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -o $@ $^

$(OBJS):%.c.o:%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $^ -o $@

clean:
	-rm -rf $(OBJS) $(TARGET)

install:
	-mkdir -p $(CONFIG_PREFIX)/bin
	-cp $(TARGET) $(CONFIG_PREFIX)/bin

.PHONY:all clean $(TARGET) install
