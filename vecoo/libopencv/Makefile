include $(TOPDIR)/rules.mk

# Name and release number of this package
PKG_NAME:=libopencv
PKG_VERSION:=3.2.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(COMPILE_DIR)/$(PKG_NAME)

include $(BUILD_DIR)/package.mk

define Package/libopencv
  SECTION:=vecoo
  CATEGORY:=Vecoo
  TITLE:=opencv3.2 userspace libs
  DEPENDS:=+libpthread +librt +libstdcpp +libjpeg +libpng +zlib \
		  +glib2
endef

# Uncomment portion below for Kamikaze and delete DESCRIPTION variable above
define Package/libopencv/description
	opencv userspace libraries
endef
 
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -u -d ./$(TARGET)/lib/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
endef

define Build/InstallDev
	mkdir -p $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr/include/
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib/
	mkdir -p $(PKG_INSTALL_DIR)/usr/share/
	$(CP) -u ./$(TARGET)/include/* $(PKG_INSTALL_DIR)/usr/include/
	$(CP) -u -d ./$(TARGET)/lib/* $(PKG_INSTALL_DIR)/usr/lib/
	$(CP) -u -d ./$(TARGET)/share/* $(PKG_INSTALL_DIR)/usr/share/
	mkdir -p $(1)/usr/include
	mkdir -p $(1)/usr/lib
	mkdir -p $(1)/usr/share
	$(CP) -u ./$(TARGET)/include/* $(1)/usr/include/
	$(CP) -u -d ./$(TARGET)/lib/* $(1)/usr/lib
	$(CP) -u -d ./$(TARGET)/share/* $(1)/usr/share
endef

define Package/libopencv/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) -u -d ./$(TARGET)/lib/* $(1)/usr/lib
	$(CP) -u -d ./$(TARGET)/share/* $(1)/usr/share
	$(CP) ./$(TARGET)/bin/* $(1)/usr/bin
endef

$(eval $(call BuildPackage,libopencv))
