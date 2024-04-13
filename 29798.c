channel_open_message(void)
{
	Buffer buffer;
	Channel *c;
	char buf[1024], *cp;
	u_int i;

	buffer_init(&buffer);
	snprintf(buf, sizeof buf, "The following connections are open:\r\n");
	buffer_append(&buffer, buf, strlen(buf));
	for (i = 0; i < channels_alloc; i++) {
		c = channels[i];
		if (c == NULL)
			continue;
		switch (c->type) {
		case SSH_CHANNEL_X11_LISTENER:
		case SSH_CHANNEL_PORT_LISTENER:
		case SSH_CHANNEL_RPORT_LISTENER:
		case SSH_CHANNEL_CLOSED:
		case SSH_CHANNEL_AUTH_SOCKET:
		case SSH_CHANNEL_ZOMBIE:
		case SSH_CHANNEL_ABANDONED:
		case SSH_CHANNEL_MUX_CLIENT:
		case SSH_CHANNEL_MUX_LISTENER:
		case SSH_CHANNEL_UNIX_LISTENER:
		case SSH_CHANNEL_RUNIX_LISTENER:
			continue;
		case SSH_CHANNEL_LARVAL:
		case SSH_CHANNEL_OPENING:
		case SSH_CHANNEL_CONNECTING:
		case SSH_CHANNEL_DYNAMIC:
		case SSH_CHANNEL_OPEN:
		case SSH_CHANNEL_X11_OPEN:
		case SSH_CHANNEL_INPUT_DRAINING:
		case SSH_CHANNEL_OUTPUT_DRAINING:
			snprintf(buf, sizeof buf,
			    "  #%d %.300s (t%d r%d i%d/%d o%d/%d fd %d/%d cc %d)\r\n",
			    c->self, c->remote_name,
			    c->type, c->remote_id,
			    c->istate, buffer_len(&c->input),
			    c->ostate, buffer_len(&c->output),
			    c->rfd, c->wfd, c->ctl_chan);
			buffer_append(&buffer, buf, strlen(buf));
			continue;
		default:
			fatal("channel_open_message: bad channel type %d", c->type);
			/* NOTREACHED */
		}
	}
	buffer_append(&buffer, "\0", 1);
	cp = xstrdup((char *)buffer_ptr(&buffer));
	buffer_free(&buffer);
	return cp;
}
