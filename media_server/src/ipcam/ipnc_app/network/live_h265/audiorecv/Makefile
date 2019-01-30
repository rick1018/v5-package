#ROOTDIR = ../../../..
#include $(ROOTDIR)/Rules.make

#include ../../Makefile.param

LIVE_DIR = ..
EXEC = audio-receiver

all:	$(EXEC)

install:	$(EXEC)
	install $(EXEC) $(EXEC_DIR)

CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)c++

INCLUDES = -I$(LIVE_DIR) \
	-I$(LIVE_DIR)/BasicUsageEnvironment/include \
	-I$(LIVE_DIR)/UsageEnvironment/include \
	-I$(LIVE_DIR)/groupsock/include \
	-I$(LIVE_DIR)/liveMedia/include \
	-I$(APP_INC)

CFLAGS = $(INCLUDES) -D_LINUX -g -Wall -O3
LDFLAGS += -s

#LIBS =	$(CMEM_INSTALL_DIR)/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \

LIBS = -L$(LIVE_DIR)/BasicUsageEnvironment -lBasicUsageEnvironment \
	-L$(LIVE_DIR)/UsageEnvironment -lUsageEnvironment \
	-L$(LIVE_DIR)/groupsock -lgroupsock \
	-L$(LIVE_DIR)/liveMedia -lliveMedia \
	-L$(APP_LIB) -lmsgutil -lsemutil -lsharemem -lfilemsgdrv -lavdatadrv

OBJS = audio-receiver.o AudioSink.o 	
#	$(APP_LIB)/Appro_interface.a

$(EXEC): $(OBJS)
#	echo $(LIBS) >> debug.txt
	$(CPLUSPLUS) $(CFLAGS) $(LDFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

Err.cpp:		Err.hh

AudioSink.cpp:		AudioSink.hh 

audio-receiver.cpp:	AudioSink.hh playCommon.hh
 
.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~
	rm -f $(EXEC)
