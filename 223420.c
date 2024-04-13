static bool tipc_node_cleanup(struct tipc_node *peer)
{
	struct tipc_node *temp_node;
	struct tipc_net *tn = tipc_net(peer->net);
	bool deleted = false;

	/* If lock held by tipc_node_stop() the node will be deleted anyway */
	if (!spin_trylock_bh(&tn->node_list_lock))
		return false;

	tipc_node_write_lock(peer);

	if (!node_is_up(peer) && time_after(jiffies, peer->delete_at)) {
		tipc_node_clear_links(peer);
		tipc_node_delete_from_list(peer);
		deleted = true;
	}
	tipc_node_write_unlock(peer);

	if (!deleted) {
		spin_unlock_bh(&tn->node_list_lock);
		return deleted;
	}

	/* Calculate cluster capabilities */
	tn->capabilities = TIPC_NODE_CAPABILITIES;
	list_for_each_entry_rcu(temp_node, &tn->node_list, list) {
		tn->capabilities &= temp_node->capabilities;
	}
	tipc_bcast_toggle_rcast(peer->net,
				(tn->capabilities & TIPC_BCAST_RCAST));
	spin_unlock_bh(&tn->node_list_lock);
	return deleted;
}