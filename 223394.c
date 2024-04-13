static void tipc_node_delete(struct tipc_node *node)
{
	trace_tipc_node_delete(node, true, " ");
	tipc_node_delete_from_list(node);

	del_timer_sync(&node->timer);
	tipc_node_put(node);
}