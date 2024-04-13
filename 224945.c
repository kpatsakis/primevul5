 * non-zero.
 */
struct iscsi_cls_conn *
iscsi_create_conn(struct iscsi_cls_session *session, int dd_size, uint32_t cid)
{
	struct iscsi_transport *transport = session->transport;
	struct iscsi_cls_conn *conn;
	unsigned long flags;
	int err;

	conn = kzalloc(sizeof(*conn) + dd_size, GFP_KERNEL);
	if (!conn)
		return NULL;
	if (dd_size)
		conn->dd_data = &conn[1];

	mutex_init(&conn->ep_mutex);
	INIT_LIST_HEAD(&conn->conn_list);
	INIT_LIST_HEAD(&conn->conn_list_err);
	conn->transport = transport;
	conn->cid = cid;
	conn->state = ISCSI_CONN_DOWN;

	/* this is released in the dev's release function */
	if (!get_device(&session->dev))
		goto free_conn;

	dev_set_name(&conn->dev, "connection%d:%u", session->sid, cid);
	conn->dev.parent = &session->dev;
	conn->dev.release = iscsi_conn_release;
	err = device_register(&conn->dev);
	if (err) {
		iscsi_cls_session_printk(KERN_ERR, session, "could not "
					 "register connection's dev\n");
		goto release_parent_ref;
	}
	err = transport_register_device(&conn->dev);
	if (err) {
		iscsi_cls_session_printk(KERN_ERR, session, "could not "
					 "register transport's dev\n");
		goto release_conn_ref;
	}

	spin_lock_irqsave(&connlock, flags);
	list_add(&conn->conn_list, &connlist);
	spin_unlock_irqrestore(&connlock, flags);

	ISCSI_DBG_TRANS_CONN(conn, "Completed conn creation\n");
	return conn;

release_conn_ref:
	device_unregister(&conn->dev);
	put_device(&session->dev);
	return NULL;
release_parent_ref:
	put_device(&session->dev);
free_conn: