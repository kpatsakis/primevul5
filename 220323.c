static __poll_t hung_up_tty_poll(struct file *filp, poll_table *wait)
{
	return EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDNORM | EPOLLWRNORM;
}