#include "boot_cfg.h"

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <debug.h>

#include <dev/flash.h>
#include <lib/ptable.h>
#include "mnf_info.h"

#define CMDLINE_ROOTFS_TOKEN "%ROOTFS_MTD_NAME%"

// TODO: use definitions from boot_cfg_raw static library

// Raw in-flash represantation of boot configuration
struct boot_cfg_raw {
	// Must be set to BOOT_CFG_MAGIC in order for the boot configuration
	// to be considered valid.
	uint32_t magic;

	// Must be set to a value of 'enum boot_cfg'.
	// We're forcing uint8_t rather than 'enum boot_cfg_id'
	// because the underlying data type of enums is not know.
	uint8_t id;

	// Reserved for future use
	uint8_t reserved[512];
} __attribute__((packed));

struct boot_cfg_info boot_configs[] = {
	[BOOT_CFG_A] = {
		.name = "a",
		.kernel = "boot_a",
		.rootfs = "rootfs_a"
	},
	[BOOT_CFG_B] = {
		.name = "b",
		.kernel = "boot_b",
		.rootfs = "rootfs_b"
	}
};

static int boot_cfg_raw_is_valid(struct boot_cfg_raw *cfg)
{
	if (cfg->magic != BOOT_CFG_MAGIC) {
		dprintf(CRITICAL, "Magic number missmatch (got 0x%0X, expected 0x%0X)\n",
			BOOT_CFG_MAGIC);
		return 0;
	}

	if (cfg->id != BOOT_CFG_A && cfg->id != BOOT_CFG_B) {
		return 0;
	}

	return 1;
}

int ptn_is_boot(const char *ptn_name)
{
	for (enum boot_cfg i = 0; i < BOOT_CFG_COUNT; i++) {
		if (!strcmp(ptn_name, boot_configs[i].kernel)) {
			return 1;
		}
	}

	return 0;
}

int boot_cfg_get(struct ptable *ptable, enum boot_cfg *cfg)
{
	unsigned char *buff;
	struct boot_cfg_raw *cfg_raw;
	struct ptentry *ptn;
	unsigned int page_size;
	size_t buff_size;

	ptn = ptable_find(ptable, BOOT_CFG_MTD);
	if (!ptn) {
		dprintf(CRITICAL, "Cannot find %s partition\n", BOOT_CFG_MTD);
		return 1;
	}

	page_size = flash_page_size();

	// Up allign buff_size to page_size
	buff_size = sizeof(*cfg_raw) + page_size;
	buff_size /= page_size;
	buff_size *= page_size;

	buff = malloc(buff_size);
	if (!buff) {
		dprintf(CRITICAL, "Failed to allocate memory\n");
		return 1;
	}

	if (flash_read(ptn, 0, buff, buff_size)) {
		dprintf(CRITICAL, "Failed to read the boot configuration\n");
		free(buff);
		return 1;
	}

	cfg_raw = (struct boot_cfg_raw *) buff;
	if (!boot_cfg_raw_is_valid(cfg_raw)) {
		dprintf(CRITICAL, "Boot configuration is invalid\n");
		free(buff);
		return 1;
	}

	*cfg = cfg_raw->id;

	free(buff);
	return 0;
}

int mnf_get_field_formated(const char *name , char *out) {

	mnf_field_t *field;
	static char buf[16];
	out[0] = 0;

	field = mnf_get_field_info_long(name);
	if (!field)
		return -1;

	if (mnf_flash_read(field, buf))
		return -1;

	mnf_field_to_str(field, buf, out);

	return 0;
}

char *boot_cfg_cmdline_parse(const char *cmdline, enum boot_cfg cfg)
{
	char *final;
	char *token;
	char *tmp;
	static char device[16];
	static char hwver[16];
	size_t final_size;

	token = strstr(cmdline, CMDLINE_ROOTFS_TOKEN);
	if (!token) {
		dprintf(CRITICAL, "Rootfs partition token (%s) does not exist\n",
			CMDLINE_ROOTFS_TOKEN);
		return NULL;
	}

	final_size = strlen(cmdline) -
		     strlen(CMDLINE_ROOTFS_TOKEN) +
		     strlen(boot_configs[cfg].rootfs) + 1;


	if (!mnf_get_field_formated("name", device))
		final_size +=  strlen(device) + 12; // ' tlt.device='
	if (!mnf_get_field_formated("hwver", hwver))
		final_size +=  strlen(hwver) + 11; // ' tlt.hwver='

	final = malloc(final_size);
	if (!final) {
		dprintf(CRITICAL, "Failed to allocate memory\n");
		return NULL;
	}

	tmp = final;

	// Copy everything up to CMDLINE_ROOTFS_TOKEN
	for (const char *p = cmdline; p != token; p++) {
		*tmp++ = *p;
	}

	// Copy the rootfs partition name
	strcpy(tmp, boot_configs[cfg].rootfs);
	tmp += strlen(boot_configs[cfg].rootfs);

	// Copy everything after CMDLINE_ROOTFS_TOKEN
	for (const char *p = token + strlen(CMDLINE_ROOTFS_TOKEN); *p; p++) {
		*tmp++ = *p;
	}

	if (strlen(device)) {
		strcpy(tmp, " tlt.device=");
		tmp += strlen(tmp);
		strcpy(tmp, device);
		tmp += strlen(tmp);
	}
	if (strlen(hwver)) {
		strcpy(tmp, " tlt.hwver=");
		tmp += strlen(tmp);
		strcpy(tmp, hwver);
		tmp += strlen(tmp);
	}

	return final;
}