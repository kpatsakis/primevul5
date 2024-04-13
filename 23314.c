static int xennet_xdp_xmit(struct net_device *dev, int n,
			   struct xdp_frame **frames, u32 flags)
{
	unsigned int num_queues = dev->real_num_tx_queues;
	struct netfront_info *np = netdev_priv(dev);
	struct netfront_queue *queue = NULL;
	unsigned long irq_flags;
	int nxmit = 0;
	int i;

	if (unlikely(np->broken))
		return -ENODEV;
	if (unlikely(flags & ~XDP_XMIT_FLAGS_MASK))
		return -EINVAL;

	queue = &np->queues[smp_processor_id() % num_queues];

	spin_lock_irqsave(&queue->tx_lock, irq_flags);
	for (i = 0; i < n; i++) {
		struct xdp_frame *xdpf = frames[i];

		if (!xdpf)
			continue;
		if (xennet_xdp_xmit_one(dev, queue, xdpf))
			break;
		nxmit++;
	}
	spin_unlock_irqrestore(&queue->tx_lock, irq_flags);

	return nxmit;
}