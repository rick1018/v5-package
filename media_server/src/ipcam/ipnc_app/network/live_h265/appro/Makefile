#ROOTDIR = ../../../..
#include $(ROOTDIR)/Rules.make
#include ../../Makefile.param

LIVE_DIR = ..
EXEC = wis-streamer

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

CFLAGS = $(INCLUDES) -D_LINUX -g -Wall -O3 $(C_FLAGS)
LDFLAGS += -s

#LIBS =	$(CMEM_INSTALL_DIR)/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \

LIBS = -L$(LIVE_DIR)/BasicUsageEnvironment -lBasicUsageEnvironment \
	-L$(LIVE_DIR)/UsageEnvironment -lUsageEnvironment \
	-L$(LIVE_DIR)/groupsock -lgroupsock \
	-L$(LIVE_DIR)/liveMedia -lliveMedia \
	-L$(APP_SO_LIB) -lfilemsgdrv -lavdatadrv -lmsgutil -lsemutil -lsharemem 
#	-L$(APP_SO_LIB) -lfilemsgdrv -lavdatadrv
#	-L$(APP_SO_LIB) -lfilemsgdrv -lmsgutil -lsysctrl -lsysmsgdrv -lsystemcallmsgdrv -lApprointerface 	
#	-L$(APP_SO_LIB) -lfilemsgdrv -lmsgutil -lsysctrl -lsysmsgdrv -lsystemcallmsgdrv $(APP_LIB)/Appro_interface.a 

OBJS = wis-streamer.o Err.o APPROInput.o \
	WISServerMediaSubsession.o \
	WISJPEGStreamSource.o \
	WISJPEGVideoServerMediaSubsession.o \
	WISMPEG4VideoServerMediaSubsession.o \
	WISH264VideoServerMediaSubsession.o \
	WISH265VideoServerMediaSubsession.o \
	WISPCMAudioServerMediaSubsession.o
#	WISPCMAudioServerMediaSubsession.o \
#	WISOnvifStreamSource.o \
#	WISOnvifServerMediaSubsession.o 
	
#	$(APP_LIB)/Appro_interface.a

#	$(LIB_DIR)/Appro_interface.a

$(EXEC): $(OBJS)
	$(CPLUSPLUS) $(CFLAGS) $(LDFLAGS) -o $(EXEC) $(OBJS) $(LIBS)


wis-streamer.cpp:			Err.hh

Err.cpp:				Err.hh

APPROInput.cpp:				APPROInput.hh Err.hh

WISServerMediaSubsession.cpp:		WISServerMediaSubsession.hh

WISServerMediaSubsession.hh:		APPROInput.hh
WISMPEG4VideoServerMediaSubsession.hh:	WISServerMediaSubsession.hh

WISMPEG4VideoServerMediaSubsession.cpp:	WISMPEG4VideoServerMediaSubsession.hh

WISH264VideoServerMediaSubsession.hh:	WISServerMediaSubsession.hh

WISH264VideoServerMediaSubsession.cpp:	WISH264VideoServerMediaSubsession.hh

WISH265VideoServerMediaSubsession.hh:	WISServerMediaSubsession.hh

WISH265VideoServerMediaSubsession.cpp:	WISH265VideoServerMediaSubsession.hh

WISJPEGStreamSource.cpp:		WISJPEGStreamSource.hh
#WISOnvifStreamSource.cpp:		WISOnvifStreamSource.hh
WISPCMAudioServerMediaSubsession.cpp:	WISPCMAudioServerMediaSubsession.hh

#WISOnvifServerMediaSubsession.cpp:	WISOnvifServerMediaSubsession.hh


.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~
	rm -f $(EXEC)
