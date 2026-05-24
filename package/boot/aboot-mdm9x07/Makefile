#
# Copyright (C) 2022 Teltonika
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/download.mk

PKG_NAME:=aboot-mdm9x07

PKG_SOURCE_VERSION:=1.8.2

PKG_ASLR_PIE:=0

PKG_ORIGIN_URL:=https://android.googlesource.com/kernel/lk/+/b42836029ffba42d4d5b6dd6fb7e136cd5cd418e/
PKG_LICENSE:=BSD-3-Clause

include $(INCLUDE_DIR)/package.mk

define Package/aboot-mdm9x07
	SECTION:=boot
	CATEGORY:=Boot Loaders
	TITLE:=Android Bootloader for MDM9X07 targets
endef

BOARD_NAME:=mdm9607

define Build/Compile
	$(MAKE) \
		-C $(PKG_BUILD_DIR) \
		$(BOARD_NAME) \
		SIGNED_KERNEL=1 \
		TOOLCHAIN_PREFIX="$(TARGET_CROSS)" \
		LIBGCC="$(LIBGCC_A)"
endef

define Package/aboot-mdm9x07/install
	$(CP) $(PKG_BUILD_DIR)/build-$(BOARD_NAME)/*.mbn $(BIN_DIR)/trb14x-aboot.mbn
endef

$(eval $(call BuildPackage,aboot-mdm9x07))
