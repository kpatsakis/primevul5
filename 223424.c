bool tipc_node_is_up(struct net *net, u32 addr)
{
	struct tipc_node *n;
	bool retval = false;

	if (in_own_node(net, addr))
		return true;

	n = tipc_node_find(net, addr);
	if (!n)
		return false;
	retval = node_is_up(n);
	tipc_node_put(n);
	return retval;
}