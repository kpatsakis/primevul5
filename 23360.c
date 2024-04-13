static void xennet_mark_tx_pending(struct netfront_queue *queue)
{
	unsigned int i;

	while ((i = get_id_from_list(&queue->tx_pend_queue, queue->tx_link)) !=
	       TX_LINK_NONE)
		queue->tx_link[i] = TX_PENDING;
}