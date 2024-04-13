channel_pre_open(Channel *c, fd_set *readset, fd_set *writeset)
{
	u_int limit = compat20 ? c->remote_window : packet_get_maxsize();

	if (c->istate == CHAN_INPUT_OPEN &&
	    limit > 0 &&
	    buffer_len(&c->input) < limit &&
	    buffer_check_alloc(&c->input, CHAN_RBUF))
		FD_SET(c->rfd, readset);
	if (c->ostate == CHAN_OUTPUT_OPEN ||
	    c->ostate == CHAN_OUTPUT_WAIT_DRAIN) {
		if (buffer_len(&c->output) > 0) {
			FD_SET(c->wfd, writeset);
		} else if (c->ostate == CHAN_OUTPUT_WAIT_DRAIN) {
			if (CHANNEL_EFD_OUTPUT_ACTIVE(c))
				debug2("channel %d: obuf_empty delayed efd %d/(%d)",
				    c->self, c->efd, buffer_len(&c->extended));
			else
				chan_obuf_empty(c);
		}
	}
	/** XXX check close conditions, too */
	if (compat20 && c->efd != -1 && 
	    !(c->istate == CHAN_INPUT_CLOSED && c->ostate == CHAN_OUTPUT_CLOSED)) {
		if (c->extended_usage == CHAN_EXTENDED_WRITE &&
		    buffer_len(&c->extended) > 0)
			FD_SET(c->efd, writeset);
		else if (c->efd != -1 && !(c->flags & CHAN_EOF_SENT) &&
		    (c->extended_usage == CHAN_EXTENDED_READ ||
		    c->extended_usage == CHAN_EXTENDED_IGNORE) &&
		    buffer_len(&c->extended) < c->remote_window)
			FD_SET(c->efd, readset);
	}
	/* XXX: What about efd? races? */
}
