static void tipc_node_link_down(struct tipc_node *n, int bearer_id, bool delete)
{
	struct tipc_link_entry *le = &n->links[bearer_id];
	struct tipc_media_addr *maddr = NULL;
	struct tipc_link *l = le->link;
	int old_bearer_id = bearer_id;
	struct sk_buff_head xmitq;

	if (!l)
		return;

	__skb_queue_head_init(&xmitq);

	tipc_node_write_lock(n);
	if (!tipc_link_is_establishing(l)) {
		__tipc_node_link_down(n, &bearer_id, &xmitq, &maddr);
	} else {
		/* Defuse pending tipc_node_link_up() */
		tipc_link_reset(l);
		tipc_link_fsm_evt(l, LINK_RESET_EVT);
	}
	if (delete) {
		kfree(l);
		le->link = NULL;
		n->link_cnt--;
	}
	trace_tipc_node_link_down(n, true, "node link down or deleted!");
	tipc_node_write_unlock(n);
	if (delete)
		tipc_mon_remove_peer(n->net, n->addr, old_bearer_id);
	if (!skb_queue_empty(&xmitq))
		tipc_bearer_xmit(n->net, bearer_id, &xmitq, maddr, n);
	tipc_sk_rcv(n->net, &le->inputq);
}