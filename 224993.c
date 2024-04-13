 *
 * This can be called from a LLD or iscsi_transport.
 */
int iscsi_destroy_conn(struct iscsi_cls_conn *conn)
{
	unsigned long flags;

	spin_lock_irqsave(&connlock, flags);
	list_del(&conn->conn_list);
	list_del(&conn->conn_list_err);
	spin_unlock_irqrestore(&connlock, flags);

	transport_unregister_device(&conn->dev);
	ISCSI_DBG_TRANS_CONN(conn, "Completing conn destruction\n");