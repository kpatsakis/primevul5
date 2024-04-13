static int __tipc_nl_add_node_links(struct net *net, struct tipc_nl_msg *msg,
				    struct tipc_node *node, u32 *prev_link,
				    bool bc_link)
{
	u32 i;
	int err;

	for (i = *prev_link; i < MAX_BEARERS; i++) {
		*prev_link = i;

		if (!node->links[i].link)
			continue;

		err = __tipc_nl_add_link(net, msg,
					 node->links[i].link, NLM_F_MULTI);
		if (err)
			return err;
	}

	if (bc_link) {
		*prev_link = i;
		err = tipc_nl_add_bc_link(net, msg, node->bc_entry.link);
		if (err)
			return err;
	}

	*prev_link = 0;

	return 0;
}