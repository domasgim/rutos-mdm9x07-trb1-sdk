include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=mdm9x07
BOARDNAME:=Qualcomm MDM9x07
SUBTARGETS:=generic
FEATURES:=squashfs fpu nand usbgadget baseband reset_button
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4

KERNEL_PATCHVER:=5.4

define Target/Description
	Build firmware image for Qualcomm MDM9x07 SoC devices.
endef

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	swconfig

KERNELNAME:=zImage dtbs


$(eval $(call BuildTarget))
