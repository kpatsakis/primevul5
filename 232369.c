static void io_get_req_task(struct io_kiocb *req)
{
	if (req->flags & REQ_F_TASK_PINNED)
		return;
	get_task_struct(req->task);
	req->flags |= REQ_F_TASK_PINNED;
}