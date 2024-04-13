static int handle_incoming_queue(struct netfront_queue *queue,
				 struct sk_buff_head *rxq)
{
	struct netfront_stats *rx_stats = this_cpu_ptr(queue->info->rx_stats);
	int packets_dropped = 0;
	struct sk_buff *skb;

	while ((skb = __skb_dequeue(rxq)) != NULL) {
		int pull_to = NETFRONT_SKB_CB(skb)->pull_to;

		if (pull_to > skb_headlen(skb))
			__pskb_pull_tail(skb, pull_to - skb_headlen(skb));

		/* Ethernet work: Delayed to here as it peeks the header. */
		skb->protocol = eth_type_trans(skb, queue->info->netdev);
		skb_reset_network_header(skb);

		if (checksum_setup(queue->info->netdev, skb)) {
			kfree_skb(skb);
			packets_dropped++;
			queue->info->netdev->stats.rx_errors++;
			continue;
		}

		u64_stats_update_begin(&rx_stats->syncp);
		rx_stats->packets++;
		rx_stats->bytes += skb->len;
		u64_stats_update_end(&rx_stats->syncp);

		/* Pass it up. */
		napi_gro_receive(&queue->napi, skb);
	}

	return packets_dropped;
}