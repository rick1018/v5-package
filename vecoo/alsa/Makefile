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

PKG_NAME:=alsa
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(COMPILE_DIR)/$(PKG_NAME)

include $(BUILD_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Vecoo
  TITLE:= v5 alsa library
  DEPENDS:= +middleware-libs
endef

define Package/$(PKG_NAME)/description
  contains many alsa libs
endef

define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/ $(1)/usr/lib/ $(1)/usr/share/
	$(CP) ./usr/lib/smixer $(1)/usr/lib/
	$(CP) ./usr/share/alsa $(1)/usr/share/
	$(CP) ./etc/asound.conf $(1)/etc/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
