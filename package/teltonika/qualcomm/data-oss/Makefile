#
# Copyright (C) 2023 Teltonika-Networks
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=data-oss

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.codelinaro.org/clo/la/platform/vendor/qcom-opensource/dataservices.git


ifeq ($(CONFIG_TARGET_sdxpinn),y)
PKG_SOURCE_VERSION:=OWRT.PRODUCT.2.0.1.r2-01000-SDX75.0
else
PKG_SOURCE_VERSION:=LE.UM.4.4.2.r1-10700-SDX55
endif

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

PKG_ORIGIN_URL:=https://android.googlesource.com/device/google/marlin/+/refs/heads/oreo-dr3-release/dataservices
PKG_LICENSE:=BSD-3-Clause

include $(INCLUDE_DIR)/package.mk

define Package/data-oss
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Data Services Open Source
	DEPENDS:=+glib2
endef

CONFIGURE_ARGS +=--with-glib
CONFIGURE_ARGS +=--with-sanitized-headers=$(LINUX_DIR)/user_headers/include

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{la,so*} $(1)/usr/lib/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc \
		$(1)/usr/lib/pkgconfig/
endef

define Package/data-oss/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/librmnetctl.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,data-oss))
