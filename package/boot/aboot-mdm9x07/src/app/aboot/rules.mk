LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += \
	-I$(LK_TOP_DIR)/platform/msm_shared/include \
	-I$(LK_TOP_DIR)/lib/zlib_inflate

MODULES += lib/zlib_inflate

OBJS += \
	$(LOCAL_DIR)/aboot.o \
	$(LOCAL_DIR)/fastboot.o \
	$(LOCAL_DIR)/boot_cfg.o \
	$(LOCAL_DIR)/recovery.o

ifeq ($(ENABLE_UNITTEST_FW), 1)
OBJS += \
	$(LOCAL_DIR)/fastboot_test.o
endif

ifeq ($(ENABLE_MDTP_SUPPORT), 1)
OBJS += \
	$(LOCAL_DIR)/mdtp.o \
	$(LOCAL_DIR)/mdtp_ui.o \
	$(LOCAL_DIR)/mdtp_fuse.o \
	$(LOCAL_DIR)/mdtp_defs.o
endif

ifeq ($(ENABLE_MNFINFO_SUPPORT), 1)
MODULES += lib/mnfinfo
OBJS += $(LOCAL_DIR)/mnf_cmd.o
CFLAGS += -Wno-format-zero-length
endif
