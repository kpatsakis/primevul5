static void iscsi_transport_release(struct device *dev)
{
	struct iscsi_internal *priv = dev_to_iscsi_internal(dev);
	kfree(priv);
}