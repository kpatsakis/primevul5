static void xennet_tx_setup_grant(unsigned long gfn, unsigned int offset,
				  unsigned int len, void *data)
{
	struct xennet_gnttab_make_txreq *info = data;
	unsigned int id;
	struct xen_netif_tx_request *tx;
	grant_ref_t ref;
	/* convenient aliases */
	struct page *page = info->page;
	struct netfront_queue *queue = info->queue;
	struct sk_buff *skb = info->skb;

	id = get_id_from_list(&queue->tx_skb_freelist, queue->tx_link);
	tx = RING_GET_REQUEST(&queue->tx, queue->tx.req_prod_pvt++);
	ref = gnttab_claim_grant_reference(&queue->gref_tx_head);
	WARN_ON_ONCE(IS_ERR_VALUE((unsigned long)(int)ref));

	gnttab_grant_foreign_access_ref(ref, queue->info->xbdev->otherend_id,
					gfn, GNTMAP_readonly);

	queue->tx_skbs[id] = skb;
	queue->grant_tx_page[id] = page;
	queue->grant_tx_ref[id] = ref;

	info->tx_local.id = id;
	info->tx_local.gref = ref;
	info->tx_local.offset = offset;
	info->tx_local.size = len;
	info->tx_local.flags = 0;

	*tx = info->tx_local;

	/*
	 * Put the request in the pending queue, it will be set to be pending
	 * when the producer index is about to be raised.
	 */
	add_id_to_list(&queue->tx_pend_queue, queue->tx_link, id);

	info->tx = tx;
	info->size += info->tx_local.size;
}