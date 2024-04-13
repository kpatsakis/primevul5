}
EXPORT_SYMBOL_GPL(iscsi_recv_pdu);

int iscsi_offload_mesg(struct Scsi_Host *shost,
		       struct iscsi_transport *transport, uint32_t type,
		       char *data, uint16_t data_size)
{
	struct nlmsghdr	*nlh;
	struct sk_buff *skb;
	struct iscsi_uevent *ev;
	int len = nlmsg_total_size(sizeof(*ev) + data_size);

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
		printk(KERN_ERR "can not deliver iscsi offload message:OOM\n");
		return -ENOMEM;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = nlmsg_data(nlh);
	memset(ev, 0, sizeof(*ev));
	ev->type = type;
	ev->transport_handle = iscsi_handle(transport);
	switch (type) {
	case ISCSI_KEVENT_PATH_REQ:
		ev->r.req_path.host_no = shost->host_no;
		break;
	case ISCSI_KEVENT_IF_DOWN:
		ev->r.notify_if_down.host_no = shost->host_no;
		break;
	}

	memcpy((char *)ev + sizeof(*ev), data, data_size);