static void tipc_node_reset_links(struct tipc_node *n)
{
	int i;

	pr_warn("Resetting all links to %x\n", n->addr);

	trace_tipc_node_reset_links(n, true, " ");
	for (i = 0; i < MAX_BEARERS; i++) {
		tipc_node_link_down(n, i, false);
	}
}