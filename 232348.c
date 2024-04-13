static void io_submit_state_start(struct io_submit_state *state,
				  struct io_ring_ctx *ctx, unsigned int max_ios)
{
	blk_start_plug(&state->plug);
#ifdef CONFIG_BLOCK
	state->plug.nowait = true;
#endif
	state->comp.nr = 0;
	INIT_LIST_HEAD(&state->comp.list);
	state->comp.ctx = ctx;
	state->free_reqs = 0;
	state->file = NULL;
	state->ios_left = max_ios;
}