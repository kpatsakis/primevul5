static void tipc_node_bc_rcv(struct net *net, struct sk_buff *skb, int bearer_id)
{
	int rc;
	struct sk_buff_head xmitq;
	struct tipc_bclink_entry *be;
	struct tipc_link_entry *le;
	struct tipc_msg *hdr = buf_msg(skb);
	int usr = msg_user(hdr);
	u32 dnode = msg_destnode(hdr);
	struct tipc_node *n;

	__skb_queue_head_init(&xmitq);

	/* If NACK for other node, let rcv link for that node peek into it */
	if ((usr == BCAST_PROTOCOL) && (dnode != tipc_own_addr(net)))
		n = tipc_node_find(net, dnode);
	else
		n = tipc_node_find(net, msg_prevnode(hdr));
	if (!n) {
		kfree_skb(skb);
		return;
	}
	be = &n->bc_entry;
	le = &n->links[bearer_id];

	rc = tipc_bcast_rcv(net, be->link, skb);

	/* Broadcast ACKs are sent on a unicast link */
	if (rc & TIPC_LINK_SND_STATE) {
		tipc_node_read_lock(n);
		tipc_link_build_state_msg(le->link, &xmitq);
		tipc_node_read_unlock(n);
	}

	if (!skb_queue_empty(&xmitq))
		tipc_bearer_xmit(net, bearer_id, &xmitq, &le->maddr, n);

	if (!skb_queue_empty(&be->inputq1))
		tipc_node_mcast_rcv(n);

	/* Handle NAME_DISTRIBUTOR messages sent from 1.7 nodes */
	if (!skb_queue_empty(&n->bc_entry.namedq))
		tipc_named_rcv(net, &n->bc_entry.namedq,
			       &n->bc_entry.named_rcv_nxt,
			       &n->bc_entry.named_open);

	/* If reassembly or retransmission failure => reset all links to peer */
	if (rc & TIPC_LINK_DOWN_EVT)
		tipc_node_reset_links(n);

	tipc_node_put(n);
}