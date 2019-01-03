TARGET          = liblog.so
INCLUDES        += -I.

LIBS			:=  -lglog

SRCS := \
	glog_helper.cpp \
	log_print.cpp

$(TARGET) : $(SRCS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) $(INCLUDES) $(LDFLAGS) -shared -fPIC

all:$(TARGET) install

install:
	mkdir -p $(CONFIG_PREFIX)/usr/lib/
	mkdir -p $(CONFIG_PREFIX)/usr/include/
	cp $(TARGET) $(CONFIG_PREFIX)/usr/lib/
	cp -rf log/ $(CONFIG_PREFIX)/usr/include/

clean:
	rm -rf $(TARGET) *.o *~
