static void xennet_disconnect_backend(struct netfront_info *info)
{
	unsigned int i = 0;
	unsigned int num_queues = info->netdev->real_num_tx_queues;

	netif_carrier_off(info->netdev);

	for (i = 0; i < num_queues && info->queues; ++i) {
		struct netfront_queue *queue = &info->queues[i];

		del_timer_sync(&queue->rx_refill_timer);

		if (queue->tx_irq && (queue->tx_irq == queue->rx_irq))
			unbind_from_irqhandler(queue->tx_irq, queue);
		if (queue->tx_irq && (queue->tx_irq != queue->rx_irq)) {
			unbind_from_irqhandler(queue->tx_irq, queue);
			unbind_from_irqhandler(queue->rx_irq, queue);
		}
		queue->tx_evtchn = queue->rx_evtchn = 0;
		queue->tx_irq = queue->rx_irq = 0;

		if (netif_running(info->netdev))
			napi_synchronize(&queue->napi);

		xennet_release_tx_bufs(queue);
		xennet_release_rx_bufs(queue);
		gnttab_free_grant_references(queue->gref_tx_head);
		gnttab_free_grant_references(queue->gref_rx_head);

		/* End access and free the pages */
		xennet_end_access(queue->tx_ring_ref, queue->tx.sring);
		xennet_end_access(queue->rx_ring_ref, queue->rx.sring);

		queue->tx_ring_ref = INVALID_GRANT_REF;
		queue->rx_ring_ref = INVALID_GRANT_REF;
		queue->tx.sring = NULL;
		queue->rx.sring = NULL;

		page_pool_destroy(queue->page_pool);
	}
}