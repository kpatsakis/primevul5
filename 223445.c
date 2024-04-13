static u32 tipc_node_suggest_addr(struct net *net, u32 addr)
{
	struct tipc_node *n;

	addr ^= tipc_net(net)->random;
	while ((n = tipc_node_find(net, addr))) {
		tipc_node_put(n);
		addr++;
	}
	return addr;
}