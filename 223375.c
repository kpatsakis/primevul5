static struct tipc_node *tipc_node_find_by_name(struct net *net,
						const char *link_name,
						unsigned int *bearer_id)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct tipc_link *l;
	struct tipc_node *n;
	struct tipc_node *found_node = NULL;
	int i;

	*bearer_id = 0;
	rcu_read_lock();
	list_for_each_entry_rcu(n, &tn->node_list, list) {
		tipc_node_read_lock(n);
		for (i = 0; i < MAX_BEARERS; i++) {
			l = n->links[i].link;
			if (l && !strcmp(tipc_link_name(l), link_name)) {
				*bearer_id = i;
				found_node = n;
				break;
			}
		}
		tipc_node_read_unlock(n);
		if (found_node)
			break;
	}
	rcu_read_unlock();

	return found_node;
}