int tipc_nl_node_dump_monitor(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	u32 prev_bearer = cb->args[0];
	struct tipc_nl_msg msg;
	int bearer_id;
	int err;

	if (prev_bearer == MAX_BEARERS)
		return 0;

	msg.skb = skb;
	msg.portid = NETLINK_CB(cb->skb).portid;
	msg.seq = cb->nlh->nlmsg_seq;

	rtnl_lock();
	for (bearer_id = prev_bearer; bearer_id < MAX_BEARERS; bearer_id++) {
		err = __tipc_nl_add_monitor(net, &msg, bearer_id);
		if (err)
			break;
	}
	rtnl_unlock();
	cb->args[0] = bearer_id;

	return skb->len;
}