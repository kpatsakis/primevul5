void rotate_coords(int x, int y, int *xo, int *yo, int dxi, int dyi) {
	int xi = x, yi = y;
	int Dx, Dy;

	if (dxi >= 0) {
		Dx = dxi;
		Dy = dyi;
	} else if (scaling) {
		Dx = scaled_x;
		Dy = scaled_y;
	} else {
		Dx = dpy_x;
		Dy = dpy_y;
	}

	/* ncache?? */

	if (rotating == ROTATE_NONE) {
		*xo = xi;
		*yo = yi;
	} else if (rotating == ROTATE_X) {
		*xo = Dx - xi - 1;
		*yo = yi;
	} else if (rotating == ROTATE_Y) {
		*xo = xi;
		*yo = Dy - yi - 1;
	} else if (rotating == ROTATE_XY) {
		*xo = Dx - xi - 1;
		*yo = Dy - yi - 1;
	} else if (rotating == ROTATE_90) {
		*xo = Dy - yi - 1;
		*yo = xi;
	} else if (rotating == ROTATE_90X) {
		*xo = yi;
		*yo = xi;
	} else if (rotating == ROTATE_90Y) {
		*xo = Dy - yi - 1;
		*yo = Dx - xi - 1;
	} else if (rotating == ROTATE_270) {
		*xo = yi;
		*yo = Dx - xi - 1;
	}
}