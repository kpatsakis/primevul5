void tipc_node_get(struct tipc_node *node)
{
	kref_get(&node->kref);
}