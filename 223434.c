static struct tipc_node *tipc_node_find_by_id(struct net *net, u8 *id)
{
	struct tipc_net *tn = tipc_net(net);
	struct tipc_node *n;
	bool found = false;

	rcu_read_lock();
	list_for_each_entry_rcu(n, &tn->node_list, list) {
		read_lock_bh(&n->lock);
		if (!memcmp(id, n->peer_id, 16) &&
		    kref_get_unless_zero(&n->kref))
			found = true;
		read_unlock_bh(&n->lock);
		if (found)
			break;
	}
	rcu_read_unlock();
	return found ? n : NULL;
}