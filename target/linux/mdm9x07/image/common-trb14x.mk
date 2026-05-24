include $(INCLUDE_DIR)/download.mk

define Build/finalize-tlt-static
  [ -d $(BIN_DIR)/tltFws ] || mkdir -p $(BIN_DIR)/tltFws
  VERSION=$$(echo $(TLT_VERSION_FILE) | cut -b 5-); \
  DEVNAME=$$(echo $(DEVICE_NAME) | cut -c 11-17 | tr [a-z] [A-Z]); \
  $(CP) $@ $(BIN_DIR)/tltFws/$${DEVNAME}$${VERSION}_STATIC.bin
endef

define Build/finalize-tlt-factory
  [ -d $(BIN_DIR)/tltFws ] || mkdir -p $(BIN_DIR)/tltFws
  VERSION=$$(echo $(TLT_VERSION_FILE) | cut -b 5-); \
  DEVNAME=$$(echo $(DEVICE_NAME) | cut -c 11-17 | tr [a-z] [A-Z]); \
  $(CP) $@ $(BIN_DIR)/tltFws/$${DEVNAME}$${VERSION}_MASTER_STENDUI.bin
endef

define Build/append-static-image
  sh $(TOPDIR)/scripts/sysupgrade-tar.sh \
    --board $(if $(BOARD_NAME),$(BOARD_NAME),$(DEVICE_NAME)) \
    --kernel $(call param_get_default,kernel,$(1),$(IMAGE_KERNEL)) \
    --rootfs $(call param_get_default,rootfs,$(1),$@) \
  $@
endef

define Build/append-dynamic-image
  tmpdir="$$(mktemp -d 2> /dev/null)"; \
  if [ -z "$${tmpdir}" ]; then exit 1; fi; \
  mkdir -p "$${tmpdir}/sysupgrade-trb14x"; \
  echo "BOARD=trb14x" > "$${tmpdir}/sysupgrade-trb14x/CONTROL"; \
  echo "$(TRB_CMDLINE)" > "$${tmpdir}/sysupgrade-trb14x/cmdline"; \
  $(CP) "$(IMAGE_ROOTFS)" "$${tmpdir}/sysupgrade-trb14x/root"; \
  $(CP) "$(KDIR)/zImage" "$${tmpdir}/sysupgrade-trb14x/zImage"; \
  mkdir -p "$${tmpdir}/sysupgrade-trb14x/dtb"; \
  dtb_files=$$($(FIND) $(KDIR) -iname image-$(DEVICE_DTS).dtb); \
  for d in $$dtb_files; do $(CP) $$d "$${tmpdir}/sysupgrade-trb14x/dtb/"; done; \
  (cd "$${tmpdir}"; $(TAR) cvf sysupgrade.tar sysupgrade-trb14x); \
  if [ -e "$${tmpdir}/sysupgrade.tar" ]; then $(CP) "$${tmpdir}/sysupgrade.tar" "$@"; fi; \
  rm -rf "$${tmpdir}"
endef

define Build/append-dyn-image-magic
  echo -ne "\xC0\xDE\x10\x04" | cat - $@ > $@.tmp && mv $@.tmp $@
endef

define Build/gen-boot-config
  echo -ne "\x53\x54\x49\x54\x0" > "$(BIN_DIR)/boot-config.bin"
endef

define Build/gen-storage-ubi
  tmpdir="$$(mktemp -d 2> /dev/null)"; \
  if [ -z "$${tmpdir}" ]; then exit 1; fi; \
  echo -e "[storage]\nmode=ubi\nvol_id=0\nvol_type=dynamic\nvol_name=storage\nvol_size=1KiB\nvol_flags=autoresize" > \
    "$${tmpdir}/ubinize.cfg"; \
  ubinize \
    -o "$(BIN_DIR)/$(DEVICE_NAME)-storage.ubi" \
    -p $(BLOCKSIZE:%k=%KiB) \
    -m $(PAGESIZE) \
    -s $(SUBPAGESIZE) \
    "$${tmpdir}/ubinize.cfg"; \
  rm -rf $${tmpdir}
endef

define Build/gen-modem-ubi
  tmpdir="$$(mktemp -d 2> /dev/null)"; \
  if [ -z "$${tmpdir}" ]; then exit 1; fi; \
  cd "$${tmpdir}" && git clone $(TLT_GIT)/fml/quectel-modem-firmware.git; \
  cd "$${tmpdir}/quectel-modem-firmware" && git checkout $(MODEM_TAG); \
  $(CP) "$${tmpdir}/quectel-modem-firmware/modem.ubi" \
    "$(BIN_DIR)/$(DEVICE_NAME)-modem.ubi"; \
  rm -rf $${tmpdir}
endef

define Build/append-factory-image
  tmpdir="$$(mktemp -d 2> /dev/null)"; \
  if [ -z "$${tmpdir}" ]; then exit 1; fi; \
  mkdir -p "$${tmpdir}/update"; \
  $(CP) $(TOPDIR)/target/linux/mdm9x07/image/bin/* $${tmpdir}/update/; \
  mv "$${tmpdir}/update/contents.xml" "$${tmpdir}/"; \
  pagesize="$$(expr $(PAGESIZE) / 1024)"; \
  blocksize="$$(echo $(BLOCKSIZE) | cut -c -3)"; \
  [ "$(PAGESIZE)" -eq 4096 ] && sectors="131072" || sectors="262144"; \
  cd "$${tmpdir}/update" && "$(TOOLCHAIN_DIR)/usr/local/py27/bin/python2" \
    ./nand_mbn_generator.py partition_nand.xml partition.mbn $${blocksize} $${pagesize} $${sectors}; \
  [ "$$?" -ne 0 ] && exit 1 || true; \
  $(RM) "nand_mbn_generator.py"; \
  mv "partition_complete_p$${pagesize}K_b$${blocksize}K.mbn" firehose; \
  mv "patch_p$${pagesize}K_b$${blocksize}K.xml" firehose; \
  mv "rawprogram_nand_p$${pagesize}K_b$${blocksize}K_update.xml" firehose; \
  $(CP) $@ "$${tmpdir}/update/rootfs.ubi"; \
  $(CP) $(IMAGE_KERNEL) "$${tmpdir}/update/kernel.img"; \
  $(CP) $(BIN_DIR)/boot-config.bin "$${tmpdir}/update/"; \
  $(CP) $(BIN_DIR)/$(DEVICE_NAME)-storage.ubi "$${tmpdir}/update/storage.ubi"; \
  $(CP) $(BIN_DIR)/$(DEVICE_NAME)-modem.ubi "$${tmpdir}/update/modem.ubi"; \
  $(CP) $(BIN_DIR)/trb14x-aboot.mbn "$${tmpdir}/update/aboot.mbn"; \
  VERSION=$$(echo $(TLT_VERSION) | cut -b 5-); \
  DEVNAME=$$(echo $(DEVICE_NAME) | cut -c 11-17 | tr [a-z] [A-Z]); \
  echo "$${DEVNAME}$${VERSION}" > $${tmpdir}/version; \
  rm $@ && cd "$${tmpdir}" && zip -r $@ contents.xml version update/*; \
  rm -rf $${tmpdir}
endef
