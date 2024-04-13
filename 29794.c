channel_find_open(void)
{
	u_int i;
	Channel *c;

	for (i = 0; i < channels_alloc; i++) {
		c = channels[i];
		if (c == NULL || c->remote_id < 0)
			continue;
		switch (c->type) {
		case SSH_CHANNEL_CLOSED:
		case SSH_CHANNEL_DYNAMIC:
		case SSH_CHANNEL_X11_LISTENER:
		case SSH_CHANNEL_PORT_LISTENER:
		case SSH_CHANNEL_RPORT_LISTENER:
		case SSH_CHANNEL_MUX_LISTENER:
		case SSH_CHANNEL_MUX_CLIENT:
		case SSH_CHANNEL_OPENING:
		case SSH_CHANNEL_CONNECTING:
		case SSH_CHANNEL_ZOMBIE:
		case SSH_CHANNEL_ABANDONED:
		case SSH_CHANNEL_UNIX_LISTENER:
		case SSH_CHANNEL_RUNIX_LISTENER:
			continue;
		case SSH_CHANNEL_LARVAL:
		case SSH_CHANNEL_AUTH_SOCKET:
		case SSH_CHANNEL_OPEN:
		case SSH_CHANNEL_X11_OPEN:
			return i;
		case SSH_CHANNEL_INPUT_DRAINING:
		case SSH_CHANNEL_OUTPUT_DRAINING:
			if (!compat13)
				fatal("cannot happen: OUT_DRAIN");
			return i;
		default:
			fatal("channel_find_open: bad channel type %d", c->type);
			/* NOTREACHED */
		}
	}
	return -1;
}
