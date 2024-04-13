static void tipc_node_timeout(struct timer_list *t)
{
	struct tipc_node *n = from_timer(n, t, timer);
	struct tipc_link_entry *le;
	struct sk_buff_head xmitq;
	int remains = n->link_cnt;
	int bearer_id;
	int rc = 0;

	trace_tipc_node_timeout(n, false, " ");
	if (!node_is_up(n) && tipc_node_cleanup(n)) {
		/*Removing the reference of Timer*/
		tipc_node_put(n);
		return;
	}

#ifdef CONFIG_TIPC_CRYPTO
	/* Take any crypto key related actions first */
	tipc_crypto_timeout(n->crypto_rx);
#endif
	__skb_queue_head_init(&xmitq);

	/* Initial node interval to value larger (10 seconds), then it will be
	 * recalculated with link lowest tolerance
	 */
	tipc_node_read_lock(n);
	n->keepalive_intv = 10000;
	tipc_node_read_unlock(n);
	for (bearer_id = 0; remains && (bearer_id < MAX_BEARERS); bearer_id++) {
		tipc_node_read_lock(n);
		le = &n->links[bearer_id];
		if (le->link) {
			spin_lock_bh(&le->lock);
			/* Link tolerance may change asynchronously: */
			tipc_node_calculate_timer(n, le->link);
			rc = tipc_link_timeout(le->link, &xmitq);
			spin_unlock_bh(&le->lock);
			remains--;
		}
		tipc_node_read_unlock(n);
		tipc_bearer_xmit(n->net, bearer_id, &xmitq, &le->maddr, n);
		if (rc & TIPC_LINK_DOWN_EVT)
			tipc_node_link_down(n, bearer_id, false);
	}
	mod_timer(&n->timer, jiffies + msecs_to_jiffies(n->keepalive_intv));
}