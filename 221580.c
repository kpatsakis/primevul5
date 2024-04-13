static void io_req_task_queue(struct io_kiocb *req)
{
	req->task_work.func = io_req_task_submit;

	if (unlikely(io_req_task_work_add(req)))
		io_req_task_queue_fail(req, -ECANCELED);
}