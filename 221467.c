
static void io_queue_sqe(struct io_kiocb *req)
{
	int ret;

	ret = io_req_defer(req);
	if (ret) {
		if (ret != -EIOCBQUEUED) {
fail_req:
			io_req_complete_failed(req, ret);
		}
	} else if (req->flags & REQ_F_FORCE_ASYNC) {
		ret = io_req_prep_async(req);
		if (unlikely(ret))
			goto fail_req;
		io_queue_async_work(req);
	} else {
		__io_queue_sqe(req);
	}