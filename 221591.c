static bool io_poll_remove_one(struct io_kiocb *req)
	__must_hold(&req->ctx->completion_lock)
{
	bool do_complete;

	do_complete = io_poll_remove_waitqs(req);
	if (do_complete) {
		io_cqring_fill_event(req->ctx, req->user_data, -ECANCELED, 0);
		io_commit_cqring(req->ctx);
		req_set_fail_links(req);
		io_put_req_deferred(req, 1);
	}

	return do_complete;