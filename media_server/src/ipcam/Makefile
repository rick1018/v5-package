OEM_ID=GL
MODEL_ID=VCT

ROOT_DIR ?= $(PWD)
PROJ_ROOT ?= $(ROOT_DIR)
APP_DIR ?= $(ROOT_DIR)/ipnc_app
TARGET_FS ?= $(ROOT_DIR)/rootfs
RELEASE_DIR ?= $(ROOT_DIR)/images
TFTPDIR ?= $(ROOT_DIR)/tftpboot
AV_DIR?=$(ROOT_DIR)/av_capture
AV_INC?=$(AV_DIR)/include
AV_LIB?=$(AV_DIR)/lib
APP_DIR?=$(ROOT_DIR)/ipnc_app
APP_INC?=$(ROOT_DIR)/include
APP_LIB?=$(ROOT_DIR)/lib
APP_SO_LIB?=$(TARGET_FS)/lib
EXEC_DIR=$(TARGET_FS)/bin

export CROSS_COMPILE OEM_ID MODEL_ID ROOT_DIR SDK_SRC SDK_INC SDK_LIB DIR_IPCAM_SRC PROJ_ROOT
export APP_INC APP_LIB APP_SO_LIB AV_INC AV_LIB  AV_DIR APP_DIR TARGET_FS


IPNC_SRC+=av_capture
IPNC_SRC+=ipnc_app

SUBDIRS = $(IPNC_SRC) 

CLEANSUBDIRS = $(addsuffix .clean, $(SUBDIRS))

.PHONY: $(SUBDIRS) $(DEBUGSUBDIRS) $(RELEASESUBDIRS) $(INSTALLSUBDIRS) \
	$(CLEANSUBDIRS)	

all: $(SUBDIRS) 

clean: $(CLEANSUBDIRS)	
	rm -rf *.o *.gdb 

$(SUBDIRS):
	@echo
	@echo Making all in subdirectory $@...
	@$(MAKE) -C $@	
	
$(CLEANSUBDIRS):
	@cd $(basename $@) ; $(MAKE) clean

