static void __init hugepage_exit_sysfs(struct kobject *hugepage_kobj)
{
	sysfs_remove_group(hugepage_kobj, &khugepaged_attr_group);
	sysfs_remove_group(hugepage_kobj, &hugepage_attr_group);
	kobject_put(hugepage_kobj);
}