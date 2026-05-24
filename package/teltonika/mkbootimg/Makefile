#
# Copyright (C) 2021 Teltonika
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mkbootimg
PKG_VERSION:=2020-03-03
PKG_RELEASE:=1

PKG_ORIGIN_URL:=https://android.googlesource.com/platform/system/core/+/android-4.4_r1/mkbootimg

PKG_LICENSE:=Apache-2.0
PKG_LICENSE_FILES:=mkbootimg.c

include $(INCLUDE_DIR)/package.mk

define Package/mkbootimg
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Android mkbootimg for TRB14X targets
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) $(TOPDIR)/tools/$(PKG_NAME)/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(CFLAGS) $(PKG_BUILD_DIR)/*.c -o $(PKG_BUILD_DIR)/mkbootimg
endef

define Package/mkbootimg/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mkbootimg $(1)/usr/bin/
endef

$(eval $(call BuildPackage,mkbootimg))
