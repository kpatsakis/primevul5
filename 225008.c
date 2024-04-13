/*
 * Returns the matching connection to a given sid / cid tuple
 */
static struct iscsi_cls_conn *iscsi_conn_lookup(uint32_t sid, uint32_t cid)
{
	unsigned long flags;
	struct iscsi_cls_conn *conn;

	spin_lock_irqsave(&connlock, flags);
	list_for_each_entry(conn, &connlist, conn_list) {
		if ((conn->cid == cid) && (iscsi_conn_get_sid(conn) == sid)) {
			spin_unlock_irqrestore(&connlock, flags);
			return conn;
		}
	}