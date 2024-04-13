}
EXPORT_SYMBOL_GPL(iscsi_destroy_flashnode_sess);

static int iscsi_iter_destroy_flashnode_fn(struct device *dev, void *data)
{
	if (!iscsi_flashnode_bus_match(dev, NULL))
		return 0;
