static void node_lost_contact(struct tipc_node *n,
			      struct sk_buff_head *inputq)
{
	struct tipc_sock_conn *conn, *safe;
	struct tipc_link *l;
	struct list_head *conns = &n->conn_sks;
	struct sk_buff *skb;
	uint i;

	pr_debug("Lost contact with %x\n", n->addr);
	n->delete_at = jiffies + msecs_to_jiffies(NODE_CLEANUP_AFTER);
	trace_tipc_node_lost_contact(n, true, " ");

	/* Clean up broadcast state */
	tipc_bcast_remove_peer(n->net, n->bc_entry.link);
	skb_queue_purge(&n->bc_entry.namedq);

	/* Abort any ongoing link failover */
	for (i = 0; i < MAX_BEARERS; i++) {
		l = n->links[i].link;
		if (l)
			tipc_link_fsm_evt(l, LINK_FAILOVER_END_EVT);
	}

	/* Notify publications from this node */
	n->action_flags |= TIPC_NOTIFY_NODE_DOWN;
	n->peer_net = NULL;
	n->peer_hash_mix = 0;
	/* Notify sockets connected to node */
	list_for_each_entry_safe(conn, safe, conns, list) {
		skb = tipc_msg_create(TIPC_CRITICAL_IMPORTANCE, TIPC_CONN_MSG,
				      SHORT_H_SIZE, 0, tipc_own_addr(n->net),
				      conn->peer_node, conn->port,
				      conn->peer_port, TIPC_ERR_NO_NODE);
		if (likely(skb))
			skb_queue_tail(inputq, skb);
		list_del(&conn->list);
		kfree(conn);
	}
}