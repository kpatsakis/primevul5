static u16 packet_pick_tx_queue(struct sk_buff *skb)
{
	struct net_device *dev = skb->dev;
	const struct net_device_ops *ops = dev->netdev_ops;
	int cpu = raw_smp_processor_id();
	u16 queue_index;

#ifdef CONFIG_XPS
	skb->sender_cpu = cpu + 1;
#endif
	skb_record_rx_queue(skb, cpu % dev->real_num_tx_queues);
	if (ops->ndo_select_queue) {
		queue_index = ops->ndo_select_queue(dev, skb, NULL);
		queue_index = netdev_cap_txqueue(dev, queue_index);
	} else {
		queue_index = netdev_pick_tx(dev, skb, NULL);
	}

	return queue_index;
}