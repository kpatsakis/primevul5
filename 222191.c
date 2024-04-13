evbuffer_search_eol(struct evbuffer *buffer,
    struct evbuffer_ptr *start, size_t *eol_len_out,
    enum evbuffer_eol_style eol_style)
{
	struct evbuffer_ptr it, it2;
	size_t extra_drain = 0;
	int ok = 0;

	EVBUFFER_LOCK(buffer);

	if (start) {
		memcpy(&it, start, sizeof(it));
	} else {
		it.pos = 0;
		it._internal.chain = buffer->first;
		it._internal.pos_in_chain = 0;
	}

	/* the eol_style determines our first stop character and how many
	 * characters we are going to drain afterwards. */
	switch (eol_style) {
	case EVBUFFER_EOL_ANY:
		if (evbuffer_find_eol_char(&it) < 0)
			goto done;
		memcpy(&it2, &it, sizeof(it));
		extra_drain = evbuffer_strspn(&it2, "\r\n");
		break;
	case EVBUFFER_EOL_CRLF_STRICT: {
		it = evbuffer_search(buffer, "\r\n", 2, &it);
		if (it.pos < 0)
			goto done;
		extra_drain = 2;
		break;
	}
	case EVBUFFER_EOL_CRLF:
		while (1) {
			if (evbuffer_find_eol_char(&it) < 0)
				goto done;
			if (evbuffer_getchr(&it) == '\n') {
				extra_drain = 1;
				break;
			} else if (!evbuffer_ptr_memcmp(
				    buffer, &it, "\r\n", 2)) {
				extra_drain = 2;
				break;
			} else {
				if (evbuffer_ptr_set(buffer, &it, 1,
					EVBUFFER_PTR_ADD)<0)
					goto done;
			}
		}
		break;
	case EVBUFFER_EOL_LF:
		if (evbuffer_strchr(&it, '\n') < 0)
			goto done;
		extra_drain = 1;
		break;
	default:
		goto done;
	}

	ok = 1;
done:
	EVBUFFER_UNLOCK(buffer);

	if (!ok) {
		it.pos = -1;
	}
	if (eol_len_out)
		*eol_len_out = extra_drain;

	return it;
}