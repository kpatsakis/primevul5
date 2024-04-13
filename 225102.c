static void vgacon_set_palette(struct vc_data *vc, const unsigned char *table)
{
	if (vga_video_type != VIDEO_TYPE_VGAC || vga_palette_blanked
	    || !con_is_visible(vc))
		return;
	vga_set_palette(vc, table);
}