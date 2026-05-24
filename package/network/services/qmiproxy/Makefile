include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=qmiproxy
PKG_VERSION:=2020-03-26
PKG_RELEASE:=1

PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_SOURCE_VERSION)-$(PKG_VERSION)

PKG_LICENSE:=GPL-3.0-or-later
PKG_LICENSE_FILES:=COPYING

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/scintill/qmiserial2qmuxd.git
PKG_SOURCE_VERSION:=d362b032ac8aaf8831afea840a07628acfe9b01f

include $(INCLUDE_DIR)/package.mk

define Package/qmiproxy
	SECTION:=net
	CATEGORY:=Network
	TITLE:=QMI Serial to QMUXD proxy service
	DEPENDS:=+qmuxd +data
endef

define Package/qmiproxy/description
	Proxy to allow programs such as qmicli (libqmi), uqmi, and oFono to work with qmuxd.
endef

define Package/qmiproxy/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/bin
	$(INSTALL_BIN) ./files/qmiproxy.init $(1)/etc/init.d/qmiproxy
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/qmiserial2qmuxd $(1)/usr/bin/qmiproxy
endef

$(eval $(call BuildPackage,qmiproxy))
