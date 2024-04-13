static int xennet_close(struct net_device *dev)
{
	struct netfront_info *np = netdev_priv(dev);
	unsigned int num_queues = dev->real_num_tx_queues;
	unsigned int i;
	struct netfront_queue *queue;
	netif_tx_stop_all_queues(np->netdev);
	for (i = 0; i < num_queues; ++i) {
		queue = &np->queues[i];
		napi_disable(&queue->napi);
	}
	return 0;
}