channel_pre_open_13(Channel *c, fd_set *readset, fd_set *writeset)
{
	if (buffer_len(&c->input) < packet_get_maxsize())
		FD_SET(c->sock, readset);
	if (buffer_len(&c->output) > 0)
		FD_SET(c->sock, writeset);
}
