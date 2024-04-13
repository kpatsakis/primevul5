static void iscsi_endpoint_release(struct device *dev)
{
	struct iscsi_endpoint *ep = iscsi_dev_to_endpoint(dev);
	kfree(ep);
}