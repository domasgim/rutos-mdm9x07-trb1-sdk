/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of The Linux Foundation nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <app.h>
#include <debug.h>
#include <arch/arm.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <kernel/thread.h>
#include <arch/ops.h>

#include <dev/flash.h>
#include <dev/flash-ubi.h>
#include <lib/ptable.h>
#include <dev/keys.h>
#include <dev/fbcon.h>
#include <baseband.h>
#include <target.h>
#include <mmc.h>
#include <partition_parser.h>
#include <platform.h>
#include <crypto_hash.h>
#include <boot_stats.h>
#include <sha.h>
#include <platform/iomap.h>
#include <boot_device.h>
#include <boot_verifier.h>
#include <image_verify.h>
#include <decompress.h>
#include <platform/timer.h>
#if USE_RPMB_FOR_DEVINFO
#include <rpmb.h>
#endif

#if ENABLE_WBC
#include <pm_app_smbchg.h>
#endif

#if DEVICE_TREE
#include <libfdt.h>
#include <dev_tree.h>
#endif

#if WDOG_SUPPORT
#include <wdog.h>
#endif

#if ENABLE_MNFINFO_SUPPORT
#include "mnfinfo.h"
#endif

#include <reboot.h>
#include "image_verify.h"
#include "recovery.h"
#include "bootimg.h"
#include "fastboot.h"
#include "mmc.h"
#include "devinfo.h"
#include "board.h"
#include "scm.h"
#include "mdtp.h"
#include "fastboot_test.h"
#include "qpic_nand.h"

#include "boot_cfg.h"

extern  bool target_use_signed_kernel(void);
extern void platform_uninit(void);
extern void target_uninit(void);
extern int get_target_boot_params(const char *cmdline, const char *part, char **buf);

int cefs_backup_success;

extern int qseecom_test_cmd_handler(const char *arg);

void *info_buf;
static int aboot_save_boot_hash_mmc(uint32_t image_addr, uint32_t image_size);

/* fastboot command function pointer */
typedef void (*fastboot_cmd_fn) (const char *, void *, unsigned);

struct fastboot_cmd_desc {
	char * name;
	fastboot_cmd_fn cb;
};

#define EXPAND(NAME) #NAME
#define TARGET(NAME) EXPAND(NAME)

#ifndef MEMSIZE
#define MEMSIZE 1024 * 1024
#endif

#define MAX_TAGS_SIZE 1024

/* make 4096 as default size to ensure EFS,EXT4's erasing */
#define DEFAULT_ERASE_SIZE 4096
#define MAX_PANEL_BUF_SIZE 196
#define FOOTER_SIZE	   16384

#define DISPLAY_DEFAULT_PREFIX "mdss_mdp"
#define BOOT_DEV_MAX_LEN       64

#define IS_ARM64(ptr) (ptr->magic_64 == KERNEL64_HDR_MAGIC) ? true : false

#define ADD_OF(a, b) (UINT_MAX - b > a) ? (a + b) : UINT_MAX

static unsigned block_size = 0;

//Size of the header that is used in case the boot image has
//a uncompressed kernel + appended dtb
#define PATCHED_KERNEL_HEADER_SIZE 20

//String used to determine if the boot image has
//a uncompressed kernel + appended dtb
#define PATCHED_KERNEL_MAGIC "UNCOMPRESSED_IMG"

/* commands that required to check allowed or not */
#define CMD_FLASH 1
#define CMD_ERASE 2
#define CMD_BOOT  3
#define CMD_META  4

#define SYS_REV_PARTITION_NAME "sys_rev"

/*As per spec delay wait time before shutdown in Red state*/
#define DELAY_WAIT 30000
static char *quectel_debug_cmdline = " debug=0";
//add by len[for quectel cmdline] 2018-1-18
static char *quectel_cmdline = " recovery=0";
bool boot_into_debug	     = false;
//add by [francis],20180814,add debug kernel for opening in aboot
static char *quectel_dump_cmdline = " dump=0";
bool boot_into_dump		  = false;
//add by [gary] 20200911 add dump for opening in aboot
//add end
static unsigned page_size	   = 0;
static unsigned page_mask	   = 0;
static unsigned mmc_blocksize	   = 0;
static unsigned mmc_blocksize_mask = 0;
static char ffbm_mode_string[FFBM_MODE_BUF_SIZE];
static bool boot_into_ffbm;
static char *target_boot_params = NULL;
static bool boot_reason_alarm;
static bool devinfo_present = true;
bool boot_into_fastboot	    = false;
static uint32_t dt_size	    = 0;

/* Assuming unauthorized kernel image by default */
static int auth_kernel_img = 0;
#if VBOOT_MOTA
static device_info device = { DEVICE_MAGIC, 0, 0, 0, 0, { 0 }, { 0 }, { 0 } };
#else
static device_info device	= { DEVICE_MAGIC, 0, 0, 0, 0, { 0 }, { 0 }, { 0 }, 1 };
#endif
static bool is_allow_unlock = 0;

static char frp_ptns[2][8] = { "config", "frp" };

static const char *critical_flash_allowed_ptn[] = { "aboot", "rpm", "tz",   "sbl",	  "sdi",     "sbl1",
						    "xbl",   "hyp", "pmic", "bootloader", "devinfo", "partition" };

struct atag_ptbl_entry {
	char name[16];
	unsigned offset;
	unsigned size;
	unsigned flags;
};

/*
 * Partition info, required to be published
 * for fastboot
 */
struct getvar_partition_info {
	const char part_name[MAX_GPT_NAME_SIZE]; /* Partition name */
	char getvar_size[MAX_GET_VAR_NAME_SIZE]; /* fastboot get var name for size */
	char getvar_type[MAX_GET_VAR_NAME_SIZE]; /* fastboot get var name for type */
	char size_response[MAX_RSP_SIZE];        /* fastboot response for size */
	char type_response[MAX_RSP_SIZE];        /* fastboot response for type */
};

#if ENABLE_FETCH_SUPPORT
/* Offsets and sizes in blocks (I guess...)
    0:  SBL         offs=0x00000000 size=0x0000000a attr: 0x00ff01ff
    1:  MIBIB       offs=0x0000000a size=0x0000000a attr: 0x00ff01ff
    2:  EFS2        offs=0x00000014 size=0x000000b0 attr: 0x00ff01ff
    3:  RAWDATA     offs=0x000000c4 size=0x00000018 attr: 0x000001ff
    4:  TZ          offs=0x000000dc size=0x0000000a attr: 0x000001ff
    5:  RPM         offs=0x000000e6 size=0x0000000a attr: 0x000001ff
    6:  aboot       offs=0x000000f0 size=0x0000000a attr: 0x000001ff
    7:  mnf_info    offs=0x000000fa size=0x0000000a attr: 0x000001ff
    8:  boot_config offs=0x00000104 size=0x0000000a attr: 0x000001ff
    9:  boot_a      offs=0x0000010e size=0x00000048 attr: 0x000001ff
    10: boot_b      offs=0x00000156 size=0x00000048 attr: 0x000001ff
    11: modem       offs=0x0000019e size=0x000001e0 attr: 0x000001ff
    12: rootfs_a    offs=0x0000037e size=0x000003ba attr: 0x000001ff
    13: rootfs_b    offs=0x00000738 size=0x000003ba attr: 0x000001ff
    14: storage     offs=0x00000af2 size=0x0000050e attr: 0x000001ff
 */

/* Size in bytes */
struct getvar_partition_info part_info[] =
{
	{ "sbl", 			"partition-size:", "partition-type:", 	"0x00280000", "" },
	{ "mibib", 			"partition-size:", "partition-type:", 	"0x00280000", "" },
	{ "efs2", 			"partition-size:", "partition-type:", 	"0x01600000", "" },
	{ "rawdata", 		"partition-size:", "partition-type:", 	"0x00300000", "" },
	{ "tz", 			"partition-size:", "partition-type:", 	"0x00140000", "" },
	{ "rpm", 			"partition-size:", "partition-type:", 	"0x00180000", "" },
	{ "aboot", 			"partition-size:", "partition-type:", 	"0x00140000", "" },
	{ "mnf_info", 		"partition-size:", "partition-type:", 	"0x00140000", "" },
	{ "boot_config", 	"partition-size:", "partition-type:", 	"0x00140000", "" },
	{ "boot_a", 		"partition-size:", "partition-type:", 	"0x00900000", "" },
	{ "boot_b", 		"partition-size:", "partition-type:", 	"0x00900000", "" },
	{ "modem", 			"partition-size:", "partition-type:", 	"0x03c00000", "" },
	{ "rootfs_a", 		"partition-size:", "partition-type:", 	"0x07740000", "" },
	{ "rootfs_b", 		"partition-size:", "partition-type:", 	"0x07740000", "" },
	{ "storage", 		"partition-size:", "partition-type:", 	"0x09f00000", "" },

	{ "all", 			"partition-size:", "partition-type:", 	"0x20000000", "" },
};

char max_fetch_size[MAX_RSP_SIZE];
#endif //ENABLE_FETCH_SUPPORT

char max_download_size[MAX_RSP_SIZE];
char sn_buf[13];
char display_panel_buf[MAX_PANEL_BUF_SIZE];
char panel_display_mode[MAX_RSP_SIZE];

#if NO_KEYPAD_DRIVER
extern int fastboot_trigger(void);
#endif

#if 1 // modem backup  use a individual partition for modem restore.

#define DATA_CACHE_LEN	 (4200) //һpage 4096ֽ
#define CEFS_FILE_MAGIC1 (0x51D24368)
#define CEFS_FILE_MAGIC2 (0x4378AC6E)

#define NEW_CEFS_FILE_MAGIC1 (0x52D24368)
#define NEW_CEFS_FILE_MAGIC2 (0x4378AC6E)

#define QUEC_BACKUP_MAGIC1 (0x78E5D4C2)
#define QUEC_BACKUP_MAGIC2 (0x54F7D60E)

