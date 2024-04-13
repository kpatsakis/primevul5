
static int io_async_cancel_one(struct io_uring_task *tctx, u64 user_data,
			       struct io_ring_ctx *ctx)
{
	struct io_cancel_data data = { .ctx = ctx, .user_data = user_data, };
	enum io_wq_cancel cancel_ret;
	int ret = 0;

	if (!tctx || !tctx->io_wq)
		return -ENOENT;

	cancel_ret = io_wq_cancel_cb(tctx->io_wq, io_cancel_cb, &data, false);
	switch (cancel_ret) {
	case IO_WQ_CANCEL_OK:
		ret = 0;
		break;
	case IO_WQ_CANCEL_RUNNING:
		ret = -EALREADY;
		break;
	case IO_WQ_CANCEL_NOTFOUND:
		ret = -ENOENT;
		break;
	}

	return ret;