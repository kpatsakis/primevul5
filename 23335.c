static int xennet_create_page_pool(struct netfront_queue *queue)
{
	int err;
	struct page_pool_params pp_params = {
		.order = 0,
		.flags = 0,
		.pool_size = NET_RX_RING_SIZE,
		.nid = NUMA_NO_NODE,
		.dev = &queue->info->netdev->dev,
		.offset = XDP_PACKET_HEADROOM,
		.max_len = XEN_PAGE_SIZE - XDP_PACKET_HEADROOM,
	};

	queue->page_pool = page_pool_create(&pp_params);
	if (IS_ERR(queue->page_pool)) {
		err = PTR_ERR(queue->page_pool);
		queue->page_pool = NULL;
		return err;
	}

	err = xdp_rxq_info_reg(&queue->xdp_rxq, queue->info->netdev,
			       queue->id, 0);
	if (err) {
		netdev_err(queue->info->netdev, "xdp_rxq_info_reg failed\n");
		goto err_free_pp;
	}

	err = xdp_rxq_info_reg_mem_model(&queue->xdp_rxq,
					 MEM_TYPE_PAGE_POOL, queue->page_pool);
	if (err) {
		netdev_err(queue->info->netdev, "xdp_rxq_info_reg_mem_model failed\n");
		goto err_unregister_rxq;
	}
	return 0;

err_unregister_rxq:
	xdp_rxq_info_unreg(&queue->xdp_rxq);
err_free_pp:
	page_pool_destroy(queue->page_pool);
	queue->page_pool = NULL;
	return err;
}