
static void io_req_caches_free(struct io_ring_ctx *ctx)
{
	struct io_submit_state *submit_state = &ctx->submit_state;
	struct io_comp_state *cs = &ctx->submit_state.comp;

	mutex_lock(&ctx->uring_lock);

	if (submit_state->free_reqs) {
		kmem_cache_free_bulk(req_cachep, submit_state->free_reqs,
				     submit_state->reqs);
		submit_state->free_reqs = 0;
	}

	io_flush_cached_locked_reqs(ctx, cs);
	io_req_cache_free(&cs->free_list, NULL);
	mutex_unlock(&ctx->uring_lock);