
void iscsi_destroy_iface(struct iscsi_iface *iface)
{
	sysfs_remove_group(&iface->dev.kobj, &iscsi_iface_group);
	device_unregister(&iface->dev);