evbuffer_remove_all_callbacks(struct evbuffer *buffer)
{
	struct evbuffer_cb_entry *cbent;

	while ((cbent = TAILQ_FIRST(&buffer->callbacks))) {
	    TAILQ_REMOVE(&buffer->callbacks, cbent, next);
	    mm_free(cbent);
	}
}