static void tipc_node_read_lock(struct tipc_node *n)
{
	read_lock_bh(&n->lock);
}