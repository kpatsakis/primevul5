evbuffer_readln(struct evbuffer *buffer, size_t *n_read_out,
		enum evbuffer_eol_style eol_style)
{
	struct evbuffer_ptr it;
	char *line;
	size_t n_to_copy=0, extra_drain=0;
	char *result = NULL;

	EVBUFFER_LOCK(buffer);

	if (buffer->freeze_start) {
		goto done;
	}

	it = evbuffer_search_eol(buffer, NULL, &extra_drain, eol_style);
	if (it.pos < 0)
		goto done;
	n_to_copy = it.pos;

	if ((line = mm_malloc(n_to_copy+1)) == NULL) {
		event_warn("%s: out of memory", __func__);
		goto done;
	}

	evbuffer_remove(buffer, line, n_to_copy);
	line[n_to_copy] = '\0';

	evbuffer_drain(buffer, extra_drain);
	result = line;
done:
	EVBUFFER_UNLOCK(buffer);

	if (n_read_out)
		*n_read_out = result ? n_to_copy : 0;

	return result;
}