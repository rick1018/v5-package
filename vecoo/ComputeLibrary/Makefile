#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ComputeLibrary
PKG_VERSION:=17.06
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

PKG_LICENSE_FILES:=COPYING

PKG_BUILD_DEPENDS:=

include $(BUILD_DIR)/package.mk
include $(BUILD_DIR)/scons.mk

define Package/ComputeLibrary
  SECTION:=vecoo
  CATEGORY:=Vecoo
  DEPENDS:=+libstdcpp
  TITLE:=arm ComputeLibrary
endef

define Package/ComputeLibrary/description
  arm ComputeLibrary
endef

SCONS_VARS += \
	LINKFLAGS="$(TARGET_LDFLAGS)"

SCONS_OPTIONS += \
	Werror=1 \
 	-j8 \
	debug=1 \
	neon=1 \
 	opencl=0 \
 	os=linux \
 	arch=armv7a \
	examples=1 \
	openmp=1 \
	cppthreads=1 \
	benchmark_tests=0

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/arm_compute $(1)/usr/include/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/*.{so,a} $(1)/usr/lib
endef

define Package/ComputeLibrary/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/*.so $(1)/usr/lib
endef

$(eval $(call BuildPackage,ComputeLibrary))
