OEM_ID=GL
MODEL_ID=VCT

ROOT_DIR=$(SRC_PATH)
PROJ_ROOT=$(ROOT_DIR)
SDK_SRC=$(ROOT_DIR)/ipcam
KERNEL_DIR=$(ROOT_DIR)/../../kernel
SDK_INC=$(SDK_SRC)/lib/include
SDK_LIB=$(SDK_SRC)/lib/lib
DIR_IPCAM_SRC=$(SDK_SRC)/user/rt2880_app/ipcam

AV_DIR=$(ROOT_DIR)/ipcam/av_capture
AV_INC=$(AV_DIR)/include
AV_LIB=$(AV_DIR)/lib

GEOSW_ROOT=$(AV_DIR)/gc6500-6.3.9

APP_DIR=$(ROOT_DIR)/ipcam/ipnc_app
APP_INC=$(ROOT_DIR)/ipcam/include
APP_LIB=$(ROOT_DIR)/ipcam/lib

ROMFS=$(SDK_SRC)/romfs
ROMFS_SO_LIB=$(ROMFS)/lib

TARGET_FS=$(ROOT_DIR)/ipcam/rootfs
APP_SO_LIB=$(TARGET_FS)/lib
EXEC_DIR=$(TARGET_FS)/bin


export CROSS_COMPILE OEM_ID MODEL_ID ROOT_DIR SDK_SRC SDK_INC SDK_LIB DIR_IPCAM_SRC PROJ_ROOT ROMFS_SO_LIB
export APP_INC APP_LIB APP_SO_LIB AV_INC AV_LIB  AV_DIR APP_DIR TARGET_FS EXEC_DIR KERNEL_DIR GEOSW_ROOT

SUBDIRS = $(SDK_SRC) 

CLEANSUBDIRS = $(addsuffix .clean, $(SUBDIRS))

.PHONY: $(SUBDIRS) $(DEBUGSUBDIRS) $(RELEASESUBDIRS) $(INSTALLSUBDIRS) \
	$(CLEANSUBDIRS)
	
all: $(SUBDIRS)

$(SUBDIRS):
	@echo
	@echo Making all in subdirectory $@...
	@$(MAKE) -C $@

av_all: av_clean av

av:	
		@$(MAKE) -C $(DIR_IPCAM_SRC)/av_capture

av_clean:	
		@$(MAKE) -C $(DIR_IPCAM_SRC)/av_capture clean

app_all: app_clean app	

app:	
		@$(MAKE) -C $(DIR_IPCAM_SRC)/ipnc_app

app_clean:	
		@$(MAKE) -C $(DIR_IPCAM_SRC)/ipnc_app clean
		
ipnc_all:	av_all app_all all

ipnc:	av app all
#		@$(MAKE) -C $(DIR_IPCAM_SRC)

ipnc_clean:	av_clean app_clean
		@$(MAKE) -C $(DIR_IPCAM_SRC) clean		
		
clean:	$(CLEANSUBDIRS) 
	@rm -rf *.o


$(CLEANSUBDIRS):
	@cd $(basename $@) ; $(MAKE) clean
	
