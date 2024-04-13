
static void io_init_poll_iocb(struct io_poll_iocb *poll, __poll_t events,
			      wait_queue_func_t wake_func)
{
	poll->head = NULL;
	poll->done = false;
	poll->canceled = false;
#define IO_POLL_UNMASK	(EPOLLERR|EPOLLHUP|EPOLLNVAL|EPOLLRDHUP)
	/* mask in events that we always want/need */
	poll->events = events | IO_POLL_UNMASK;
	INIT_LIST_HEAD(&poll->wait.entry);
	init_waitqueue_func_entry(&poll->wait, wake_func);