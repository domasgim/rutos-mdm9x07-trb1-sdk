#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/uidgid.h>
#include <soc/qcom/subsystem_restart.h>

MODULE_LICENSE("GPL");

#define MODEM_SUBSYS_NAME "modem"

static void qm_kobj_get_ownership(struct kobject *kobj, kuid_t *uid, kgid_t *gid)
{
	(void)kobj;

	*uid = GLOBAL_ROOT_UID;
	*gid = GLOBAL_GPIO_GID;
}

static struct kobj_type qm_kobj_ktype = {
	.sysfs_ops     = &kobj_sysfs_ops,
	.get_ownership = &qm_kobj_get_ownership,
};

static ssize_t reboot_store(struct device *dev, struct device_attribute *attr, const char *buff, size_t count)
{
	int ret;

	if (!count || (strncmp(buff, "y", 1) && strncmp(buff, "1", 1))) {
		return -EINVAL;
	}

	ret = subsystem_restart(MODEM_SUBSYS_NAME);
	if (ret) {
		return -ret;
	}

	return count;
}

static struct device_attribute g_kobj_attr[] = { __ATTR(reboot, 0220, NULL, reboot_store) };

static struct attribute *g_attrs[] = {
	&g_kobj_attr[0].attr,
	NULL,
};

static struct attribute_group g_attr_group = { .attrs = g_attrs };
static struct kobject g_kobj		   = { 0 };

static int __init modem_start(void)
{
	void *ret;

	printk(KERN_INFO "Loading modem subsystem...\n");
	ret = subsystem_get_with_fwname(MODEM_SUBSYS_NAME, MODEM_SUBSYS_NAME);

	if (IS_ERR(ret)) {
		return PTR_ERR(ret);
	}

	kobject_init(&g_kobj, &qm_kobj_ktype);

	if (kobject_add(&g_kobj, NULL, MODEM_SUBSYS_NAME)) {
		printk(KERN_ERR "Unable to create `%s` kobject!\n", MODEM_SUBSYS_NAME);
		kobject_put(&g_kobj);
	}

	if (sysfs_create_group(&g_kobj, &g_attr_group)) {
		kobject_put(&g_kobj);
		printk(KERN_ERR "Unable to create `%s` sysfs group!\n", MODEM_SUBSYS_NAME);
		return -ENOMEM;
	}

	return 0;
}

module_init(modem_start);
