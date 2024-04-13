u32 tipc_node_get_addr(struct tipc_node *node)
{
	return (node) ? node->addr : 0;
}