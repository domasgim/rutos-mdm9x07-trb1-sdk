#
# Copyright (C) 2024 Teltonika-Networks
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=data


ifeq ($(CONFIG_TARGET_sdxprairie),y)
PKG_SOURCE_VERSION:=1.2
else
ifeq ($(CONFIG_TARGET_sdxpinn),y)
PKG_SOURCE_VERSION:=3.1
else
PKG_SOURCE_VERSION:=2.2
endif
endif

PKG_BUILD_DEPENDS:=qti-common-headers android-system-core-headers
PKG_FIXUP:=autoreconf

PKG_LICENSE:=Teltonika-3p-closed

include $(INCLUDE_DIR)/package.mk

define Package/data
	SECTION:=base
	CATEGORY:=Base system
	TITLE:=Qualcomm Technologies Data Modules
	DEPENDS:=+libopenssl +diag +dsutils +glib2 +libqmi-ex +qmi-framework +qmi-client-helper +xmllib \
		+data-oss +configdb +libcutils +cJSON +libstdcpp +TARGET_sdxpinn:ubox
endef

define Package/data/config
	source "$(SOURCE)/Config.in"
endef

CONFIGURE_ARGS +=--with-glib
CONFIGURE_ARGS +=--with-updated-linux-libc-headers
CONFIGURE_ARGS +=--with-common-includes=$(STAGING_DIR)/usr/include/qti-common
CONFIGURE_ARGS +=--with-sanitized-headers=$(LINUX_DIR)/user_headers/include

ifeq ($(CONFIG_TARGET_sdxpinn),y)
CONFIGURE_ARGS += --with-common-includes=$(STAGING_DIR)/usr/include/qmi-client-helper
CONFIGURE_ARGS += --with-common-includes=$(STAGING_DIR)/usr/include/data-oss
else
CONFIGURE_ARGS +=--with-qxdm
endif

CONFIGURE_ARGS +=$(if $(CONFIG_TARGET_mdm9x07),--enable-target=mdm9607)
CONFIGURE_ARGS +=$(if $(or $(CONFIG_TARGET_sdxprairie),$(CONFIG_TARGET_sdxlemur),$(CONFIG_TARGET_sdxnightjar)),--enable-target=sdxprairie)
CONFIGURE_ARGS +=$(if $(CONFIG_TARGET_sdxpinn),--enable-target=sdxpinn)

TARGET_CFLAGS :=-I$(STAGING_DIR)/usr/include/system-core

ifeq ($(CONFIG_TARGET_sdxpinn),y)

TARGET_CFLAGS += -fpic -pie \
        -DIMAGE_APPS_PROC \
        -DFEATURE_DS_LINUX_NO_RPC \
        -DFEATURE_DATA_LINUX_LE \
        -DFEATURE_DATA_SOFTAP_V6 \
        -DFEATURE_LINUX \
        -DFEATURE_DATA_PREFIX_DELEGATION \
        -DPLATFORM_OPENWRT

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/qti-common -I$(STAGING_DIR)/usr/include/qmi-client-helper

TARGET_LDFLAGS += \
 	-lubus \
 	-lubox \
 	-lqmi_client_helper \
 	-lrmnetctl
endif


define Package/data/install
	$(INSTALL_DIR) $(1)/etc/data $(1)/etc/init.d $(1)/usr/bin $(1)/usr/lib

	$(if $(CONFIG_DATA_QTI), \
		$(INSTALL_BIN) ./files/qti.init $(1)/etc/init.d/qti; \
		$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/qti $(1)/usr/bin \
	)
	$(if $(CONFIG_TARGET_sdxprairie),, \
		$(INSTALL_BIN) ./files/adpl.init $(1)/etc/init.d/adpl; \
		$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/adpl $(1)/usr/bin; \
	)

	$(INSTALL_BIN) ./files/netmgrd.init $(1)/etc/init.d/netmgrd
	$(CP) $(PKG_INSTALL_DIR)/etc/data/netmgr_config.xml $(1)/etc/data
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/netmgrd $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnetmgr_common.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnetmgr.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqcmapipc.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libqcmaputils.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,data))
