static struct sk_buff *xennet_get_rx_skb(struct netfront_queue *queue,
					 RING_IDX ri)
{
	int i = xennet_rxidx(ri);
	struct sk_buff *skb = queue->rx_skbs[i];
	queue->rx_skbs[i] = NULL;
	return skb;
}