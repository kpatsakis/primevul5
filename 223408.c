static void tipc_node_assign_peer_net(struct tipc_node *n, u32 hash_mixes)
{
	int net_id = tipc_netid(n->net);
	struct tipc_net *tn_peer;
	struct net *tmp;
	u32 hash_chk;

	if (n->peer_net)
		return;

	for_each_net_rcu(tmp) {
		tn_peer = tipc_net(tmp);
		if (!tn_peer)
			continue;
		/* Integrity checking whether node exists in namespace or not */
		if (tn_peer->net_id != net_id)
			continue;
		if (memcmp(n->peer_id, tn_peer->node_id, NODE_ID_LEN))
			continue;
		hash_chk = tipc_net_hash_mixes(tmp, tn_peer->random);
		if (hash_mixes ^ hash_chk)
			continue;
		n->peer_net = tmp;
		n->peer_hash_mix = hash_mixes;
		break;
	}
}