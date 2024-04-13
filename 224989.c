}

static int
iscsi_if_destroy_conn(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
	struct iscsi_cls_conn *conn;
	unsigned long flags;

	conn = iscsi_conn_lookup(ev->u.d_conn.sid, ev->u.d_conn.cid);
	if (!conn)
		return -EINVAL;

	spin_lock_irqsave(&connlock, flags);
	if (!list_empty(&conn->conn_list_err)) {
		spin_unlock_irqrestore(&connlock, flags);
		return -EAGAIN;
	}
	spin_unlock_irqrestore(&connlock, flags);

	ISCSI_DBG_TRANS_CONN(conn, "Destroying transport conn\n");

	mutex_lock(&conn_mutex);
	if (transport->destroy_conn)
		transport->destroy_conn(conn);
	mutex_unlock(&conn_mutex);