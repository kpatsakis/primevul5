struct tipc_crypto *tipc_node_crypto_rx_by_addr(struct net *net, u32 addr)
{
	struct tipc_node *n;

	n = tipc_node_find(net, addr);
	return (n) ? n->crypto_rx : NULL;
}