DEUBG = -DXOP_DEBUG

TARGET1 = rtsp_server
TARGET2 = rtsp_pusher

OBJS_PATH = objs

INC  = -I$(shell pwd)/src -I$(shell pwd)/src/xop -I. \
                   -I$(STAGING_DIR)/usr/include \
                   -I$(STAGING_DIR)/usr/include/utils \
                   -I$(STAGING_DIR)/usr/include/device \
                   -I$(STAGING_DIR)/usr/include/media \
                   -I$(STAGING_DIR)/usr/include/vo

LIB  = -lmpp_vi \
	-lmpp_comm \
	-lmedia_mpp \
	-lmpp_vo \
	-lmedia_utils

LD_FLAGS  = -lrt -pthread -lpthread -ldl -lm $(LIB) $(DEBUG)
CXX_FLAGS = -std=c++11

O_FLAG   = -O2

SRC1  = $(notdir $(wildcard ./src/*.cpp))
OBJS1 = $(patsubst %.cpp,$(OBJS_PATH)/%.o,$(SRC1))

SRC2  = $(notdir $(wildcard ./src/xop/*.cpp))
OBJS2 = $(patsubst %.cpp,$(OBJS_PATH)/%.o,$(SRC2))

SRC3  = $(notdir $(wildcard ./example/rtsp_server.cpp))
OBJS3 = $(patsubst %.cpp,$(OBJS_PATH)/%.o,$(SRC3))

SRC4  = $(notdir $(wildcard ./example/rtsp_pusher.cpp))
OBJS4 = $(patsubst %.cpp,$(OBJS_PATH)/%.o,$(SRC4))

all: BUILD_DIR $(TARGET1) $(TARGET2)

BUILD_DIR:
	@-mkdir -p $(OBJS_PATH)

$(TARGET1) : $(OBJS1) $(OBJS2) $(OBJS3)
	$(CXX) $^ -o $@ $(CFLAGS) $(LD_FLAGS) $(CXX_FLAGS)

$(TARGET2) : $(OBJS1) $(OBJS2) $(OBJS4)
	$(CXX) $^ -o $@ $(CFLAGS) $(LD_FLAGS) $(CXX_FLAGS)
    
$(OBJS_PATH)/%.o : ./example/%.cpp
	$(CXX) -c  $< -o  $@  $(CXX_FLAGS) $(INC)
$(OBJS_PATH)/%.o : ./src/%.cpp
	$(CXX) -c  $< -o  $@  $(CXX_FLAGS) $(INC)
$(OBJS_PATH)/%.o : ./src/xop/%.cpp
	$(CXX) -c  $< -o  $@  $(CXX_FLAGS) $(INC)

clean:
	-rm -rf $(OBJS_PATH) $(TARGET1) $(TARGET2)
