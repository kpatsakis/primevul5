detach(void)
{
#if defined(HAVE_DAEMON) && !defined(DEPRECATED_DAEMON)
	/* use POSIX daemon(3) function */
	if(daemon(1, 0) != 0)
		fatal_exit("daemon failed: %s", strerror(errno));
#else /* no HAVE_DAEMON */
#ifdef HAVE_FORK
	int fd;
	/* Take off... */
	switch (fork()) {
		case 0:
			break;
		case -1:
			fatal_exit("fork failed: %s", strerror(errno));
		default:
			/* exit interactive session */
			exit(0);
	}
	/* detach */
#ifdef HAVE_SETSID
	if(setsid() == -1)
		fatal_exit("setsid() failed: %s", strerror(errno));
#endif
	if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
		(void)dup2(fd, STDIN_FILENO);
		(void)dup2(fd, STDOUT_FILENO);
		(void)dup2(fd, STDERR_FILENO);
		if (fd > 2)
			(void)close(fd);
	}
#endif /* HAVE_FORK */
#endif /* HAVE_DAEMON */
}