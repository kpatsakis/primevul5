static void xennet_release_rx_bufs(struct netfront_queue *queue)
{
	int id, ref;

	spin_lock_bh(&queue->rx_lock);

	for (id = 0; id < NET_RX_RING_SIZE; id++) {
		struct sk_buff *skb;
		struct page *page;

		skb = queue->rx_skbs[id];
		if (!skb)
			continue;

		ref = queue->grant_rx_ref[id];
		if (ref == INVALID_GRANT_REF)
			continue;

		page = skb_frag_page(&skb_shinfo(skb)->frags[0]);

		/* gnttab_end_foreign_access() needs a page ref until
		 * foreign access is ended (which may be deferred).
		 */
		get_page(page);
		gnttab_end_foreign_access(ref, page);
		queue->grant_rx_ref[id] = INVALID_GRANT_REF;

		kfree_skb(skb);
	}

	spin_unlock_bh(&queue->rx_lock);
}