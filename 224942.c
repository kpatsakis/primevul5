	return dev->release == iscsi_conn_release;
}

static int iscsi_iter_destroy_conn_fn(struct device *dev, void *data)
{
	if (!iscsi_is_conn_dev(dev))