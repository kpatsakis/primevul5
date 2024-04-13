static int xennet_xdp_xmit_one(struct net_device *dev,
			       struct netfront_queue *queue,
			       struct xdp_frame *xdpf)
{
	struct netfront_info *np = netdev_priv(dev);
	struct netfront_stats *tx_stats = this_cpu_ptr(np->tx_stats);
	struct xennet_gnttab_make_txreq info = {
		.queue = queue,
		.skb = NULL,
		.page = virt_to_page(xdpf->data),
	};
	int notify;

	xennet_make_first_txreq(&info,
				offset_in_page(xdpf->data),
				xdpf->len);

	xennet_mark_tx_pending(queue);

	RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&queue->tx, notify);
	if (notify)
		notify_remote_via_irq(queue->tx_irq);

	u64_stats_update_begin(&tx_stats->syncp);
	tx_stats->bytes += xdpf->len;
	tx_stats->packets++;
	u64_stats_update_end(&tx_stats->syncp);

	xennet_tx_buf_gc(queue);

	return 0;
}