static int iscsi_match_epid(struct device *dev, const void *data)
{
	struct iscsi_endpoint *ep = iscsi_dev_to_endpoint(dev);
	const uint64_t *epid = data;

	return *epid == ep->id;
}