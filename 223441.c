static void tipc_lxc_xmit(struct net *peer_net, struct sk_buff_head *list)
{
	struct tipc_msg *hdr = buf_msg(skb_peek(list));
	struct sk_buff_head inputq;

	switch (msg_user(hdr)) {
	case TIPC_LOW_IMPORTANCE:
	case TIPC_MEDIUM_IMPORTANCE:
	case TIPC_HIGH_IMPORTANCE:
	case TIPC_CRITICAL_IMPORTANCE:
		if (msg_connected(hdr) || msg_named(hdr) ||
		    msg_direct(hdr)) {
			tipc_loopback_trace(peer_net, list);
			spin_lock_init(&list->lock);
			tipc_sk_rcv(peer_net, list);
			return;
		}
		if (msg_mcast(hdr)) {
			tipc_loopback_trace(peer_net, list);
			skb_queue_head_init(&inputq);
			tipc_sk_mcast_rcv(peer_net, list, &inputq);
			__skb_queue_purge(list);
			skb_queue_purge(&inputq);
			return;
		}
		return;
	case MSG_FRAGMENTER:
		if (tipc_msg_assemble(list)) {
			tipc_loopback_trace(peer_net, list);
			skb_queue_head_init(&inputq);
			tipc_sk_mcast_rcv(peer_net, list, &inputq);
			__skb_queue_purge(list);
			skb_queue_purge(&inputq);
		}
		return;
	case GROUP_PROTOCOL:
	case CONN_MANAGER:
		tipc_loopback_trace(peer_net, list);
		spin_lock_init(&list->lock);
		tipc_sk_rcv(peer_net, list);
		return;
	case LINK_PROTOCOL:
	case NAME_DISTRIBUTOR:
	case TUNNEL_PROTOCOL:
	case BCAST_PROTOCOL:
		return;
	default:
		return;
	}
}