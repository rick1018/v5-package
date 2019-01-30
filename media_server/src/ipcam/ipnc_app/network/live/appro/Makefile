LIVE_DIR = ..
EXEC = wis-streamer

all:	$(EXEC)

install:	$(EXEC)
	install $(EXEC) $(EXEC_DIR)

INCLUDES = -I$(LIVE_DIR) \
	-I$(LIVE_DIR)/BasicUsageEnvironment/include \
	-I$(LIVE_DIR)/UsageEnvironment/include \
	-I$(LIVE_DIR)/groupsock/include \
	-I$(LIVE_DIR)/liveMedia/include \
	-I$(APP_INC)

LIBS1 = -L$(LIVE_DIR)/BasicUsageEnvironment -lBasicUsageEnvironment \
	-L$(LIVE_DIR)/UsageEnvironment -lUsageEnvironment \
	-L$(LIVE_DIR)/groupsock -lgroupsock \
	-L$(LIVE_DIR)/liveMedia -lliveMedia \
	-L$(APP_SO_LIB) -lmsgutil -lsemutil -lsharemem -lfilemsgdrv -lavdatadrv

OBJS = wis-streamer.o Err.o APPROInput.o \
	WISServerMediaSubsession.o \
	WISJPEGStreamSource.o \
	WISOnvifStreamSource.o \
	WISJPEGVideoServerMediaSubsession.o \
	WISMPEG4VideoServerMediaSubsession.o \
	WISH264VideoServerMediaSubsession.o \
	WISPCMAudioServerMediaSubsession.o \
	WISOnvifServerMediaSubsession.o 
	

$(EXEC): $(OBJS)
	$(CXX) $(INCLUDES) -D_LINUX -g -Wall -O3 -s -o $(EXEC) $(OBJS) $(LIBS1)


wis-streamer.cpp:			Err.hh

Err.cpp:				Err.hh

APPROInput.cpp:				APPROInput.hh Err.hh

WISServerMediaSubsession.cpp:		WISServerMediaSubsession.hh

WISServerMediaSubsession.hh:		APPROInput.hh
WISMPEG4VideoServerMediaSubsession.hh:	WISServerMediaSubsession.hh

WISMPEG4VideoServerMediaSubsession.cpp:	WISMPEG4VideoServerMediaSubsession.hh

WISH264VideoServerMediaSubsession.hh:	WISServerMediaSubsession.hh

WISH264VideoServerMediaSubsession.cpp:	WISH264VideoServerMediaSubsession.hh

WISJPEGStreamSource.cpp:		WISJPEGStreamSource.hh
WISOnvifStreamSource.cpp:		WISOnvifStreamSource.hh
WISPCMAudioServerMediaSubsession.cpp:	WISPCMAudioServerMediaSubsession.hh

WISOnvifServerMediaSubsession.cpp:	WISOnvifServerMediaSubsession.hh


.c.o:
	$(CC) -c $(INCLUDES) -D_LINUX -g -Wall -O3 $< -o $@

.cpp.o:
	$(CXX) -c $(INCLUDES) -D_LINUX -g -Wall -O3 $< -o $@

clean:
	rm -f *.o *~
	rm -f $(EXEC)
