static void io_req_task_cancel(struct callback_head *cb)
{
	struct io_kiocb *req = container_of(cb, struct io_kiocb, task_work);

	__io_req_task_cancel(req, -ECANCELED);
}