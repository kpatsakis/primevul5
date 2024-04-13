RESTORE_PINNED(struct evbuffer *src, struct evbuffer_chain *pinned,
		struct evbuffer_chain *last)
{
	ASSERT_EVBUFFER_LOCKED(src);

	if (!pinned) {
		ZERO_CHAIN(src);
		return;
	}

	src->first = pinned;
	src->last = last;
	src->last_with_datap = &src->first;
	src->total_len = 0;
}