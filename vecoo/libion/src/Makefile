TARGET          = libion.so
INCLUDES        += -Iinclude \
                   -Iinclude/utils \
  
LIBS			:= -llog

SRCS := \
	ion_memmanager.c \
	ion.c

$(TARGET) : $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(INCLUDES) $(LDFLAGS) -shared -fPIC

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/lib/
	mkdir -p $(CONFIG_PREFIX)/usr/include/
	cp $(TARGET) $(CONFIG_PREFIX)/usr/lib/
	cp -rf 	include/utils $(CONFIG_PREFIX)/usr/include/
	cp -rf 	include/ion_memmanager.h $(CONFIG_PREFIX)/usr/include/

clean:
	rm -rf $(TARGET) *.o *~
