TARGET := libIM_OMX_Core.so

commonSources += \
	IM_OMX_Component_Register.c \
	IM_OMX_Core.c

local_CFLAGS := \
		-I../im_osal/inc \
		-I../omx_component/common

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
