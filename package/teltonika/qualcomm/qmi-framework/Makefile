#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=qmi-framework


PKG_SOURCE_VERSION:=1.2

PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-3p-closed

include $(INCLUDE_DIR)/package.mk

define Package/qmi-framework
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=QMI Framework Library
	DEPENDS:=+libqmi-ex +qmi-framework-vendor +glib2 +libqrtr-glib +qsocket
endef

CONFIGURE_ARGS +=--with-glib
CONFIGURE_ARGS +=$(if $(or $(CONFIG_TARGET_sdxprairie),$(CONFIG_TARGET_sdxlemur),$(CONFIG_TARGET_sdxnightjar)) ,--enable-target=sdxprairie)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{la,so*} $(1)/usr/lib/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc \
		$(1)/usr/lib/pkgconfig/
endef


define Package/qmi-framework/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqmi_sap.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqmi_common_so.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqmi_csi.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqmi_encdec.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqmi_cci.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,qmi-framework))
