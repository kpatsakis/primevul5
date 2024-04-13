static enum dc_status remove_dsc_from_stream_resource(struct dc *dc,
		struct dc_state *new_ctx,
		struct dc_stream_state *dc_stream)
{
	struct pipe_ctx *pipe_ctx = NULL;
	int i;

	for (i = 0; i < MAX_PIPES; i++) {
		if (new_ctx->res_ctx.pipe_ctx[i].stream == dc_stream && !new_ctx->res_ctx.pipe_ctx[i].top_pipe) {
			pipe_ctx = &new_ctx->res_ctx.pipe_ctx[i];

			if (pipe_ctx->stream_res.dsc)
				release_dsc(&new_ctx->res_ctx, dc->res_pool, &pipe_ctx->stream_res.dsc);
		}
	}

	if (!pipe_ctx)
		return DC_ERROR_UNEXPECTED;
	else
		return DC_OK;
}