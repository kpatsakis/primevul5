}
EXPORT_SYMBOL_GPL(iscsi_find_flashnode_conn);

static int iscsi_iter_destroy_flashnode_conn_fn(struct device *dev, void *data)
{
	if (!iscsi_is_flashnode_conn_dev(dev, NULL))
		return 0;