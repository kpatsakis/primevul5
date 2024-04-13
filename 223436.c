static void tipc_node_read_unlock(struct tipc_node *n)
{
	read_unlock_bh(&n->lock);
}