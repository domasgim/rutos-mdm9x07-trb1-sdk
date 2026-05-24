#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=diag-old

PKG_SOURCE_VERSION:=1.1

PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-3p-closed

include $(INCLUDE_DIR)/package.mk

define Package/diag-old
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Qualcomm diagnostic utility
	DEPENDS:=+libcutils +glib2
endef

CONFIGURE_ARGS +=--with-glib
CONFIGURE_ARGS +=--with-common-includes=$(STAGING_DIR)/usr/include/qti-common

TARGET_CPPFLAGS :=-Dstrlcpy=g_strlcpy
TARGET_CPPFLAGS :=-I$(STAGING_DIR)/usr/include/system-core


define Package/diag-old/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/lib

	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/diag_mdlog $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdiagold.so* $(1)/usr/lib
endef

$(eval $(call BuildPackage,diag-old))
