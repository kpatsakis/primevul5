static struct sk_buff *xennet_alloc_one_rx_buffer(struct netfront_queue *queue)
{
	struct sk_buff *skb;
	struct page *page;

	skb = __netdev_alloc_skb(queue->info->netdev,
				 RX_COPY_THRESHOLD + NET_IP_ALIGN,
				 GFP_ATOMIC | __GFP_NOWARN);
	if (unlikely(!skb))
		return NULL;

	page = page_pool_alloc_pages(queue->page_pool,
				     GFP_ATOMIC | __GFP_NOWARN | __GFP_ZERO);
	if (unlikely(!page)) {
		kfree_skb(skb);
		return NULL;
	}
	skb_add_rx_frag(skb, 0, page, 0, 0, PAGE_SIZE);

	/* Align ip header to a 16 bytes boundary */
	skb_reserve(skb, NET_IP_ALIGN);
	skb->dev = queue->info->netdev;

	return skb;
}