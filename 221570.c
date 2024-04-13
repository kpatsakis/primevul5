					   __u64 user_data)
	__must_hold(&ctx->completion_lock)
{
	struct io_timeout_data *io;
	struct io_kiocb *req;
	bool found = false;

	list_for_each_entry(req, &ctx->timeout_list, timeout.list) {
		found = user_data == req->user_data;
		if (found)
			break;
	}
	if (!found)
		return ERR_PTR(-ENOENT);

	io = req->async_data;
	if (hrtimer_try_to_cancel(&io->timer) == -1)
		return ERR_PTR(-EALREADY);
	list_del_init(&req->timeout.list);
	return req;