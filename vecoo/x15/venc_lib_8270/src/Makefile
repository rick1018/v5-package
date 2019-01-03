LOCAL_CFLAGS = -DNDEBUG \
			  -DEWL_NO_HW_TIMEOUT \
			  -DMPEG4_HW_VLC_MODE_ENABLED \
			  -DMPEG4_HW_RLC_MODE_ENABLED \
			  -DREMOVE_HANTRO_USER_DATA \
			  -DSINGLE_THREAD_PROGRAM  \
			  -DEWL_FILE \
			  -DTARGET_SYSTEM=FS_LINUX

LOCAL_CFLAGS += \
			  -I./linux_reference/ewl \
			  -I./linux_reference/debug_trace \
			  -I./linux_reference/kernel_module \
			  -I./linux_reference/memalloc \
			  -I./source/h264 \
			  -I./source/jpeg \
			  -I./source/common \
			  -I./source/camstab \
			  -I./inc  \
			  -I./../include

SRC_ENC_COMMON	= source/common
SRC_H264		= source/h264
SRC_JPEG		= source/jpeg
SRC_CAM			= source/camstab
SRC_EWL			= linux_reference/ewl

SRCS += \
		$(SRC_ENC_COMMON)/encasiccontroller_v2.c \
		$(SRC_ENC_COMMON)/encasiccontroller.c \
		$(SRC_ENC_COMMON)/encpreprocess.c \
		$(SRC_H264)/H264CodeFrame.c \
		$(SRC_H264)/H264Init.c \
		$(SRC_H264)/H264NalUnit.c \
		$(SRC_H264)/H264PictureParameterSet.c \
		$(SRC_H264)/H264PutBits.c \
		$(SRC_H264)/H264RateControl.c \
		$(SRC_H264)/H264SequenceParameterSet.c \
		$(SRC_H264)/H264Slice.c \
		$(SRC_H264)/H264EncApi.c \
		$(SRC_H264)/H264Sei.c \
		$(SRC_H264)/H264Cabac.c \
		$(SRC_H264)/h264encapi_ext.c \
		$(SRC_H264)/H264Mad.c \
		$(SRC_H264)/H264TestId.c \
		$(SRC_JPEG)/EncJpeg.c \
		$(SRC_JPEG)/EncJpegInit.c \
		$(SRC_JPEG)/EncJpegCodeFrame.c \
		$(SRC_JPEG)/EncJpegPutBits.c \
		$(SRC_JPEG)/JpegEncApi.c \
		$(SRC_CAM)/vidstabcommon.c \
		$(SRC_CAM)/vidstabalg.c \
		$(SRC_CAM)/vidstabapi.c \
		$(SRC_CAM)/vidstabinternal.c \
		$(SRC_EWL)/ewl_x280_common.c \
		$(SRC_EWL)/ewl_x280_irq.c \

OBJS = $(patsubst %.c, %.c.o, $(SRCS))
TARGET = lib8270enc.so

all:$(TARGET) install

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -o $@ $^ -fPIC -shared

$(OBJS):%.c.o:%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LOCAL_CFLAGS) $(LOCAL_LDFLAGS) -c $^ -o $@ -fPIC -shared

clean:
	-rm -rf $(OBJS) $(TARGET)

install:
	-mkdir -p $(CONFIG_PREFIX)/lib
	-mkdir -p $(CONFIG_PREFIX)/include
	-cp $(TARGET) $(CONFIG_PREFIX)/lib/
	-cp inc/*.h $(CONFIG_PREFIX)/include/

.PHONY:all clean $(TARGET) install
