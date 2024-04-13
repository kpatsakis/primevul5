enum dc_status dcn20_build_mapped_resource(const struct dc *dc, struct dc_state *context, struct dc_stream_state *stream)
{
	enum dc_status status = DC_OK;
	struct pipe_ctx *pipe_ctx = resource_get_head_pipe_for_stream(&context->res_ctx, stream);

	/*TODO Seems unneeded anymore */
	/*	if (old_context && resource_is_stream_unchanged(old_context, stream)) {
			if (stream != NULL && old_context->streams[i] != NULL) {
				 todo: shouldn't have to copy missing parameter here
				resource_build_bit_depth_reduction_params(stream,
						&stream->bit_depth_params);
				stream->clamping.pixel_encoding =
						stream->timing.pixel_encoding;

				resource_build_bit_depth_reduction_params(stream,
								&stream->bit_depth_params);
				build_clamping_params(stream);

				continue;
			}
		}
	*/

	if (!pipe_ctx)
		return DC_ERROR_UNEXPECTED;


	status = build_pipe_hw_param(pipe_ctx);

	return status;
}