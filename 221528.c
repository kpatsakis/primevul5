static void io_req_task_queue_fail(struct io_kiocb *req, int ret)
{
	req->result = ret;
	req->task_work.func = io_req_task_cancel;

	if (unlikely(io_req_task_work_add(req)))
		io_req_task_work_add_fallback(req, io_req_task_cancel);
}