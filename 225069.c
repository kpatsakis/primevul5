static int __init dlpar_sysfs_init(void)
{
	int rc;

	rc = dlpar_workqueue_init();
	if (rc)
		return rc;

	return sysfs_create_file(kernel_kobj, &class_attr_dlpar.attr);
}