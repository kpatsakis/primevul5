static void vgacon_save_screen(struct vc_data *c)
{
	static int vga_bootup_console = 0;

	if (!vga_bootup_console) {
		/* This is a gross hack, but here is the only place we can
		 * set bootup console parameters without messing up generic
		 * console initialization routines.
		 */
		vga_bootup_console = 1;
		c->state.x = screen_info.orig_x;
		c->state.y = screen_info.orig_y;
	}

	/* We can't copy in more than the size of the video buffer,
	 * or we'll be copying in VGA BIOS */

	if (!vga_is_gfx)
		scr_memcpyw((u16 *) c->vc_screenbuf, (u16 *) c->vc_origin,
			    c->vc_screenbuf_size > vga_vram_size ? vga_vram_size : c->vc_screenbuf_size);
}