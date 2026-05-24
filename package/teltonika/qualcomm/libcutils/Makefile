#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libcutils
PKG_VERSION:=1.0



PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=NOTICE

PKG_BUILD_DEPENDS:=android-system-core-headers
PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libcutils
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Build Android libcutils
endef

CONFIGURE_ARGS +=LE_PROPERTIES_ENABLED=true
CONFIGURE_ARGS +=--with-core-includes=$(STAGING_DIR)/usr/include/system-core


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/include/cutils $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{la,so*} $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libcutils/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcutils.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libcutils))
