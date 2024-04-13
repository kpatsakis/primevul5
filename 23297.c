static bool xennet_handle_rx(struct netfront_queue *queue, unsigned int *eoi)
{
	unsigned int work_queued;
	unsigned long flags;

	if (unlikely(queue->info->broken))
		return false;

	spin_lock_irqsave(&queue->rx_cons_lock, flags);
	work_queued = XEN_RING_NR_UNCONSUMED_RESPONSES(&queue->rx);
	if (work_queued > queue->rx_rsp_unconsumed) {
		queue->rx_rsp_unconsumed = work_queued;
		*eoi = 0;
	} else if (unlikely(work_queued < queue->rx_rsp_unconsumed)) {
		const struct device *dev = &queue->info->netdev->dev;

		spin_unlock_irqrestore(&queue->rx_cons_lock, flags);
		dev_alert(dev, "RX producer index going backwards\n");
		dev_alert(dev, "Disabled for further use\n");
		queue->info->broken = true;
		return false;
	}
	spin_unlock_irqrestore(&queue->rx_cons_lock, flags);

	if (likely(netif_carrier_ok(queue->info->netdev) && work_queued))
		napi_schedule(&queue->napi);

	return true;
}