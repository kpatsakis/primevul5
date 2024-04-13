EXPORT_SYMBOL_GPL(iscsi_ping_comp_event);

static int
iscsi_if_send_reply(u32 portid, int type, void *payload, int size)
{
	struct sk_buff	*skb;
	struct nlmsghdr	*nlh;
	int len = nlmsg_total_size(size);

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
		printk(KERN_ERR "Could not allocate skb to send reply.\n");
		return -ENOMEM;
	}

	nlh = __nlmsg_put(skb, 0, 0, type, (len - sizeof(*nlh)), 0);