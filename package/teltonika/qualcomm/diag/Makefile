#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=diag

PKG_SOURCE_VERSION:=1.1

PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-3p-closed

include $(INCLUDE_DIR)/package.mk

define Package/diag
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Library and routing applications for diagnostic traffic
	DEPENDS:=+glib2 +time-genoff
endef

CONFIGURE_ARGS +=--with-glib
CONFIGURE_ARGS +=--with-common-includes=$(STAGING_DIR)/usr/include/qti-common

TARGET_CPPFLAGS :=-Dstrlcat=g_strlcat

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{la,so*} $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc \
		$(1)/usr/lib/pkgconfig/
endef


define Package/diag/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdiag.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,diag))
