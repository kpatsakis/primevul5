void tipc_node_unsubscribe(struct net *net, struct list_head *subscr, u32 addr)
{
	struct tipc_node *n;

	if (in_own_node(net, addr))
		return;

	n = tipc_node_find(net, addr);
	if (!n) {
		pr_warn("Node unsubscribe rejected, unknown node 0x%x\n", addr);
		return;
	}
	tipc_node_write_lock(n);
	list_del_init(subscr);
	tipc_node_write_unlock_fast(n);
	tipc_node_put(n);
}