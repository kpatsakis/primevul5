static void io_req_task_work_add_fallback(struct io_kiocb *req,
					  task_work_func_t cb)
{
	init_task_work(&req->task_work, cb);
	io_task_work_add_head(&req->ctx->exit_task_work, &req->task_work);
}