//add by francis ,20180613,add data magic for linux
#define QUEC_DATA_MAGIC1 (0x5F3759DF)
#define QUEC_DATA_MAGIC2 (0x43A2E930)

#define QUEC_BACKUP_INFO_BLOCK_NUMS (3) // the 3 block used for save restore flag
//the "Cache_EFS2" partition last CEFS_BACKUP_INFO_BLOCK_NUMS blocks for save restroe flag
//#ifdef QUECTEL_ALL_RESTORE
//#define QUEC_ALL_RESTORE_FLAG_BLOCK_INDEX (6) // the 3 block reserved for All parition restoring flag
//the "Cache_EFS2" partition last QUEC_ALL_RESTORE_FLAG_BLOCK_INDEX to CEFS_BACKUP_INFO_BLOCK_NUMS reserved for this flag store.
//#endif

#define BACKUP_SYSTEM_SIZE     (50 * 1024 * 1024)
#define BACKUP_EFS2_SIZE       (3 * 1024 * 1024) // 3M for cefs backup is enough
#define BACKUP_MODEM_SIZE      (50 * 1024 * 1024) //  modem ubi backup 50M,
#define BACKUP_RECOVERYFS_SIZE (13 * 1024 * 1024)
#define BACKUP_IMAGE_SIZE      (8 * 1024 * 1024) // boot img backup size

static char temp_buf[4096];
#define BACKUP_INFO_BLOCK_NUMS (3)

typedef enum {
	QUECTEL_RESTOREFLAG_NONE = 0,
	QUECTEL_RESTOREFLAG_LINUXFS,
	QUECTEL_RESTOREFLAG_MODEM,
	QUECTEL_RESTOREFLAG_IMAGE,
	QUECTEL_RESTOREFLAG_RECOVERYFS,
#ifdef QUECTEL_ALL_RESTORE
	QUECTEL_ALL_RESTORE_BEGIN,
#endif
	QUECTEL_RESTOREFLAG_CEFS = 100,
	QUECTEL_CHECKEFSFLAG_ERR = 101,

} quectel_RestoreFlg_type;

typedef struct {
	uint32 magic1;
	uint32 magic2;
	uint32 page_count;
	uint32 data_crc;

	uint32 reserve1;
	uint32 reserve2;
	uint32 reserve3;
	uint32 reserve4;
} quec_cefs_file_header_type;

typedef struct {
	char ql_mtd_name[12];
	char ql_mtd_restore_name[12];
	uint64_t total_size;
	uint32_t ql_mtd_nub;
	uint32_t restore_flag;
	uint32_t restore_times;
	uint32_t backup_times;
	uint32_t crash[12];
} Ql_Mtd_Info;

typedef struct {
	uint32_t magic1;
	uint32_t magic2;

	uint32_t cefs_restore_flag;
	uint32_t cefs_restore_times;
	uint32_t cefs_backup_times;
	uint32_t cefs_crash[10]; // cefs crash where

	// Ramos add for linux fs backup restore times
	uint32_t linuxfs_restore_flag;
	uint32_t linuxfs_restore_times;
	uint32_t linuxfs_backup_times;
	uint32_t linuxfs_crash[10]; //  linux fs crash where

	// modem backup restore flag
	uint32_t modem_restore_flag;
	uint32_t modem_restore_times;
	uint32_t modem_backup_times;
	uint32_t modem_crash[10]; // modem crash where

	// other image restore flag
	uint32_t image_restoring_flag;
	uint32_t reserved1;
	uint32_t reserved2[100];

	uint32_t data_magic1;
	uint32_t data_magic2;

	Ql_Mtd_Info ql_mtd_info[6];
	//add by [Francis.huan] ,20180416 ,partition info that who  need restore or erase
} quec_backup_info_type;

extern uint32 Q_crc_32_calc(uint8 *buf_ptr, uint16 len, uint32 seed);
int Ql_SetRestorecountClearFlag(const char *ql_mtd_clear_name)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	uint32_t offset	   = 0;
	uint32_t pagesize  = flash_page_size();
	uint32_t blocksize = flash_block_size();
	quec_backup_info_type QuecBackupInfo;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "@Ramos ERROR: Partition table not found\n");
		return -1;
	}

	ptn = ptable_find(ptable, SYS_REV_PARTITION_NAME);
	if (ptn == NULL) {
		dprintf(CRITICAL, "                    %s %d\n", __func__, __LINE__);
		dprintf(CRITICAL, "@Ramos ERROR: No misc partition found\n");
		return -1;
	}

	offset = (ptn->length - QUEC_BACKUP_INFO_BLOCK_NUMS) * blocksize; // offset, read restore information flag
	memset((void *)temp_buf, 0x00, pagesize);
	if (Quectel_flash_nand_read(ptn, 0, offset, (void *)temp_buf, pagesize)) {
		dprintf(CRITICAL, "@Ramos ERROR: Cannot read Restore Flag header\n");
		return -1;
	}

	memcpy(&QuecBackupInfo, temp_buf, sizeof(QuecBackupInfo));
	// modeify by [francis.huan],20180416,match mtd for restore partition
	for (int i = 0; i < 6; i++) {
		if (!strcmp(QuecBackupInfo.ql_mtd_info[i].ql_mtd_name, ql_mtd_clear_name)) {
			QuecBackupInfo.ql_mtd_info[i].restore_times++;
			dprintf(INFO, "\r\n\r\n\r\nrestore times=%d!!!\r\n\r\n\r\n",
				QuecBackupInfo.ql_mtd_info[i].restore_times);
			QuecBackupInfo.ql_mtd_info[i].restore_flag = 0;
		}
	}
	if (!strcmp("efs2", ql_mtd_clear_name)) {
		QuecBackupInfo.cefs_restore_flag = 0;
		QuecBackupInfo.cefs_restore_times++;
		dprintf(INFO, "\r\n\r\n\r\ncefs restore times=%d!!!\r\n\r\n\r\n", QuecBackupInfo.cefs_restore_times);
	}
	memcpy(temp_buf, &QuecBackupInfo, sizeof(QuecBackupInfo));
	offset = (ptn->length - QUEC_BACKUP_INFO_BLOCK_NUMS) * blocksize; // offset ,write restore information
	if (Quectel_flash_write(ptn, offset, 0, temp_buf, pagesize)) {
		dprintf(CRITICAL, "@Ramos ERROR: flash write fail!\n");
		return -1;
	}
	memset((void *)temp_buf, 0x00, pagesize);
	if (flash_read(ptn, offset, (void *)temp_buf, pagesize)) {
		dprintf(CRITICAL, "@Ramos ERROR: Cannot read Restore Flag header\n");
		return -1;
	}
	return 0;
}

quectel_RestoreFlg_type Ql_check_RestoreFlag(void)
{
	quec_backup_info_type QuecBackupInfo;
	struct ptentry *ptn;
	struct ptable *ptable;
	uint32_t offset	   = 0;
	uint32_t pagesize  = flash_page_size();
	uint32_t blocksize = flash_block_size();
	int result	   = 0;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "@Ramos ERROR: Partition table not found\n");
		return QUECTEL_RESTOREFLAG_NONE;
	}
	ptn = ptable_find(ptable, SYS_REV_PARTITION_NAME);
	if (ptn == NULL) {
		dprintf(CRITICAL, "                    %s %d\n", __func__, __LINE__);
		dprintf(CRITICAL, "@Ramos  ERROR: No misc partition found\n");
		return QUECTEL_RESTOREFLAG_NONE;
	}

	offset = (ptn->length - QUEC_BACKUP_INFO_BLOCK_NUMS) * blocksize; // offset, read restore information
	memset((void *)temp_buf, 00, pagesize);
	result = Quectel_flash_nand_read(ptn, 0, offset, (void *)temp_buf, pagesize);
	if (NANDC_RESULT_BAD_PAGE == result) {
		/* NANDC_RESULT_BAD_PAGE == result  that means the block unsteadiness, In the Quectel_flash_nand_read function we have erased this block
		but FAE of naya think maby the block have bitfilp also, now we write it to aviod this issue
		*/
		QuecBackupInfo.magic1 = NEW_CEFS_FILE_MAGIC1;
		QuecBackupInfo.magic2 = NEW_CEFS_FILE_MAGIC2;

		memset((void *)temp_buf, 0xFF, pagesize);
		memcpy(temp_buf, &QuecBackupInfo, sizeof(QuecBackupInfo));
		Quectel_flash_write(ptn, offset, 0, temp_buf, pagesize);
		dprintf(INFO, "@Ramos the system restore flag block error!!!!!\n");
		// reboot system
		mdelay(200);
		reboot_device(0);
	} else if (NANDC_RESULT_SUCCESS != result) {
		dprintf(CRITICAL, "@Ramos ERROR: Cannot read Restore Flag header\n");
		return QUECTEL_RESTOREFLAG_NONE;
	}

	memcpy((void *)&QuecBackupInfo, temp_buf, sizeof(quec_backup_info_type));
	dprintf(CRITICAL, "@Ramos Ql_check_RestoreFlag:offset=%x, magic1=%x,magic2=%x", offset, QuecBackupInfo.magic1,
		QuecBackupInfo.magic2);

	if ((QUEC_BACKUP_MAGIC1 != QuecBackupInfo.magic1) || (QUEC_BACKUP_MAGIC2 != QuecBackupInfo.magic2)) {
		return QUECTEL_RESTOREFLAG_NONE;
	}

	struct recovery_message msg1;
	memset(&msg1, 0, sizeof(msg1));
	// get recovery message
	if (get_recovery_message(&msg1) == 0) {
		dprintf(CRITICAL, "[carl]msg1.backup_flag = %s\n", msg1.backup_flag);
		if (strncmp("fail", msg1.backup_flag, 4) == 0) {
			dprintf(CRITICAL, "[carl] quec_backup_cefs is err\n");
			QuecBackupInfo.cefs_restore_flag = 0;
			memset((void *)temp_buf, 0xFF, pagesize);
			memcpy(temp_buf, &QuecBackupInfo, sizeof(QuecBackupInfo));
			Quectel_flash_write(ptn, offset, 0, temp_buf, pagesize);
			mdelay(200);
			set_recovery_message(&msg1);
			cefs_backup_success = 1;
			return QUECTEL_RESTOREFLAG_NONE;
		}
		dprintf(CRITICAL, "[carl] quec_backup_cefs is ok\n");
	}

	if (1 == QuecBackupInfo.cefs_restore_flag) {
		return QUECTEL_RESTOREFLAG_CEFS;
	}

	dprintf(CRITICAL, "@francis QuecBackupInfo.cefs_restore_flag=%d \n", QuecBackupInfo.cefs_restore_flag);
	if ((QUEC_DATA_MAGIC1 != QuecBackupInfo.data_magic1) || (QUEC_DATA_MAGIC2 != QuecBackupInfo.data_magic2))
		return QUECTEL_RESTOREFLAG_NONE;

	for (int i = 0; i < 6; i++) {
		if (QuecBackupInfo.ql_mtd_info[i].ql_mtd_name[0] != 0 &&
		    strlen(QuecBackupInfo.ql_mtd_info[i].ql_mtd_name) != 0) {
			dprintf(CRITICAL, "Ql_check_RestoreFlag mtd :name=%s, mtd_nub=%d,mtd_flag=%d\n",
				QuecBackupInfo.ql_mtd_info[i].ql_mtd_name, QuecBackupInfo.ql_mtd_info[i].ql_mtd_nub,
				QuecBackupInfo.ql_mtd_info[i].restore_flag);
			if (1 == QuecBackupInfo.ql_mtd_info[i].restore_flag)
				return QuecBackupInfo.ql_mtd_info[i].ql_mtd_nub;
		}
	}

	//struct recovery_message msg1;
	memset(&msg1, 0, sizeof(msg1));
	// get recovery message
	if (get_recovery_message(&msg1) == 0) {
		if (strncmp("first", msg1.first_in, 4) == 0) {
			return QUECTEL_CHECKEFSFLAG_ERR;
		}
	}

	return QUECTEL_RESTOREFLAG_NONE;
}

