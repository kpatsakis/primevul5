SYSCALL_DEFINE6(epoll_pwait, int, epfd, struct epoll_event __user *, events,
		int, maxevents, int, timeout, const sigset_t __user *, sigmask,
		size_t, sigsetsize)
{
	int error;

	/*
	 * If the caller wants a certain signal mask to be set during the wait,
	 * we apply it here.
	 */
	error = set_user_sigmask(sigmask, sigsetsize);
	if (error)
		return error;

	error = do_epoll_wait(epfd, events, maxevents, timeout);
	restore_saved_sigmask_unless(error == -EINTR);

	return error;
}