static bool dcn20_validate_dsc(struct dc *dc, struct dc_state *new_ctx)
{
	int i;

	/* Validate DSC config, dsc count validation is already done */
	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		struct pipe_ctx *pipe_ctx = &new_ctx->res_ctx.pipe_ctx[i];
		struct dc_stream_state *stream = pipe_ctx->stream;
		struct dsc_config dsc_cfg;
		struct pipe_ctx *odm_pipe;
		int opp_cnt = 1;

		for (odm_pipe = pipe_ctx->next_odm_pipe; odm_pipe; odm_pipe = odm_pipe->next_odm_pipe)
			opp_cnt++;

		/* Only need to validate top pipe */
		if (pipe_ctx->top_pipe || pipe_ctx->prev_odm_pipe || !stream || !stream->timing.flags.DSC)
			continue;

		dsc_cfg.pic_width = (stream->timing.h_addressable + stream->timing.h_border_left
				+ stream->timing.h_border_right) / opp_cnt;
		dsc_cfg.pic_height = stream->timing.v_addressable + stream->timing.v_border_top
				+ stream->timing.v_border_bottom;
		dsc_cfg.pixel_encoding = stream->timing.pixel_encoding;
		dsc_cfg.color_depth = stream->timing.display_color_depth;
		dsc_cfg.dc_dsc_cfg = stream->timing.dsc_cfg;
		dsc_cfg.dc_dsc_cfg.num_slices_h /= opp_cnt;

		if (!pipe_ctx->stream_res.dsc->funcs->dsc_validate_stream(pipe_ctx->stream_res.dsc, &dsc_cfg))
			return false;
	}
	return true;
}