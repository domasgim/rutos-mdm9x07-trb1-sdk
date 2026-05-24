#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dtbtool
PKG_VERSION:=2020-03-03
PKG_RELEASE:=1

PKG_ORIGIN_URL:=https://github.com/rajatgupta1998/android_tools_system_dtbTool
PKG_LICENSE:=BSD-3-Clause
PKG_LICENSE_FILES:=dtbtool.c

include $(INCLUDE_DIR)/package.mk

define Package/dtbtool
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=DTBTool for TRB14X targets
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(TOPDIR)/tools/$(PKG_NAME)/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(CFLAGS) $(PKG_BUILD_DIR)/dtbtool.c -o $(PKG_BUILD_DIR)/dtbtool
endef

define Package/dtbtool/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dtbtool $(1)/usr/bin/
endef

$(eval $(call BuildPackage,dtbtool))
