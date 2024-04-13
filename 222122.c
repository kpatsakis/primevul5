evbuffer_search_range(struct evbuffer *buffer, const char *what, size_t len, const struct evbuffer_ptr *start, const struct evbuffer_ptr *end)
{
	struct evbuffer_ptr pos;
	struct evbuffer_chain *chain, *last_chain = NULL;
	const unsigned char *p;
	char first;

	EVBUFFER_LOCK(buffer);

	if (start) {
		memcpy(&pos, start, sizeof(pos));
		chain = pos._internal.chain;
	} else {
		pos.pos = 0;
		chain = pos._internal.chain = buffer->first;
		pos._internal.pos_in_chain = 0;
	}

	if (end)
		last_chain = end->_internal.chain;

	if (!len || len > EV_SSIZE_MAX)
		goto done;

	first = what[0];

	while (chain) {
		const unsigned char *start_at =
		    chain->buffer + chain->misalign +
		    pos._internal.pos_in_chain;
		p = memchr(start_at, first,
		    chain->off - pos._internal.pos_in_chain);
		if (p) {
			pos.pos += p - start_at;
			pos._internal.pos_in_chain += p - start_at;
			if (!evbuffer_ptr_memcmp(buffer, &pos, what, len)) {
				if (end && pos.pos + (ev_ssize_t)len > end->pos)
					goto not_found;
				else
					goto done;
			}
			++pos.pos;
			++pos._internal.pos_in_chain;
			if (pos._internal.pos_in_chain == chain->off) {
				chain = pos._internal.chain = chain->next;
				pos._internal.pos_in_chain = 0;
			}
		} else {
			if (chain == last_chain)
				goto not_found;
			pos.pos += chain->off - pos._internal.pos_in_chain;
			chain = pos._internal.chain = chain->next;
			pos._internal.pos_in_chain = 0;
		}
	}

not_found:
	pos.pos = -1;
	pos._internal.chain = NULL;
done:
	EVBUFFER_UNLOCK(buffer);
	return pos;
}