#endif

/**
 * Author : Darren
 * Date : 2017/7/21
 * get_ubiimg_size -- the function can accurately get ubi
 * image.the flash bad block unaffect this fuction.
 * 0 -- get ubi image size failed
 * positive -- get ubi image size success
 */
static int32_t get_ubiimg_size(struct ptentry *ptn)
{
	uint32_t offset		    = 0;
	uint32_t image_size_in_byte = 0;
	void *pagebuf		    = NULL;
	uint32_t pagesize	    = flash_page_size();
	uint32_t blocksize	    = flash_block_size();
	if (ptn == NULL) {
		dprintf(INFO, "%s:invalid parameter ptn\n", __func__);
		return 0;
	}
	pagebuf = malloc(pagesize);
	ASSERT(pagebuf);
	memset(pagebuf, 0, pagesize);
	dprintf(INFO, "get \"%s\" partition ubi image size!\n", ptn->name);
	do {
		flash_read(ptn, offset, pagebuf, pagesize);
		if (!memcmp((void *)pagebuf, UBI_MAGIC, UBI_MAGIC_SIZE)) {
			offset += blocksize;
			memset(pagebuf, 0, pagesize);
			continue;
		}
		image_size_in_byte = offset;
		break;
	} while (1);
	dprintf(INFO, "%s,pagesize = %d KiB blocksize = %d KiB, img_size = %d KiB\n", __func__, pagesize / 1024,
		blocksize / 1024, image_size_in_byte / 1024);
	free(pagebuf);
	return image_size_in_byte;
}

int Ql_SetRestorecrcErrFlag(void)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	uint32_t offset	   = 0;
	uint32_t pagesize  = flash_page_size();
	uint32_t blocksize = flash_block_size();
	quec_backup_info_type QuecBackupInfo;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "@Ramos ERROR: Partition table not found\n");
		return -1;
	}
	ptn = ptable_find(ptable, "cache_efs2");

	if (ptn == NULL) {
		dprintf(CRITICAL, "                    %s %d\n", __func__, __LINE__);
		dprintf(CRITICAL, "@Ramos ERROR: No misc partition found\n");
		return -1;
	}

	offset = (ptn->length - QUEC_BACKUP_INFO_BLOCK_NUMS) * blocksize; // offset, read restore information flag
	memset((void *)temp_buf, 0x00, pagesize);
	if (Quectel_flash_nand_read(ptn, 0, offset, (void *)temp_buf, pagesize)) {
		dprintf(CRITICAL, "@Ramos ERROR: Cannot read Restore Flag header\n");
		return -1;
	}
	memcpy(&QuecBackupInfo, temp_buf, sizeof(QuecBackupInfo));
	// modeify by [francis.huan],20180416,match mtd for restore partition
	{
		QuecBackupInfo.reserved1 = 1;
	}
	memcpy(temp_buf, &QuecBackupInfo, sizeof(QuecBackupInfo));
	offset = (ptn->length - QUEC_BACKUP_INFO_BLOCK_NUMS) * blocksize; // offset ,write restore information
	if (Quectel_flash_write(ptn, offset, 0, temp_buf, pagesize)) {
		dprintf(CRITICAL, "@Ramos ERROR: flash write fail!\n");
		return -1;
	}
	memset((void *)temp_buf, 0x00, pagesize);
	if (flash_read(ptn, offset, (void *)temp_buf, pagesize)) {
		dprintf(CRITICAL, "@Ramos ERROR: Cannot read Restore Flag header\n");
		return -1;
	}
	return 0;
}

bool Ql_Restore_partition(int mtd_nub)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned extra = 0;
	char *data     = (char *)0x89000000;
	uint32_t sz    = 0;
	struct recovery_message msg;
	uint32_t offset	   = 0;
	uint32_t pagesize  = flash_page_size();
	uint32_t blocksize = flash_block_size();

	//add by [francis.huan] , 20180416, get quecbackup info
	quec_backup_info_type QuecBackupInfo;
	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "@Ramos ERROR: Partition table not found\n");
		return -1;
	}
	//read restroe data form the  backup  partition
	ptn = ptable_find(ptable, SYS_REV_PARTITION_NAME);
	if (ptn == NULL) {
		dprintf(CRITICAL, "@Ramos ERROR: No %s partition found\n", SYS_REV_PARTITION_NAME);
		return -1;
	}

	offset = (ptn->length - QUEC_BACKUP_INFO_BLOCK_NUMS) * blocksize; // offset, read restore informati
	memset((void *)temp_buf, 0x00, pagesize);
	if (Quectel_flash_nand_read(ptn, 0, offset, (void *)temp_buf, pagesize)) {
		dprintf(CRITICAL, "@Ramos ERROR: Cannot read Restore Flag header\n");
		return -1;
	}
	memcpy(&QuecBackupInfo, temp_buf, sizeof(QuecBackupInfo));
	//datalen = BACKUP_SYSTEM_SIZE;   // read enough vaild data form the backup partition,

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		fastboot_fail("partition table doesn't exist");
		return FALSE;
	}
	//read restroe data form the  backup  partition
	for (int i = 0; i < 6; i++) {
		if (QuecBackupInfo.ql_mtd_info[i].ql_mtd_nub != mtd_nub) {
			continue;
		}

		dprintf(CRITICAL, "Ramos QuecBackupInfo.ql_mtd_info[i].ql_mtd_restore_name =%s\n",
			QuecBackupInfo.ql_mtd_info[i].ql_mtd_restore_name);

		if (strlen(QuecBackupInfo.ql_mtd_info[i].ql_mtd_restore_name) == 0) {
			dprintf(INFO, "erase partition name (%s). Trying updatevol\n",
				QuecBackupInfo.ql_mtd_info[i].ql_mtd_name);
			flash_erase(ptable_find(ptable, QuecBackupInfo.ql_mtd_info[i].ql_mtd_name));
			for (int i = 0; i < 6; i++) {
				if (QuecBackupInfo.ql_mtd_info[i].ql_mtd_nub == mtd_nub)
					Ql_SetRestorecountClearFlag(QuecBackupInfo.ql_mtd_info[i].ql_mtd_name);
			}
			return TRUE;
		}

		ptn = ptable_find(ptable, QuecBackupInfo.ql_mtd_info[i].ql_mtd_restore_name);
		if (ptn == NULL) {
			dprintf(INFO, "unknown partition name (%s). Trying erase\n",
				QuecBackupInfo.ql_mtd_info[i].ql_mtd_name);
			flash_erase(ptable_find(ptable, QuecBackupInfo.ql_mtd_info[i].ql_mtd_name));
			return FALSE;
		}

		dprintf(INFO, "@Ramos Get backup partition:%s, size=%d\r\n", ptn->name, ptn->length);
		data = (char *)VA((addr_t)data);
		sz   = Ql_flash_read_ext(ptn, 0, 0, data);
		if (sz == 0) {
			sz = QuecBackupInfo.ql_mtd_info[i].total_size;
		}

		// find the restroe partition
		ptn = ptable_find(ptable, QuecBackupInfo.ql_mtd_info[i].ql_mtd_name);
		if (ptn == NULL) {
			dprintf(INFO, "@Ramos unknown partition name (%s).\n",
				QuecBackupInfo.ql_mtd_info[i].ql_mtd_name);
			return FALSE;
		}
		dprintf(INFO, "@Ramos Restroe  partition:%s, size=%d\r\n", ptn->name, ptn->length);
	}

	if (mtd_nub == QUECTEL_RESTOREFLAG_CEFS || mtd_nub == QUECTEL_CHECKEFSFLAG_ERR) {
		dprintf(INFO, "@Ramos get efs data\n");
		ptn  = ptable_find(ptable, SYS_REV_PARTITION_NAME);
		data = (char *)VA((addr_t)data);
		flash_read(ptn, 0, data, BACKUP_EFS2_SIZE);
	}

	if (!strcmp(ptn->name, "boot") || !strcmp(ptn->name, "recovery")) {
		if (memcmp((void *)data, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
			dprintf(INFO, "@Ramos image is not a boot image");
			return FALSE;
		}
	}

	char *dest_ptn;
	for (int i = 0; i < 6; i++) {
		if (QuecBackupInfo.ql_mtd_info[i].ql_mtd_nub == mtd_nub)
			dest_ptn = QuecBackupInfo.ql_mtd_info[i].ql_mtd_name;
	}

	if (mtd_nub == QUECTEL_RESTOREFLAG_CEFS || mtd_nub == QUECTEL_CHECKEFSFLAG_ERR) {
		// Cache_EFS2 first page is use save the cefs backup information ,header and crc value
		quec_cefs_file_header_type BackupCefs_Info;
		uint32_t crc = 0;
		uint32_t i   = 0;
		static char s_buff_temp[DATA_CACHE_LEN];
		memset((void *)&BackupCefs_Info, 0x00, sizeof(quec_cefs_file_header_type));
		memcpy((void *)&BackupCefs_Info, data, sizeof(quec_cefs_file_header_type));
		dprintf(INFO, "@Ramos efs2 restore file magic1 =%x,magic2=%x !\r\n\r\n", BackupCefs_Info.magic1,
			BackupCefs_Info.magic2);

		if (((CEFS_FILE_MAGIC1 != BackupCefs_Info.magic1) || (CEFS_FILE_MAGIC2 != BackupCefs_Info.magic2)) &&
		    ((NEW_CEFS_FILE_MAGIC1 != BackupCefs_Info.magic1) ||
		     (NEW_CEFS_FILE_MAGIC2 != BackupCefs_Info.magic2))) {
			dprintf(INFO, "@Ramos efs2 restore file magic1 error !!!\r\n\r\n");

			strlcpy(msg.command, "", sizeof(msg.command)); // to safe against multiple reboot into recovery
			strlcpy(msg.status, "OKAY", sizeof(msg.status));
			set_recovery_message(&msg); // send recovery message
			if (0 == Ql_SetRestorecrcErrFlag()) {
				mdelay(1000);
				reboot_device(0);
			}
			return FALSE;
		}

		sz   = (BackupCefs_Info.page_count) * pagesize;
		data = data + pagesize;

		for (i = 0; i < BackupCefs_Info.page_count; i++) {
			memset(s_buff_temp, 0xFF,
			       sizeof(s_buff_temp)); // set buff to 0xff, because modem backup cefs set it is 0xff,  or  crc check error
			memcpy(s_buff_temp, (data + i * pagesize), pagesize);
			crc = Q_crc_32_calc((void *)s_buff_temp, 2048 * 8, crc); // 2048*8 keep with modem side
		}

		dprintf(INFO, "@Ramos efs2 restore sz=%d, page_count=%d,file magic1=%x,crc=%x,data_crc=%x\n", sz,
			BackupCefs_Info.page_count, BackupCefs_Info.magic1, crc, BackupCefs_Info.data_crc);
		if (crc != BackupCefs_Info.data_crc) {
			dprintf(INFO, "@Ramos efs2 restore file CRC check  error !!!\r\n\r\n");

			strlcpy(msg.command, "", sizeof(msg.command)); // to safe against multiple reboot into recovery
			strlcpy(msg.status, "OKAY", sizeof(msg.status));
			set_recovery_message(&msg); // send recovery message
			if (0 == Ql_SetRestorecrcErrFlag()) {
				mdelay(1000);
				reboot_device(0);
			}

			return FALSE;
		}

		if (mtd_nub == QUECTEL_CHECKEFSFLAG_ERR)
			return TRUE;

		ptn = ptable_find(ptable, "efs2");
		flash_erase(ptn); // efs partition erase all
	}

	sz = ROUND_TO_PAGE(sz, page_mask);
	dprintf(INFO, "@Ramos writing 0x%x bytes to '%s'\n", sz, ptn->name);
	Quectel_flash_erase(ptn, sz); // erase the partiton first
	if (flash_write(ptn, 0, data, sz)) //this fuction not use the write restore flag
	{
		dprintf(INFO, "@Ramos flash write failure !!!!!\r\n\r\n\r\n");
		return FALSE;
	}

	//clear the restroe flag , record the restroe tiems
	Ql_SetRestorecountClearFlag(ptn->name);
	dprintf(INFO, "partition '%s' Restroe  succeed, reboot Now !!!!!\n", ptn->name);
	//  success ,  reboot
	mdelay(1000);
	reboot_device(0);
	return TRUE;
}

