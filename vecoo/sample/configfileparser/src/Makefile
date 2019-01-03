TARGET          = libsample_confparser.so
INCLUDES        += -I. \
                   -I$(STAGING_DIR)/usr/include \
                   -I$(STAGING_DIR)/usr/include/utils \

LIBS			:= -llog
 
SRCS := \
	confparser.c

$(TARGET) : $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(INCLUDES) $(LDFLAGS) -shared -fPIC

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/lib/
	mkdir -p $(CONFIG_PREFIX)/usr/include/
	cp $(TARGET) $(CONFIG_PREFIX)/usr/lib/
	cp confparser.h $(CONFIG_PREFIX)/usr/include/

clean:
	rm -rf $(TARGET) *.o *~
