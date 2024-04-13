evbuffer_chain_insert(struct evbuffer *buf,
    struct evbuffer_chain *chain)
{
	ASSERT_EVBUFFER_LOCKED(buf);
	if (*buf->last_with_datap == NULL) {
		/* There are no chains data on the buffer at all. */
		EVUTIL_ASSERT(buf->last_with_datap == &buf->first);
		EVUTIL_ASSERT(buf->first == NULL);
		buf->first = buf->last = chain;
	} else {
		struct evbuffer_chain **ch = buf->last_with_datap;
		/* Find the first victim chain.  It might be *last_with_datap */
		while ((*ch) && ((*ch)->off != 0 || CHAIN_PINNED(*ch)))
			ch = &(*ch)->next;
		if (*ch == NULL) {
			/* There is no victim; just append this new chain. */
			buf->last->next = chain;
			if (chain->off)
				buf->last_with_datap = &buf->last->next;
		} else {
			/* Replace all victim chains with this chain. */
			EVUTIL_ASSERT(evbuffer_chains_all_empty(*ch));
			evbuffer_free_all_chains(*ch);
			*ch = chain;
		}
		buf->last = chain;
	}
	buf->total_len += chain->off;
}