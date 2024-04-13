static void iscsi_iface_release(struct device *dev)
{
	struct iscsi_iface *iface = iscsi_dev_to_iface(dev);
	struct device *parent = iface->dev.parent;

	kfree(iface);
	put_device(parent);
}