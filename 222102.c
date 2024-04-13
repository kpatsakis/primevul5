evbuffer_deferred_callback(struct deferred_cb *cb, void *arg)
{
	struct bufferevent *parent = NULL;
	struct evbuffer *buffer = arg;

	/* XXXX It would be better to run these callbacks without holding the
	 * lock */
	EVBUFFER_LOCK(buffer);
	parent = buffer->parent;
	evbuffer_run_callbacks(buffer, 1);
	_evbuffer_decref_and_unlock(buffer);
	if (parent)
		bufferevent_decref(parent);
}