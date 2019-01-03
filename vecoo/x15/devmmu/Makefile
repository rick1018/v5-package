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
PKG_NAME:=devmmu
PKG_RELEASE:=1


#PKG_BUILD_DIR := $(COMPILE_DIR)/$(PKG_NAME)-$(PKG_VERSION)
HAVE_SOURCE := yes

include $(BUILD_DIR)/package.mk

define Package/devmmu/Default
  SECTION:=utils
  CATEGORY:=Vecoo
  SUBMENU:=X15
  DEPENDS:=
endef

define Package/devmmu
  $(call Package/devmmu/Default)
  TITLE:=x15 decoder devmmu library
endef

define Package/devmmu/description
  infotm x15 decoder devmmu library
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

define Package/devmmu/install
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/lib/* $(1)/lib
endef

$(eval $(call BuildPackage,devmmu))
