include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
include basebands.mk
include common-trb14x.mk

DEVICE_VARS += DEVICE_MODEL_TYPE DEVICE_MODEL_REGION

BASE_ADDR:=0x80000000
TAGS_ADDR:=0x81E00000

MASTER_DTB_NAME:=masterDTB

TRB_CMDLINE:="console=ttyMSM0,115200,n8 \
  earlycon=msm-uartdm,0x78b3000 \
  ubi.fm_autoconvert=1 \
  ubi.mtd=%ROOTFS_MTD_NAME% \
  ubi.block=0,0 \
  root=/dev/ubiblock0_0 \
  rootfstype=squashfs \
  ubi.mtd=modem \
  ubi.block=1,0 \
  ubi.mtd=storage"

define Build/master_dtb
  $(STAGING_DIR_HOST)/bin/dtbtool $(KDIR)/ \
    -s $(PAGESIZE) \
    -o $(KDIR)/$(MASTER_DTB_NAME) \
    -p $(LINUX_DIR)/scripts/dtc/
endef

define Build/mkimage
  $(STAGING_DIR_HOST)/bin/mkbootimg \
    --kernel $(KDIR)/zImage$(if $(1),-$(1)) \
    --dt $(KDIR)/$(MASTER_DTB_NAME) \
    --ramdisk NONE \
    --cmdline $(TRB_CMDLINE) \
    --pagesize $(PAGESIZE) \
    --base $(BASE_ADDR) \
    --tags-addr $(TAGS_ADDR) \
    --output $@
endef

define Device/Default
  DEVICE_VENDOR := TELTONIKA
  DEVICE_MODEL_TYPE = $$(shell echo $$(1) | cut -c 16-16)
  DEVICE_MODEL_REGION = $$(shell echo $$(1) | cut -c 17-17)
  DEVICE_MODEL = TRB14$$(DEVICE_MODEL_TYPE) $$(MODEM_LABEL) (Region $$(DEVICE_MODEL_REGION))
  PROFILES := Default
  DEVICE_MULTI_PROFILE_NAME := DEVICE_teltonika_trb1
  DEVICE_FEATURES := gateway mbus rs232 rs485 modbus ios single_port \
		  multi-device mobile port_link dot1x-server 128mb_ram esim-p reset_button no-wired-wan

  GPL_PREFIX := GPL
  KERNEL := master_dtb | mkimage
  KERNEL_INITRAMFS := master_dtb initramfs | mkimage initramfs
  SOC := mdm9607
  DEVICE_DTS_DIR := ../dts
  DEVICE_DTS = $$(SOC)_trb14x
  KERNEL_SIZE := 12216k
  IMAGE_SIZE := 122112k
  IMAGES := $(if $(CONFIG_BUILD_FACTORY_IMAGE),factory.bin) \
            $(if $(CONFIG_BUILD_FACTORY_IMAGE),,static.bin)
  IMAGE/static.bin := append-ubi | append-static-image | \
                      append-metadata | finalize-tlt-static
  IMAGE/dynamic.bin := append-dynamic-image | append-dyn-image-magic | \
                       append-metadata | finalize-tlt-webui
  IMAGE/factory.bin := gen-boot-config | gen-storage-ubi | gen-modem-ubi | \
                       append-ubi | append-factory-image | \
                       append-version | finalize-tlt-factory
  SUPPORTED_DEVICES := teltonika,trb14x
  # Default common packages for TRB1XX series
  # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Essential must-have:
  DEVICE_PACKAGES := kmod-at803x-phy kmod-en8801s-phy kmod-motorcomm-phy \
                     kmod-qcom-emac
  DEVICE_PACKAGES.basic := dtbtool dtc mkbootimg

  # Modem baseband related:
  DEVICE_PACKAGES += kmod-qcom-modem
  DEVICE_PACKAGES.basic += qmiproxy diag-old

  # I2C/SPI/1W peripherals:
  DEVICE_PACKAGES.basic += kmod-w1 kmod-w1-master-ds2482 kmod-w1-slave-therm \
                     kmod-w1-slave-ds28e17 spi-tools i2c-tools kmod-spi-dev
  # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  HW_MODS := motorcomm_phy trb143 econet_phy \
            quectel_embargo trb145_hw_txen econet_phy_v4 blv1

  INCLUDED_DEVICES := \
                     TEMPLATE_teltonika_trb140 \
                     TEMPLATE_teltonika_trb141 \
                     TEMPLATE_teltonika_trb142 \
                     TEMPLATE_teltonika_trb143 \
                     TEMPLATE_teltonika_trb145

  DEVICE_MODEM_VENDORS := Quectel
  DEVICE_MODEM_LIST := EC25 EC21 EC20

endef

include $(SUBTARGET).mk

$(eval $(call BuildImage))
