static int setup_netfront(struct xenbus_device *dev,
			struct netfront_queue *queue, unsigned int feature_split_evtchn)
{
	struct xen_netif_tx_sring *txs;
	struct xen_netif_rx_sring *rxs;
	int err;

	queue->tx_ring_ref = INVALID_GRANT_REF;
	queue->rx_ring_ref = INVALID_GRANT_REF;
	queue->rx.sring = NULL;
	queue->tx.sring = NULL;

	err = xenbus_setup_ring(dev, GFP_NOIO | __GFP_HIGH, (void **)&txs,
				1, &queue->tx_ring_ref);
	if (err)
		goto fail;

	XEN_FRONT_RING_INIT(&queue->tx, txs, XEN_PAGE_SIZE);

	err = xenbus_setup_ring(dev, GFP_NOIO | __GFP_HIGH, (void **)&rxs,
				1, &queue->rx_ring_ref);
	if (err)
		goto fail;

	XEN_FRONT_RING_INIT(&queue->rx, rxs, XEN_PAGE_SIZE);

	if (feature_split_evtchn)
		err = setup_netfront_split(queue);
	/* setup single event channel if
	 *  a) feature-split-event-channels == 0
	 *  b) feature-split-event-channels == 1 but failed to setup
	 */
	if (!feature_split_evtchn || err)
		err = setup_netfront_single(queue);

	if (err)
		goto fail;

	return 0;

 fail:
	xenbus_teardown_ring((void **)&queue->rx.sring, 1, &queue->rx_ring_ref);
	xenbus_teardown_ring((void **)&queue->tx.sring, 1, &queue->tx_ring_ref);

	return err;
}