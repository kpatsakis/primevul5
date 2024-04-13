}

static int
iscsi_if_create_conn(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
	struct iscsi_cls_conn *conn;
	struct iscsi_cls_session *session;

	session = iscsi_session_lookup(ev->u.c_conn.sid);
	if (!session) {
		printk(KERN_ERR "iscsi: invalid session %d.\n",
		       ev->u.c_conn.sid);
		return -EINVAL;
	}

	conn = transport->create_conn(session, ev->u.c_conn.cid);
	if (!conn) {
		iscsi_cls_session_printk(KERN_ERR, session,
					 "couldn't create a new connection.");
		return -ENOMEM;
	}

	ev->r.c_conn_ret.sid = session->sid;
	ev->r.c_conn_ret.cid = conn->cid;
