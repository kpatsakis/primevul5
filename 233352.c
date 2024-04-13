static enum dc_status add_dsc_to_stream_resource(struct dc *dc,
		struct dc_state *dc_ctx,
		struct dc_stream_state *dc_stream)
{
	enum dc_status result = DC_OK;
	int i;
	const struct resource_pool *pool = dc->res_pool;

	/* Get a DSC if required and available */
	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		struct pipe_ctx *pipe_ctx = &dc_ctx->res_ctx.pipe_ctx[i];

		if (pipe_ctx->stream != dc_stream)
			continue;

		acquire_dsc(&dc_ctx->res_ctx, pool, &pipe_ctx->stream_res.dsc);

		/* The number of DSCs can be less than the number of pipes */
		if (!pipe_ctx->stream_res.dsc) {
			dm_output_to_console("No DSCs available\n");
			result = DC_NO_DSC_RESOURCE;
		}

		break;
	}

	return result;
}