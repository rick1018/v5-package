TARGET          = sample_vi2venc_denc2vo_4k
INCLUDES        += -I. \
                   -I$(STAGING_DIR)/usr/include \
                   -I$(STAGING_DIR)/usr/include/utils \
                   -I$(STAGING_DIR)/usr/include/device \
                   -I$(STAGING_DIR)/usr/include/media \
                   -I$(STAGING_DIR)/usr/include/vo
LIBS			:= -lmpp_comm \
		-lmedia_mpp
				   
LD_FLAG   += -lcedarxrender -lmpp_vi -lmpp_isp -lmpp_ise -lmpp_vo -lmpp_component
LD_FLAG   += -lmedia_mpp -lcdx_common -lISP -lMemAdapter -lvencoder -lmedia_utils -lhwdisplay
LD_FLAG   += -l_ise_bi -l_ise_bi_soft -l_ise_mo -l_ise_sti -lion -lcedarxstream
LD_FLAG   += -lcdx_parser -lcdx_stream  -lvdecoder -lnormal_audio
LD_FLAG   += -lcutils -lcedarx_aencoder -lisp_ini  -lVE  -lcdc_base -lcdx_base
LD_FLAG   += -lvideoengine -ladecoder
LD_FLAG   += -L. -lTinyServer

SRCS := sample_vi_venc.cpp 

OBJS    = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -std=c++11 -c -o $@ $<

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(LIBS) $(LD_FLAG) -o $@

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/bin
	cp $(TARGET) $(CONFIG_PREFIX)/usr/bin/

clean:
	rm -rf $(TARGET) *.o *~
