void scale_and_mark_rect(int X1, int Y1, int X2, int Y2, int mark) {
	char *dst_fb, *src_fb = main_fb;
	int dst_bpl, Bpp = bpp/8, fac = 1;

	if (!screen || !rfb_fb || !main_fb) {
		return;
	}
	if (! screen->serverFormat.trueColour) {
		/*
		 * PseudoColor colormap... blending leads to random colors.
		 * User can override with ":fb"
		 */
		if (scaling_blend == 1) {
			/* :fb option sets it to 2 */
			if (default_visual->class == StaticGray) {
				/*
				 * StaticGray can be blended OK, otherwise
				 * user can disable with :nb
				 */
				;
			} else {
				scaling_blend = 0;
			}
		}
	}

	if (cmap8to24 && cmap8to24_fb) {
		src_fb = cmap8to24_fb;
		if (scaling) {
			if (depth <= 8) {
				fac = 4;
			} else if (depth <= 16) {
				fac = 2;
			}
		}
	}
	dst_fb = rfb_fb;
	dst_bpl = rfb_bytes_per_line;

	scale_rect(scale_fac_x, scale_fac_y, scaling_blend, scaling_interpolate, fac * Bpp,
	    src_fb, fac * main_bytes_per_line, dst_fb, dst_bpl, dpy_x, dpy_y,
	    scaled_x, scaled_y, X1, Y1, X2, Y2, mark);
}