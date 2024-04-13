evbuffer_unfreeze(struct evbuffer *buffer, int start)
{
	EVBUFFER_LOCK(buffer);
	if (start)
		buffer->freeze_start = 0;
	else
		buffer->freeze_end = 0;
	EVBUFFER_UNLOCK(buffer);
	return 0;
}