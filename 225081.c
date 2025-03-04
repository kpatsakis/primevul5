static int vgacon_blank(struct vc_data *c, int blank, int mode_switch)
{
	switch (blank) {
	case 0:		/* Unblank */
		if (vga_vesa_blanked) {
			vga_vesa_unblank(&vgastate);
			vga_vesa_blanked = 0;
		}
		if (vga_palette_blanked) {
			vga_set_palette(c, color_table);
			vga_palette_blanked = false;
			return 0;
		}
		vga_is_gfx = false;
		/* Tell console.c that it has to restore the screen itself */
		return 1;
	case 1:		/* Normal blanking */
	case -1:	/* Obsolete */
		if (!mode_switch && vga_video_type == VIDEO_TYPE_VGAC) {
			vga_pal_blank(&vgastate);
			vga_palette_blanked = true;
			return 0;
		}
		vgacon_set_origin(c);
		scr_memsetw((void *) vga_vram_base, BLANK,
			    c->vc_screenbuf_size);
		if (mode_switch)
			vga_is_gfx = true;
		return 1;
	default:		/* VESA blanking */
		if (vga_video_type == VIDEO_TYPE_VGAC) {
			vga_vesa_blank(&vgastate, blank - 1);
			vga_vesa_blanked = blank;
		}
		return 0;
	}
}