TARGET          = libisp_ini.so
INCLUDES        += -I. \
                   -I$(STAGING_DIR)/usr/include \
                   -I$(STAGING_DIR)/usr/include/utils \

LIBS			:= -llog
 
SRCS := \
	isp_ini_parse.c

$(TARGET) : $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(INCLUDES) $(LDFLAGS) -shared -fPIC

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/lib/
	cp $(TARGET) $(CONFIG_PREFIX)/usr/lib/

clean:
	rm -rf $(TARGET) *.o *~
