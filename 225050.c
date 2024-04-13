 * stop work. But, in that case, it is guaranteed not to race with
 * iscsi_destroy by conn_mutex.
 */
static void iscsi_if_stop_conn(struct iscsi_cls_conn *conn, int flag)
{
	/*
	 * It is important that this path doesn't rely on
	 * rx_queue_mutex, otherwise, a thread doing allocation on a
	 * start_session/start_connection could sleep waiting on a
	 * writeback to a failed iscsi device, that cannot be recovered
	 * because the lock is held.  If we don't hold it here, the
	 * kernel stop_conn_work_fn has a chance to stop the broken
	 * session and resolve the allocation.
	 *
	 * Still, the user invoked .stop_conn() needs to be serialized
	 * with stop_conn_work_fn by a private mutex.  Not pretty, but
	 * it works.
	 */
	mutex_lock(&conn_mutex);
	conn->transport->stop_conn(conn, flag);