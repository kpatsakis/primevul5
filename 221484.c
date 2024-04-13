static int io_async_buf_func(struct wait_queue_entry *wait, unsigned mode,
			     int sync, void *arg)
{
	struct wait_page_queue *wpq;
	struct io_kiocb *req = wait->private;
	struct wait_page_key *key = arg;

	wpq = container_of(wait, struct wait_page_queue, wait);

	if (!wake_page_match(wpq, key))
		return 0;

	req->rw.kiocb.ki_flags &= ~IOCB_WAITQ;
	list_del_init(&wait->entry);

	/* submit ref gets dropped, acquire a new one */
	req_ref_get(req);
	io_req_task_queue(req);
	return 1;
}