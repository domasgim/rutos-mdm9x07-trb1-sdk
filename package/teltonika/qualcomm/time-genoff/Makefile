#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=time-genoff

PKG_SOURCE_VERSION:=1.0

PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-3p-closed

include $(INCLUDE_DIR)/package.mk

MAKE_PATH:=time-genoff
CONFIGURE_PATH:=time-genoff

define Package/time-genoff
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=Time Genoff Library
endef

TARGET_CFLAGS +=-I$(PKG_BUILD_DIR)
TARGET_CFLAGS +=-DWORKSPACE=.

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{la,so*} $(1)/usr/lib/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc \
		$(1)/usr/lib/pkgconfig/
endef


define Package/time-genoff/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtime_genoff.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,time-genoff))
