SYSCALL_DEFINE1(epoll_create1, int, flags)
{
	return do_epoll_create(flags);
}