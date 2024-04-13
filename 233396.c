static enum dc_status build_pipe_hw_param(struct pipe_ctx *pipe_ctx)
{

	get_pixel_clock_parameters(pipe_ctx, &pipe_ctx->stream_res.pix_clk_params);

	pipe_ctx->clock_source->funcs->get_pix_clk_dividers(
		pipe_ctx->clock_source,
		&pipe_ctx->stream_res.pix_clk_params,
		&pipe_ctx->pll_settings);

	pipe_ctx->stream->clamping.pixel_encoding = pipe_ctx->stream->timing.pixel_encoding;

	resource_build_bit_depth_reduction_params(pipe_ctx->stream,
					&pipe_ctx->stream->bit_depth_params);
	build_clamping_params(pipe_ctx->stream);

	return DC_OK;
}