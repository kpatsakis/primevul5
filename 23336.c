static void xennet_alloc_rx_buffers(struct netfront_queue *queue)
{
	RING_IDX req_prod = queue->rx.req_prod_pvt;
	int notify;
	int err = 0;

	if (unlikely(!netif_carrier_ok(queue->info->netdev)))
		return;

	for (req_prod = queue->rx.req_prod_pvt;
	     req_prod - queue->rx.rsp_cons < NET_RX_RING_SIZE;
	     req_prod++) {
		struct sk_buff *skb;
		unsigned short id;
		grant_ref_t ref;
		struct page *page;
		struct xen_netif_rx_request *req;

		skb = xennet_alloc_one_rx_buffer(queue);
		if (!skb) {
			err = -ENOMEM;
			break;
		}

		id = xennet_rxidx(req_prod);

		BUG_ON(queue->rx_skbs[id]);
		queue->rx_skbs[id] = skb;

		ref = gnttab_claim_grant_reference(&queue->gref_rx_head);
		WARN_ON_ONCE(IS_ERR_VALUE((unsigned long)(int)ref));
		queue->grant_rx_ref[id] = ref;

		page = skb_frag_page(&skb_shinfo(skb)->frags[0]);

		req = RING_GET_REQUEST(&queue->rx, req_prod);
		gnttab_page_grant_foreign_access_ref_one(ref,
							 queue->info->xbdev->otherend_id,
							 page,
							 0);
		req->id = id;
		req->gref = ref;
	}

	queue->rx.req_prod_pvt = req_prod;

	/* Try again later if there are not enough requests or skb allocation
	 * failed.
	 * Enough requests is quantified as the sum of newly created slots and
	 * the unconsumed slots at the backend.
	 */
	if (req_prod - queue->rx.rsp_cons < NET_RX_SLOTS_MIN ||
	    unlikely(err)) {
		mod_timer(&queue->rx_refill_timer, jiffies + (HZ/10));
		return;
	}

	RING_PUSH_REQUESTS_AND_CHECK_NOTIFY(&queue->rx, notify);
	if (notify)
		notify_remote_via_irq(queue->rx_irq);
}