#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=middleware-libs
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(COMPILE_DIR)/$(PKG_NAME)

include $(BUILD_DIR)/package.mk

define Package/$(PKG_NAME)
  TITLE:= V5 middleware
  SECTION:=libs
  CATEGORY:=Vecoo
  TITLE+= library
  DEPENDS:= +libstdcpp +libcutils +liblog +libion +libhwdisplay +libisp_ini
endef

define Package/$(PKG_NAME)/description
  contains many middleware libs
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include

	$(CP) ./include/utils $(1)/usr/include/
	$(CP) ./include/media $(1)/usr/include/

	$(CP) ./media/include/*.h $(1)/usr/include/
	$(CP) ./media/include/utils/*.h $(1)/usr/include/
	$(CP) ./media/include/component/*.h $(1)/usr/include/

	$(CP) ./media/LIBRARY/libISE/*.h $(1)/usr/include/

	$(CP) ./media/LIBRARY/include_stream/*.h $(1)/usr/include/
	$(CP) ./media/LIBRARY/include_muxer/*.h $(1)/usr/include/
	$(CP) ./media/LIBRARY/include_FsWriter/*.h $(1)/usr/include/
	$(CP) ./media/LIBRARY/libcedarc/include/*.h $(1)/usr/include/
	$(CP) ./media/LIBRARY/libcedarx/libcore/common/iniparser/*.h $(1)/usr/include/

	$(CP) ./app_mpp_sample/include/rtsp $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) ./lib/*.{a,so*} $(1)/usr/lib/
	$(CP) ./app_mpp_sample/lib/libTinyServer.so $(1)/usr/lib/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib $(1)/etc/init.d/ $(1)/usr/bin/
	$(CP) ./lib/*.so* $(1)/usr/lib/
	$(CP) ./media/LIBRARY/libkfc/*.so* $(1)/usr/lib/
	$(CP) ./app_mpp_sample/lib/libTinyServer.so $(1)/usr/lib/
	$(INSTALL_BIN) ./media/LIBRARY/libkfc/iploader_server $(1)/usr/bin/
	$(INSTALL_BIN) ./media/LIBRARY/libkfc/kfc_server_pack $(1)/usr/bin/
	$(INSTALL_BIN) ./media/LIBRARY/libkfc/iploader_server.init $(1)/etc/init.d/iploader_server
	$(INSTALL_BIN) ./media/LIBRARY/libkfc/kfc_server_pack.init $(1)/etc/init.d/kfc_server_pack
	$(INSTALL_CONF) ./media/LIBRARY/libcedarx/config/V5IPCDemo_cedarx.conf $(1)/etc/cedarx.conf
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
