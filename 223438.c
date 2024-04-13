u16 tipc_node_get_capabilities(struct net *net, u32 addr)
{
	struct tipc_node *n;
	u16 caps;

	n = tipc_node_find(net, addr);
	if (unlikely(!n))
		return TIPC_NODE_CAPABILITIES;
	caps = n->capabilities;
	tipc_node_put(n);
	return caps;
}