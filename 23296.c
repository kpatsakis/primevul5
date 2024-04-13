static void xennet_release_tx_bufs(struct netfront_queue *queue)
{
	struct sk_buff *skb;
	int i;

	for (i = 0; i < NET_TX_RING_SIZE; i++) {
		/* Skip over entries which are actually freelist references */
		if (!queue->tx_skbs[i])
			continue;

		skb = queue->tx_skbs[i];
		queue->tx_skbs[i] = NULL;
		get_page(queue->grant_tx_page[i]);
		gnttab_end_foreign_access(queue->grant_tx_ref[i],
					  queue->grant_tx_page[i]);
		queue->grant_tx_page[i] = NULL;
		queue->grant_tx_ref[i] = INVALID_GRANT_REF;
		add_id_to_list(&queue->tx_skb_freelist, queue->tx_link, i);
		dev_kfree_skb_irq(skb);
	}
}