static void tipc_node_bc_sync_rcv(struct tipc_node *n, struct tipc_msg *hdr,
				  int bearer_id, struct sk_buff_head *xmitq)
{
	struct tipc_link *ucl;
	int rc;

	rc = tipc_bcast_sync_rcv(n->net, n->bc_entry.link, hdr, xmitq);

	if (rc & TIPC_LINK_DOWN_EVT) {
		tipc_node_reset_links(n);
		return;
	}

	if (!(rc & TIPC_LINK_SND_STATE))
		return;

	/* If probe message, a STATE response will be sent anyway */
	if (msg_probe(hdr))
		return;

	/* Produce a STATE message carrying broadcast NACK */
	tipc_node_read_lock(n);
	ucl = n->links[bearer_id].link;
	if (ucl)
		tipc_link_build_state_msg(ucl, xmitq);
	tipc_node_read_unlock(n);
}