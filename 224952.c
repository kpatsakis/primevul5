struct iscsi_endpoint *iscsi_lookup_endpoint(u64 handle)
{
	struct iscsi_endpoint *ep;
	struct device *dev;

	dev = class_find_device(&iscsi_endpoint_class, NULL, &handle,
				iscsi_match_epid);
	if (!dev)
		return NULL;

	ep = iscsi_dev_to_endpoint(dev);
	/*
	 * we can drop this now because the interface will prevent
	 * removals and lookups from racing.
	 */
	put_device(dev);
	return ep;
}