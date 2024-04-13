channel_pre_connecting(Channel *c, fd_set *readset, fd_set *writeset)
{
	debug3("channel %d: waiting for connection", c->self);
	FD_SET(c->sock, writeset);
}
