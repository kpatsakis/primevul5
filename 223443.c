int tipc_nl_node_dump_monitor_peer(struct sk_buff *skb,
				   struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	u32 prev_node = cb->args[1];
	u32 bearer_id = cb->args[2];
	int done = cb->args[0];
	struct tipc_nl_msg msg;
	int err;

	if (!prev_node) {
		struct nlattr **attrs = genl_dumpit_info(cb)->attrs;
		struct nlattr *mon[TIPC_NLA_MON_MAX + 1];

		if (!attrs[TIPC_NLA_MON])
			return -EINVAL;

		err = nla_parse_nested_deprecated(mon, TIPC_NLA_MON_MAX,
						  attrs[TIPC_NLA_MON],
						  tipc_nl_monitor_policy,
						  NULL);
		if (err)
			return err;

		if (!mon[TIPC_NLA_MON_REF])
			return -EINVAL;

		bearer_id = nla_get_u32(mon[TIPC_NLA_MON_REF]);

		if (bearer_id >= MAX_BEARERS)
			return -EINVAL;
	}

	if (done)
		return 0;

	msg.skb = skb;
	msg.portid = NETLINK_CB(cb->skb).portid;
	msg.seq = cb->nlh->nlmsg_seq;

	rtnl_lock();
	err = tipc_nl_add_monitor_peer(net, &msg, bearer_id, &prev_node);
	if (!err)
		done = 1;

	rtnl_unlock();
	cb->args[0] = done;
	cb->args[1] = prev_node;
	cb->args[2] = bearer_id;

	return skb->len;
}