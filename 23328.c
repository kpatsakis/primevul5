static void xennet_maybe_wake_tx(struct netfront_queue *queue)
{
	struct net_device *dev = queue->info->netdev;
	struct netdev_queue *dev_queue = netdev_get_tx_queue(dev, queue->id);

	if (unlikely(netif_tx_queue_stopped(dev_queue)) &&
	    netfront_tx_slot_available(queue) &&
	    likely(netif_running(dev)))
		netif_tx_wake_queue(netdev_get_tx_queue(dev, queue->id));
}