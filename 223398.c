u32 tipc_node_try_addr(struct net *net, u8 *id, u32 addr)
{
	struct tipc_net *tn = tipc_net(net);
	struct tipc_node *n;
	bool preliminary;
	u32 sugg_addr;

	/* Suggest new address if some other peer is using this one */
	n = tipc_node_find(net, addr);
	if (n) {
		if (!memcmp(n->peer_id, id, NODE_ID_LEN))
			addr = 0;
		tipc_node_put(n);
		if (!addr)
			return 0;
		return tipc_node_suggest_addr(net, addr);
	}

	/* Suggest previously used address if peer is known */
	n = tipc_node_find_by_id(net, id);
	if (n) {
		sugg_addr = n->addr;
		preliminary = n->preliminary;
		tipc_node_put(n);
		if (!preliminary)
			return sugg_addr;
	}

	/* Even this node may be in conflict */
	if (tn->trial_addr == addr)
		return tipc_node_suggest_addr(net, addr);

	return 0;
}