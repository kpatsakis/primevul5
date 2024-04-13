static void set_fs_factor(int max) {
	int f, fac = 1, n = dpy_y;

	fs_factor = 0;
	if ((bpp/8) * dpy_x * dpy_y <= max)  {
		fs_factor = 1;
		return;
	}
	for (f=2; f <= 101; f++) {
		while (n % f == 0) {
			n = n / f;
			fac = fac * f;
			if ( (bpp/8) * dpy_x * (dpy_y/fac) <= max )  {
				fs_factor = fac;
				return;
			}
		}
	}
}