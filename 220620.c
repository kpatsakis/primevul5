static int packet_direct_xmit(struct sk_buff *skb)
{
	return dev_direct_xmit(skb, packet_pick_tx_queue(skb));
}