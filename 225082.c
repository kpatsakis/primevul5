static void vgacon_init(struct vc_data *c, int init)
{
	struct uni_pagedir *p;

	/*
	 * We cannot be loaded as a module, therefore init is always 1,
	 * but vgacon_init can be called more than once, and init will
	 * not be 1.
	 */
	c->vc_can_do_color = vga_can_do_color;

	/* set dimensions manually if init != 0 since vc_resize() will fail */
	if (init) {
		c->vc_cols = vga_video_num_columns;
		c->vc_rows = vga_video_num_lines;
	} else
		vc_resize(c, vga_video_num_columns, vga_video_num_lines);

	c->vc_scan_lines = vga_scan_lines;
	c->vc_font.height = vga_video_font_height;
	c->vc_complement_mask = 0x7700;
	if (vga_512_chars)
		c->vc_hi_font_mask = 0x0800;
	p = *c->vc_uni_pagedir_loc;
	if (c->vc_uni_pagedir_loc != &vgacon_uni_pagedir) {
		con_free_unimap(c);
		c->vc_uni_pagedir_loc = &vgacon_uni_pagedir;
		vgacon_refcount++;
	}
	if (!vgacon_uni_pagedir && p)
		con_set_default_unimap(c);

	/* Only set the default if the user didn't deliberately override it */
	if (global_cursor_default == -1)
		global_cursor_default =
			!(screen_info.flags & VIDEO_FLAGS_NOCURSOR);
}