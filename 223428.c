static void  tipc_node_clear_links(struct tipc_node *node)
{
	int i;

	for (i = 0; i < MAX_BEARERS; i++) {
		struct tipc_link_entry *le = &node->links[i];

		if (le->link) {
			kfree(le->link);
			le->link = NULL;
			node->link_cnt--;
		}
	}
}