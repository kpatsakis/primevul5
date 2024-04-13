enum dc_status dcn20_add_stream_to_ctx(struct dc *dc, struct dc_state *new_ctx, struct dc_stream_state *dc_stream)
{
	enum dc_status result = DC_ERROR_UNEXPECTED;

	result = resource_map_pool_resources(dc, new_ctx, dc_stream);

	if (result == DC_OK)
		result = resource_map_phy_clock_resources(dc, new_ctx, dc_stream);

#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
	/* Get a DSC if required and available */
	if (result == DC_OK && dc_stream->timing.flags.DSC)
		result = add_dsc_to_stream_resource(dc, new_ctx, dc_stream);
#endif

	if (result == DC_OK)
		result = dcn20_build_mapped_resource(dc, new_ctx, dc_stream);

	return result;
}