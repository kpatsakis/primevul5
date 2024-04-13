}
EXPORT_SYMBOL_GPL(iscsi_post_host_event);

void iscsi_ping_comp_event(uint32_t host_no, struct iscsi_transport *transport,
			   uint32_t status, uint32_t pid, uint32_t data_size,
			   uint8_t *data)
{
	struct nlmsghdr *nlh;
	struct sk_buff *skb;
	struct iscsi_uevent *ev;
	int len = nlmsg_total_size(sizeof(*ev) + data_size);

	skb = alloc_skb(len, GFP_NOIO);
	if (!skb) {
		printk(KERN_ERR "gracefully ignored ping comp: OOM\n");
		return;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = nlmsg_data(nlh);
	ev->transport_handle = iscsi_handle(transport);
	ev->type = ISCSI_KEVENT_PING_COMP;
	ev->r.ping_comp.host_no = host_no;
	ev->r.ping_comp.status = status;
	ev->r.ping_comp.pid = pid;
	ev->r.ping_comp.data_size = data_size;
	memcpy((char *)ev + sizeof(*ev), data, data_size);