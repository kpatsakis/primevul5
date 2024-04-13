static u16 xennet_select_queue(struct net_device *dev, struct sk_buff *skb,
			       struct net_device *sb_dev)
{
	unsigned int num_queues = dev->real_num_tx_queues;
	u32 hash;
	u16 queue_idx;

	/* First, check if there is only one queue */
	if (num_queues == 1) {
		queue_idx = 0;
	} else {
		hash = skb_get_hash(skb);
		queue_idx = hash % num_queues;
	}

	return queue_idx;
}