static void update_ker_tags_rdisk_addr(struct boot_img_hdr *hdr, bool is_arm64)
{
	/* overwrite the destination of specified for the project */
#ifdef ABOOT_IGNORE_BOOT_HEADER_ADDRS
	if (is_arm64)
		hdr->kernel_addr = ABOOT_FORCE_KERNEL64_ADDR;
	else
		hdr->kernel_addr = ABOOT_FORCE_KERNEL_ADDR;
	hdr->ramdisk_addr = ABOOT_FORCE_RAMDISK_ADDR;
	hdr->tags_addr = ABOOT_FORCE_TAGS_ADDR;

#endif
}

static void ptentry_to_tag(unsigned **ptr, struct ptentry *ptn)
{
	struct atag_ptbl_entry atag_ptn;

	memcpy(atag_ptn.name, ptn->name, 16);
	atag_ptn.name[15] = '\0';
	atag_ptn.offset = ptn->start;
	atag_ptn.size = ptn->length;
	atag_ptn.flags = ptn->flags;
	memcpy(*ptr, &atag_ptn, sizeof(struct atag_ptbl_entry));
	*ptr += sizeof(struct atag_ptbl_entry) / sizeof(unsigned);
}

unsigned *atag_core(unsigned *ptr)
{
	/* CORE */
	*ptr++ = 2;
	*ptr++ = 0x54410001;

	return ptr;

}

unsigned *atag_ramdisk(unsigned *ptr, void *ramdisk,
							   unsigned ramdisk_size)
{
	if (ramdisk_size) {
		*ptr++ = 4;
		*ptr++ = 0x54420005;
		*ptr++ = (unsigned)ramdisk;
		*ptr++ = ramdisk_size;
	}

	return ptr;
}

unsigned *atag_ptable(unsigned **ptr_addr)
{
	int i;
	struct ptable *ptable;

	if ((ptable = flash_get_ptable()) && (ptable->count != 0)) {
		*(*ptr_addr)++ = 2 + (ptable->count * (sizeof(struct atag_ptbl_entry) /
							sizeof(unsigned)));
		*(*ptr_addr)++ = 0x4d534d70;
		for (i = 0; i < ptable->count; ++i)
			ptentry_to_tag(ptr_addr, ptable_get(ptable, i));
	}

	return (*ptr_addr);
}

unsigned *atag_cmdline(unsigned *ptr, const char *cmdline)
{
	int cmdline_length = 0;
	int n;
	char *dest;

	cmdline_length = strlen((const char*)cmdline);
	n = (cmdline_length + 4) & (~3);

	*ptr++ = (n / 4) + 2;
	*ptr++ = 0x54410009;
	dest = (char *) ptr;
	while ((*dest++ = *cmdline++));
	ptr += (n / 4);

	return ptr;
}

unsigned *atag_end(unsigned *ptr)
{
	/* END */
	*ptr++ = 0;
	*ptr++ = 0;

	return ptr;
}

void generate_atags(unsigned *ptr, const char *cmdline,
                    void *ramdisk, unsigned ramdisk_size)
{

	ptr = atag_core(ptr);
	ptr = atag_ramdisk(ptr, ramdisk, ramdisk_size);
	ptr = target_atag_mem(ptr);
	ptr = atag_cmdline(ptr, cmdline);
	ptr = atag_end(ptr);
}

typedef void entry_func_ptr(unsigned, unsigned, unsigned*);
void boot_linux(void *kernel, unsigned *tags,
		const char *cmdline, unsigned machtype,
		void *ramdisk, unsigned ramdisk_size, enum boot_cfg cfg)
{
	entry_func_ptr *entry = (entry_func_ptr *) (PA((addr_t) kernel));
	uint32_t tags_phys = PA((addr_t) tags);
	struct kernel64_hdr *kptr = (struct kernel64_hdr *) (PA((addr_t) kernel));
	char *final_cmdline;

	ramdisk = (void *)PA((addr_t) ramdisk);

	final_cmdline = boot_cfg_cmdline_parse(cmdline, cfg);
	if (!final_cmdline) {
		dprintf(CRITICAL, "Failed to get the final command line");
		ASSERT(0);
	}

#if DEVICE_TREE
	if(update_device_tree((void *)tags, final_cmdline, ramdisk, ramdisk_size)) {
		dprintf(CRITICAL, "ERROR: Updating Device Tree Failed \n");
		ASSERT(0);
	}
#else
	/* Generating the Atags */
	generate_atags(tags, final_cmdline, ramdisk, ramdisk_size);
#endif

	dprintf(INFO, "boot configuration: %s, kernel: %s, rootfs: %s\n",
		boot_configs[cfg].name,
		boot_configs[cfg].kernel,
		boot_configs[cfg].rootfs);

	dprintf(INFO, "cmdline: %s\n", final_cmdline);

	dprintf(INFO, "booting linux @%p, ramdisk @%p (%d), tags/device tree @%p\n",
		entry, ramdisk, ramdisk_size, (void *)tags_phys);

	free(final_cmdline);

	/* Perform target specific cleanup */
	target_uninit();

	enter_critical_section();

	/* Initialise wdog to catch early kernel crashes */
#if WDOG_SUPPORT
	msm_wdog_init();
#endif
	/* do any platform specific cleanup before kernel entry */
	platform_uninit();

	arch_disable_cache(UCACHE);

#if ARM_WITH_MMU
	arch_disable_mmu();
#endif
	bs_set_timestamp(BS_KERNEL_ENTRY);

	if (IS_ARM64(kptr))
		/* Jump to a 64bit kernel */
		scm_elexec_call((paddr_t)kernel, tags_phys);
	else
		/* Jump to a 32bit kernel */
		entry(0, machtype, (unsigned*)tags_phys);
}

