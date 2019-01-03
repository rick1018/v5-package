TARGET          = awTuningApp
INCLUDES        += -I. \
                   -Ilibvi_venc_rtsp \
                   -I$(STAGING_DIR)/usr/include \
                   -I$(STAGING_DIR)/usr/include/utils \
                   -I$(STAGING_DIR)/usr/include/device \
                   -I$(STAGING_DIR)/usr/include/media \
                   -I$(STAGING_DIR)/usr/include/vo
LIBS			:= -llog \
					-lpthread \
					-lMemAdapter \
					-lvdecoder \
					-lmedia_utils \
					-lmedia_mpp \
					-lmpp_vi \
					-lmpp_vo \
					-lmpp_isp \
					-lmpp_ise \
					-lmpp_component \
					-lsample_confparser \
					-lISP \
					-lTinyServer
				   
SRCS := \
		tuning_app/awTuningApp.c \
		tuning_app/log_handle.c \
		tuning_app/socket_protocol.c \
		tuning_app/thread_pool.c \
		tuning_app/server/capture_image.c \
		tuning_app/server/isp_handle.c \
		tuning_app/server/mini_shell.c \
		tuning_app/server/server.c \
		tuning_app/server/server_api.c \
		tuning_app/server/server_core.c \
		tuning_app/server/register_opt.c \
		tuning_app/server/raw_flow_opt.c \
		
SRCS_CPP := \
		libvi_venc_rtsp/vi_venc_rtsp.cpp 

OBJS    = $(SRCS:.c=.o)
CPP_OBJS    = $(SRCS_CPP:.cpp=.o)

$(OBJS) : %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -DSUPPORT_VENC_RTSP -fPIC -Wno-error=format-security -c -o $@ $<

$(CPP_OBJS) : %.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -DSUPPORT_VENC_RTSP -fPIC -std=gnu++11 -c -o $@ $<

$(TARGET): $(OBJS) $(CPP_OBJS)
	$(CC) $(LDFLAGS) $(INCLUDES) $(LIBS) -lstdc++ -o $@ $^

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/bin
	cp $(TARGET) $(CONFIG_PREFIX)/usr/bin

clean:
	rm -rf $(TARGET) *.o *~
