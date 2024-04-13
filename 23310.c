static grant_ref_t xennet_get_rx_ref(struct netfront_queue *queue,
					    RING_IDX ri)
{
	int i = xennet_rxidx(ri);
	grant_ref_t ref = queue->grant_rx_ref[i];
	queue->grant_rx_ref[i] = INVALID_GRANT_REF;
	return ref;
}