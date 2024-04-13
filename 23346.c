static int xennet_fill_frags(struct netfront_queue *queue,
			     struct sk_buff *skb,
			     struct sk_buff_head *list)
{
	RING_IDX cons = queue->rx.rsp_cons;
	struct sk_buff *nskb;

	while ((nskb = __skb_dequeue(list))) {
		struct xen_netif_rx_response rx;
		skb_frag_t *nfrag = &skb_shinfo(nskb)->frags[0];

		RING_COPY_RESPONSE(&queue->rx, ++cons, &rx);

		if (skb_shinfo(skb)->nr_frags == MAX_SKB_FRAGS) {
			unsigned int pull_to = NETFRONT_SKB_CB(skb)->pull_to;

			BUG_ON(pull_to < skb_headlen(skb));
			__pskb_pull_tail(skb, pull_to - skb_headlen(skb));
		}
		if (unlikely(skb_shinfo(skb)->nr_frags >= MAX_SKB_FRAGS)) {
			xennet_set_rx_rsp_cons(queue,
					       ++cons + skb_queue_len(list));
			kfree_skb(nskb);
			return -ENOENT;
		}

		skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags,
				skb_frag_page(nfrag),
				rx.offset, rx.status, PAGE_SIZE);

		skb_shinfo(nskb)->nr_frags = 0;
		kfree_skb(nskb);
	}

	xennet_set_rx_rsp_cons(queue, cons);

	return 0;
}