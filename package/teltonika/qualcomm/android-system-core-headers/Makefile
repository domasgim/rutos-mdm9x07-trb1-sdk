#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=android-system-core-headers
PKG_VERSION:=1.0


PKG_LICENSE:=Apache-2.0

include $(INCLUDE_DIR)/package.mk

define Package/android-system-core-headers
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=Android system/core components headers
endef

define Build/Compile
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/system-core

	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include/system-core/
endef

$(eval $(call BuildPackage,android-system-core-headers))
