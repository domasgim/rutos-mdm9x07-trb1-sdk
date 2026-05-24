#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xmllib

PKG_SOURCE_VERSION:=1.0

PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-3p-closed

include $(INCLUDE_DIR)/package.mk

define Package/xmllib
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Qualcomm XML Library
	DEPENDS:=+diag +glib2
endef

CONFIGURE_ARGS +=--with-glib
CONFIGURE_ARGS +=--with-common-includes=$(STAGING_DIR)/usr/include/qti-common

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{a,la,so*} $(1)/usr/lib/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc \
		$(1)/usr/lib/pkgconfig/
endef


define Package/xmllib/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libxml.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,xmllib))
