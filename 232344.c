static void __io_uring_show_fdinfo(struct io_ring_ctx *ctx, struct seq_file *m)
{
	int i;

	mutex_lock(&ctx->uring_lock);
	seq_printf(m, "UserFiles:\t%u\n", ctx->nr_user_files);
	for (i = 0; i < ctx->nr_user_files; i++) {
		struct fixed_file_table *table;
		struct file *f;

		table = &ctx->file_data->table[i >> IORING_FILE_TABLE_SHIFT];
		f = table->files[i & IORING_FILE_TABLE_MASK];
		if (f)
			seq_printf(m, "%5u: %s\n", i, file_dentry(f)->d_iname);
		else
			seq_printf(m, "%5u: <none>\n", i);
	}
	seq_printf(m, "UserBufs:\t%u\n", ctx->nr_user_bufs);
	for (i = 0; i < ctx->nr_user_bufs; i++) {
		struct io_mapped_ubuf *buf = &ctx->user_bufs[i];

		seq_printf(m, "%5u: 0x%llx/%u\n", i, buf->ubuf,
						(unsigned int) buf->len);
	}
	if (!idr_is_empty(&ctx->personality_idr)) {
		seq_printf(m, "Personalities:\n");
		idr_for_each(&ctx->personality_idr, io_uring_show_cred, m);
	}
	seq_printf(m, "PollList:\n");
	spin_lock_irq(&ctx->completion_lock);
	for (i = 0; i < (1U << ctx->cancel_hash_bits); i++) {
		struct hlist_head *list = &ctx->cancel_hash[i];
		struct io_kiocb *req;

		hlist_for_each_entry(req, list, hash_node)
			seq_printf(m, "  op=%d, task_works=%d\n", req->opcode,
					req->task->task_works != NULL);
	}
	spin_unlock_irq(&ctx->completion_lock);
	mutex_unlock(&ctx->uring_lock);
}