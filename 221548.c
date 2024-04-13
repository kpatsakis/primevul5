
static void __io_sqe_files_unregister(struct io_ring_ctx *ctx)
{
#if defined(CONFIG_UNIX)
	if (ctx->ring_sock) {
		struct sock *sock = ctx->ring_sock->sk;
		struct sk_buff *skb;

		while ((skb = skb_dequeue(&sock->sk_receive_queue)) != NULL)
			kfree_skb(skb);
	}
#else
	int i;

	for (i = 0; i < ctx->nr_user_files; i++) {
		struct file *file;

		file = io_file_from_index(ctx, i);
		if (file)
			fput(file);
	}
#endif
	io_free_file_tables(&ctx->file_table, ctx->nr_user_files);
	io_rsrc_data_free(ctx->file_data);
	ctx->file_data = NULL;
	ctx->nr_user_files = 0;