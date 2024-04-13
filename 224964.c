	}
}

void iscsi_conn_error_event(struct iscsi_cls_conn *conn, enum iscsi_err error)
{
	struct nlmsghdr	*nlh;
	struct sk_buff	*skb;
	struct iscsi_uevent *ev;
	struct iscsi_internal *priv;
	int len = nlmsg_total_size(sizeof(*ev));
	unsigned long flags;

	spin_lock_irqsave(&connlock, flags);
	list_add(&conn->conn_list_err, &connlist_err);
	spin_unlock_irqrestore(&connlock, flags);
	queue_work(system_unbound_wq, &stop_conn_work);

	priv = iscsi_if_transport_lookup(conn->transport);
	if (!priv)
		return;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
		iscsi_cls_conn_printk(KERN_ERR, conn, "gracefully ignored "
				      "conn error (%d)\n", error);
		return;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = nlmsg_data(nlh);
	ev->transport_handle = iscsi_handle(conn->transport);
	ev->type = ISCSI_KEVENT_CONN_ERROR;
	ev->r.connerror.error = error;
	ev->r.connerror.cid = conn->cid;
	ev->r.connerror.sid = iscsi_conn_get_sid(conn);

	iscsi_multicast_skb(skb, ISCSI_NL_GRP_ISCSID, GFP_ATOMIC);
