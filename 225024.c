}
EXPORT_SYMBOL_GPL(iscsi_conn_error_event);

void iscsi_conn_login_event(struct iscsi_cls_conn *conn,
			    enum iscsi_conn_state state)
{
	struct nlmsghdr *nlh;
	struct sk_buff  *skb;
	struct iscsi_uevent *ev;
	struct iscsi_internal *priv;
	int len = nlmsg_total_size(sizeof(*ev));

	priv = iscsi_if_transport_lookup(conn->transport);
	if (!priv)
		return;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
		iscsi_cls_conn_printk(KERN_ERR, conn, "gracefully ignored "
				      "conn login (%d)\n", state);
		return;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = nlmsg_data(nlh);
	ev->transport_handle = iscsi_handle(conn->transport);
	ev->type = ISCSI_KEVENT_CONN_LOGIN_STATE;
	ev->r.conn_login.state = state;
	ev->r.conn_login.cid = conn->cid;
	ev->r.conn_login.sid = iscsi_conn_get_sid(conn);
	iscsi_multicast_skb(skb, ISCSI_NL_GRP_ISCSID, GFP_ATOMIC);