/* Function to check if the memory address range falls within the aboot
 * boundaries.
 * start: Start of the memory region
 * size: Size of the memory region
 */
int check_aboot_addr_range_overlap(uint32_t start, uint32_t size)
{
	/* Check for boundary conditions. */
	if ((UINT_MAX - start) < size)
		return -1;

	/* Check for memory overlap. */
	if ((start < MEMBASE) && ((start + size) <= MEMBASE))
		return 0;
	else if (start >= (MEMBASE + MEMSIZE))
		return 0;
	else
		return -1;
}

#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))

BUF_DMA_ALIGN(buf, BOOT_IMG_MAX_PAGE_SIZE); //Equal to max-supported pagesize
#if DEVICE_TREE
BUF_DMA_ALIGN(dt_buf, BOOT_IMG_MAX_PAGE_SIZE);
#endif

static void verify_signed_bootimg(uint32_t bootimg_addr, uint32_t bootimg_size)
{
	int ret;

#if !VERIFIED_BOOT
#if IMAGE_VERIF_ALGO_SHA1
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA1;
#else
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA256;
#endif
#endif

	dprintf(INFO, "Authenticating boot image (%d): start\n", bootimg_size);

#if VERIFIED_BOOT
	ret = boot_verify_image((unsigned char *)bootimg_addr,
			bootimg_size, "/boot");
	boot_verify_print_state();
#else
	ret = image_verify((unsigned char *)bootimg_addr,
					   (unsigned char *)(bootimg_addr + bootimg_size),
					   bootimg_size,
					   auth_algo);
#endif
	dprintf(INFO, "Authenticating boot image: done return value = %d\n", ret);

#ifdef MDTP_SUPPORT
	{
		/* Verify MDTP lock.
		 * For boot & recovery partitions, use aboot's verification result.
		 */
		mdtp_ext_partition_verification_t ext_partition;
		ext_partition.partition = MDTP_PARTITION_BOOT;
		ext_partition.integrity_state = MDTP_PARTITION_STATE_VALID;
		ext_partition.page_size = 0; /* Not needed since already validated */
		ext_partition.image_addr = 0; /* Not needed since already validated */
		ext_partition.image_size = 0; /* Not needed since already validated */
		ext_partition.sig_avail = FALSE; /* Not needed since already validated */
		mdtp_fwlock_verify_lock(&ext_partition);
	}
#endif /* MDTP_SUPPORT */

#if VERIFIED_BOOT
	if(boot_verify_get_state() == RED)
	{
		dprintf(CRITICAL, "Device verification failed.\n");
		assert(0);
	}
#endif
}

void boot_verifier_init()
{
	uint32_t boot_state;

	boot_verify_send_event(BOOT_INIT);

	/* Initialize keystore */
	boot_state = boot_verify_keystore_init();
	if(boot_state == YELLOW)
	{
		boot_verify_print_state();
		dprintf(CRITICAL, "Keystore verification failed! Continuing anyways...\n");
	}
}

int boot_linux_from_flash(void)
{
	struct boot_img_hdr *hdr = (void*) buf;
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned offset = 0;

	unsigned char *image_addr = 0;
	unsigned kernel_actual;
	unsigned ramdisk_actual;
	unsigned imagesize_actual;
	unsigned second_actual = 0;
	enum boot_cfg cfg;
#if DEVICE_TREE
	struct dt_table *table;
	struct dt_entry dt_entry;
	unsigned dt_table_offset;
	uint32_t dt_actual;
	uint32_t dt_hdr_size;
	unsigned int dtb_size = 0;
	unsigned char *best_match_dt_addr = NULL;
#endif

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}

	if (boot_cfg_get(ptable, &cfg)) {
		// TODO: add better error handling
		cfg = BOOT_CFG_DEFAULT;
		dprintf(INFO, "Failed to get the boot configuration, using default (%s)\n",
			boot_configs[cfg].name);
	}

	struct recovery_message msg = { 0 };

	// get recovery message
	if (get_recovery_message(&msg)) {
		dprintf(CRITICAL, "ERROR: cannot get recovery flag .boot recoveryfs\n");
	}

	ptn = ptable_find(ptable, boot_configs[cfg].kernel);
	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No boot partition found\n");
		return -1;
	}

	if (flash_read(ptn, offset, buf, page_size)) {
		dprintf(CRITICAL, "ERROR: Cannot read boot image header\n");
		return -1;
	}

	if (memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		dprintf(CRITICAL, "ERROR: Invalid boot image header\n");
		return -1;
	}

	if (hdr->page_size != page_size) {
		dprintf(CRITICAL, "ERROR: Invalid boot image pagesize. Device pagesize: %d, Image pagesize: %d\n",page_size,hdr->page_size);
		return -1;
	}

	/* ensure commandline is terminated */
	hdr->cmdline[BOOT_ARGS_SIZE-1] = 0;

	/*
	 * Update the kernel/ramdisk/tags address if the boot image header
	 * has default values, these default values come from mkbootimg when
	 * the boot image is flashed using fastboot flash:raw
	 */
	update_ker_tags_rdisk_addr(hdr, false);

	/* Get virtual addresses since the hdr saves physical addresses. */
	hdr->kernel_addr = VA((addr_t)(hdr->kernel_addr));
	hdr->ramdisk_addr = VA((addr_t)(hdr->ramdisk_addr));
	hdr->tags_addr = VA((addr_t)(hdr->tags_addr));

	kernel_actual  = ROUND_TO_PAGE(hdr->kernel_size,  page_mask);
	ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);

	/* Check if the addresses in the header are valid. */
	if (check_aboot_addr_range_overlap(hdr->kernel_addr, kernel_actual) ||
		check_aboot_addr_range_overlap(hdr->ramdisk_addr, ramdisk_actual))
	{
		dprintf(CRITICAL, "kernel/ramdisk addresses overlap with aboot addresses.\n");
		return -1;
	}

#ifndef DEVICE_TREE
		if (check_aboot_addr_range_overlap(hdr->tags_addr, MAX_TAGS_SIZE))
		{
			dprintf(CRITICAL, "Tags addresses overlap with aboot addresses.\n");
			return -1;
		}
#endif

	/* Authenticate Kernel */
	if(target_use_signed_kernel())
	{
		image_addr = (unsigned char *)target_get_scratch_address();
		offset = 0;

#if DEVICE_TREE
		dt_actual = ROUND_TO_PAGE(hdr->dt_size, page_mask);
		imagesize_actual = (page_size + kernel_actual + ramdisk_actual + dt_actual);

		if (check_aboot_addr_range_overlap(hdr->tags_addr, hdr->dt_size))
		{
			dprintf(CRITICAL, "Device tree addresses overlap with aboot addresses.\n");
			return -1;
		}
#else
		imagesize_actual = (page_size + kernel_actual + ramdisk_actual);
#endif

		bs_set_timestamp(BS_KERNEL_LOAD_START);

		/* Read image without signature */
		if (flash_read(ptn, offset, (void *)image_addr, imagesize_actual))
		{
			dprintf(CRITICAL, "ERROR: Cannot read boot image\n");
				return -1;
		}

		bs_set_timestamp(BS_KERNEL_LOAD_DONE);

		offset = imagesize_actual;
		/* Read signature */
		if (flash_read(ptn, offset, (void *)(image_addr + offset), page_size))
		{
			dprintf(CRITICAL, "ERROR: Cannot read boot image signature\n");
			return -1;
		}

		verify_signed_bootimg((uint32_t)image_addr, imagesize_actual);

		/* Move kernel and ramdisk to correct address */
		memmove((void*) hdr->kernel_addr, (char*) (image_addr + page_size), hdr->kernel_size);
		memmove((void*) hdr->ramdisk_addr, (char*) (image_addr + page_size + kernel_actual), hdr->ramdisk_size);
#if DEVICE_TREE
		if(hdr->dt_size != 0) {

			dt_table_offset = ((uint32_t)image_addr + page_size + kernel_actual + ramdisk_actual + second_actual);

			table = (struct dt_table*) dt_table_offset;

			if (dev_tree_validate(table, hdr->page_size, &dt_hdr_size) != 0){
				dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
				return -1;
			}

			/* Find index of device tree within device tree table */
			if(dev_tree_get_entry_info(table, &dt_entry) != 0){
				dprintf(CRITICAL, "ERROR: Getting device tree address failed\n");
				return -1;
			}

			/* Validate and Read device device tree in the "tags_add */
			if (check_aboot_addr_range_overlap(hdr->tags_addr, dt_entry.size)){
				dprintf(CRITICAL, "Device tree addresses overlap with aboot addresses.\n");
				return -1;
			}

			best_match_dt_addr = (unsigned char *)table + dt_entry.offset;
			dtb_size = dt_entry.size;
			memmove((void *)hdr->tags_addr, (char *)best_match_dt_addr, dtb_size);
		}
#endif
	}
	else
	{
		offset = page_size;
		
		kernel_actual = ROUND_TO_PAGE(hdr->kernel_size, page_mask);
		ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
		second_actual = ROUND_TO_PAGE(hdr->second_size, page_mask);

		bs_set_timestamp(BS_KERNEL_LOAD_START);

		if (UINT_MAX - offset < kernel_actual)
		{
			dprintf(CRITICAL, "ERROR: Integer overflow in boot image header %s\t%d\n",__func__,__LINE__);
			return -1;
		}
		if (flash_read(ptn, offset, (void *)hdr->kernel_addr, kernel_actual)) {
			dprintf(CRITICAL, "ERROR: Cannot read kernel image\n");
			return -1;
		}
		offset += kernel_actual;
		if (UINT_MAX - offset < ramdisk_actual)
		{
			dprintf(CRITICAL, "ERROR: Integer overflow in boot image header %s\t%d\n",__func__,__LINE__);
			return -1;
		}
		if (flash_read(ptn, offset, (void *)hdr->ramdisk_addr, ramdisk_actual)) {
			dprintf(CRITICAL, "ERROR: Cannot read ramdisk image\n");
			return -1;
		}

		offset += ramdisk_actual;

		bs_set_timestamp(BS_KERNEL_LOAD_DONE);

		if(hdr->second_size != 0) {
			if (UINT_MAX - offset < second_actual)
			{
				dprintf(CRITICAL, "ERROR: Integer overflow in boot image header %s\t%d\n",__func__,__LINE__);
				return -1;
			}
			offset += second_actual;
			/* Second image loading not implemented. */
			ASSERT(0);
		}

#if DEVICE_TREE
		if(hdr->dt_size != 0) {

			/* Read the device tree table into buffer */
			if(flash_read(ptn, offset, (void *) dt_buf, page_size)) {
				dprintf(CRITICAL, "ERROR: Cannot read the Device Tree Table\n");
				return -1;
			}

			table = (struct dt_table*) dt_buf;

			if (dev_tree_validate(table, hdr->page_size, &dt_hdr_size) != 0) {
				dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
				return -1;
			}

			table = (struct dt_table*) memalign(CACHE_LINE, dt_hdr_size);
			if (!table)
				return -1;

			/* Read the entire device tree table into buffer */
			if(flash_read(ptn, offset, (void *)table, dt_hdr_size)) {
				dprintf(CRITICAL, "ERROR: Cannot read the Device Tree Table\n");
				return -1;
			}


			/* Find index of device tree within device tree table */
			if(dev_tree_get_entry_info(table, &dt_entry) != 0){
				dprintf(CRITICAL, "ERROR: Getting device tree address failed\n");
				return -1;
			}

			/* Validate and Read device device tree in the "tags_add */
			if (check_aboot_addr_range_overlap(hdr->tags_addr, dt_entry.size))
			{
				dprintf(CRITICAL, "Device tree addresses overlap with aboot addresses.\n");
				return -1;
			}

			/* Read device device tree in the "tags_add */
			if(flash_read(ptn, offset + dt_entry.offset,
						 (void *)hdr->tags_addr, dt_entry.size)) {
				dprintf(CRITICAL, "ERROR: Cannot read device tree\n");
				return -1;
			}
		}
#endif

	}

	/* TODO: create/pass atags to kernel */
	boot_linux((void *)hdr->kernel_addr, (void *)hdr->tags_addr,
		   (const char *)hdr->cmdline, board_machtype(),
		   (void *)hdr->ramdisk_addr, hdr->ramdisk_size, cfg);

	return 0;
}

