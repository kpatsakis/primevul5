static int xennet_count_skb_slots(struct sk_buff *skb)
{
	int i, frags = skb_shinfo(skb)->nr_frags;
	int slots;

	slots = gnttab_count_grant(offset_in_page(skb->data),
				   skb_headlen(skb));

	for (i = 0; i < frags; i++) {
		skb_frag_t *frag = skb_shinfo(skb)->frags + i;
		unsigned long size = skb_frag_size(frag);
		unsigned long offset = skb_frag_off(frag);

		/* Skip unused frames from start of page */
		offset &= ~PAGE_MASK;

		slots += gnttab_count_grant(offset, size);
	}

	return slots;
}