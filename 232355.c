static void io_ring_ctx_free(struct io_ring_ctx *ctx)
{
	io_finish_async(ctx);
	io_sqe_buffer_unregister(ctx);
	if (ctx->sqo_mm) {
		mmdrop(ctx->sqo_mm);
		ctx->sqo_mm = NULL;
	}

	io_sqe_files_unregister(ctx);
	io_eventfd_unregister(ctx);
	io_destroy_buffers(ctx);
	idr_destroy(&ctx->personality_idr);

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
	put_cred(ctx->creds);
	kfree(ctx->cancel_hash);
	kmem_cache_free(req_cachep, ctx->fallback_req);
	kfree(ctx);
}