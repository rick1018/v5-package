TARGET          = sample_vi2venc
INCLUDES        += -I. \
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
					-lmpp_comm \
					-lstdc++ \
					-lpthread \
					-lTinyServer \
					-l_ise_mo
				   
SRCS := \
	sample_vi2venc.cpp

OBJS    = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -std=gnu++11 -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(INCLUDES) $(OBJS) $(LIBS) -o $@

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/bin
	cp $(TARGET) $(CONFIG_PREFIX)/usr/bin

clean:
	rm -rf $(TARGET) *.o *~
