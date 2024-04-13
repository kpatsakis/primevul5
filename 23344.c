struct sk_buff *bounce_skb(const struct sk_buff *skb)
{
	unsigned int headerlen = skb_headroom(skb);
	/* Align size to allocate full pages and avoid contiguous data leaks */
	unsigned int size = ALIGN(skb_end_offset(skb) + skb->data_len,
				  XEN_PAGE_SIZE);
	struct sk_buff *n = alloc_skb(size, GFP_ATOMIC | __GFP_ZERO);

	if (!n)
		return NULL;

	if (!IS_ALIGNED((uintptr_t)n->head, XEN_PAGE_SIZE)) {
		WARN_ONCE(1, "misaligned skb allocated\n");
		kfree_skb(n);
		return NULL;
	}

	/* Set the data pointer */
	skb_reserve(n, headerlen);
	/* Set the tail pointer and length */
	skb_put(n, skb->len);

	BUG_ON(skb_copy_bits(skb, -headerlen, n->head, headerlen + skb->len));

	skb_copy_header(n, skb);
	return n;
}