#if DEVICE_TREE
int copy_dtb(uint8_t *boot_image_start, unsigned int scratch_offset)
{
	uint32 dt_image_offset = 0;
	uint32_t n;
	struct dt_table *table;
	struct dt_entry dt_entry;
	uint32_t dt_hdr_size;
	unsigned int compressed_size = 0;
	unsigned int dtb_size = 0;
	unsigned int out_avai_len = 0;
	unsigned char *out_addr = NULL;
	unsigned char *best_match_dt_addr = NULL;
	int rc;

	struct boot_img_hdr *hdr = (struct boot_img_hdr *) (boot_image_start);

	if(hdr->dt_size != 0) {
		/* add kernel offset */
		dt_image_offset += page_size;
		n = ROUND_TO_PAGE(hdr->kernel_size, page_mask);
		dt_image_offset += n;

		/* add ramdisk offset */
		n = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
		dt_image_offset += n;

		/* add second offset */
		if(hdr->second_size != 0) {
			n = ROUND_TO_PAGE(hdr->second_size, page_mask);
			dt_image_offset += n;
		}

		/* offset now point to start of dt.img */
		table = (struct dt_table*)(boot_image_start + dt_image_offset);

		if (dev_tree_validate(table, hdr->page_size, &dt_hdr_size) != 0) {
			dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
			return -1;
		}
		/* Find index of device tree within device tree table */
		if(dev_tree_get_entry_info(table, &dt_entry) != 0){
			dprintf(CRITICAL, "ERROR: Getting device tree address failed\n");
			return -1;
		}

		best_match_dt_addr = (unsigned char *)boot_image_start + dt_image_offset + dt_entry.offset;
		if (is_gzip_package(best_match_dt_addr, dt_entry.size))
		{
			out_addr = (unsigned char *)target_get_scratch_address() + scratch_offset;
			out_avai_len = target_get_max_flash_size() - scratch_offset;
			dprintf(INFO, "decompressing dtb: start\n");
			rc = decompress(best_match_dt_addr,
					dt_entry.size, out_addr, out_avai_len,
					&compressed_size, &dtb_size);
			if (rc)
			{
				dprintf(CRITICAL, "decompressing dtb failed!!!\n");
				ASSERT(0);
			}

			dprintf(INFO, "decompressing dtb: done\n");
			best_match_dt_addr = out_addr;
		} else {
			dtb_size = dt_entry.size;
		}
		/* Validate and Read device device tree in the "tags_add */
		if (check_aboot_addr_range_overlap(hdr->tags_addr, dtb_size))
		{
			dprintf(CRITICAL, "Device tree addresses overlap with aboot addresses.\n");
			return -1;
		}

		/* Read device device tree in the "tags_add */
		memmove((void*) hdr->tags_addr, (void *)best_match_dt_addr, dtb_size);
	} else
		return -1;

	/* Everything looks fine. Return success. */
	return 0;
}
#endif

void cmd_erase_nand(const char *arg, void *data, unsigned sz)
{
	struct ptentry *ptn;
	struct ptable *ptable;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		fastboot_fail("partition table doesn't exist");
		return;
	}

	ptn = ptable_find(ptable, arg);
	if (ptn == NULL) {
		fastboot_fail("unknown partition name");
		return;
	}
    
	if (flash_erase(ptn)) {
		fastboot_fail("failed to erase partition");
		return;
	}
	fastboot_okay("");
}

void cmd_erase(const char *arg, void *data, unsigned sz)
{
	cmd_erase_nand(arg, data, sz);
}

static uint32_t aboot_get_secret_key()
{
	/* 0 is invalid secret key, update this implementation to return
	 * device specific unique secret key
	 */
	return 0;
}

void cmd_flash_mmc_img(const char *arg, void *data, unsigned sz)
{
	unsigned long long ptn = 0;
	unsigned long long size = 0;
	int index = INVALID_PTN;
	char *token = NULL;
	char *pname = NULL;
	char *sp;
	uint8_t lun = 0;
	bool lun_set = false;

	token = strtok_r((char *)arg, ":", &sp);
	pname = token;
	token = strtok_r(NULL, ":", &sp);
	if(token)
	{
		lun = atoi(token);
		mmc_set_lun(lun);
		lun_set = true;
	}

	if (pname)
	{
		if (!strcmp(pname, "partition"))
		{
			dprintf(INFO, "Attempt to write partition image.\n");
			if (write_partition(sz, (unsigned char *) data)) {
				fastboot_fail("failed to write partition");
				return;
			}
		}
		else
		{
#if VERIFIED_BOOT
			if(!strcmp(pname, KEYSTORE_PTN_NAME))
			{
				if(!boot_verify_validate_keystore((unsigned char *)data))
				{
					fastboot_fail("image is not a keystore file");
					return;
				}
			}
#endif
			index = partition_get_index(pname);
			ptn = partition_get_offset(index);
			if(ptn == 0) {
				fastboot_fail("partition table doesn't exist");
				return;
			}
			if (ptn_is_boot(pname)) {
				if (memcmp((void *)data, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
					fastboot_fail("image is not a boot image");
					return;
				}
			}

			if(!lun_set)
			{
				lun = partition_get_lun(index);
				mmc_set_lun(lun);
			}

			size = partition_get_size(index);
			if (ROUND_TO_PAGE(sz,511) > size) {
				fastboot_fail("size too large");
				return;
			}
			else if (mmc_write(ptn , sz, (unsigned int *)data)) {
				fastboot_fail("flash write failure");
				return;
			}
		}
	}
	fastboot_okay("");
	return;
}

void cmd_flash_mmc(const char *arg, void *data, unsigned sz)
{
#ifdef SSD_ENABLE
	/* 8 Byte Magic + 2048 Byte xml + Encrypted Data */
	unsigned int *magic_number = (unsigned int *) data;
	int              ret=0;
	uint32           major_version=0;
	uint32           minor_version=0;

	ret = scm_svc_version(&major_version,&minor_version);
	if(!ret)
	{
		if(major_version >= 2)
		{
			if( !strcmp(arg, "ssd") || !strcmp(arg, "tqs") )
			{
				ret = encrypt_scm((uint32 **) &data, &sz);
				if (ret != 0) {
					dprintf(CRITICAL, "ERROR: Encryption Failure\n");
					return;
				}

				/* Protect only for SSD */
				if (!strcmp(arg, "ssd")) {
					ret = scm_protect_keystore((uint32 *) data, sz);
					if (ret != 0) {
						dprintf(CRITICAL, "ERROR: scm_protect_keystore Failed\n");
						return;
					}
				}
			}
			else
			{
				ret = decrypt_scm_v2((uint32 **) &data, &sz);
				if(ret != 0)
				{
					dprintf(CRITICAL,"ERROR: Decryption Failure\n");
					return;
				}
			}
		}
		else
		{
			if (magic_number[0] == DECRYPT_MAGIC_0 &&
			magic_number[1] == DECRYPT_MAGIC_1)
			{
				ret = decrypt_scm((uint32 **) &data, &sz);
				if (ret != 0) {
					dprintf(CRITICAL, "ERROR: Invalid secure image\n");
					return;
				}
			}
			else if (magic_number[0] == ENCRYPT_MAGIC_0 &&
				magic_number[1] == ENCRYPT_MAGIC_1)
			{
				ret = encrypt_scm((uint32 **) &data, &sz);
				if (ret != 0) {
					dprintf(CRITICAL, "ERROR: Encryption Failure\n");
					return;
				}
			}
		}
	}
	else
	{
		dprintf(CRITICAL,"INVALID SVC Version\n");
		return;
	}
#endif /* SSD_ENABLE */

	cmd_flash_mmc_img(arg, data, sz);
	return;
}

void cmd_flash_nand(const char *arg, void *data, unsigned sz)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned extra = 0;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		fastboot_fail("partition table doesn't exist");
		return;
	}

	ptn = ptable_find(ptable, arg);
	if (ptn == NULL) {
		fastboot_fail("partition not found");
		return;
	}
	
    	if (ptn_is_boot(arg)) {
		if (memcmp((void *)data, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
			fastboot_fail("image is not a boot image");
			return;
		}
	}

	if (!strcmp(ptn->name, "rootfs_") ||
	    !strcmp(ptn->name, "storage") ||
	    !strcmp(ptn->name, "modem"))
		extra = 1;
	else
		sz = ROUND_TO_PAGE(sz, page_mask);

	dprintf(INFO, "writing %d bytes to '%s'\n", sz, ptn->name);
	if (!memcmp((void *)data, UBI_MAGIC, UBI_MAGIC_SIZE)) {
		if (flash_ubi_img(ptn, data, sz)) {
			fastboot_fail("flash write failure");
			return;
		}
	} else {
		if (flash_write(ptn, extra, data, sz)) {
			fastboot_fail("flash write failure");
			return;
		}
	}
	dprintf(INFO, "partition '%s' updated\n", ptn->name);
	fastboot_okay("");
}

