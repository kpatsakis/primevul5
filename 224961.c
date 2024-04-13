	return nlmsg_unicast(nls, skb, portid);
}

int iscsi_recv_pdu(struct iscsi_cls_conn *conn, struct iscsi_hdr *hdr,
		   char *data, uint32_t data_size)
{
	struct nlmsghdr	*nlh;
	struct sk_buff *skb;
	struct iscsi_uevent *ev;
	char *pdu;
	struct iscsi_internal *priv;
	int len = nlmsg_total_size(sizeof(*ev) + sizeof(struct iscsi_hdr) +
				   data_size);

	priv = iscsi_if_transport_lookup(conn->transport);
	if (!priv)
		return -EINVAL;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
		iscsi_conn_error_event(conn, ISCSI_ERR_CONN_FAILED);
		iscsi_cls_conn_printk(KERN_ERR, conn, "can not deliver "
				      "control PDU: OOM\n");
		return -ENOMEM;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = nlmsg_data(nlh);
	memset(ev, 0, sizeof(*ev));
	ev->transport_handle = iscsi_handle(conn->transport);
	ev->type = ISCSI_KEVENT_RECV_PDU;
	ev->r.recv_req.cid = conn->cid;
	ev->r.recv_req.sid = iscsi_conn_get_sid(conn);
	pdu = (char*)ev + sizeof(*ev);
	memcpy(pdu, hdr, sizeof(struct iscsi_hdr));
	memcpy(pdu + sizeof(struct iscsi_hdr), data, data_size);