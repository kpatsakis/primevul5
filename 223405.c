int tipc_nl_node_flush_key(struct sk_buff *skb, struct genl_info *info)
{
	int err;

	rtnl_lock();
	err = __tipc_nl_node_flush_key(skb, info);
	rtnl_unlock();

	return err;
}