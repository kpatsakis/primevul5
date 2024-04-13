static struct tipc_node *tipc_node_find(struct net *net, u32 addr)
{
	struct tipc_net *tn = tipc_net(net);
	struct tipc_node *node;
	unsigned int thash = tipc_hashfn(addr);

	rcu_read_lock();
	hlist_for_each_entry_rcu(node, &tn->node_htable[thash], hash) {
		if (node->addr != addr || node->preliminary)
			continue;
		if (!kref_get_unless_zero(&node->kref))
			node = NULL;
		break;
	}
	rcu_read_unlock();
	return node;
}