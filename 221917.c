static void fuse_sysfs_cleanup(void)
{
	sysfs_remove_mount_point(fuse_kobj, "connections");
	kobject_put(fuse_kobj);
}