channel_stop_listening(void)
{
	u_int i;
	Channel *c;

	for (i = 0; i < channels_alloc; i++) {
		c = channels[i];
		if (c != NULL) {
			switch (c->type) {
			case SSH_CHANNEL_AUTH_SOCKET:
			case SSH_CHANNEL_PORT_LISTENER:
			case SSH_CHANNEL_RPORT_LISTENER:
			case SSH_CHANNEL_X11_LISTENER:
			case SSH_CHANNEL_UNIX_LISTENER:
			case SSH_CHANNEL_RUNIX_LISTENER:
				channel_close_fd(&c->sock);
				channel_free(c);
				break;
			}
		}
	}
}
