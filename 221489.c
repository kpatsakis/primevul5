static void io_queue_async_work(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;
	struct io_kiocb *link = io_prep_linked_timeout(req);
	struct io_uring_task *tctx = req->task->io_uring;

	BUG_ON(!tctx);
	BUG_ON(!tctx->io_wq);

	/* init ->work of the whole link before punting */
	io_prep_async_link(req);
	trace_io_uring_queue_async_work(ctx, io_wq_is_hashed(&req->work), req,
					&req->work, req->flags);
	io_wq_enqueue(tctx->io_wq, &req->work);
	if (link)
		io_queue_linked_timeout(link);
}