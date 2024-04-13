static struct pipe_ctx *dcn20_find_secondary_pipe(struct dc *dc,
		struct resource_context *res_ctx,
		const struct resource_pool *pool,
		const struct pipe_ctx *primary_pipe)
{
	struct pipe_ctx *secondary_pipe = NULL;

	if (dc && primary_pipe) {
		int j;
		int preferred_pipe_idx = 0;

		/* first check the prev dc state:
		 * if this primary pipe has a bottom pipe in prev. state
		 * and if the bottom pipe is still available (which it should be),
		 * pick that pipe as secondary
		 * Same logic applies for ODM pipes. Since mpo is not allowed with odm
		 * check in else case.
		 */
		if (dc->current_state->res_ctx.pipe_ctx[primary_pipe->pipe_idx].bottom_pipe) {
			preferred_pipe_idx = dc->current_state->res_ctx.pipe_ctx[primary_pipe->pipe_idx].bottom_pipe->pipe_idx;
			if (res_ctx->pipe_ctx[preferred_pipe_idx].stream == NULL) {
				secondary_pipe = &res_ctx->pipe_ctx[preferred_pipe_idx];
				secondary_pipe->pipe_idx = preferred_pipe_idx;
			}
		} else if (dc->current_state->res_ctx.pipe_ctx[primary_pipe->pipe_idx].next_odm_pipe) {
			preferred_pipe_idx = dc->current_state->res_ctx.pipe_ctx[primary_pipe->pipe_idx].next_odm_pipe->pipe_idx;
			if (res_ctx->pipe_ctx[preferred_pipe_idx].stream == NULL) {
				secondary_pipe = &res_ctx->pipe_ctx[preferred_pipe_idx];
				secondary_pipe->pipe_idx = preferred_pipe_idx;
			}
		}

		/*
		 * if this primary pipe does not have a bottom pipe in prev. state
		 * start backward and find a pipe that did not used to be a bottom pipe in
		 * prev. dc state. This way we make sure we keep the same assignment as
		 * last state and will not have to reprogram every pipe
		 */
		if (secondary_pipe == NULL) {
			for (j = dc->res_pool->pipe_count - 1; j >= 0; j--) {
				if (dc->current_state->res_ctx.pipe_ctx[j].top_pipe == NULL) {
					preferred_pipe_idx = j;

					if (res_ctx->pipe_ctx[preferred_pipe_idx].stream == NULL) {
						secondary_pipe = &res_ctx->pipe_ctx[preferred_pipe_idx];
						secondary_pipe->pipe_idx = preferred_pipe_idx;
						break;
					}
				}
			}
		}
		/*
		 * We should never hit this assert unless assignments are shuffled around
		 * if this happens we will prob. hit a vsync tdr
		 */
		ASSERT(secondary_pipe);
		/*
		 * search backwards for the second pipe to keep pipe
		 * assignment more consistent
		 */
		if (secondary_pipe == NULL) {
			for (j = dc->res_pool->pipe_count - 1; j >= 0; j--) {
				preferred_pipe_idx = j;

				if (res_ctx->pipe_ctx[preferred_pipe_idx].stream == NULL) {
					secondary_pipe = &res_ctx->pipe_ctx[preferred_pipe_idx];
					secondary_pipe->pipe_idx = preferred_pipe_idx;
					break;
				}
			}
		}
	}

	return secondary_pipe;
}