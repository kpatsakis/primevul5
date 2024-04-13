}
EXPORT_SYMBOL_GPL(iscsi_conn_login_event);

void iscsi_post_host_event(uint32_t host_no, struct iscsi_transport *transport,
			   enum iscsi_host_event_code code, uint32_t data_size,
			   uint8_t *data)
{
	struct nlmsghdr *nlh;
	struct sk_buff *skb;
	struct iscsi_uevent *ev;
	int len = nlmsg_total_size(sizeof(*ev) + data_size);

	skb = alloc_skb(len, GFP_NOIO);
	if (!skb) {
		printk(KERN_ERR "gracefully ignored host event (%d):%d OOM\n",
		       host_no, code);
		return;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = nlmsg_data(nlh);
	ev->transport_handle = iscsi_handle(transport);
	ev->type = ISCSI_KEVENT_HOST_EVENT;
	ev->r.host_event.host_no = host_no;
	ev->r.host_event.code = code;
	ev->r.host_event.data_size = data_size;

	if (data_size)
		memcpy((char *)ev + sizeof(*ev), data, data_size);