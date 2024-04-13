static void tipc_node_write_lock(struct tipc_node *n)
{
	write_lock_bh(&n->lock);
}