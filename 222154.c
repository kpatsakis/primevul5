evbuffer_free(struct evbuffer *buffer)
{
	EVBUFFER_LOCK(buffer);
	_evbuffer_decref_and_unlock(buffer);
}