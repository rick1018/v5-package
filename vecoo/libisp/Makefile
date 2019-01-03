##############################################
# OpenWrt Makefile for helloworld program
#
#
# Most of the variables used here are defined in
# the include directives below. We just need to
# specify a basic description of the package,
# where to build our program, where to find
# the source files, and where to install the
# compiled program on the router.
#
# Be very careful of spacing in this file.
# Indents should be tabs, not spaces, and
# there should be no trailing whitespace in
# lines that are not commented.
#
##############################################
include $(TOPDIR)/rules.mk
include $(BUILD_DIR)/kernel.mk

PKG_NAME:=libisp_ini
PKG_RELEASE:=1

PKG_BUILD_DIR := $(COMPILE_DIR)/$(PKG_NAME)

include $(BUILD_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Vecoo
  TITLE:= parser isp info from config file
  DEPENDS:= +liblog
endef

define Package/$(PKG_NAME)/description
	parser isp info from config file, come from v5 platform!
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./* $(PKG_BUILD_DIR)/
	mkdir -p $(STAGING_DIR)/usr/include/device/
	mkdir -p $(STAGING_DIR)/usr/include/platform/
	$(CP) ./isp_cfg/SENSOR_H/* $(STAGING_DIR)/usr/include/
	$(CP) ./include/*.h $(STAGING_DIR)/usr/include/
	$(CP) ./include/device/* $(STAGING_DIR)/usr/include/device/
	$(CP) ./include/platform/*.h $(STAGING_DIR)/usr/include/platform/
	$(CP) ./include/V4l2Camera/*.h $(STAGING_DIR)/usr/include/
	$(CP) ./iniparser/src/* $(STAGING_DIR)/usr/include/
	$(CP) ./isp_dev/*.h $(STAGING_DIR)/usr/include/
	$(CP) ./isp_tuning/*.h $(STAGING_DIR)/usr/include/
	$(CP) ./*.h $(STAGING_DIR)/usr/include/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/isp_cfg/ \
		ARCH="$(TARGET_ARCH)" \
		AR="$(TARGET_AR)" \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		CONFIG_PREFIX="$(PKG_INSTALL_DIR)" \
		all
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libisp_ini.so $(1)/usr/lib/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libisp_ini.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
