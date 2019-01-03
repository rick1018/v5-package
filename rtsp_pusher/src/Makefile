TARGET          = tcp_server
INCLUDES        += -I. \
                   -I./include \
                   -I$(STAGING_DIR)/usr/include \
                   -I$(STAGING_DIR)/usr/include/utils \
                   -I$(STAGING_DIR)/usr/include/device \
                   -I$(STAGING_DIR)/usr/include/media \
                   -I$(STAGING_DIR)/usr/include/vo
LIBS			:= -lmpp_comm \
			   -lmedia_mpp \
			   -lmedia_utils \
			   -lmpp_component
				   
SRCS := sample_vi_venc_server.cpp 

OBJS    = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -std=c++11 -c -o $@ $<

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(LIBS) $(LD_FLAG) -o $@

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/bin
	cp $(TARGET) $(CONFIG_PREFIX)/usr/bin

clean:
	rm -rf $(TARGET) *.o *~
