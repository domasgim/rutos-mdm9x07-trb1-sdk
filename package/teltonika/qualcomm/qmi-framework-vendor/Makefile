#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=qmi-framework-vendor

PKG_SOURCE_VERSION:=1.3


PKG_LICENSE:=BSD-2-Clause

include $(INCLUDE_DIR)/package.mk

define Package/qmi-framework-vendor
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=QMI Framework Applications
	DEPENDS:=+libqmi-ex +glib2 +qsocket
endef

define Package/qmi-framework-vendor/config
	source "$(SOURCE)/Config.in"
endef

CONFIGURE_ARGS +=--with-glib
CONFIGURE_ARGS +=$(if $(or $(CONFIG_TARGET_sdxprairie),$(CONFIG_TARGET_sdxlemur),$(CONFIG_TARGET_sdxnightjar),$(CONFIG_TARGET_mdm9x07)) ,--enable-target=sdxprairie)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{la,so*} $(1)/usr/lib/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/*.pc \
		$(1)/usr/lib/pkgconfig/
endef


define Package/qmi-framework-vendor/install
	$(INSTALL_DIR) $(1)/usr/lib

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqrtr.{la,so*} $(1)/usr/lib/

	$(if $(CONFIG_QMI_FRAMEWORK_VENDOR_QRTR_NS), \
		$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/bin; \
		$(INSTALL_BIN) ./files/qrtr-ns.init $(1)/etc/init.d/qrtr-ns; \
		$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/qrtr-ns $(1)/usr/bin \
	)
endef

$(eval $(call BuildPackage,qmi-framework-vendor))
