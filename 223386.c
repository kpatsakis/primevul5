static void tipc_node_write_unlock_fast(struct tipc_node *n)
{
	write_unlock_bh(&n->lock);
}