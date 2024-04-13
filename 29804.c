channel_pre_output_draining(Channel *c, fd_set *readset, fd_set *writeset)
{
	if (buffer_len(&c->output) == 0)
		chan_mark_dead(c);
	else
		FD_SET(c->sock, writeset);
}
