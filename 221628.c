
static void io_uring_try_cancel(struct files_struct *files)
{
	struct io_uring_task *tctx = current->io_uring;
	struct io_tctx_node *node;
	unsigned long index;

	xa_for_each(&tctx->xa, index, node) {
		struct io_ring_ctx *ctx = node->ctx;

		/* sqpoll task will cancel all its requests */
		if (!ctx->sq_data)
			io_uring_try_cancel_requests(ctx, current, files);
	}