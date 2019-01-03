CC ?= g++

TARGET = smartlink_soundwaves
INCLUDES += -I. \
		-I./include \

SRCS := \
	soundwaves_demo.c \

SUB_LIB := \
	-lpthread \
	-lasound

$(TARGET):libADT.a
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(SRCS) -o $@ $^ $(SUB_LIB)

all:$(TARGET) install

clean:
	rm -rf $(TARGET) *.o *.a *~
install:
	mkdir -p $(CONFIG_PREFIX)/usr/bin
	mkdir -p $(CONFIG_PREFIX)/usr/lib/
	cp $(TARGET) $(CONFIG_PREFIX)/usr/bin
	cp libADT.a $(CONFIG_PREFIX)/usr/lib
