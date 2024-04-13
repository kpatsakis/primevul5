void rotate_fb(int x1, int y1, int x2, int y2) {
	int x, y, xn, yn, r_x1, r_y1, r_x2, r_y2, Bpp = bpp/8;
	int fbl = rfb_bytes_per_line;
	int rbl = rot_bytes_per_line;
	int Dx, Dy;
	char *src, *dst;
	char *src_0 = rfb_fb;
	char *dst_0 = rot_fb;

	if (! rotating || ! rot_fb) {
		return;
	}

	if (scaling) {
		Dx = scaled_x;
		Dy = scaled_y;
	} else {
		Dx = dpy_x;
		Dy = dpy_y;
	}
	rotate_coords(x1, y1, &r_x1, &r_y1, -1, -1);
	rotate_coords(x2, y2, &r_x2, &r_y2, -1, -1);

	dst = rot_fb;

	if (rotating == ROTATE_X) {
		for (y = y1; y < y2; y++)  {
			for (x = x1; x < x2; x++)  {
				xn = Dx - x - 1;
				yn = y;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_Y) {
		for (y = y1; y < y2; y++)  {
			for (x = x1; x < x2; x++)  {
				xn = x;
				yn = Dy - y - 1;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_XY) {
		for (y = y1; y < y2; y++)  {
			for (x = x1; x < x2; x++)  {
				xn = Dx - x - 1;
				yn = Dy - y - 1;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_90) {
		for (y = y1; y < y2; y++)  {
			for (x = x1; x < x2; x++)  {
				xn = Dy - y - 1;
				yn = x;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_90X) {
		for (y = y1; y < y2; y++)  {
			for (x = x1; x < x2; x++)  {
				xn = y;
				yn = x;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_90Y) {
		for (y = y1; y < y2; y++)  {
			for (x = x1; x < x2; x++)  {
				xn = Dy - y - 1;
				yn = Dx - x - 1;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_270) {
		for (y = y1; y < y2; y++)  {
			for (x = x1; x < x2; x++)  {
				xn = y;
				yn = Dx - x - 1;
				ROT_COPY
			}
		}
	}
}