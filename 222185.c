evbuffer_new(void)
{
	struct evbuffer *buffer;

	buffer = mm_calloc(1, sizeof(struct evbuffer));
	if (buffer == NULL)
		return (NULL);

	TAILQ_INIT(&buffer->callbacks);
	buffer->refcnt = 1;
	buffer->last_with_datap = &buffer->first;

	return (buffer);
}