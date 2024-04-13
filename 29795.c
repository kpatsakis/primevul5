channel_free(Channel *c)
{
	char *s;
	u_int i, n;
	struct channel_confirm *cc;

	for (n = 0, i = 0; i < channels_alloc; i++)
		if (channels[i])
			n++;
	debug("channel %d: free: %s, nchannels %u", c->self,
	    c->remote_name ? c->remote_name : "???", n);

	s = channel_open_message();
	debug3("channel %d: status: %s", c->self, s);
	free(s);

	if (c->sock != -1)
		shutdown(c->sock, SHUT_RDWR);
	channel_close_fds(c);
	buffer_free(&c->input);
	buffer_free(&c->output);
	buffer_free(&c->extended);
	free(c->remote_name);
	c->remote_name = NULL;
	free(c->path);
	c->path = NULL;
	free(c->listening_addr);
	c->listening_addr = NULL;
	while ((cc = TAILQ_FIRST(&c->status_confirms)) != NULL) {
		if (cc->abandon_cb != NULL)
			cc->abandon_cb(c, cc->ctx);
		TAILQ_REMOVE(&c->status_confirms, cc, entry);
		explicit_bzero(cc, sizeof(*cc));
		free(cc);
	}
	if (c->filter_cleanup != NULL && c->filter_ctx != NULL)
		c->filter_cleanup(c->self, c->filter_ctx);
	channels[c->self] = NULL;
	free(c);
}
