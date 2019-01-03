TARGET          = libmpp_comm.so
INCLUDES        += -Icommon \
                   -I$(STAGING_DIR)/usr/include \
                   -I$(STAGING_DIR)/usr/include/utils \
                   -I$(STAGING_DIR)/usr/include/device \
                   -I$(STAGING_DIR)/usr/include/media \
                   -I$(STAGING_DIR)/usr/include/vo
LIBS			:= -llog \
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
					-l_ise_mo
				   
SRCS := \
	common/common.c \
	common/rgb_ctrl.c \
	mpp_comm_audio.c \
	mpp_comm_demux.c \
	mpp_comm_ise.c \
	mpp_comm_mux.c \
	mpp_comm_sys.c \
	mpp_comm_vdec.c \
	mpp_comm_venc.c \
	mpp_comm_vi.c \
	mpp_comm_vo.c

$(TARGET) : $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(INCLUDES) $(LDFLAGS) -shared -fPIC

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/lib/
	mkdir -p $(CONFIG_PREFIX)/usr/include/
	cp $(TARGET) $(CONFIG_PREFIX)/usr/lib/
	cp -rf common/common.h $(CONFIG_PREFIX)/usr/include/
	cp -rf mpp_comm.h $(CONFIG_PREFIX)/usr/include/

clean:
	rm -rf $(TARGET) *.o *~
