include $(TOPDIR)/rules.mk

PKG_NAME:=libmbus
PKG_VERSION:=2026-04-09
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=0.9.0
PKG_SOURCE_URL:=https://github.com/rscada/$(PKG_NAME)

PKG_FIXUP:=autoreconf
PKG_SOURCE:=$(PKG_VERSION).tar.gz
PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=LICENSE COPYING
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libmbus
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=libmbus
endef

define Package/libmbus-util
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=libmbus utility programs
	DEPENDS:=+libmbus
endef

define Package/libmbus/description
	Meter-bus library
endef

define Package/libmbus-util/description
	Meter-bus utility programs
endef

define Package/libmbus/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmbus.so* $(1)/usr/lib
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/mbus/format
	$(CP) $(PKG_BUILD_DIR)/mbus/*.h $(1)/usr/include/mbus
	$(CP) $(PKG_BUILD_DIR)/mbus/format/*.h $(1)/usr/include/mbus/format
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmbus.so* $(1)/usr/lib
endef

$(eval $(call BuildPackage,libmbus))
