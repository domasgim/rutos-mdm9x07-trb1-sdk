define KernelPackage/at803x-phy
  SUBMENU:=Network Devices
  TITLE:=Atheros AT803X PHY Driver
  KCONFIG:=CONFIG_AT803X_PHY
  FILES:=$(LINUX_DIR)/drivers/net/phy/at803x.ko
  DEPENDS:=@TARGET_mdm9x07 +kmod-libphy +kmod-swconfig
endef

$(eval $(call KernelPackage,at803x-phy))

define KernelPackage/en8801s-phy  
  SUBMENU:=Network Devices
  TITLE:=Airoha EN8801S driver
  KCONFIG:=CONFIG_AIROHA_EN8801S_PHY
  FILES:=$(LINUX_DIR)/drivers/net/phy/en8801s.ko
  DEPENDS:=@TARGET_mdm9x07 +kmod-libphy +kmod-swconfig
endef

$(eval $(call KernelPackage,en8801s-phy))

define KernelPackage/qcom-emac
  SUBMENU:=Network Support
  TITLE:=Qualcomm EMAC Gigabit Ethernet Driver
  KCONFIG:=CONFIG_QCOM_EMAC
  FILES:=$(LINUX_DIR)/drivers/net/ethernet/qualcomm/emac/qcom-emac.ko
  DEPENDS:=@TARGET_mdm9x07 +kmod-of-mdio +kmod-libphy
endef

$(eval $(call KernelPackage,qcom-emac))
