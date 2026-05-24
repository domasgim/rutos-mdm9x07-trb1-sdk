include devices/trb14x_family.mk

define Device/teltonika_trb1
  DEVICE_MODEL := TRB14X Dynamic (All regions)
  KERNEL := append-kernel
  KERNEL_INITRAMFS := append-kernel
  KERNEL_SIZE :=
  IMAGES := dynamic.bin
endef
TARGET_DEVICES += teltonika_trb1

define Device/teltonika_trb1400
  $(Device/trb1-2k-ec25-eu)
endef
TARGET_DEVICES += teltonika_trb1400

define Device/teltonika_trb1401
  $(Device/trb1-4k-ec25-au)
endef
TARGET_DEVICES += teltonika_trb1401

define Device/teltonika_trb1404
  $(Device/trb1-4k-ec25-j)
endef
TARGET_DEVICES += teltonika_trb1404

define Device/teltonika_trb1410
  $(Device/trb1-2k-ec21-eu)
endef
TARGET_DEVICES += teltonika_trb1410

define Device/teltonika_trb1411
  $(Device/trb1-4k-ec21-au)
endef
TARGET_DEVICES += teltonika_trb1411

define Device/teltonika_trb1412
  $(Device/trb1-2k-ec25-eu)
endef
TARGET_DEVICES += teltonika_trb1412

define Device/teltonika_trb1414
  $(Device/trb1-4k-ec21-j)
endef
TARGET_DEVICES += teltonika_trb1414

define Device/teltonika_trb1420
  $(Device/trb1-2k-ec21-eu)
endef
TARGET_DEVICES += teltonika_trb1420

define Device/teltonika_trb1421
  $(Device/trb1-4k-ec21-au)
endef
TARGET_DEVICES += teltonika_trb1421

define Device/teltonika_trb1422
  $(Device/trb1-4k-ec20-ce)
endef
TARGET_DEVICES += teltonika_trb1422

define Device/teltonika_trb1423
  $(Device/trb1-4k-ec25-au)
endef
TARGET_DEVICES += teltonika_trb1423

define Device/teltonika_trb1424
  $(Device/trb1-4k-ec21-j)
endef
TARGET_DEVICES += teltonika_trb1424

define Device/teltonika_trb1430
  $(Device/trb1-2k-ec25-eu)
endef
TARGET_DEVICES += teltonika_trb1430

define Device/teltonika_trb1434
  $(Device/trb1-4k-ec25-j)
endef
TARGET_DEVICES += teltonika_trb1434

define Device/teltonika_trb1450
  $(Device/trb1-2k-ec21-eu)
endef
TARGET_DEVICES += teltonika_trb1450

define Device/teltonika_trb1451
  $(Device/trb1-4k-ec21-au)
endef
TARGET_DEVICES += teltonika_trb1451

define Device/teltonika_trb1452
  $(Device/trb1-4k-ec25-au)
endef
TARGET_DEVICES += teltonika_trb1452

define Device/teltonika_trb1454
  $(Device/trb1-4k-ec21-j)
endef
TARGET_DEVICES += teltonika_trb1454
