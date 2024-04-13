}
EXPORT_SYMBOL_GPL(iscsi_create_session);

static void iscsi_conn_release(struct device *dev)
{
	struct iscsi_cls_conn *conn = iscsi_dev_to_conn(dev);
	struct device *parent = conn->dev.parent;

	ISCSI_DBG_TRANS_CONN(conn, "Releasing conn\n");