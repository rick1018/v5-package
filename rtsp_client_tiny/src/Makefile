TARGET          = rtsp_client_tiny
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
			   -lTinyServer \
			   -ldl -lpthread  -lrt -lavformat -lavcodec -lavutil
				   
SRCS := rtsp_client.cpp

OBJS    = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -fPIC -shared -c -std=c++11 -o $@ $<

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(LIBS1) $(LD_FLAG) -o $@

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/bin
	cp $(TARGET) $(CONFIG_PREFIX)/usr/bin

clean:
	rm -rf $(TARGET) *.o *~
