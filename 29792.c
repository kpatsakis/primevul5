channel_close_fds(Channel *c)
{
	channel_close_fd(&c->sock);
	channel_close_fd(&c->rfd);
	channel_close_fd(&c->wfd);
	channel_close_fd(&c->efd);
}
