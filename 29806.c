channel_register_fds(Channel *c, int rfd, int wfd, int efd,
    int extusage, int nonblock, int is_tty)
{
	/* Update the maximum file descriptor value. */
	channel_max_fd = MAX(channel_max_fd, rfd);
	channel_max_fd = MAX(channel_max_fd, wfd);
	channel_max_fd = MAX(channel_max_fd, efd);

	if (rfd != -1)
		fcntl(rfd, F_SETFD, FD_CLOEXEC);
	if (wfd != -1 && wfd != rfd)
		fcntl(wfd, F_SETFD, FD_CLOEXEC);
	if (efd != -1 && efd != rfd && efd != wfd)
		fcntl(efd, F_SETFD, FD_CLOEXEC);

	c->rfd = rfd;
	c->wfd = wfd;
	c->sock = (rfd == wfd) ? rfd : -1;
	c->efd = efd;
	c->extended_usage = extusage;

	if ((c->isatty = is_tty) != 0)
		debug2("channel %d: rfd %d isatty", c->self, c->rfd);
#ifdef _AIX
	/* XXX: Later AIX versions can't push as much data to tty */
	c->wfd_isatty = is_tty || isatty(c->wfd);
#endif

	/* enable nonblocking mode */
	if (nonblock) {
		if (rfd != -1)
			set_nonblock(rfd);
		if (wfd != -1)
			set_nonblock(wfd);
		if (efd != -1)
			set_nonblock(efd);
	}
}
