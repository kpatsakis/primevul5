static int xennet_open(struct net_device *dev)
{
	struct netfront_info *np = netdev_priv(dev);
	unsigned int num_queues = dev->real_num_tx_queues;
	unsigned int i = 0;
	struct netfront_queue *queue = NULL;

	if (!np->queues || np->broken)
		return -ENODEV;

	for (i = 0; i < num_queues; ++i) {
		queue = &np->queues[i];
		napi_enable(&queue->napi);

		spin_lock_bh(&queue->rx_lock);
		if (netif_carrier_ok(dev)) {
			xennet_alloc_rx_buffers(queue);
			queue->rx.sring->rsp_event = queue->rx.rsp_cons + 1;
			if (RING_HAS_UNCONSUMED_RESPONSES(&queue->rx))
				napi_schedule(&queue->napi);
		}
		spin_unlock_bh(&queue->rx_lock);
	}

	netif_tx_start_all_queues(dev);

	return 0;
}