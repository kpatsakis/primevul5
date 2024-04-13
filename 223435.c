int tipc_node_get_linkname(struct net *net, u32 bearer_id, u32 addr,
			   char *linkname, size_t len)
{
	struct tipc_link *link;
	int err = -EINVAL;
	struct tipc_node *node = tipc_node_find(net, addr);

	if (!node)
		return err;

	if (bearer_id >= MAX_BEARERS)
		goto exit;

	tipc_node_read_lock(node);
	link = node->links[bearer_id].link;
	if (link) {
		strncpy(linkname, tipc_link_name(link), len);
		err = 0;
	}
	tipc_node_read_unlock(node);
exit:
	tipc_node_put(node);
	return err;
}