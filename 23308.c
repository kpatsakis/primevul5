static int xennet_init_queue(struct netfront_queue *queue)
{
	unsigned short i;
	int err = 0;
	char *devid;

	spin_lock_init(&queue->tx_lock);
	spin_lock_init(&queue->rx_lock);
	spin_lock_init(&queue->rx_cons_lock);

	timer_setup(&queue->rx_refill_timer, rx_refill_timeout, 0);

	devid = strrchr(queue->info->xbdev->nodename, '/') + 1;
	snprintf(queue->name, sizeof(queue->name), "vif%s-q%u",
		 devid, queue->id);

	/* Initialise tx_skb_freelist as a free chain containing every entry. */
	queue->tx_skb_freelist = 0;
	queue->tx_pend_queue = TX_LINK_NONE;
	for (i = 0; i < NET_TX_RING_SIZE; i++) {
		queue->tx_link[i] = i + 1;
		queue->grant_tx_ref[i] = INVALID_GRANT_REF;
		queue->grant_tx_page[i] = NULL;
	}
	queue->tx_link[NET_TX_RING_SIZE - 1] = TX_LINK_NONE;

	/* Clear out rx_skbs */
	for (i = 0; i < NET_RX_RING_SIZE; i++) {
		queue->rx_skbs[i] = NULL;
		queue->grant_rx_ref[i] = INVALID_GRANT_REF;
	}

	/* A grant for every tx ring slot */
	if (gnttab_alloc_grant_references(NET_TX_RING_SIZE,
					  &queue->gref_tx_head) < 0) {
		pr_alert("can't alloc tx grant refs\n");
		err = -ENOMEM;
		goto exit;
	}

	/* A grant for every rx ring slot */
	if (gnttab_alloc_grant_references(NET_RX_RING_SIZE,
					  &queue->gref_rx_head) < 0) {
		pr_alert("can't alloc rx grant refs\n");
		err = -ENOMEM;
		goto exit_free_tx;
	}

	return 0;

 exit_free_tx:
	gnttab_free_grant_references(queue->gref_tx_head);
 exit:
	return err;
}