
static void io_ring_ctx_free(struct io_ring_ctx *ctx)
{
	io_sq_thread_finish(ctx);

	if (ctx->mm_account) {
		mmdrop(ctx->mm_account);
		ctx->mm_account = NULL;
	}

	mutex_lock(&ctx->uring_lock);
	if (io_wait_rsrc_data(ctx->buf_data))
		__io_sqe_buffers_unregister(ctx);
	if (io_wait_rsrc_data(ctx->file_data))
		__io_sqe_files_unregister(ctx);
	if (ctx->rings)
		__io_cqring_overflow_flush(ctx, true);
	mutex_unlock(&ctx->uring_lock);
	io_eventfd_unregister(ctx);
	io_destroy_buffers(ctx);
	if (ctx->sq_creds)
		put_cred(ctx->sq_creds);

	/* there are no registered resources left, nobody uses it */
	if (ctx->rsrc_node)
		io_rsrc_node_destroy(ctx->rsrc_node);
	if (ctx->rsrc_backup_node)
		io_rsrc_node_destroy(ctx->rsrc_backup_node);
	flush_delayed_work(&ctx->rsrc_put_work);

	WARN_ON_ONCE(!list_empty(&ctx->rsrc_ref_list));
	WARN_ON_ONCE(!llist_empty(&ctx->rsrc_put_llist));

#if defined(CONFIG_UNIX)
	if (ctx->ring_sock) {
		ctx->ring_sock->file = NULL; /* so that iput() is called */
		sock_release(ctx->ring_sock);
	}
#endif

	io_mem_free(ctx->rings);
	io_mem_free(ctx->sq_sqes);

	percpu_ref_exit(&ctx->refs);
	free_uid(ctx->user);
	io_req_caches_free(ctx);
	if (ctx->hash_map)
		io_wq_put_hash(ctx->hash_map);
	kfree(ctx->cancel_hash);
	kfree(ctx->dummy_ubuf);
	kfree(ctx);