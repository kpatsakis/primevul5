evbuffer_remove_cb_entry(struct evbuffer *buffer,
			 struct evbuffer_cb_entry *ent)
{
	EVBUFFER_LOCK(buffer);
	TAILQ_REMOVE(&buffer->callbacks, ent, next);
	EVBUFFER_UNLOCK(buffer);
	mm_free(ent);
	return 0;
}