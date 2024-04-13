void iscsi_destroy_endpoint(struct iscsi_endpoint *ep)
{
	sysfs_remove_group(&ep->dev.kobj, &iscsi_endpoint_group);
	device_unregister(&ep->dev);
}