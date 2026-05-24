#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=dsutils

PKG_SOURCE_VERSION:=1.2

PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-3p-closed

include $(INCLUDE_DIR)/package.mk

define Package/dsutils
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Qualcomm Data DSutils Module
	DEPENDS:=+diag +glib2 +libcutils
endef

CONFIGURE_ARGS +=--with-qxdm --with-glib
CONFIGURE_ARGS +=--with-common-includes=$(STAGING_DIR)/usr/include/qti-common
CONFIGURE_ARGS +=--with-sanitized-headers=$(LINUX_DIR)/user_headers/include

TARGET_CFLAGS :=-I$(STAGING_DIR)/usr/include/system-core

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{la,so*} $(1)/usr/lib/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc \
		$(1)/usr/lib/pkgconfig/
endef


define Package/dsutils/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdsutils.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,dsutils))
