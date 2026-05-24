#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qcom-modem
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define KernelPackage/qcom-modem
  SUBMENU:=Other modules
  TITLE:=Qualcomm modem initialization module
  FILES:=$(PKG_BUILD_DIR)/qcom-modem.ko
  AUTOLOAD:=$(call AutoProbe,qcom-modem)
endef

define KernelPackage/qcom-modem/description
 Kernel module used for the Qualcomm modem subsystem initialization.
endef

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	M="$(PKG_BUILD_DIR)"

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,qcom-modem))