#if ENABLE_FETCH_SUPPORT
static void cmd_fetch(const char *arg, void *data, unsigned sz)
{
	(void)data;
	(void)sz;

	struct ptentry *p = NULL;
	struct ptable *ptable;
	static char buf[DEFAULT_ERASE_SIZE]; // flash page size
	unsigned read = 1, count = 0, off = 0, size = 0, s_piv, f_piv;
	char *sp, *pname = NULL, *token = NULL;
	bool disabled_part = false;

	token = strtok_r((char *)arg, ":", &sp);
	pname = token;

	token = strtok_r(NULL, ":", &sp);
	if(token) {
		off = atoul(token);
	}

	token = strtok_r(NULL, ":", &sp);
	if(token) {
		size = atoul(token);
	}

	ptable = flash_get_ptable();
	if (!ptable) {
		fastboot_fail("partition table doesn't exist");
		return;
	}

	if(!strcmp("all", pname)){
		for (int i = 0; i < ptable->count; i++) {
			p = &ptable->parts[i];
			if (off < p->length * block_size){
				dprintf(SPEW, "Fetching part[%d]:'%s'\n", i, p->name);

				if (!strcmp(p->name, "sbl") || !strcmp(p->name, "mibib") ||
						!strcmp(p->name, "efs2") || !strcmp(p->name, "rawdata") ||
						!strcmp(p->name, "tz") || !strcmp(p->name, "rpm")) {
					disabled_part = true;
				}
				break;
			}
			off -= p->length * block_size;
		}
	} else {
		p = ptable_find(ptable, pname);
		if (!p) {
			fastboot_fail("partition not found");
			return;
		}
	}

	while(count < size) {
		s_piv = 0;
		f_piv = page_size;

		/* Wants to start reading from the middle of the page */
		if (off & page_mask) {
			s_piv = off & page_mask;
			off &= ~page_mask; //round down, but not underflow
		}

		/* Wants to stop reading int the middle of the page */
		if ((s_piv + size) < page_mask) {
			f_piv = s_piv + size;
		}


		if (disabled_part) {
			memset(buf, 0xff, page_size);
		} else {
			if (flash_read(p, off, buf, page_size)) {
				fastboot_fail("flash read failed");
				return;
			}
		}

		read = f_piv - s_piv;
		fastboot_send_data(buf + s_piv, read);

		count += read;
		off += page_size;
	}

	fastboot_okay("");
}
#endif //ENABLE_FETCH_SUPPORT

void cmd_boot(const char *arg, void *data, unsigned sz)
{
	struct boot_img_hdr *hdr = (void*) data;
	unsigned offset = 0;

	unsigned char *image_addr = 0;
	unsigned kernel_actual;
	unsigned ramdisk_actual;
	unsigned imagesize_actual;
	unsigned second_actual = 0;
	enum boot_cfg cfg;
	char buffer[128];
#if DEVICE_TREE
	struct dt_table *table;
	struct dt_entry dt_entry;
	unsigned dt_table_offset;
	uint32_t dt_actual;
	uint32_t dt_hdr_size;
	unsigned int dtb_size = 0;
	unsigned char *best_match_dt_addr = NULL;
#endif

	if (memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		fastboot_fail("ERROR: Invalid boot image header");
		return;
	}

	if (hdr->page_size != page_size) {
		snprintf(buffer, sizeof(buffer), "ERROR: Invalid boot image pagesize. Device pagesize: %d, Image pagesize: %d",page_size,hdr->page_size);
		fastboot_fail(buffer);
		return;
	}

	/* ensure commandline is terminated */
	hdr->cmdline[BOOT_ARGS_SIZE-1] = 0;

	/*
	 * Update the kernel/ramdisk/tags address if the boot image header
	 * has default values, these default values come from mkbootimg when
	 * the boot image is flashed using fastboot flash:raw
	 */
	update_ker_tags_rdisk_addr(hdr, false);

	/* Get virtual addresses since the hdr saves physical addresses. */
	hdr->kernel_addr = VA((addr_t)(hdr->kernel_addr));
	hdr->ramdisk_addr = VA((addr_t)(hdr->ramdisk_addr));
	hdr->tags_addr = VA((addr_t)(hdr->tags_addr));

	kernel_actual  = ROUND_TO_PAGE(hdr->kernel_size,  page_mask);
	ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);

	/* Check if the addresses in the header are valid. */
	if (check_aboot_addr_range_overlap(hdr->kernel_addr, kernel_actual) ||
		check_aboot_addr_range_overlap(hdr->ramdisk_addr, ramdisk_actual))
	{
		fastboot_fail("kernel/ramdisk addresses overlap with aboot addresses");
		return;
	}

#ifndef DEVICE_TREE
		if (check_aboot_addr_range_overlap(hdr->tags_addr, MAX_TAGS_SIZE))
		{
			fastboot_fail("Tags addresses overlap with aboot addresses")
			return;
		}
#endif

	/* Authenticate Kernel */
	if(target_use_signed_kernel())
	{
		image_addr = (unsigned char *)target_get_scratch_address();
		offset = 0;

#if DEVICE_TREE
		dt_actual = ROUND_TO_PAGE(hdr->dt_size, page_mask);
		imagesize_actual = (page_size + kernel_actual + ramdisk_actual + dt_actual);

		if (check_aboot_addr_range_overlap(hdr->tags_addr, hdr->dt_size))
		{
			fastboot_fail("Device tree addresses overlap with aboot addresses");
			return;
		}
#else
		imagesize_actual = (page_size + kernel_actual + ramdisk_actual);
#endif

		bs_set_timestamp(BS_KERNEL_LOAD_START);

		/* Read image without signature */
		memcpy((void *)image_addr, data + offset, imagesize_actual);

		bs_set_timestamp(BS_KERNEL_LOAD_DONE);

		offset = imagesize_actual;
		/* Read signature */
		memcpy((void *)(image_addr + offset), data + offset, page_size);

		verify_signed_bootimg((uint32_t)image_addr, imagesize_actual);

		/* Move kernel and ramdisk to correct address */
		memmove((void*) hdr->kernel_addr, (char*) (image_addr + page_size), hdr->kernel_size);
		memmove((void*) hdr->ramdisk_addr, (char*) (image_addr + page_size + kernel_actual), hdr->ramdisk_size);
#if DEVICE_TREE
		if(hdr->dt_size != 0) {

			dt_table_offset = ((uint32_t)image_addr + page_size + kernel_actual + ramdisk_actual + second_actual);

			table = (struct dt_table*) dt_table_offset;

			if (dev_tree_validate(table, hdr->page_size, &dt_hdr_size) != 0){
				fastboot_fail("ERROR: Cannot validate Device Tree Table");
				return;
			}

			/* Find index of device tree within device tree table */
			if(dev_tree_get_entry_info(table, &dt_entry) != 0){
				fastboot_fail("ERROR: Getting device tree address failed");
				return;
			}

			/* Validate and Read device device tree in the "tags_add */
			if (check_aboot_addr_range_overlap(hdr->tags_addr, dt_entry.size)){
				fastboot_fail("Device tree addresses overlap with aboot addresses");
				return;
			}

			best_match_dt_addr = (unsigned char *)table + dt_entry.offset;
			dtb_size = dt_entry.size;
			memmove((void *)hdr->tags_addr, (char *)best_match_dt_addr, dtb_size);
		}
#endif
	}
	else
	{
		offset = page_size;

		kernel_actual = ROUND_TO_PAGE(hdr->kernel_size, page_mask);
		ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
		second_actual = ROUND_TO_PAGE(hdr->second_size, page_mask);

		bs_set_timestamp(BS_KERNEL_LOAD_START);

		if (UINT_MAX - offset < kernel_actual)
		{
			snprintf(buffer, sizeof(buffer), "ERROR: Integer overflow in boot image header %s\t%d",__func__,__LINE__);
			fastboot_fail(buffer);
			return;
		}
		memcpy((void *)hdr->kernel_addr, data + offset, kernel_actual);

		offset += kernel_actual;
		if (UINT_MAX - offset < ramdisk_actual)
		{
			snprintf(buffer, sizeof(buffer), "ERROR: Integer overflow in boot image header %s\t%d",__func__,__LINE__);
			fastboot_fail(buffer);
			return;
		}

		memcpy((void *)hdr->ramdisk_addr, data + offset, ramdisk_actual);

		offset += ramdisk_actual;

		bs_set_timestamp(BS_KERNEL_LOAD_DONE);

		if(hdr->second_size != 0) {
			if (UINT_MAX - offset < second_actual)
			{
				snprintf(buffer, sizeof(buffer), "ERROR: Integer overflow in boot image header %s\t%d",__func__,__LINE__);
				fastboot_fail(buffer);
				return;
			}
			offset += second_actual;
			/* Second image loading not implemented. */
			ASSERT(0);
		}

#if DEVICE_TREE
		if(hdr->dt_size != 0) {

			/* Read the device tree table into buffer */
			memcpy((void *) dt_buf, data + offset, page_size);

			table = (struct dt_table*) dt_buf;

			if (dev_tree_validate(table, hdr->page_size, &dt_hdr_size) != 0) {
				fastboot_fail("ERROR: Cannot validate Device Tree Table");
				return;
			}

			table = (struct dt_table*) memalign(CACHE_LINE, dt_hdr_size);
			if (!table)
				return;

			/* Read the entire device tree table into buffer */
			memcpy((void *) table, data + offset, dt_hdr_size);


			/* Find index of device tree within device tree table */
			if(dev_tree_get_entry_info(table, &dt_entry) != 0){
				fastboot_fail("ERROR: Getting device tree address failed");
				return;
			}

			/* Validate and Read device device tree in the "tags_add */
			if (check_aboot_addr_range_overlap(hdr->tags_addr, dt_entry.size))
			{
				fastboot_fail("Device tree addresses overlap with aboot addresses.");
				return;
			}

			/* Read device device tree in the "tags_add */
			memcpy((void *)hdr->tags_addr, data + offset + dt_entry.offset, dt_entry.size);
		}
#endif

	}

	fastboot_okay("");

	/* TODO: create/pass atags to kernel */
	boot_linux((void *)hdr->kernel_addr, (void *)hdr->tags_addr,
		   (const char *)hdr->cmdline, board_machtype(),
		   (void *)hdr->ramdisk_addr, hdr->ramdisk_size, cfg);

}

