void rotate_curs(char *dst_0, char *src_0, int Dx, int Dy, int Bpp) {
	int x, y, xn, yn;
	char *src, *dst;
	int fbl, rbl;

	if (! rotating) {
		return;
	}

	fbl = Dx * Bpp;
	if (rotating_same) {
		rbl = Dx * Bpp;
	} else {
		rbl = Dy * Bpp;
	}

	if (rotating == ROTATE_X) {
		for (y = 0; y < Dy; y++)  {
			for (x = 0; x < Dx; x++)  {
				xn = Dx - x - 1;
				yn = y;
				ROT_COPY
if (0) fprintf(stderr, "rcurs: %d %d  %d %d\n", x, y, xn, yn);
			}
		}
	} else if (rotating == ROTATE_Y) {
		for (y = 0; y < Dy; y++)  {
			for (x = 0; x < Dx; x++)  {
				xn = x;
				yn = Dy - y - 1;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_XY) {
		for (y = 0; y < Dy; y++)  {
			for (x = 0; x < Dx; x++)  {
				xn = Dx - x - 1;
				yn = Dy - y - 1;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_90) {
		for (y = 0; y < Dy; y++)  {
			for (x = 0; x < Dx; x++)  {
				xn = Dy - y - 1;
				yn = x;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_90X) {
		for (y = 0; y < Dy; y++)  {
			for (x = 0; x < Dx; x++)  {
				xn = y;
				yn = x;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_90Y) {
		for (y = 0; y < Dy; y++)  {
			for (x = 0; x < Dx; x++)  {
				xn = Dy - y - 1;
				yn = Dx - x - 1;
				ROT_COPY
			}
		}
	} else if (rotating == ROTATE_270) {
		for (y = 0; y < Dy; y++)  {
			for (x = 0; x < Dx; x++)  {
				xn = y;
				yn = Dx - x - 1;
				ROT_COPY
			}
		}
	}
}