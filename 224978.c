
}

static void stop_conn_work_fn(struct work_struct *work)
{
	struct iscsi_cls_conn *conn, *tmp;
	unsigned long flags;
	LIST_HEAD(recovery_list);

	spin_lock_irqsave(&connlock, flags);
	if (list_empty(&connlist_err)) {
		spin_unlock_irqrestore(&connlock, flags);
		return;
	}
	list_splice_init(&connlist_err, &recovery_list);
	spin_unlock_irqrestore(&connlock, flags);

	list_for_each_entry_safe(conn, tmp, &recovery_list, conn_list_err) {
		uint32_t sid = iscsi_conn_get_sid(conn);
		struct iscsi_cls_session *session;

		session = iscsi_session_lookup(sid);
		if (session) {
			if (system_state != SYSTEM_RUNNING) {
				session->recovery_tmo = 0;
				iscsi_if_stop_conn(conn, STOP_CONN_TERM);
			} else {
				iscsi_if_stop_conn(conn, STOP_CONN_RECOVER);
			}
		}
