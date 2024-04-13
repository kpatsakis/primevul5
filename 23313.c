static void xennet_destroy_queues(struct netfront_info *info)
{
	unsigned int i;

	for (i = 0; i < info->netdev->real_num_tx_queues; i++) {
		struct netfront_queue *queue = &info->queues[i];

		if (netif_running(info->netdev))
			napi_disable(&queue->napi);
		netif_napi_del(&queue->napi);
	}

	kfree(info->queues);
	info->queues = NULL;
}