void tipc_crypto_msg_rcv(struct net *net, struct sk_buff *skb)
{
	struct tipc_crypto *rx;
	struct tipc_msg *hdr;

	if (unlikely(skb_linearize(skb)))
		goto exit;

	hdr = buf_msg(skb);
	rx = tipc_node_crypto_rx_by_addr(net, msg_prevnode(hdr));
	if (unlikely(!rx))
		goto exit;

	switch (msg_type(hdr)) {
	case KEY_DISTR_MSG:
		if (tipc_crypto_key_rcv(rx, hdr))
			goto exit;
		break;
	default:
		break;
	}

	tipc_node_put(rx->node);

exit:
	kfree_skb(skb);
}