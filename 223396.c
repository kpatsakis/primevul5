int tipc_node_xmit(struct net *net, struct sk_buff_head *list,
		   u32 dnode, int selector)
{
	struct tipc_link_entry *le = NULL;
	struct tipc_node *n;
	struct sk_buff_head xmitq;
	bool node_up = false;
	int bearer_id;
	int rc;

	if (in_own_node(net, dnode)) {
		tipc_loopback_trace(net, list);
		spin_lock_init(&list->lock);
		tipc_sk_rcv(net, list);
		return 0;
	}

	n = tipc_node_find(net, dnode);
	if (unlikely(!n)) {
		__skb_queue_purge(list);
		return -EHOSTUNREACH;
	}

	tipc_node_read_lock(n);
	node_up = node_is_up(n);
	if (node_up && n->peer_net && check_net(n->peer_net)) {
		/* xmit inner linux container */
		tipc_lxc_xmit(n->peer_net, list);
		if (likely(skb_queue_empty(list))) {
			tipc_node_read_unlock(n);
			tipc_node_put(n);
			return 0;
		}
	}

	bearer_id = n->active_links[selector & 1];
	if (unlikely(bearer_id == INVALID_BEARER_ID)) {
		tipc_node_read_unlock(n);
		tipc_node_put(n);
		__skb_queue_purge(list);
		return -EHOSTUNREACH;
	}

	__skb_queue_head_init(&xmitq);
	le = &n->links[bearer_id];
	spin_lock_bh(&le->lock);
	rc = tipc_link_xmit(le->link, list, &xmitq);
	spin_unlock_bh(&le->lock);
	tipc_node_read_unlock(n);

	if (unlikely(rc == -ENOBUFS))
		tipc_node_link_down(n, bearer_id, false);
	else
		tipc_bearer_xmit(net, bearer_id, &xmitq, &le->maddr, n);

	tipc_node_put(n);

	return rc;
}