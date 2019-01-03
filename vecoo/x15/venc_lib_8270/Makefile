##############################################
# OpenWrt Makefile for smartlinkd program
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

# Name and release number of this package
PKG_NAME:=venc_lib_8270
PKG_RELEASE:=1


#PKG_BUILD_DIR := $(COMPILE_DIR)/$(PKG_NAME)-$(PKG_VERSION)
HAVE_SOURCE := yes

include $(BUILD_DIR)/package.mk

define Package/venc_lib_8270/Default
  SECTION:=utils
  CATEGORY:=Vecoo
  SUBMENU:=X15
  DEPENDS:=
endef

define Package/venc_lib_8270
  $(call Package/venc_lib_8270/Default)
  TITLE:=x15 decoder venc_lib_8270 library
endef

define Package/venc_lib_8270/description
  infotm x15 decoder venc_lib_8270 library
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)/
	$(CP) -r ./src $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
		ARCH="$(TARGET_ARCH)" \
		AR="$(TARGET_AR)" \
		CC="$(TARGET_CC)" \
		CXX="$(TARGET_CXX)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		CONFIG_PREFIX="$(PKG_INSTALL_DIR)" \
		all
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_DIR) $(1)/include
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/* $(1)/lib/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/include/* $(1)/include/
endef

define Package/venc_lib_8270/install
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/* $(1)/lib
endef

$(eval $(call BuildPackage,venc_lib_8270))
