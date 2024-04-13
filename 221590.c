
static void io_uring_clean_tctx(struct io_uring_task *tctx)
{
	struct io_tctx_node *node;
	unsigned long index;

	xa_for_each(&tctx->xa, index, node)
		io_uring_del_task_file(index);
	if (tctx->io_wq) {
		io_wq_put_and_exit(tctx->io_wq);
		tctx->io_wq = NULL;
	}