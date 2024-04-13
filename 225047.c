}
EXPORT_SYMBOL_GPL(iscsi_is_session_dev);

static int iscsi_iter_session_fn(struct device *dev, void *data)
{
	void (* fn) (struct iscsi_cls_session *) = data;

	if (!iscsi_is_session_dev(dev))
		return 0;