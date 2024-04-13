evbuffer_get_length(const struct evbuffer *buffer)
{
	size_t result;

	EVBUFFER_LOCK(buffer);

	result = (buffer->total_len);

	EVBUFFER_UNLOCK(buffer);

	return result;
}