static void dcn20_split_stream_for_mpc(
		struct resource_context *res_ctx,
		const struct resource_pool *pool,
		struct pipe_ctx *primary_pipe,
		struct pipe_ctx *secondary_pipe)
{
	int pipe_idx = secondary_pipe->pipe_idx;
	struct pipe_ctx *sec_bot_pipe = secondary_pipe->bottom_pipe;

	*secondary_pipe = *primary_pipe;
	secondary_pipe->bottom_pipe = sec_bot_pipe;

	secondary_pipe->pipe_idx = pipe_idx;
	secondary_pipe->plane_res.mi = pool->mis[secondary_pipe->pipe_idx];
	secondary_pipe->plane_res.hubp = pool->hubps[secondary_pipe->pipe_idx];
	secondary_pipe->plane_res.ipp = pool->ipps[secondary_pipe->pipe_idx];
	secondary_pipe->plane_res.xfm = pool->transforms[secondary_pipe->pipe_idx];
	secondary_pipe->plane_res.dpp = pool->dpps[secondary_pipe->pipe_idx];
	secondary_pipe->plane_res.mpcc_inst = pool->dpps[secondary_pipe->pipe_idx]->inst;
#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
	secondary_pipe->stream_res.dsc = NULL;
#endif
	if (primary_pipe->bottom_pipe && primary_pipe->bottom_pipe != secondary_pipe) {
		ASSERT(!secondary_pipe->bottom_pipe);
		secondary_pipe->bottom_pipe = primary_pipe->bottom_pipe;
		secondary_pipe->bottom_pipe->top_pipe = secondary_pipe;
	}
	primary_pipe->bottom_pipe = secondary_pipe;
	secondary_pipe->top_pipe = primary_pipe;

	ASSERT(primary_pipe->plane_state);
	resource_build_scaling_params(primary_pipe);
	resource_build_scaling_params(secondary_pipe);
}