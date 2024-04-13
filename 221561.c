static void io_free_req_deferred(struct io_kiocb *req)
{
	req->task_work.func = io_put_req_deferred_cb;
	if (unlikely(io_req_task_work_add(req)))
		io_req_task_work_add_fallback(req, io_put_req_deferred_cb);
}