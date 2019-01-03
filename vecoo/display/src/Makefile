TARGET          = libhwdisplay.so
INCLUDES        += -Iinclude


LIBS			:= -llog
 
SRCS := \
	hwdisplay.c

$(TARGET) : $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDES) $(LDFLAGS) -shared -fPIC

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/lib/
	mkdir -p $(CONFIG_PREFIX)/usr/include/
	cp $(TARGET) $(CONFIG_PREFIX)/usr/lib/
	cp -rf 	include/vo $(CONFIG_PREFIX)/usr/include/

clean:
	rm -rf $(TARGET) *.o *~
