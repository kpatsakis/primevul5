static int vgacon_font_set(struct vc_data *c, struct console_font *font,
			   unsigned int flags)
{
	unsigned charcount = font->charcount;
	int rc;

	if (vga_video_type < VIDEO_TYPE_EGAM)
		return -EINVAL;

	if (font->width != VGA_FONTWIDTH ||
	    (charcount != 256 && charcount != 512))
		return -EINVAL;

	rc = vgacon_do_font_op(&vgastate, font->data, 1, charcount == 512);
	if (rc)
		return rc;

	if (!(flags & KD_FONT_FLAG_DONT_RECALC))
		rc = vgacon_adjust_height(c, font->height);
	return rc;
}