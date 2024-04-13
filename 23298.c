static void xennet_poll_controller(struct net_device *dev)
{
	/* Poll each queue */
	struct netfront_info *info = netdev_priv(dev);
	unsigned int num_queues = dev->real_num_tx_queues;
	unsigned int i;

	if (info->broken)
		return;

	for (i = 0; i < num_queues; ++i)
		xennet_interrupt(0, &info->queues[i]);
}