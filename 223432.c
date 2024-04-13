int tipc_nl_node_set_monitor(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr *attrs[TIPC_NLA_MON_MAX + 1];
	struct net *net = sock_net(skb->sk);
	int err;

	if (!info->attrs[TIPC_NLA_MON])
		return -EINVAL;

	err = nla_parse_nested_deprecated(attrs, TIPC_NLA_MON_MAX,
					  info->attrs[TIPC_NLA_MON],
					  tipc_nl_monitor_policy,
					  info->extack);
	if (err)
		return err;

	if (attrs[TIPC_NLA_MON_ACTIVATION_THRESHOLD]) {
		u32 val;

		val = nla_get_u32(attrs[TIPC_NLA_MON_ACTIVATION_THRESHOLD]);
		err = tipc_nl_monitor_set_threshold(net, val);
		if (err)
			return err;
	}

	return 0;
}