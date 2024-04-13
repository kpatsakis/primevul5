static bool fanout_flow_is_huge(struct packet_sock *po, struct sk_buff *skb)
{
	u32 *history = po->rollover->history;
	u32 victim, rxhash;
	int i, count = 0;

	rxhash = skb_get_hash(skb);
	for (i = 0; i < ROLLOVER_HLEN; i++)
		if (READ_ONCE(history[i]) == rxhash)
			count++;

	victim = prandom_u32() % ROLLOVER_HLEN;

	/* Avoid dirtying the cache line if possible */
	if (READ_ONCE(history[victim]) != rxhash)
		WRITE_ONCE(history[victim], rxhash);

	return count > (ROLLOVER_HLEN >> 1);
}