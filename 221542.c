static bool io_flush_cached_reqs(struct io_ring_ctx *ctx)
{
	struct io_submit_state *state = &ctx->submit_state;
	struct io_comp_state *cs = &state->comp;
	int nr;

	/*
	 * If we have more than a batch's worth of requests in our IRQ side
	 * locked cache, grab the lock and move them over to our submission
	 * side cache.
	 */
	if (READ_ONCE(cs->locked_free_nr) > IO_COMPL_BATCH)
		io_flush_cached_locked_reqs(ctx, cs);

	nr = state->free_reqs;
	while (!list_empty(&cs->free_list)) {
		struct io_kiocb *req = list_first_entry(&cs->free_list,
						struct io_kiocb, compl.list);

		list_del(&req->compl.list);
		state->reqs[nr++] = req;
		if (nr == ARRAY_SIZE(state->reqs))
			break;
	}

	state->free_reqs = nr;
	return nr != 0;
}