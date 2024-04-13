static void io_poll_complete(struct io_kiocb *req, __poll_t mask, int error)
{
	struct io_ring_ctx *ctx = req->ctx;

	io_poll_remove_double(req, req->io);
	req->poll.done = true;
	io_cqring_fill_event(req, error ? error : mangle_poll(mask));
	io_commit_cqring(ctx);
}