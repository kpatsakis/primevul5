static void __io_put_req_task(struct io_kiocb *req)
{
	if (req->flags & REQ_F_TASK_PINNED)
		put_task_struct(req->task);
}