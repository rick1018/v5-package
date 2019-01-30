include $(TOPDIR)/rules.mk
include $(BUILD_DIR)/kernel.mk

PKG_NAME:=media_server
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_RELEASE)

include $(BUILD_DIR)/package.mk


define Package/media_server
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=media_server
  DEPENDS:=libstdcpp
endef

define Package/media_server/description
	Command Line Interface.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp -a ./src/* $(PKG_BUILD_DIR)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/ \
		SRC_PATH=$(PKG_BUILD_DIR) \
		ARCH="$(TARGET_ARCH)" \
		LIBS=" -lc" \
		AR="$(TARGET_AR)" \
		CC="$(TARGET_CC)" \
		CXX="$(TARGET_CXX)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		LDXXFLAGS="$(TARGET_LDFLAGS)" \
		CONFIG_PREFIX="$(PKG_INSTALL_DIR)" \
		all
endef

define Package/media_server/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/lib
	cp -a $(PKG_BUILD_DIR)/ipcam/lib/*.so $(1)/lib/
	cp -a $(PKG_BUILD_DIR)/ipcam/ipnc_app/network/live/appro/wis-streamer $(1)/usr/bin/
endef
$(eval $(call BuildPackage,media_server))
