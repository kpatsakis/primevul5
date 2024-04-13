	return err;
}

static int iscsi_session_has_conns(int sid)
{
	struct iscsi_cls_conn *conn;
	unsigned long flags;
	int found = 0;

	spin_lock_irqsave(&connlock, flags);
	list_for_each_entry(conn, &connlist, conn_list) {
		if (iscsi_conn_get_sid(conn) == sid) {
			found = 1;
			break;
		}
	}
	spin_unlock_irqrestore(&connlock, flags);