#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=mbus_client
PKG_VERSION:=2026-04-09
PKG_RELEASE:=1

PKG_SOURCE_VERSION:=7.23
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_LICENSE:=Teltonika-closed

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SECTION:=net
	CATEGORY:=Network
	TITLE:=M-Bus client daemon by Teltonika
	DEPENDS:=+libuci +libtlt_uci +libmbus +libsqlite3 +libubus +libubox +rpcd +librt \
			 +libtlt-logger +libtag +libboardjson +libticket_mutex +libatomic
endef

define Package/$(PKG_NAME)/description
	M-Bus client daemon by Teltonika
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/$(PKG_NAME)
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/lib/rpcd
	$(INSTALL_DIR) $(1)/usr/share/acl.d
	$(INSTALL_DIR) $(1)/etc/permtab.d
	$(INSTALL_DIR) $(1)/etc/uci-defaults/7.17

	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/mbus_client.conf $(1)/etc/config/mbus_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/mbus_client.init $(1)/etc/init.d/mbus_client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mbus_client $(1)/usr/sbin/mbus_client
	$(CP) $(PKG_BUILD_DIR)/mbus_rpc.so $(1)/usr/lib/rpcd
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/mbus_client.json $(1)/usr/share/acl.d/mbus_client.json
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/mbus_client.permtab $(1)/etc/permtab.d/mbus_client
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/migrations/7.17/01_mbus_client_move_flash_db $(1)/etc/uci-defaults/7.17/01_mbus_client_move_flash_db
endef

define Package/$(PKG_NAME)/postinst
	#!/bin/sh

	. "$${IPKG_INSTROOT}/lib/functions.sh"

	group_add_user rpcd mbus_client
	group_add_user dialout mbus_client
endef

define Package/$(PKG_NAME)/prerm
	#!/bin/sh

	rm -f /usr/local/share/mbus_db
	rm -f /var/run/mbus_client/mbus_db
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
