#ifndef _BOOT_CFG_H
#define _BOOT_CFG_H

#include <lib/ptable.h>

// TODO: use declarations from boot_cfg_raw static library

#define BOOT_CFG_MAGIC 0X54495453

#define BOOT_CFG_MTD "boot_config"

enum boot_cfg {
	BOOT_CFG_A = 0,
	BOOT_CFG_B = 1,
	_BOOT_CFG_LAST
};

struct boot_cfg_info {
	const char *name;
	const char *kernel;
	const char *rootfs;
};

#define BOOT_CFG_DEFAULT (BOOT_CFG_A)
#define BOOT_CFG_COUNT   (_BOOT_CFG_LAST - BOOT_CFG_A)

extern struct boot_cfg_info boot_configs[];

int ptn_is_boot(const char *ptn_name);
int boot_cfg_get(struct ptable *ptable, enum boot_cfg *cfg);
char *boot_cfg_cmdline_parse(const char *cmdline, enum boot_cfg cfg);

#endif