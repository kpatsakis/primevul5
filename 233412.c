enum dc_status dcn20_remove_stream_from_ctx(struct dc *dc, struct dc_state *new_ctx, struct dc_stream_state *dc_stream)
{
	enum dc_status result = DC_OK;

#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
	result = remove_dsc_from_stream_resource(dc, new_ctx, dc_stream);
#endif

	return result;
}