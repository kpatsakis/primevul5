int tipc_node_get_mtu(struct net *net, u32 addr, u32 sel, bool connected)
{
	struct tipc_node *n;
	int bearer_id;
	unsigned int mtu = MAX_MSG_SIZE;

	n = tipc_node_find(net, addr);
	if (unlikely(!n))
		return mtu;

	/* Allow MAX_MSG_SIZE when building connection oriented message
	 * if they are in the same core network
	 */
	if (n->peer_net && connected) {
		tipc_node_put(n);
		return mtu;
	}

	bearer_id = n->active_links[sel & 1];
	if (likely(bearer_id != INVALID_BEARER_ID))
		mtu = n->links[bearer_id].mtu;
	tipc_node_put(n);
	return mtu;
}