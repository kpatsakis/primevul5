static bool dcn20_split_stream_for_odm(
		struct resource_context *res_ctx,
		const struct resource_pool *pool,
		struct pipe_ctx *prev_odm_pipe,
		struct pipe_ctx *next_odm_pipe)
{
	int pipe_idx = next_odm_pipe->pipe_idx;

	*next_odm_pipe = *prev_odm_pipe;

	next_odm_pipe->pipe_idx = pipe_idx;
	next_odm_pipe->plane_res.mi = pool->mis[next_odm_pipe->pipe_idx];
	next_odm_pipe->plane_res.hubp = pool->hubps[next_odm_pipe->pipe_idx];
	next_odm_pipe->plane_res.ipp = pool->ipps[next_odm_pipe->pipe_idx];
	next_odm_pipe->plane_res.xfm = pool->transforms[next_odm_pipe->pipe_idx];
	next_odm_pipe->plane_res.dpp = pool->dpps[next_odm_pipe->pipe_idx];
	next_odm_pipe->plane_res.mpcc_inst = pool->dpps[next_odm_pipe->pipe_idx]->inst;
#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
	next_odm_pipe->stream_res.dsc = NULL;
#endif
	if (prev_odm_pipe->next_odm_pipe && prev_odm_pipe->next_odm_pipe != next_odm_pipe) {
		ASSERT(!next_odm_pipe->next_odm_pipe);
		next_odm_pipe->next_odm_pipe = prev_odm_pipe->next_odm_pipe;
		next_odm_pipe->next_odm_pipe->prev_odm_pipe = next_odm_pipe;
	}
	prev_odm_pipe->next_odm_pipe = next_odm_pipe;
	next_odm_pipe->prev_odm_pipe = prev_odm_pipe;
	ASSERT(next_odm_pipe->top_pipe == NULL);

	if (prev_odm_pipe->plane_state) {
		struct scaler_data *sd = &prev_odm_pipe->plane_res.scl_data;
		int new_width;

		/* HACTIVE halved for odm combine */
		sd->h_active /= 2;
		/* Calculate new vp and recout for left pipe */
		/* Need at least 16 pixels width per side */
		if (sd->recout.x + 16 >= sd->h_active)
			return false;
		new_width = sd->h_active - sd->recout.x;
		sd->viewport.width -= dc_fixpt_floor(dc_fixpt_mul_int(
				sd->ratios.horz, sd->recout.width - new_width));
		sd->viewport_c.width -= dc_fixpt_floor(dc_fixpt_mul_int(
				sd->ratios.horz_c, sd->recout.width - new_width));
		sd->recout.width = new_width;

		/* Calculate new vp and recout for right pipe */
		sd = &next_odm_pipe->plane_res.scl_data;
		/* HACTIVE halved for odm combine */
		sd->h_active /= 2;
		/* Need at least 16 pixels width per side */
		if (new_width <= 16)
			return false;
		new_width = sd->recout.width + sd->recout.x - sd->h_active;
		sd->viewport.width -= dc_fixpt_floor(dc_fixpt_mul_int(
				sd->ratios.horz, sd->recout.width - new_width));
		sd->viewport_c.width -= dc_fixpt_floor(dc_fixpt_mul_int(
				sd->ratios.horz_c, sd->recout.width - new_width));
		sd->recout.width = new_width;
		sd->viewport.x += dc_fixpt_floor(dc_fixpt_mul_int(
				sd->ratios.horz, sd->h_active - sd->recout.x));
		sd->viewport_c.x += dc_fixpt_floor(dc_fixpt_mul_int(
				sd->ratios.horz_c, sd->h_active - sd->recout.x));
		sd->recout.x = 0;
	}
	next_odm_pipe->stream_res.opp = pool->opps[next_odm_pipe->pipe_idx];
#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
	if (next_odm_pipe->stream->timing.flags.DSC == 1) {
		acquire_dsc(res_ctx, pool, &next_odm_pipe->stream_res.dsc);
		ASSERT(next_odm_pipe->stream_res.dsc);
		if (next_odm_pipe->stream_res.dsc == NULL)
			return false;
	}
#endif

	return true;
}