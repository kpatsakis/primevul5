void util_set_fd_cloexec(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFD);
	if (flags < 0)
		flags = FD_CLOEXEC;
	else
		flags |= FD_CLOEXEC;
	fcntl(fd, F_SETFD, flags);
}