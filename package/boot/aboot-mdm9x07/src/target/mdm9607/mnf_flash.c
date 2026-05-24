#include <mnf_info.h>
#include <lib/ptable.h>
#include <dev/flash.h>

#define MNF_INFO_SIZE 4096

static char mnf_cache[MNF_INFO_SIZE];

extern int mnf_flash_read(const mnf_field_t *field, char *result)
{
        // Seems like we don;t have a lot of stack space, gotta use static buffers
        static char page[MNF_INFO_SIZE];
        struct ptentry *ptn;
	struct ptable *ptable;

        ptable = flash_get_ptable();
        if (!ptable)
                return 1;

        ptn = ptable_find(ptable, "mnf_info");
        if (!ptn)
                return 1;

        if (flash_read(ptn, 0, page, flash_page_size()))
                return 1;

        memcpy(result, page + field->offset, field->length);

        return 0;
}

extern int mnf_flash_write_init(void)
{
        struct ptentry *ptn;
	struct ptable *ptable;

        ptable = flash_get_ptable();
        if (!ptable)
                return 1;

        ptn = ptable_find(ptable, "mnf_info");
        if (!ptn)
                return 1;

        return flash_read(ptn, 0, mnf_cache, flash_page_size());
}

extern int mnf_flash_write(const mnf_field_t *field, const char *buf)
{
        memcpy(mnf_cache + field->offset, buf, field->length);
        return 0;
}

extern int mnf_flash_write_finalize(void)
{
        struct ptentry *ptn;
	struct ptable *ptable;

        ptable = flash_get_ptable();
        if (!ptable)
                return 1;

        ptn = ptable_find(ptable, "mnf_info");
        if (!ptn)
                return 1;

        return flash_write(ptn, 0, mnf_cache, flash_page_size());
}
