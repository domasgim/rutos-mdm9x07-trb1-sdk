#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libqmi-ex


ifeq ($(CONFIG_TARGET_sdxprairie),y)
PKG_SOURCE_VERSION:=1.1
else
ifeq ($(CONFIG_TARGET_sdxpinn),y)
PKG_SOURCE_VERSION:=sdxpinn1.0
else
PKG_SOURCE_VERSION:=sdxlemur1.0
endif
endif

PKG_BUILD_DEPENDS:=qti-common-headers
PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-3p-closed

include $(INCLUDE_DIR)/package.mk

define Package/libqmi-ex
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Qualcomm MSM Interface (QMI) Library
	DEPENDS:=+configdb +diag +dsutils +libcutils
endef

CONFIGURE_ARGS +=--with-qxdm
CONFIGURE_ARGS +=--with-common-includes=$(STAGING_DIR)/usr/include/qti-common

TARGET_CFLAGS :=-fforward-propagate

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{la,so*} $(1)/usr/lib/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc \
		$(1)/usr/lib/pkgconfig/
endef


define Package/libqmi-ex/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqmiidl.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqmi_client_qmux.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqmiservices.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqmi.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libqmi-ex))
