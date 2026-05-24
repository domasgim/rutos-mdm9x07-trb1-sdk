#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=qmi-client-helper

PKG_SOURCE_VERSION:=1.0

PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-3p-closed

include $(INCLUDE_DIR)/package.mk

define Package/qmi-client-helper
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=QMI Client Helper Module
	DEPENDS:=+diag +dsutils +libqmi-ex +qmi-framework
endef


CONFIGURE_ARGS +=--with-qxdm
CONFIGURE_ARGS +=--with-common-includes=$(STAGING_DIR)/usr/include/qti-common

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{la,so*} $(1)/usr/lib/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc \
		$(1)/usr/lib/pkgconfig/
endef


define Package/qmi-client-helper/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqmi_client_helper.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,qmi-client-helper))
