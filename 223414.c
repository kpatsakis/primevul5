void tipc_node_broadcast(struct net *net, struct sk_buff *skb, int rc_dests)
{
	struct sk_buff_head xmitq;
	struct sk_buff *txskb;
	struct tipc_node *n;
	u16 dummy;
	u32 dst;

	/* Use broadcast if all nodes support it */
	if (!rc_dests && tipc_bcast_get_mode(net) != BCLINK_MODE_RCAST) {
		__skb_queue_head_init(&xmitq);
		__skb_queue_tail(&xmitq, skb);
		tipc_bcast_xmit(net, &xmitq, &dummy);
		return;
	}

	/* Otherwise use legacy replicast method */
	rcu_read_lock();
	list_for_each_entry_rcu(n, tipc_nodes(net), list) {
		dst = n->addr;
		if (in_own_node(net, dst))
			continue;
		if (!node_is_up(n))
			continue;
		txskb = pskb_copy(skb, GFP_ATOMIC);
		if (!txskb)
			break;
		msg_set_destnode(buf_msg(txskb), dst);
		tipc_node_xmit_skb(net, txskb, dst, 0);
	}
	rcu_read_unlock();
	kfree_skb(skb);
}