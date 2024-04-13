bool tipc_node_get_id(struct net *net, u32 addr, u8 *id)
{
	u8 *own_id = tipc_own_id(net);
	struct tipc_node *n;

	if (!own_id)
		return true;

	if (addr == tipc_own_addr(net)) {
		memcpy(id, own_id, TIPC_NODEID_LEN);
		return true;
	}
	n = tipc_node_find(net, addr);
	if (!n)
		return false;

	memcpy(id, &n->peer_id, TIPC_NODEID_LEN);
	tipc_node_put(n);
	return true;
}