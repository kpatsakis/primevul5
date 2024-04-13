void tipc_node_pre_cleanup_net(struct net *exit_net)
{
	struct tipc_node *n;
	struct tipc_net *tn;
	struct net *tmp;

	rcu_read_lock();
	for_each_net_rcu(tmp) {
		if (tmp == exit_net)
			continue;
		tn = tipc_net(tmp);
		if (!tn)
			continue;
		spin_lock_bh(&tn->node_list_lock);
		list_for_each_entry_rcu(n, &tn->node_list, list) {
			if (!n->peer_net)
				continue;
			if (n->peer_net != exit_net)
				continue;
			tipc_node_write_lock(n);
			n->peer_net = NULL;
			n->peer_hash_mix = 0;
			tipc_node_write_unlock_fast(n);
			break;
		}
		spin_unlock_bh(&tn->node_list_lock);
	}
	rcu_read_unlock();
}