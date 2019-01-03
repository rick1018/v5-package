TARGET := libim_osal.so

FIXED_ARCH_TYPE   ?= android

commonSources += \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Event.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Queue.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_ETC.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Mutex.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Thread.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Memory.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Semaphore.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Library.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Trace.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_List.c \
	$(FIXED_ARCH_TYPE)/IM_OSAL_Buffer.c

local_CFLAGS = \
		-D_ANDROID_ \
		-DTARGET_SYSTEM=FS_LINUX \
		-DTS_VER_MAJOR=4 \
		-DTS_VER_MINOR=3 \
		-DTS_VER_PATCH=0 \
		-DVDEC_LIB_G1 \
		-Iinc \
		-I../include/infotm \
		-I../omx_core/inc/ \
		-I../omx_component/common/

local_CFLAGS +=	-DVDEC_LIB_G1

OBJS = $(patsubst %.c, %.c.o, $(commonSources))

$(TARGET):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(local_CFLAGS) -o $@ $^ -fPIC -shared

$(OBJS):%.c.o:%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(local_CFLAGS) -c $^ -o $@ -fPIC -shared

all: $(TARGET) install

clean:
	-rm $(OBJS) $(TARGET)
install:
	mkdir -p $(CONFIG_PREFIX)/usr/lib
	cp $(TARGET) $(CONFIG_PREFIX)/usr/lib

	mkdir -p $(CONFIG_PREFIX)/usr/include
	cp -r inc/*.h $(CONFIG_PREFIX)/usr/include
