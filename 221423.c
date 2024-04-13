	__must_hold(&req->ctx->completion_lock)
{
	struct io_kiocb *nxt, *link = req->link;

	req->link = NULL;
	while (link) {
		nxt = link->link;
		link->link = NULL;

		trace_io_uring_fail_link(req, link);
		io_cqring_fill_event(link->ctx, link->user_data, -ECANCELED, 0);
		io_put_req_deferred(link, 2);
		link = nxt;
	}
}