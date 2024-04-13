channel_pre_listener(Channel *c, fd_set *readset, fd_set *writeset)
{
	FD_SET(c->sock, readset);
}
