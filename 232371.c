static inline int kiocb_wait_page_queue_init(struct kiocb *kiocb,
					     struct wait_page_queue *wait,
					     wait_queue_func_t func,
					     void *data)
{
	/* Can't support async wakeup with polled IO */
	if (kiocb->ki_flags & IOCB_HIPRI)
		return -EINVAL;
	if (kiocb->ki_filp->f_mode & FMODE_BUF_RASYNC) {
		wait->wait.func = func;
		wait->wait.private = data;
		wait->wait.flags = 0;
		INIT_LIST_HEAD(&wait->wait.entry);
		kiocb->ki_flags |= IOCB_WAITQ;
		kiocb->ki_waitq = wait;
		return 0;
	}

	return -EOPNOTSUPP;
}