void cmd_flash(const char *arg, void *data, unsigned sz)
{
	cmd_flash_nand(arg, data, sz);
}

void cmd_continue(const char *arg, void *data, unsigned sz)
{
	fastboot_okay("");
	fastboot_stop();

	boot_linux_from_flash();
}

void cmd_reboot(const char *arg, void *data, unsigned sz)
{
	dprintf(INFO, "rebooting the device\n");
	fastboot_okay("");
	reboot_device(0);
}

void cmd_reboot_bootloader(const char *arg, void *data, unsigned sz)
{
	dprintf(INFO, "rebooting the device\n");
	fastboot_okay("");
	reboot_device(FASTBOOT_MODE);
}

void cmd_preflash(const char *arg, void *data, unsigned sz)
{
	fastboot_okay("");
}

/* Get the size from partiton name */
static void get_partition_size(const char *arg, char *response)
{
	uint64_t ptn = 0;
	uint64_t size;
	int index = INVALID_PTN;

	index = partition_get_index(arg);

	if (index == INVALID_PTN)
	{
		dprintf(CRITICAL, "Invalid partition index\n");
		return;
	}

	ptn = partition_get_offset(index);

	if(!ptn)
	{
		dprintf(CRITICAL, "Invalid partition name %s\n", arg);
		return;
	}

	size = partition_get_size(index);

	snprintf(response, MAX_RSP_SIZE, "\t 0x%llx", size);
	return;
}

/*
 * Publish the partition type & size info
 * fastboot getvar will publish the required information.
 * fastboot getvar partition_size:<partition_name>: partition size in hex
 * fastboot getvar partition_type:<partition_name>: partition type (ext/fat)
 */
static void publish_getvar_partition_info(struct getvar_partition_info *info, uint8_t num_parts)
{
	uint8_t i;

	for (i = 0; i < num_parts; i++) {
		if (!info[i].size_response[0]) {
			get_partition_size(info[i].part_name, info[i].size_response);
		}

		if (strlcat(info[i].getvar_size, info[i].part_name, MAX_GET_VAR_NAME_SIZE) >= MAX_GET_VAR_NAME_SIZE)
		{
			dprintf(CRITICAL, "partition size name truncated\n");
			return;
		}
		if (strlcat(info[i].getvar_type, info[i].part_name, MAX_GET_VAR_NAME_SIZE) >= MAX_GET_VAR_NAME_SIZE)
		{
			dprintf(CRITICAL, "partition type name truncated\n");
			return;
		}

		/* publish partition size & type info */
		fastboot_publish((const char *) info[i].getvar_size, (const char *) info[i].size_response);
		fastboot_publish((const char *) info[i].getvar_type, (const char *) info[i].type_response);
	}
}

/* register commands and variables for fastboot */
void aboot_fastboot_register_commands(void)
{
	const struct fastboot_cmd_desc *walk;
	static const struct fastboot_cmd_desc cmd_list[] = {
		{"flash:", cmd_flash},
		{"erase:", cmd_erase},
		{"continue", cmd_continue},
		{"reboot", cmd_reboot},
		{"reboot-bootloader", cmd_reboot_bootloader},
		{"boot", cmd_boot},
#if ENABLE_FETCH_SUPPORT
		{"fetch", cmd_fetch},
#endif //ENABLE_FETCH_SUPPORT

#if ENABLE_MNFINFO_SUPPORT
		{"oem mnf", cmd_mnfinfo},
#endif
		{NULL, NULL}
	};

	walk = cmd_list;
	while (walk->name != NULL && walk->cb != NULL) {
		fastboot_register(walk->name, walk->cb);
		walk++;
	}

	/* Max download size supported */
	snprintf(max_download_size, MAX_RSP_SIZE, "\t0x%x",
			target_get_max_flash_size());
	fastboot_publish("max-download-size", (const char *) max_download_size);

#if ENABLE_FETCH_SUPPORT
	/* Max fetch size supported */
	snprintf(max_fetch_size, MAX_RSP_SIZE, "\t0x%x", flash_page_size());
	fastboot_publish("max-fetch-size", (const char *) max_fetch_size);
	publish_getvar_partition_info(part_info, ARRAY_SIZE(part_info));
#endif //ENABLE_FETCH_SUPPORT
}

#if MMC_SDHCI_SUPPORT
void target_sdc_init(void);
void target_sdc_uninit(void);
#endif

void aboot_init(const struct app_descriptor *app)
{
	unsigned reboot_mode = 0;

	page_size = flash_page_size();
	block_size = flash_block_size();
	page_mask = page_size - 1;

	ASSERT((MEMBASE + MEMSIZE) > MEMBASE);

	target_serialno((unsigned char *) sn_buf);
	dprintf(SPEW,"serial number: %s\n",sn_buf);

	memset(display_panel_buf, '\0', MAX_PANEL_BUF_SIZE);

	printf("CTRL+C: enter fasboot\n");
	for (int i = 10; i != 0; --i) {
		char ch;
		if (!getc(&ch) && ch == 0x03) {
			goto fastboot;
		}
		mdelay(10);
	}

#if 1 // quectel restore function
	//modify by [Francis.huan],20180416,get mtd nub which need to restore
	int ret = Ql_check_RestoreFlag();
	if (0 != ret) {
		dprintf(CRITICAL, "@Ramos Restore the  start  now\n");
		Ql_Restore_partition(ret);
	}
#endif

	/*
	 * Check power off reason if user force reset,
	 * if yes phone will do normal boot.
	 */
	if (is_user_force_reset())
		goto normal_boot;

	/* Check if we should do something other than booting up */
	if (keys_get_state(KEY_VOLUMEUP) && keys_get_state(KEY_VOLUMEDOWN))
	{
		dprintf(ALWAYS,"dload mode key sequence detected\n");
		if (set_download_mode(EMERGENCY_DLOAD))
		{
			dprintf(CRITICAL,"dload mode not supported by target\n");
		}
		else
		{
			reboot_device(DLOAD);
			dprintf(CRITICAL,"Failed to reboot into dload mode\n");
		}
		boot_into_fastboot = true;
	}

	if (!boot_into_fastboot)
	{
		if (keys_get_state(KEY_BACK) || keys_get_state(KEY_VOLUMEDOWN))
			boot_into_fastboot = true;
	}
	#if NO_KEYPAD_DRIVER
	if (fastboot_trigger())
		boot_into_fastboot = true;
	#endif

#if USE_PON_REBOOT_REG
	reboot_mode = check_hard_reboot_mode();
#else
	reboot_mode = check_reboot_mode();
#endif

	if (reboot_mode == RECOVERY_MODE)
	{
       		dprintf(CRITICAL, "ERROR: Recovery is not supported.\n");
		assert(0);
	}
	else if(reboot_mode == FASTBOOT_MODE)
	{
		boot_into_fastboot = true;
	}
	else if(reboot_mode == ALARM_BOOT)
	{
		boot_reason_alarm = true;
	}

normal_boot:
	if (!boot_into_fastboot)
	{
		boot_linux_from_flash();
		
		dprintf(CRITICAL, "ERROR: Could not do normal boot. Reverting "
			"to fastboot mode.\n");
	}

fastboot:
	/* We are here means regular boot did not happen. Start fastboot. */

	/* register aboot specific fastboot commands */
	aboot_fastboot_register_commands();

	/* dump partition table for debug info */
	partition_dump();

	/* initialize and start fastboot */
	fastboot_init(target_get_scratch_address(), target_get_max_flash_size());
}

uint32_t get_page_size()
{
	return page_size;
}

/*
 * Calculated and save hash (SHA256) for non-signed boot image.
 *
 * @param image_addr - Boot image address
 * @param image_size - Size of the boot image
 *
 * @return int - 0 on success, negative value on failure.
 */
static int aboot_save_boot_hash_mmc(uint32_t image_addr, uint32_t image_size)
{
	unsigned int digest[8];
#if IMAGE_VERIF_ALGO_SHA1
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA1;
#else
	uint32_t auth_algo = CRYPTO_AUTH_ALG_SHA256;
#endif

	target_crypto_init_params();
	hash_find((unsigned char *) image_addr, image_size, (unsigned char *)&digest, auth_algo);

	save_kernel_hash_cmd(digest);
	dprintf(INFO, "aboot_save_boot_hash_mmc: imagesize_actual size %d bytes.\n", (int) image_size);

	return 0;
}

APP_START(aboot)
	.init = aboot_init,
APP_END
