static int xennet_create_queues(struct netfront_info *info,
				unsigned int *num_queues)
{
	unsigned int i;
	int ret;

	info->queues = kcalloc(*num_queues, sizeof(struct netfront_queue),
			       GFP_KERNEL);
	if (!info->queues)
		return -ENOMEM;

	for (i = 0; i < *num_queues; i++) {
		struct netfront_queue *queue = &info->queues[i];

		queue->id = i;
		queue->info = info;

		ret = xennet_init_queue(queue);
		if (ret < 0) {
			dev_warn(&info->xbdev->dev,
				 "only created %d queues\n", i);
			*num_queues = i;
			break;
		}

		/* use page pool recycling instead of buddy allocator */
		ret = xennet_create_page_pool(queue);
		if (ret < 0) {
			dev_err(&info->xbdev->dev, "can't allocate page pool\n");
			*num_queues = i;
			return ret;
		}

		netif_napi_add(queue->info->netdev, &queue->napi,
			       xennet_poll, 64);
		if (netif_running(info->netdev))
			napi_enable(&queue->napi);
	}

	netif_set_real_num_tx_queues(info->netdev, *num_queues);

	if (*num_queues == 0) {
		dev_err(&info->xbdev->dev, "no queues\n");
		return -EINVAL;
	}
	return 0;
}