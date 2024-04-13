static int vgacon_resize(struct vc_data *c, unsigned int width,
			 unsigned int height, unsigned int user)
{
	if ((width << 1) * height > vga_vram_size)
		return -EINVAL;

	if (width % 2 || width > screen_info.orig_video_cols ||
	    height > (screen_info.orig_video_lines * vga_default_font_height)/
	    c->vc_font.height)
		/* let svgatextmode tinker with video timings and
		   return success */
		return (user) ? 0 : -EINVAL;

	if (con_is_visible(c) && !vga_is_gfx) /* who knows */
		vgacon_doresize(c, width, height);
	return 0;
}