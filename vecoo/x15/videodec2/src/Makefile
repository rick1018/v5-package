LOCAL_CFLAGS += \
		-DVDEC_LIB_G1 \
		-I./../include	\
		-I./vp6lib/include	\
		-DIM_DEBUG \
		-D__STDC_CONSTANT_MACROS \
		-DIM_VDEC_DBG_LEVEL=2 \
		-DMOSAIC_PIC_OUT \
		-DTARGET_SYSTEM=FS_LINUX \
		-I$(INCLUDE)/libavformat	\
		-I$(INCLUDE)/libavcodec	\
		-I$(INCLUDE)/libavutil  \
		-I$(INCLUDE)/libswscale 

SRCS += \
	vdec.cpp       \
	IM_VideoDec.cpp \
	IM_Mp4HwDec.cpp \
	IM_MultiBuffer.cpp \
	IM_H264HwDec.cpp \
	IM_Mpeg2HwDec.cpp \
	IM_RvHwDec.cpp \
	IM_VC1HwDec.cpp \
	IM_MjpegHwDec.cpp \
	IM_AvsHwDec.cpp \
	IM_Vp6HwDec.cpp \
	IM_Vp8HwDec.cpp 
	#IM_SwFFmpegDec.cpp \

OBJS = $(patsubst %.cpp, %.c.o, $(SRCS))
TARGET = libInfotmMediaVideoDec2.so

all:$(TARGET) install

$(TARGET):$(OBJS)
	$(CXX) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -o $@ $^ -fPIC -shared

$(OBJS):%.c.o:%.cpp
	$(CXX) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $^ -o $@ -fPIC -shared

clean:
	-rm -rf $(OBJS) $(TARGET)

install:
	-mkdir -p $(CONFIG_PREFIX)/lib
	-mkdir -p $(CONFIG_PREFIX)/include
	-cp $(TARGET) $(CONFIG_PREFIX)/lib/
	-cp *.h $(CONFIG_PREFIX)/include/

.PHONY:all clean $(TARGET) install
