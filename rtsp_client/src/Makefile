TARGET          = rtsp_client
INCLUDES        += -I. \
                   -I$(STAGING_DIR)/usr/include \
                   -I$(STAGING_DIR)/usr/include/utils \
                   -I$(STAGING_DIR)/usr/include/device \
                   -I$(STAGING_DIR)/usr/include/media \
                   -I$(STAGING_DIR)/usr/include/vo
LIBS1			:= -lmpp_comm \
			   -lmedia_mpp \
			   -lmedia_utils \
			   -lmpp_component \
			   -ldl -lpthread  -lrt -lavformat -lavcodec -lavutil \
			   -L$(STAGING_DIR)/rootfs/usr/lib \
		           -lstreamdrv -lfilemsgdrv -lmsgutil -lavdatadrv -lsemutil -lsharemem
				   
SRCS := rtsp_client.c

OBJS    = $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CXXFLAGS) $(INCLUDES) -fPIC -shared -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS1) $(LD_FLAG) -o $@

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/bin
	cp $(TARGET) $(CONFIG_PREFIX)/usr/bin

clean:
	rm -rf $(TARGET) *.o *~
