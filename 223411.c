void tipc_node_stop(struct net *net)
{
	struct tipc_net *tn = tipc_net(net);
	struct tipc_node *node, *t_node;

	spin_lock_bh(&tn->node_list_lock);
	list_for_each_entry_safe(node, t_node, &tn->node_list, list)
		tipc_node_delete(node);
	spin_unlock_bh(&tn->node_list_lock);
}