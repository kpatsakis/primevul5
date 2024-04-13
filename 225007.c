	return err;
}

static int iscsi_if_ep_disconnect(struct iscsi_transport *transport,
				  u64 ep_handle)
{
	struct iscsi_cls_conn *conn;
	struct iscsi_endpoint *ep;

	if (!transport->ep_disconnect)
		return -EINVAL;

	ep = iscsi_lookup_endpoint(ep_handle);
	if (!ep)
		return -EINVAL;
	conn = ep->conn;
	if (conn) {
		mutex_lock(&conn->ep_mutex);
		conn->ep = NULL;
		mutex_unlock(&conn->ep_mutex);
	}
