#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk

PKG_NAME:=qmuxd

PKG_SOURCE_VERSION:=1.0

PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-3p-closed

include $(INCLUDE_DIR)/package.mk

define Package/qmuxd
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Qualcomm QMUXD Module
	DEPENDS:=+diag +dsutils +configdb +qmi-framework
endef


CONFIGURE_ARGS +=--with-qxdm
CONFIGURE_ARGS +=--with-common-includes=$(STAGING_DIR)/usr/include/qti-common

TARGET_CFLAGS +=-fforward-propagate


define Package/qmuxd/install
	$(INSTALL_DIR) $(1)/etc/data $(1)/etc/init.d $(1)/usr/bin

	$(CP) $(PKG_INSTALL_DIR)/etc/data/qmi_config.xml $(1)/etc/data/
	$(CP) files/qmuxd.init $(1)/etc/init.d/qmuxd
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/qmuxd $(1)/usr/bin
endef

$(eval $(call BuildPackage,qmuxd))
