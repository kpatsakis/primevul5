static int io_timeout_cancel(struct io_ring_ctx *ctx, __u64 user_data)
{
	struct io_kiocb *req;
	int ret = -ENOENT;

	list_for_each_entry(req, &ctx->timeout_list, timeout.list) {
		if (user_data == req->user_data) {
			list_del_init(&req->timeout.list);
			ret = 0;
			break;
		}
	}

	if (ret == -ENOENT)
		return ret;

	ret = hrtimer_try_to_cancel(&req->io->timeout.timer);
	if (ret == -1)
		return -EALREADY;

	req_set_fail_links(req);
	req->flags |= REQ_F_COMP_LOCKED;
	io_cqring_fill_event(req, -ECANCELED);
	io_put_req(req);
	return 0;
}