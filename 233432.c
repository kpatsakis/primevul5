enum dc_status dcn20_get_default_swizzle_mode(struct dc_plane_state *plane_state)
{
	enum dc_status result = DC_OK;

	enum surface_pixel_format surf_pix_format = plane_state->format;
	unsigned int bpp = resource_pixel_format_to_bpp(surf_pix_format);

	enum swizzle_mode_values swizzle = DC_SW_LINEAR;

	if (bpp == 64)
		swizzle = DC_SW_64KB_D;
	else
		swizzle = DC_SW_64KB_S;

	plane_state->tiling_info.gfx9.swizzle = swizzle;
	return result;
}