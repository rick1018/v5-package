TARGET := libim_basecomponent.so

commonSources += \
	IM_OMX_Basecomponent.c \
	IM_OMX_Baseport.c \
	IM_OMX_Resourcemanager.c

local_CFLAGS := \
		-I../../im_osal/inc \
		-I../../include/khronos \
		-I../omx_component/common \
		-I../infotm

OBJS = $(patsubst %.c, %.c.o, $(commonSources))

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(local_CFLAGS) -o $@ $^ -fPIC -shared

$(OBJS):%.c.o:%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(local_CFLAGS) -c $^ -o $@ -fPIC -shared

all: $(TARGET) install

clean:
	-rm $(OBJS) $(TARGET)
install:
	mkdir -p $(INSTALL_PREFIX)/usr/lib
	cp $(TARGET) $(INSTALL_PREFIX)/usr/lib

	mkdir -p $(INSTALL_PREFIX)/usr/include
	cp -r include/* $(INSTALL_PREFIX)/usr/include
