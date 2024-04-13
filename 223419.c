static void tipc_node_link_up(struct tipc_node *n, int bearer_id,
			      struct sk_buff_head *xmitq)
{
	struct tipc_media_addr *maddr;

	tipc_node_write_lock(n);
	__tipc_node_link_up(n, bearer_id, xmitq);
	maddr = &n->links[bearer_id].maddr;
	tipc_bearer_xmit(n->net, bearer_id, xmitq, maddr, n);
	tipc_node_write_unlock(n);
}